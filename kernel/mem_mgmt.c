#include <system.h>
#include <screen.h>
#include <types.h>

#define MM_BLOCKS_PER_BYTE 8
#define MM_BLOCK_SIZE	4096
#define MM_BLOCK_ALIGN	MM_BLOCK_SIZE

static uint32_t mm_memory_size = 0;
static uint32_t mm_used_blocks = 0;
static uint32_t mm_block_count = 0;
static uint32_t *mm_memory_bitmap = 0;

/* This macros comes from the C FAQ available at: http://c-faq.com/misc/bitsets.html */
#define BITS_PER_WORD   32
#define BITMASK(b)      (1 << ((b) % BITS_PER_WORD))
#define BITSLOT(b)      ((b) / BITS_PER_WORD)
#define BITSET(a, b)    ((a)[BITSLOT(b)] |= BITMASK(b))
#define BITCLEAR(a, b)  ((a)[BITSLOT(b)] &= ~BITMASK(b))
#define BITTEST(a, b)   ((a)[BITSLOT(b)] & BITMASK(b))
#define BITNSLOTS(nb)   ((nb + BITS_PER_WORD - 1) / BITS_PER_WORD)

//! finds first free frame in the bit array and returns its index
int mm_bitmap_get_first_free()
{
    int i = 0;
    
    while (BITTEST(mm_memory_bitmap, i) && (i < mm_block_count)) {
        i++;
    }
    
    return (i==mm_block_count)? -1 : i;
}

//! finds first free "count" number of frames and returns its index
int mm_bitmap_get_n_free(size_t count)
{

    if (count == 0)
        return -1;

    if (count == 1)
        return mm_bitmap_get_first_free();
    
    int i = 0, start_index, bcount = 0;
    
    do {
        while (BITTEST(mm_memory_bitmap, i) && (i < mm_block_count)) {
            i++;
        }
        
        if (i == mm_block_count)
            return -1;
        
        start_index = i;
        while (!BITTEST(mm_memory_bitmap, i) && (i < mm_block_count)) {
            bcount++; i++;
            
            if (bcount == count)
                break;
        }
        
    } while (bcount < count);
    
    return start_index;
}

void mm_init(size_t mem_size, physical_addr_t bitmap_address)
{
    mm_memory_size = mem_size;
    mm_memory_bitmap = (uint32_t*) bitmap_address;
    mm_block_count = mm_memory_size / MM_BLOCK_SIZE;
    mm_used_blocks = mm_block_count;

    //! By default, all of memory is in use
    memset( mm_memory_bitmap, 0xff, mm_get_bitmap_size() );
}

void mm_mark_region_unused(physical_addr_t base, size_t size)
{

    int pos = base / MM_BLOCK_SIZE;
    int blocks = size / MM_BLOCK_SIZE;

    for (; blocks >= 0; blocks--) {
        BITCLEAR(mm_memory_bitmap, pos);
        pos++;
        mm_used_blocks--;
    }

    //first block is always set. This insures allocs can't be 0
    BITSET(mm_memory_bitmap, 0);
}

void mm_mark_region_used(physical_addr_t base, size_t size)
{
    int pos = base / MM_BLOCK_SIZE;
    int blocks = (size + MM_BLOCK_SIZE - 1) / MM_BLOCK_SIZE;

    kprintf("Region USED: %x Block Count: %u\n", base, blocks);

    for (; blocks >= 0; blocks--) {
        BITSET(mm_memory_bitmap, pos);
        pos++;
        mm_used_blocks++;
    }
}

void* mm_alloc_frame()
{

    if (mm_get_free_block_count() <= 0)
        return 0; //out of memory

    int frame = mm_bitmap_get_first_free();

    if (frame == -1)
        return NULL; //out of memory

    BITSET(mm_memory_bitmap, frame);

    physical_addr_t addr = frame * MM_BLOCK_SIZE;
    mm_used_blocks++;

    return (void *)addr;
}

void mm_release_frame(void* p)
{

    physical_addr_t addr = (physical_addr_t) p;
    int frame = addr / MM_BLOCK_SIZE;

    BITCLEAR(mm_memory_bitmap, frame);

    mm_used_blocks--;
}

void* mm_alloc_frames(size_t count)
{
    if (mm_get_free_block_count() <= count)
        return 0; //not enough space

    int frame = mm_bitmap_get_n_free(count);

    if (frame == -1)
        return 0; //not enough space

    int i;
    for (i = 0; i < count; i++)
        BITSET(mm_memory_bitmap, frame + i);

    physical_addr_t addr = frame * MM_BLOCK_SIZE;
    mm_used_blocks += count;

    return (void*) addr;
}

void mm_release_frames(void* p, size_t count)
{
    physical_addr_t addr = (physical_addr_t) p;
    int frame = addr / MM_BLOCK_SIZE;
    int i;
    
    for (i = 0; i < count; i++)
        BITCLEAR(mm_memory_bitmap, frame + i);

    mm_used_blocks -= count;
}

size_t mm_get_bitmap_size()
{
    return (mm_block_count + MM_BLOCKS_PER_BYTE - 1) / MM_BLOCKS_PER_BYTE;
}

size_t mm_get_memory_size()
{
    return mm_memory_size;
}

uint32_t mm_get_block_count()
{
    return mm_block_count;
}

uint32_t mm_get_use_block_count()
{
    return mm_used_blocks;
}

uint32_t mm_get_free_block_count()
{
    return mm_block_count - mm_used_blocks;
}

uint32_t mm_get_block_size()
{
    return MM_BLOCK_SIZE;
}

static inline void __mm_enable_paging()
{
    asm ( "movl %%cr0, %%eax;"
          "orl $0x80000000, %%eax;"
          "movl %%eax, %%cr0\n" : : : "eax");
}

static inline void __mm_disable_paging()
{
    asm ( "movl %%cr0, %%eax;"
          "andl $0x7FFFFFFF, %%eax;"
          "movl %%eax, %%cr0\n" : : : "eax");
}

void mm_set_paging_state(bool_t state)
{
    if (state)
        __mm_enable_paging();
    else
        __mm_disable_paging();
}

bool_t mm_is_paging_enabled()
{
    uint32_t res = 0;

    asm ("movl %%cr0, %0" : "=r" (res) : );

    return (res & 0x80000000) != 0;
}

void mm_set_pdbr(physical_addr_t addr)
{
    asm ("movl %0, %%cr3" : : "r" (addr));
}

physical_addr_t mm_get_pdbr()
{
    uint32_t result;

    asm ("movl %%cr3, %0" : "=r" (result) : );
    
    return result;
}

/* 
 * File:   system.h
 * Author: ideras
 *
 * Created on June 5, 2015, 1:37 PM
 */

#ifndef SYSTEM_H
#define	SYSTEM_H

#include <types.h>

#define KERNEL_BASE_ADDR    0x100000
#define CHECK_FLAG(flags, bit)   ((flags) & (1 << (bit)))

#define cli()   asm("cli")
#define sti()   asm("sti")

#define disable_interrupts(eflags) asm("pushf; pop %0; cli;" : "=g" ((eflags)))
#define restore_interrupts(eflags) asm("push %0; popf;" :: "g" ((eflags)))

/* This defines what the stack looks like after an ISR was running */
struct regs {
    unsigned int gs, fs, es, ds;
    unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax;
    unsigned int int_no, err_code;
    unsigned int eip, cs, eflags, useresp, ss;
};

/* This is the multiboot header structure */
struct multiboot_info {
    uint32_t flags;
    uint32_t low_mem;
    uint32_t high_mem;
    uint32_t boot_device;
    uint32_t cmdline;
    uint32_t mods_count;
    struct multibootModules* mods_addr;

    struct {
        uint32_t num;
        uint32_t size;
        uint32_t addr;
        uint32_t shndx;
    } elf_sec;
    
    unsigned long mmap_length;
    unsigned long mmap_addr;
    unsigned long drives_length;
    unsigned long drives_addr;
    unsigned long config_table;
    unsigned long boot_loader_name;
    unsigned long apm_table;
    unsigned long vbe_control_info;
    unsigned long vbe_mode_info;
    unsigned long vbe_mode;
    unsigned long vbe_interface_seg;
    unsigned long vbe_interface_off;
    unsigned long vbe_interface_len;
};
struct multibootModules{
    uint32_t modStart;
    uint32_t modEnd;
    char * cmline;
    uint32_t reservado;
            
};
#define MULTIBOOT_MEMORY_AVAILABLE              1
#define MULTIBOOT_MEMORY_RESERVED               2

struct multiboot_mmap_entry {
    uint32_t size;
    uint64_t addr;
    uint64_t len;
    uint32_t type;
} __attribute__((packed));

typedef struct multiboot_mmap_entry multiboot_memory_map_t;

/* String functions */
void *memcpy(void *dest, const void *src, size_t count);
void *memset(void *dest, char val, size_t count);
uint16_t *memsetw(uint16_t *dest, uint16_t val, size_t count);

/* Global Descriptor Table */
void gdt_set_descriptor(int num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran);
void gdt_init();

/* Interrupt Descriptor Table  */
void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags);
void idt_init();

/* Interrupt Request */
void irq_install_handler(int irq, void (*handler)(struct regs *r));
void irq_uninstall_handler(int irq);
void irq_init();

/* Interrupt Service Routines */
void isrs_init();

/* IDE */
void init_ide_devices(void);
unsigned int ide_read_blocks(unsigned int minor, offset_t block, unsigned int nblocks, void *buffer);
unsigned int ide_write_blocks(unsigned int minor, offset_t block, unsigned int nblocks, void *buffer);

/* IRQ handler type */
typedef void (*irq_handler_t)(struct regs *regs);

/* Exception handler type */
typedef void (*exception_handler_t)(struct regs *regs);

/* Memory Management */
void mm_init(size_t memSize, physical_addr_t bitmap);
void mm_mark_region_unused(physical_addr_t base, size_t size);
void mm_mark_region_used(physical_addr_t base, size_t size);
void* mm_alloc_frame();
void mm_release_frame(void* p);
void* mm_alloc_frames(size_t count);
void mm_release_frames(void* p, size_t count);
size_t mm_get_bitmap_size();
size_t mm_get_memory_size();
uint32_t mm_get_block_count();
uint32_t mm_get_use_block_count();
uint32_t mm_get_free_block_count();
uint32_t mm_get_block_size();
void mm_set_paging_state(bool_t state);
bool_t mm_is_paging_enabled();
void mm_set_pdbr(physical_addr_t addr);
physical_addr_t mm_get_pdbr();


#endif	/* SYSTEM_H */

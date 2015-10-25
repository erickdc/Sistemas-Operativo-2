#include <system.h>
#include <screen.h>
#include <low_level.h>
#include "keyboard.h"
#include "IFat16.h"

void main(struct multiboot_info *mbinfo, uint32_t kernel_end_addr)
{
    int i;
    unsigned char buffer[512];
    //void * listaA[200]; 
    gdt_init();
    idt_init();
    irq_init();
    isrs_init();
    mm_init((mbinfo->low_mem + mbinfo->high_mem +1024)*1024,kernel_end_addr);
    int kernelSize = kernel_end_addr - KERNEL_BASE_ADDR;
    multiboot_memory_map_t *mmap_ptr = (multiboot_memory_map_t*) mbinfo->mmap_addr;
    multiboot_memory_map_t *mmap_end = (multiboot_memory_map_t*) (mbinfo->mmap_addr + mbinfo->mmap_length);

    while (mmap_ptr < mmap_end) {
        if (mmap_ptr->type == MULTIBOOT_MEMORY_AVAILABLE) {
            uint32_t region_base_addr = (uint32_t) (mmap_ptr->addr & 0xffffffff);
            uint32_t region_size = (uint32_t) (mmap_ptr->len & 0xffffffff);

            mm_mark_region_unused(region_base_addr, region_size);
        }
    mmap_ptr = (multiboot_memory_map_t*) ((uint32_t) mmap_ptr + mmap_ptr->size + sizeof (uint32_t));
    
    }
    mm_mark_region_used(KERNEL_BASE_ADDR, kernelSize + mm_get_bitmap_size());
   
  
            
   
   // malloc con cantidad pequeña
 /*   for ( int i = 0 ; i<10 ; i ++)
    {
         void * prueba1 = malloc(8);
         kprintf("\nesta en la direccion:%p ",prueba1);
    }
    //malloc se pasa de un frame
   for ( int i = 0 ; i<175 ; i ++)
    {
         listaA[i] = malloc(16);
         
         kprintf("\nesta en la direccion:%p ",listaA[i]);
    }
    
    free(listaA[15]);
    
     listaA[175] = malloc(16);
      kprintf("\nesta en la direccion:%p ",listaA[175]);
      */
    InicializarEstructuraFat();
     __asm__ __volatile__("sti");
     clear_screen();
       
    
             
      irq_install_handler(1,keyboard_handler);
   //
      typedef void (*call_module_t)(unsigned);
    /* ... */
     char buffer2[1024];
   
    if(mbinfo->mods_count>0){
           // for(int numberModule=0; numberModule<mbinfo->mods_count;numberModule){
                uint32_t  mod_start =((uint32_t) mbinfo->mods_addr[0].modStart);
                 call_module_t start_program = (call_module_t) mod_start;
                 start_program(mod_start);
            //}
           
        }
     
    
    
}

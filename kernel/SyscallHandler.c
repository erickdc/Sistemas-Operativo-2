#include "system.h"
#include "screen.h"


void intHandler128(struct regs *r){
    switch(r->eax){
        case 0:
            printString((char*)r->ebx);
            break;
        case 1:
            readString((char*)r->ebx,(unsigned)r->ecx);
            break;
        case 2:
            clear_screen();
            break;
        case 3:
            ImprimirDirRoot(2089);
            break;
    }
}
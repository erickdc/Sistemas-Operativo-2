#include "system.h"

void syscallPrintString(char *buffer) {
    sti();
    asm("int $0x80": :"a"(0),"b"(buffer):);
}
void syscallReadString(char *buffer, int size) {
    sti();
    asm("int $0x80": :"a"(1),"b"(buffer),"c"(size):);
}

void syscallClearScreen(){
    sti();
    asm("int $0x80": :"a"(2):);
}


void syscallLs(){
    sti();
    asm("int $0x80": :"a"(3));
}
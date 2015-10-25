
unsigned char port_byte_in (unsigned short port) 
{
    unsigned char result ;
    
    __asm__ ("in %1, %0" : "=a" (result) : "d" (port));
    
    return result ;
}

void port_byte_out(unsigned short port, unsigned char data) 
{
    __asm__ ("out %0 , %1" : : "a" (data), "d" (port));
}

unsigned short port_word_in(unsigned short port)
{
    unsigned short result ;
    
    __asm__ ("in %1, %0" : "=a" (result) : "d" (port));
    
    return result ;
}

void port_word_out(unsigned short port, unsigned short data)
{
    __asm__ ("out %0, %1" : : "a" (data), "d" (port)); 
}

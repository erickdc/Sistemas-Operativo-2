#ifndef _LOW_LEVEL_H_
#define _LOW_LEVEL_H_

#define IRQ_PRIMARY_IDE     14
#define IRQ_SECONDARY_IDE   15

static inline void udelay(unsigned long n)
{
    if (!n)
        return;
    asm volatile ("1: dec %0; jne 1b;" : : "a" (n * 1000) );
}

static inline void mdelay(unsigned long n)
{
    while (--n)
        udelay(1000);
}

unsigned char port_byte_in (unsigned short port);
void port_byte_out(unsigned short port, unsigned char data) ;
unsigned short port_word_in(unsigned short port);
void port_word_out(unsigned short, unsigned short data);

#endif

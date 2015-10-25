/* 
 * File:   keyboard.h
 * Author: erick304
 *
 * Created on August 21, 2015, 2:26 PM
 */

#ifndef KEYBOARD_H
#define	KEYBOARD_H
#define tamanioBuffer 1024
#include "low_level.h"


#ifdef	__cplusplus
extern "C" {
#endif
 char buffer [tamanioBuffer];
 int capSon =0;
 int numLock=0;
 int shift=0;
 
 int position =0;
unsigned char kbdus[128] =
{
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8',	/* 9 */
  '9', '0', '-', '=', '\b',	/* Backspace */
  '\t',			/* Tab */
  'q', 'w', 'e', 'r',	/* 19 */
  't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',	/* Enter key */
    0,			/* 29   - Control */
  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',	/* 39 */
 '\'', '`',   0,		/* Left shift */
 '\\', 'z', 'x', 'c', 'v', 'b', 'n',			/* 49 */
  'm', ',', '.', '/',   0,				/* Right shift */
  '*',
    0,	/* Alt */
  ' ',	/* Space bar */
    0,	/* Caps lock */
    0,	/* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,	/* < ... F10 */
    0,	/* 69 - Num lock*/
    0,	/* Scroll Lock */
    '7',	/* Home key */
    '8',	/* Up Arrow */
    '9',/* Page Up */
  '-',
    '4',	/* Left Arrow */
    '5',
    '6',	/* Right Arrow */
  '+',
    '1',	/* 79 - End key*/
    '2',	/* Down Arrow */
    '3',	/* Page Down */
    '0',	/* Insert Key */
    '.',	/* Delete Key */
    0,   0,   0,
    0,	/* F11 Key */
    0,	/* F12 Key */
    0,	/* All other keys are undefined */
};
unsigned char kbdusShift[128] =
{
    0,  27, '!', '@', '#', '$', '%', '^', '&', '*',	/* 9 */
  '(', ')', '_', '+', '\b',	/* Backspace */
  '\t',			/* Tab */
  'Q', 'W', 'E', 'R',	/* 19 */
  'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',	/* Enter key */
    0,			/* 29   - Control */
  'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':',	/* 39 */
 '"', '~',   0,		/* Left shift */
 '\\', 'Z', 'X', 'C', 'V', 'B', 'N',			/* 49 */
  'M', '<', '>', '?',   0,				/* Right shift */
  '*',
    0,	/* Alt */
  ' ',	/* Space bar */
    0,	/* Caps lock */
    0,	/* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,	/* < ... F10 */
    0,	/* 69 - Num lock*/
    0,	/* Scroll Lock */
    '7',	/* Home key */
    '8',	/* Up Arrow */
    '9',/* Page Up */
  '-',
    '4',	/* Left Arrow */
    '5',
    '6',	/* Right Arrow */
  '+',
    '1',	/* 79 - End key*/
    '2',	/* Down Arrow */
    '3',	/* Page Down */
    '0',	/* Insert Key */
    '.',	/* Delete Key */
    0,   0,   0,
    0,	/* F11 Key */
    0,	/* F12 Key */
    0,	/* All other keys are undefined */
};

char readKey()
{
     char valre = buffer[position];
     buffer[position] =0;
    
     return valre; 
         
}

void inicializarBuffer()
{
    int i;
    for(i =0 ; i<tamanioBuffer;i++)
        buffer[i] =0;
}
char* toUpper(char* string)
{
    for(char* p=string; *p != '\0'; p++)
    {
        if(*p >= 'a' && *p <= 'z')  //Only if it's a lower letter
          *p -= 32;
    }
    return string;
}
int toLower(int chr)//touches only one character per call
{
    return (chr >='A' && chr<='Z') ? (chr + 32) : (chr);    
}

void backSpace(){
    int position = get_cursor();
    set_cursor(position-1);
     kprintf("%c",' ');
     set_cursor(position-1);
}

void moveArrow(int scancode){
    int position = get_cursor();
    if(scancode==75)
      set_cursor(position-1);
      if(scancode == 77)
           set_cursor(position+2);
}
void keyboard_handler(struct regs *r)
{
    unsigned char scancode;

    /* Read from the keyboard's data buffer */
    scancode = port_byte_in(0x60);

    /* If the top bit of the byte we read from the keyboard is
    *  set, that means that a key has just been released */
    if (scancode & 0x80)
    {
        if(scancode == 186 && capSon == 0){
            capSon =1;
            
        }else if (scancode == 186 && capSon == 1 ){
            capSon =0;
          
        }
        
       if(scancode == 170 || scancode == 182)
            shift =0;
            
        if(scancode == 197)
            numLock =0;
       // kprintf("%d",scancode);
    }else
    {
        if(position < tamanioBuffer)
        {
            position++;
        }
      
            char var = kbdus[scancode];
          
            if(scancode ==42 || scancode ==54)
                shift =1;
            if(scancode == 59)
                numLock =1;
            
            if(capSon==1 && shift ==0){
               var= (toUpper(&var))[0];
            }else if(shift==1 && capSon==1){
                var = toLower(kbdusShift[scancode]);
            }else if (shift==1 && capSon==0)
                var = toUpper(&kbdusShift[scancode])[0];
       //     kprintf("%d",scancode);
            if(var!=0){
                 buffer[position]= var;
            }
             
            //if(scancode==14)
              //  backSpace();
                
                
          /*  else if(scancode == 75 || scancode == 77)
                moveArrow(scancode);*/
      
    }
}

void readString(char *line,int size)
{ 
   int i=0;

   int termine = 0;
   while(!termine && i<size)
   {
       cli();
       char valor = readKey();
       sti();
      if(valor== 0)continue;
	 line[i]=valor;
	 if(line[i] =='\b' )
 	 {

	     if(i>0){

	       	line[i] = '\0';
		    i--;
		    backSpace();
	      }	       	
	 }else if(line[i]=='\n')
	 {
		line[i+1]=0x0;
		termine = 1;
	 }else
	 {	
		kprintf("%c",line[i]);
		i++;
		
	 }	
		
	    
   }
	
    
}

void printString(char * buffer){
    
    kprintf("%s",buffer);
}

#ifdef	__cplusplus
}
#endif

#endif	/* KEYBOARD_H */


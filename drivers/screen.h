#ifndef _SCREEN_H_
#define _SCREEN_H_

#define VIDEO_ADDRESS 0xb8000
#define MAX_ROWS 25
#define MAX_COLS 80

// Screen device I/O ports
#define REG_SCREEN_CTRL 0x3D4
#define REG_SCREEN_DATA 0x3D5

#define TEXT_ATTR(f_color, b_color) ((((b_color) & 0x0F) << 4) | ((f_color) & 0x0F))

/* 
 * Video colors constants 
 */
#define BLACK           0
#define BLUE            1
#define GREEN           2
#define CYAN            3
#define RED             4
#define MAGENTA         5
#define BROWN           6
#define LIGHT_GRAY      7
#define DARK_GRAY       8
#define LIGHT_BLUE      9
#define LIGHT_GREEN     10
#define LIGHT_CYAN      11
#define LIGHT_RED       12
#define LIGHT_MAGENTA   13
#define YELLOW          14
#define WHITE           15

// Attribute byte for our default colour scheme .
#define DEFAULT_TEXT_ATTR    TEXT_ATTR(LIGHT_GRAY, BLACK)

#define F_COLOR(attr)   ((attr) & 0x0F)
#define B_COLOR(attr)   (((attr) & 0xF0) >> 4)

void cputchar_at(char character, int col, int row, char attribute_byte);
int get_screen_offset(int column, int row);
int get_cursor();
void set_cursor(int offset);
void cputs_at(const char *message, int col, int row, char attribute);
void puts_at(const char *message, int col, int row);
void cputs(const char *message, char attribute);
void puts(const char *message);
void print_udec(unsigned int n);
void print_sdec(int n);
void print_hex(unsigned int n, int lower_case);
void kprintf(const char *fmt, ...);
void clear_screen();

#endif

#include <system.h>
#include <screen.h>
#include <low_level.h>

typedef char* va_list;

/* Number of bytes used by the specified type in the stack.
   This is equivalent to sizeof(long) * ceil(sizeof(type) / sizeof(long)) */
#define __va_size(type) \
    (((sizeof(type) + sizeof(long) - 1) / sizeof(long)) * sizeof(long))

/* Initializes the specified pointer (ap) to point to the first parameter
   following the specified parameter (last) */
#define va_start(ap, last) \
    ((ap) = (va_list) &(last) + __va_size(last))

/* Make the specified pointer (ap) point to the next parameter in the stack. */
#define va_arg(ap, type) \
    (*(type *)((ap) += __va_size(type), (ap) - __va_size(type)))

int handle_scrolling(int cursor_offset);

/* Print a char on the screen at col, row, or at cursor position */
void cputchar_at(char character, int col, int row, char attribute_byte)
{
    unsigned char *vidmem = (unsigned char *) VIDEO_ADDRESS;

    if (!attribute_byte) {
        attribute_byte = DEFAULT_TEXT_ATTR;
    }

    /* Get the video memory offset for the screen location */
    int offset;

    /* If col and row are non - negative, use them for offset.
     * Otherwise , use the current cursor position . */
    if (col >= 0 && row >= 0) {
        offset = get_screen_offset(col, row);
    } else {
        offset = get_cursor();
    }

    if (character == '\n') {
        int rows = offset / (2 * MAX_COLS);
        offset = get_screen_offset(MAX_COLS - 1, rows);
    } else {
        vidmem[offset] = character;
        vidmem[offset + 1] = attribute_byte;
    }
    offset += 2;
    offset = handle_scrolling(offset);

    // Update the cursor position on the screen device .
    set_cursor(offset);
}

int get_screen_offset(int column, int row)
{
    return (row * MAX_COLS + column) * 2;
}

int get_cursor()
{
    port_byte_out(REG_SCREEN_CTRL, 14);
    int offset = port_byte_in(REG_SCREEN_DATA) << 8;

    port_byte_out(REG_SCREEN_CTRL, 15);
    offset += port_byte_in(REG_SCREEN_DATA);

    // Since the cursor offset reported by the VGA hardware is the
    // number of characters , we multiply by two to convert it to
    // a character cell offset .
    return offset * 2;
}

void set_cursor(int offset)
{
    offset /= 2;
    port_word_out(REG_SCREEN_CTRL, 14 | (offset & 0xff00));
    port_word_out(REG_SCREEN_CTRL, 15 | (offset << 8));
}

void cputs_at(const char *message, int col, int row, char attribute)
{
    if (col >= 0 && row >= 0) {
        set_cursor(get_screen_offset(col, row));
    }

    // Loop through each char of the message and print it .
    const char *p = message;

    while (*p != 0) {
        cputchar_at(*p++, col, row, attribute);
    }
}

void puts_at(const char *message, int col, int row)
{
    cputs_at(message, col, row, DEFAULT_TEXT_ATTR);
}

void cputs(const char *message, char attribute)
{
    cputs_at(message, -1, -1, attribute);
}

void puts(const char *message)
{
    puts_at(message, -1, -1);
}

void print_udec(unsigned int n)
{
    unsigned char buffer[12]; // 12 elements should be enough for a 32-bit number

    if (n == 0)
        cputchar_at('0', -1, -1, DEFAULT_TEXT_ATTR);
    else {

        unsigned char *p = &buffer[11];

        *p-- = '\0';
        while (n > 0) {
            *p-- = (n % 10) + '0';
            n /= 10;
        }
        p++;

        while (*p != '\0') {
            cputchar_at(*p++, -1, -1, DEFAULT_TEXT_ATTR);
        }
    }
}

void print_sdec(int n)
{
    unsigned char buffer[12]; // 12 elements should be enough for an 32-bit number

    if (n == 0)
        cputchar_at('0', -1, -1, DEFAULT_TEXT_ATTR);
    else {

        if (n < 0) {
            cputchar_at('-', -1, -1, DEFAULT_TEXT_ATTR);
            n = -n;
        }

        unsigned char *p = &buffer[11];
        *p-- = '\0';
        while (n > 0) {
            *p-- = (n % 10) + '0';
            n /= 10;
        }
        p++;

        while (*p != '\0') {
            cputchar_at(*p++, -1, -1, DEFAULT_TEXT_ATTR);
        }
    }
}

void print_hex(unsigned int n, int lower_case)
{
    int i;
    int d, previous_write = 0;
    int hex_add = lower_case ? ('a' - 0xa) : ('A' - 0xA);

    if (n == 0) {
        cputchar_at('0', -1, -1, DEFAULT_TEXT_ATTR);
        return;
    }

    for (i = 7; i >= 0; i--) {
        d = ((0x0F << i * 4) & n) >> i * 4;

        if (d < 0xA)
            d += '0';
        else
            d += hex_add;

        if (d != '0' || previous_write) {
            cputchar_at((char) d, -1, -1, DEFAULT_TEXT_ATTR);
            previous_write = 1;
        }
    }
}

void kprintf(const char *fmt, ...)
{
    va_list va;
    char ch;
    const char *p = fmt;

    va_start(va, fmt);

    while (*p != '\0') {
        ch = *p++;
        if (ch != '%') {
            cputchar_at(ch, -1, -1, DEFAULT_TEXT_ATTR);
        } else {
            ch = *p++;
            switch (ch) {
                case '\0':
                    break;
                case 'u':
                {
                    unsigned int v = va_arg(va, unsigned int);
                    print_udec(v);
                    break;
                }
                case 'd':
                {
                    int v = va_arg(va, int);
                    print_sdec(v);
                    break;
                }
                case 'p':
                    puts("0x");
                case 'x':
                case 'X':
                {
                    unsigned int v = va_arg(va, unsigned int);
                    print_hex(v, ch == 'x');
                    break;
                }
                case 'c':
                    cputchar_at((char) (va_arg(va, int)), -1, -1, DEFAULT_TEXT_ATTR);
                    break;
                case 's':
                {
                    char *str = va_arg(va, char*);
                    puts(str);
                    break;
                }
                case '%':
                    cputchar_at('%', -1, -1, DEFAULT_TEXT_ATTR);
                default:
                    break;
            }
        }
    }
}

void clear_screen()
{
    int row, col;

    /* Loop through video memory and write blank characters . */
    for (row = 0; row < MAX_ROWS; row++) {
        for (col = 0; col < MAX_COLS; col++) {
            cputchar_at(' ', col, row, DEFAULT_TEXT_ATTR);
        }
    }

    // Move the cursor back to the top left .
    set_cursor(get_screen_offset(0, 0));
}

/* Advance the text cursor, scrolling the video buffer if necessary . */
int handle_scrolling(int cursor_offset)
{
    // If the cursor is within the screen, return it unmodified.
    if (cursor_offset < MAX_ROWS * MAX_COLS * 2) {
        return cursor_offset;
    }

    /* Shuffle the rows back one . */
    int i;

    for (i = 1; i < MAX_ROWS; i++) {
        memcpy( (char *) (get_screen_offset(0, i - 1) + VIDEO_ADDRESS),
                (char *) (get_screen_offset(0, i) + VIDEO_ADDRESS),
                MAX_COLS * 2
              );
    }

    /* Blank the last line by setting all bytes to 0 */
    char *last_line = (char *) (get_screen_offset(0, MAX_ROWS - 1) + VIDEO_ADDRESS);

    for (i = 0; i < MAX_COLS * 2; i++) {
        last_line [i] = 0;
    }

    // Move the offset back one row, such that it is now on the last row
    // rather than off the edge of the screen
    cursor_offset -= 2 * MAX_COLS;

    // Return the updated cursor position.
    return cursor_offset;
}



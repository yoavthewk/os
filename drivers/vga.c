#include <kernel/vga.h>

#include <libc/mem.h>
#include <libc/string.h>
#include <arch/x86/cpu/comm.h>

volatile uint16_t* terminal = (uint16_t*)TERMINAL_MEMORY;

uint8_t terminal_col = 0;
uint8_t terminal_row = 0;

uint8_t vga_color_entry(enum vga_color fg, enum vga_color bg) {
    return fg | bg << 4;
}

uint16_t vga_entry(char c, uint8_t color) {
    return (uint16_t)((color << 8) | c);
}

void set_cursor_position(uint8_t row, uint8_t col) {
    const uint16_t position = GET_POSITION(row, col);

    outb(REG_SCREEN_CTRL, CURSOR_LOC_HIGH); 
    outb(REG_SCREEN_DATA, position >> 8);
    outb(REG_SCREEN_CTRL, CURSOR_LOC_LOW); 
    outb(REG_SCREEN_DATA, position & 0xff);
}

/* Clear the whole terminal. */
void clear_terminal(void) {
    for (uint8_t x = 0; x < MAX_ROWS; ++x) {
        for (uint8_t y = 0; y < MAX_COLS; ++y) {
            const uint16_t index = x * MAX_COLS + y;
            terminal[index] = vga_entry(' ', VGA_COLOR_WHITE);
        }
    }

    set_cursor_position(terminal_row, terminal_col);
}

void kbackspace(void) {
    if (0 == terminal_col && 0 == terminal_row) {
        return;
    }

    if (terminal_col == 0) {
        terminal_row -= 1;
        terminal_col = MAX_COLS - 2;
    }
    else {
        --terminal_col;
    }

    const uint16_t index = terminal_row * MAX_COLS + terminal_col;
    terminal[index] = vga_entry(' ', VGA_COLOR_WHITE);
    set_cursor_position(terminal_row, terminal_col);
}

void writec(char c) {
    const uint16_t index = terminal_row * MAX_COLS + terminal_col;
    terminal[index] = vga_entry(c, VGA_COLOR_WHITE);
}

void kprint(char* str) {
    for (uint32_t index = 0; str[index] != 0; ++index) {
        kputc(str[index]);
    }
}

void kprintln(char* str) {
    for (uint32_t index = 0; str[index] != 0; ++index) {
        kputc(str[index]);
    }
    newline();
}

void newline(void) {
    ++terminal_row;
    terminal_col = 0;
}

void scroll(void) {
    const uint16_t second_row_start = 1 * MAX_COLS + 0;
    const uint16_t last_row_start = (MAX_ROWS - 1) * MAX_COLS + 0;

    // Copy terminal from second row onwards to first.
    memcpy(
        (uint8_t*)(terminal + second_row_start),
        (uint8_t*)terminal,
        (MAX_ROWS - 1) * MAX_COLS * 2
    );

    for (uint16_t col = 0; col < MAX_COLS; ++col) {
        terminal[(MAX_ROWS - 1) * MAX_COLS + col] = vga_entry(' ', VGA_COLOR_WHITE);
    }

    // We just scolled, therefore place ourselves in the correct location.
    terminal_row = MAX_ROWS - 1;
    terminal_col = 0;
}

void kputc(char c) {
    if (c == '\n') {
        newline();
    }

    if ((terminal_col + 1) >= MAX_COLS) {
        newline();
    }
    
    if (terminal_row >= MAX_ROWS) {
        scroll();
    }

    if (c != '\n') {
        writec(c);
        ++terminal_col;
    }

    set_cursor_position(terminal_row, terminal_col);
}

void do_printf(const char* fmt, va_list args) {
    char fmt_buf[MAX_FMT_BUF_LEN]; 

    while(*fmt) {
        if (*fmt == '%') {
            ++fmt;
            switch(*fmt) {
                case 'd':
                    itoa(va_arg(args, const int32_t), fmt_buf, 10);
                    kprint(fmt_buf);
                    break;
                case 'x':
                    itoa(va_arg(args, const int32_t), fmt_buf, 16);
                    kprint("0x");
                    kprint(fmt_buf);
                    break;
                case 's':
                    kprint(va_arg(args, const char*));
                    break;
                case 'c':
                    kputc(va_arg(args, const char));
                    break;
                default:
                    // not a format specifier supported!
                    kputc('%');
                    kputc(*fmt);
                    break;
            }
        }
        else {
            kputc(*fmt);
        }
        ++fmt;
    }
}

void kprintfln(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    do_printf(fmt, args);
    newline();
    va_end(args);
}

void kprintf(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    do_printf(fmt, args);
    va_end(args);
}
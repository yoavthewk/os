#include <kernel/vga.h>
#include <arch/x86/cpu/comm.h>
#include <libc/mem.h>

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
void clear_terminal() {
    for (uint8_t x = 0; x < MAX_ROWS; ++x) {
        for (uint8_t y = 0; y < MAX_COLS; ++y) {
            const uint16_t index = x * MAX_COLS + y;
            terminal[index] = vga_entry(' ', VGA_COLOR_WHITE);
        }
    }

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
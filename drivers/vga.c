#include "vga.h"

#define MAX_COLS 80
#define MAX_ROWS 25
#define TERMINAL_MEMORY 0xB8000

volatile uint16_t* terminal = (uint16_t*)TERMINAL_MEMORY;

uint8_t terminal_col = 1;
uint8_t terminal_row = 0;

/* Clear the whole terminal. */
void clear_terminal() {
    for (uint8_t x = 0; x < MAX_ROWS; ++x) {
        for (uint8_t y = 0; y < MAX_COLS; ++y) {
            const uint16_t index = x * MAX_COLS + y;
            terminal[index] = vga_entry(' ', VGA_COLOR_WHITE);
        }
    }
}

uint16_t vga_entry(char c, uint8_t color) {
    return (uint16_t)((color << 8) | c);
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

void kputc(char c) {
    if (c == '\n') {
        newline();
    }
    else {
        writec(c);
        ++terminal_col;
    }

    if (terminal_col >= MAX_COLS) {
        newline();
    }
    
    if (terminal_row >= MAX_ROWS) {
        // scroll(); - to be implemented.
    }
}
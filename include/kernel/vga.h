/**
    A driver for VGA screen output.
 */

#include <stdint.h>
#include <stdarg.h>

enum vga_color {
	VGA_COLOR_BLACK = 0,
	VGA_COLOR_BLUE = 1,
	VGA_COLOR_GREEN = 2,
	VGA_COLOR_CYAN = 3,
	VGA_COLOR_RED = 4,
	VGA_COLOR_MAGENTA = 5,
	VGA_COLOR_BROWN = 6,
	VGA_COLOR_LIGHT_GREY = 7,
	VGA_COLOR_DARK_GREY = 8,
	VGA_COLOR_LIGHT_BLUE = 9,
	VGA_COLOR_LIGHT_GREEN = 10,
	VGA_COLOR_LIGHT_CYAN = 11,
	VGA_COLOR_LIGHT_RED = 12,
	VGA_COLOR_LIGHT_MAGENTA = 13,
	VGA_COLOR_LIGHT_BROWN = 14,
	VGA_COLOR_WHITE = 15,
};

/* I/O Ports. */
#define REG_SCREEN_CTRL 0x3d4
#define REG_SCREEN_DATA 0x3d5

/* I/O Regs. */
#define CURSOR_LOC_HIGH 0x0F
#define CURSOR_LOC_LOW 0x0E

/* VGA Constants. */
#define MAX_COLS 80
#define MAX_ROWS 25
#define TERMINAL_MEMORY 0x000B8000 // Identity-mapped.

/* Offset Macros. */
#define GET_POSITION(row, col) ((row * MAX_COLS) + col)

void clear_terminal(void);
void kbackspace(void);
void writec(char c);
void kprint(const char* str);
void kprintln(const char* str);
void newline(void);
void kputc(char c);
void set_cursor_position(uint8_t row, uint8_t col);
void kprintfln(const char* fmt, ...);
void kprintf(const char* fmt, ...);

uint16_t vga_entry(char c, uint8_t color);
uint8_t vga_color_entry(enum vga_color fg, enum vga_color bg);

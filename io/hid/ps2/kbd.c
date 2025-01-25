#include <arch/x86/cpu/idt.h>
#include <arch/x86/cpu/isr.h>
#include <libc/string.h>
#include <arch/x86/cpu/comm.h>
#include <arch/x86/8259/pic.h>
#include <kernel/hid/kbd.h>
#include <kernel/vga.h>
#include <libc/mem.h>

#define KB_BUFFER_MAX 256
#define UPPERCASE_MASK 0x80
#define LEFT_SHIFT 0x36
#define RIGHT_SHIFT 0x2A
#define BACKSPACE 0x0E
#define CAPS_LOCK 0x3A
#define ENTER 0x1C

static char __kb_buf[KB_BUFFER_MAX] = { 0 };
uint8_t __kb_idx = 0;

// currently support scan set 1 - TODO: switch?
const char __sc_ascii[] = { 
    '?', '?', '1', '2', '3', '4', '5', '6',     
    '7', '8', '9', '0', '-', '=', '?', '?', 'Q', 'W', 'E', 'R', 'T', 'Y', 
    'U', 'I', 'O', 'P', '[', ']', '?', '?', 'A', 'S', 'D', 'F', 'G', 
    'H', 'J', 'K', 'L', ';', '\'', '`', '?', '\\', 'Z', 'X', 'C', 'V', 
    'B', 'N', 'M', ',', '.', '/', '?', '?', '?', ' '
};

// TODO: other mod keys.
bool __shift_down = false;
bool __caps_lock = false;
bool __enter_pressed = false;

void __kbwrite(char letter) {
    if (__kb_idx == KB_BUFFER_MAX) {
        return;
    }

    __kb_buf[__kb_idx++] = letter;
    __kb_buf[__kb_idx] = '\0';
}

void __kbbackspace() {
    __kb_idx -= __kb_idx ? 1 : 0;
    __kb_buf[__kb_idx] = 0;
}

bool __is_letter(char sc_ascii) {
    bool is_lowercase = sc_ascii <= 'z' && sc_ascii >= 'a';
    bool is_uppercase = sc_ascii <= 'Z' && sc_ascii >= 'A';

    return is_lowercase | is_uppercase;
}

void __handle_character(char sc_ascii) {
    uint8_t is_uppercase = (uint8_t)__shift_down ^ (uint8_t)__caps_lock; 

    if (__is_letter(sc_ascii)) {
        char letter = is_uppercase ? sc_ascii : sc_ascii + 0x20;
        __kbwrite(letter);
        kputc(letter);
    }
    else {
        kputc(sc_ascii);
    }
}

void __keyup(uint8_t scancode) {
    scancode &= ~(0x80);
    switch(scancode) {        
        case LEFT_SHIFT:
        case RIGHT_SHIFT:
            __shift_down = false;
            break;

        case CAPS_LOCK:
            __caps_lock = __caps_lock ? false : true;
            break;
        
        default: 
            break;
    }
}

void __keydown(uint8_t scancode) {
    switch (scancode)
    {
    case LEFT_SHIFT:
    case RIGHT_SHIFT:
        __shift_down = true;
        break;

    case CAPS_LOCK:
        break;

    case BACKSPACE:
        __kbbackspace();
        kbackspace();
        break;

    case ENTER:
        __enter_pressed = true;
        newline();
        break;
    
    // Add more as needed.
    default:
        __handle_character(__sc_ascii[scancode]);
        break;
    }
}

void __process_keyscan(uint8_t scancode) {
    if (scancode & UPPERCASE_MASK) {
        __keyup(scancode);
    }
    else {
        __keydown(scancode);
    }
}

void __ps2_kb_irq_handler(irq_frame_t* frame) {
    __process_keyscan(inb(0x60));
}

void kb_getline(char* input) {
    // hold while we do not get enter.
    while (!__enter_pressed); 
    memcpy(__kb_buf, input, __kb_idx ? __kb_idx + 1 : 0);
    __enter_pressed = false;
    __kb_idx = 0;
}

void init_ps2_kb(void) {
    irq_clear_mask(KB_IRQ_VECTOR);
    idt_install_irq_handler(KB_IDT_ENTRY, __ps2_kb_irq_handler);
}
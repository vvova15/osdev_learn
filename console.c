
#include "console.h"

static int cursor = 0;

int row = 0;
int col = 0;

void vga_putc(char c) {
    if (c == '\n') {
        row++;
        col = 0;
        return;
    }

    VGA_TEXT[row * VGA_WIDTH + col] = (0x0F << 8) | c;
    col++;

    if (col >= VGA_WIDTH) {
        col = 0;
        row++;
    }
}

void vga_print(const char* s) {
    while (*s) vga_putc(*s++);
}

char hex[] = "0123456789ABCDEF";

void vga_print_hex8(uint8_t val) {
    vga_putc(hex[(val >> 4) & 0xF]);
    vga_putc(hex[val & 0xF]);
}

void vga_print_hex16(uint16_t val) {
    vga_print_hex8(val >> 8);
    vga_print_hex8(val & 0xFF);
}



void vga_clear_screen(void) {
    uint16_t blank = (VGA_COLOR << 8) | ' ';

    for (int y = 0; y < VGA_HEIGHT; y++) {
        for (int x = 0; x < VGA_WIDTH; x++) {
            VGA_TEXT[y * VGA_WIDTH + x] = blank;
        }
    }

    row = 0;
    col = 0;
}

void vga_reset_cursor(void) {
    row = 0;
    col = 0;
}
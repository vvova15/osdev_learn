
#pragma once
#include <stdint.h>
#define VGA_WIDTH  80
#define VGA_HEIGHT 25
#define VGA_COLOR  0x0F

#define VGA_TEXT ((uint16_t*)0xB8000)

void vga_reset_cursor();
void vga_clear_screen();
void vga_print(const char* s);
void vga_print_hex8(uint8_t val);
void vga_print_hex16(uint16_t val);
void vga_print_hex32(uint32_t val);

void vga_print_dec8(uint8_t val);
void vga_print_dec16(uint16_t val);
void vga_print_dec32(uint32_t val);
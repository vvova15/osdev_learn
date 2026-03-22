#pragma once
#include "arch/x86/io.h"

/* ===================== VGA CONSTANTS ===================== */

#define VGA_WIDTH   320
#define VGA_HEIGHT  200

#define VGA_GC_INDEX      0x3CE
#define VGA_GC_DATA       0x3CF
#define VGA_SEQ_INDEX     0x3C4
#define VGA_SEQ_DATA      0x3C5
#define VGA_CRTC_INDEX    0x3D4
#define VGA_CRTC_DATA     0x3D5
#define VGA_AC_INDEX      0x3C0
#define VGA_INPUT_STATUS  0x3DA
#define VGA_MISC_WRITE    0x3C2




void vga_init(void);
static inline void vga_put_pixel(uint32_t x, uint32_t y, uint8_t color) ;

void vga_clear(uint8_t color);

void draw_filled_square(int x, int y, int size, uint8_t color) ;

void draw_test_pattern(void) ;
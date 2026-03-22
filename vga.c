#include <stdint.h>
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



/* ===================== VGA HELPERS ===================== */

static void write_register(uint16_t index_port, uint16_t data_port, uint8_t index, uint8_t value) {
    outb(index_port, index);
    outb(data_port, value);
}

static uint16_t vga_get_framebuffer_segment(void) {
    outb(VGA_GC_INDEX, 6);
    uint8_t seg = inb(VGA_GC_DATA);

    seg = (seg >> 2) & 0x3;

    switch (seg) {
        case 0:
        case 1: return 0xA000;
        case 2: return 0xB000;
        case 3: return 0xB800;
    }

    return 0xA000;
}

static inline uint8_t* vga_get_framebuffer(void) {
    return (uint8_t*)(vga_get_framebuffer_segment() << 4);
}

/* ===================== VGA INIT ===================== */

void vga_init(void) {
    /* MISC */
    outb(VGA_MISC_WRITE, 0x63);

    /* SEQUENCER */
    write_register(VGA_SEQ_INDEX, VGA_SEQ_DATA, 0x00, 0x03);
    write_register(VGA_SEQ_INDEX, VGA_SEQ_DATA, 0x01, 0x01);
    write_register(VGA_SEQ_INDEX, VGA_SEQ_DATA, 0x02, 0x0F);
    write_register(VGA_SEQ_INDEX, VGA_SEQ_DATA, 0x03, 0x00);
    write_register(VGA_SEQ_INDEX, VGA_SEQ_DATA, 0x04, 0x0E);

    /* CRTC unlock */
    write_register(VGA_CRTC_INDEX, VGA_CRTC_DATA, 0x03,
        inb(VGA_CRTC_DATA) | 0x80);
    write_register(VGA_CRTC_INDEX, VGA_CRTC_DATA, 0x11,
        inb(VGA_CRTC_DATA) & ~0x80);

    /* CRTC registers */
    uint8_t crtc[] = {
        0x5F,0x4F,0x50,0x82,0x54,0x80,0xBF,0x1F,
        0x00,0x41,0x00,0x00,0x00,0x00,0x00,0x00,
        0x9C,0x0E,0x8F,0x28,0x40,0x96,0xB9,0xA3,
        0xFF
    };

    for (uint8_t i = 0; i < sizeof(crtc); i++) {
        write_register(VGA_CRTC_INDEX, VGA_CRTC_DATA, i, crtc[i]);
    }

    /* GRAPHICS CONTROLLER */
    uint8_t gc[] = {
        0x00,0x00,0x00,0x00,0x00,0x40,0x05,0x0F,0xFF
    };

    for (uint8_t i = 0; i < sizeof(gc); i++) {
        write_register(VGA_GC_INDEX, VGA_GC_DATA, i, gc[i]);
    }

    /* ATTRIBUTE CONTROLLER */
    uint8_t ac[] = {
        0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,
        0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,
        0x41,0x00,0x0F,0x00,0x00
    };

    for (uint8_t i = 0; i < sizeof(ac); i++) {
        inb(VGA_INPUT_STATUS);
        outb(VGA_AC_INDEX, i);
        outb(VGA_AC_INDEX, ac[i]);
    }

    /* enable display */
    inb(VGA_INPUT_STATUS);
    outb(VGA_AC_INDEX, 0x20);
}

/* ===================== FRAMEBUFFER ===================== */

static inline void vga_put_pixel(uint32_t x, uint32_t y, uint8_t color) {
    uint8_t* fb = vga_get_framebuffer();
    fb[y * VGA_WIDTH + x] = color;
}

void vga_clear(uint8_t color) {
    uint8_t* fb = vga_get_framebuffer();

    for (uint32_t i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) {
        fb[i] = color;
    }
}

/* ===================== DRAWING ===================== */

void draw_filled_square(int x, int y, int size, uint8_t color) {
    for (int j = 0; j < size; j++) {
        for (int i = 0; i < size; i++) {
            vga_put_pixel(x + i, y + j, color);
        }
    }
}

void draw_test_pattern(void) {
    vga_clear(0);

    for (int y = 0; y < VGA_HEIGHT; y++) {
        vga_put_pixel(y, y, 1);
        vga_put_pixel(VGA_WIDTH - y - 1, y, 2);
    }
}
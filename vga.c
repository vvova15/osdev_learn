// vga.c
#include <stdint.h>

#define VGA_MEMORY 0xA0000
#define VGA_WIDTH  320
#define VGA_HEIGHT 200
#define	peekb(S,O)		*(unsigned char *)(16uL * (S) + (O))
#define	pokeb(S,O,V)		*(unsigned char *)(16uL * (S) + (O)) = (V)
#define	pokew(S,O,V)		*(unsigned short *)(16uL * (S) + (O)) = (V)

static inline void outb(uint16_t port, uint8_t val) {
    __asm__ volatile("outb %0,%1" : : "a"(val), "Nd"(port));
}

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile("inb %1,%0" : "=a"(ret) : "Nd"(port));
    return ret;
}

// Переключение VGA в 320x200x256
void vga_init() {
    //misc
    outb(0x3C2, 0x63);
    // Sequencer
    outb(0x3C4, 0x00); outb(0x3C5, 0x03);
    outb(0x3C4, 0x01); outb(0x3C5, 0x01);
    outb(0x3C4, 0x02); outb(0x3C5, 0x0F);
    outb(0x3C4, 0x03); outb(0x3C5, 0x00);
    outb(0x3C4, 0x04); outb(0x3C5, 0x0E);

    // CRTC Controller unlock
    outb(0x3D4, 0x03); outb(0x3D5, inb(0x3D5) | 0x80);
    outb(0x3D4, 0x11); outb(0x3D5, inb(0x3D5) & ~0x80);
    /*	0x5F, 0x4F, 0x50, 0x82, 0x54, 0x80, 0xBF, 0x1F,
	0x00, 0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x9C, 0x0E, 0x8F, 0x28,	0x40, 0x96, 0xB9, 0xA3,
	0xFF,*/
    outb(0x3D4, 0); outb(0x3D5, 0x5F);
    outb(0x3D4, 1); outb(0x3D5, 0x4F);
    outb(0x3D4, 2); outb(0x3D5, 0x50 ); 
    outb(0x3D4, 3); outb(0x3D5, 0x82| 0x80);//0x03
    outb(0x3D4, 4); outb(0x3D5, 0x54);
    outb(0x3D4, 5); outb(0x3D5, 0x80);
    outb(0x3D4, 6); outb(0x3D5, 0xBF);
    outb(0x3D4, 7); outb(0x3D5, 0x1F);

    outb(0x3D4, 8); outb(0x3D5, 0x00);
    outb(0x3D4, 9); outb(0x3D5, 0x41);
    outb(0x3D4, 10); outb(0x3D5, 0);
    outb(0x3D4, 11); outb(0x3D5, 0);
    outb(0x3D4, 12); outb(0x3D5, 0);
    outb(0x3D4, 13); outb(0x3D5, 0);
    outb(0x3D4, 14); outb(0x3D5, 0);
    outb(0x3D4, 15); outb(0x3D5, 0);

    outb(0x3D4, 16); outb(0x3D5, 0x9C);  
    outb(0x3D4, 17); outb(0x3D5, 0x0E); //& ~0x80
    outb(0x3D4, 18); outb(0x3D5, 0x8F);
    outb(0x3D4, 19); outb(0x3D5, 0x28);
    outb(0x3D4, 20); outb(0x3D5, 0x40);
    outb(0x3D4, 21); outb(0x3D5, 0x96);
    outb(0x3D4, 22); outb(0x3D5, 0xB9);
    outb(0x3D4, 23); outb(0x3D5, 0xA3);
    outb(0x3D4, 24); outb(0x3D5, 0xFF);  
    // Graphics Controller
    /*	0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x05, 0x0F,
	0xFF,*/
    outb(0x3CE, 0x00); outb(0x3CF, 0x00);
    outb(0x3CE, 0x01); outb(0x3CF, 0x00);
    outb(0x3CE, 0x02); outb(0x3CF, 0x00);
    outb(0x3CE, 0x03); outb(0x3CF, 0x00);
    outb(0x3CE, 0x04); outb(0x3CF, 0x00);
    outb(0x3CE, 0x05); outb(0x3CF, 0x40);
    outb(0x3CE, 0x06); outb(0x3CF, 0x05);
    outb(0x3CE, 0x07); outb(0x3CF, 0x0F);
    outb(0x3CE, 0x08); outb(0x3CF, 0xFF);

    /*0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
	0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
	0x41, 0x00, 0x0F, 0x00,	0x00*/
    inb(0x3DA); outb(0x3C0, 0); outb(0x3C0, 0x00);
    inb(0x3DA); outb(0x3C0, 1); outb(0x3C0, 0x01); 
    inb(0x3DA); outb(0x3C0, 2); outb(0x3C0, 0x02); 
    inb(0x3DA); outb(0x3C0, 3); outb(0x3C0, 0x03); 
    inb(0x3DA); outb(0x3C0, 4); outb(0x3C0, 0x04); 
    inb(0x3DA); outb(0x3C0, 5); outb(0x3C0, 0x05); 
    inb(0x3DA); outb(0x3C0, 6); outb(0x3C0, 0x06); 
    inb(0x3DA); outb(0x3C0, 7); outb(0x3C0, 0x07); 
    inb(0x3DA); outb(0x3C0, 8); outb(0x3C0, 0x08); 
    inb(0x3DA); outb(0x3C0, 9); outb(0x3C0, 0x09); 
    inb(0x3DA); outb(0x3C0, 10); outb(0x3C0, 0x0A); 
    inb(0x3DA); outb(0x3C0, 11); outb(0x3C0, 0x0B); 
    inb(0x3DA); outb(0x3C0, 12); outb(0x3C0, 0x0C); 
    inb(0x3DA); outb(0x3C0, 13); outb(0x3C0, 0x0D);
    inb(0x3DA); outb(0x3C0, 14); outb(0x3C0, 0x0E); 
    inb(0x3DA); outb(0x3C0, 15); outb(0x3C0, 0x0F); 
    inb(0x3DA); outb(0x3C0, 16); outb(0x3C0, 0x41); 
    inb(0x3DA); outb(0x3C0, 17); outb(0x3C0, 0x0); 
    inb(0x3DA); outb(0x3C0, 18); outb(0x3C0, 0x0F); 
    inb(0x3DA); outb(0x3C0, 19); outb(0x3C0, 0x0); 
    inb(0x3DA); outb(0x3C0, 20); outb(0x3C0, 0x0);
     inb(0x3DA); outb(0x3C0, 0x20);
}

// Копирует `n` байт из src в dest
void *memcpy(void *dest, const void *src, unsigned n) {
    uint8_t *d = (uint8_t*)dest;
    const uint8_t *s = (const uint8_t*)src;
    for (unsigned i = 0; i < n; i++) {
        d[i] = s[i];
    }
    return dest;
}

#define	_vmemwr(DS,DO,S,N)	memcpy((char *)((DS) * 16 + (DO)), S, N)

/*****************************************************************************
VGA framebuffer is at A000:0000, B000:0000, or B800:0000
depending on bits in GC 6
*****************************************************************************/
static unsigned get_fb_seg(void)
{
	unsigned seg;

	outb(0x3CE, 6);
	seg = inb(0x3CF);
	seg >>= 2;
	seg &= 3;
	switch(seg)
	{
	case 0:
	case 1:
		seg = 0xA000;
		break;
	case 2:
		seg = 0xB000;
		break;
	case 3:
		seg = 0xB800;
		break;
	}
	return seg;
}
/*****************************************************************************
*****************************************************************************/
static void vmemwr(unsigned dst_off, unsigned char *src, unsigned count)
{
	_vmemwr(get_fb_seg(), dst_off, src, count);
}
/*****************************************************************************
*****************************************************************************/
static void vpokeb(unsigned off, unsigned val)
{
	pokeb(get_fb_seg(), off, val);
}
/*****************************************************************************
*****************************************************************************/
static unsigned vpeekb(unsigned off)
{
	return peekb(get_fb_seg(), off);
}


static void write_pixel8(unsigned x, unsigned y, unsigned c)
{
	unsigned wd_in_bytes;
	unsigned off;

	wd_in_bytes = VGA_WIDTH;
	off = wd_in_bytes * y + x;
	vpokeb(off, c);
}

// Рисуем квадрат
void draw_red_square(int x, int y, int size) {
    for (int j = 0; j < size; j++)
        for (int i = 0; i < size; i++)
            write_pixel8(x+i, y+j, 4); // 4 = красный
}

 void draw_x(void)
{
	unsigned x, y;

/* clear screen */
	for(y = 0; y < VGA_HEIGHT; y++)
		for(x = 0; x < VGA_WIDTH; x++)
			write_pixel8(x, y, 0);
/* draw 2-color X */
	/*for(y = 0; y < VGA_HEIGHT; y++)
	{
		write_pixel8((VGA_WIDTH - VGA_HEIGHT) / 2 + y, y, 1);
		write_pixel8((VGA_HEIGHT + VGA_WIDTH) / 2 - y, y, 2);
	}*/
}
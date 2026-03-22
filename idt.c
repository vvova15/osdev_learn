#include "arch/x86/io.h"

struct idt_entry {
    uint16_t offset_low;
    uint16_t selector;
    uint8_t zero;
    uint8_t type_attr;
    uint16_t offset_high;
} __attribute__((packed));

struct idt_ptr {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

extern void isr0();
extern void isr32();  // IRQ0 - timer
extern void isr33(); // IRQ1 - клавиатура

extern void isr0_handler();
extern void keyboard_handler();
extern void timer_handler();

struct idt_entry idt[256];
struct idt_ptr idtp;

// Настройка IDT
void idt_set_gate(int n, uint32_t handler) {
    idt[n].offset_low = handler & 0xFFFF;
    idt[n].selector = 0x08; // сегмент кода ядра
    idt[n].zero = 0;
    idt[n].type_attr = 0x8E; // прерывание, present
    idt[n].offset_high = (handler >> 16) & 0xFFFF;
}

void idt_load() {
    idtp.limit = sizeof(idt) - 1;
    idtp.base = (uint32_t)&idt;
    __asm__ volatile("lidt %0" : : "m"(idtp));
}

// Remap PIC (для корректной работы IRQ)
void pic_remap() {
    // Инициализация PIC
    outb(0x20, 0x11);
    outb(0xA0, 0x11);

    // Смещение векторов
    outb(0x21, 0x20); // Master PIC → 0x20 (32)
    outb(0xA1, 0x28); // Slave PIC → 0x28 (40)

    // Настройка связи Master → Slave
    outb(0x21, 0x04);
    outb(0xA1, 0x02);

    // Режим 8086
    outb(0x21, 0x01);
    outb(0xA1, 0x01);

    //замаскировать все IRQ
    outb(0x21, 0xff);
    outb(0xA1, 0xff);
}

// Инициализация IDT и обработчиков
void idt_init() {
    pic_remap();

    idt_set_gate(0, (uint32_t)isr0);   // Divide by zero
    idt_set_gate(32, (uint32_t)isr32); // таймер
    idt_set_gate(33, (uint32_t)isr33); // Клавиатура IRQ1

    idt_load();
}
// разрешаем IRQ1 (клавиатура)
void irq_enable(uint8_t irq_mask) {
    uint8_t current = inb(0x21); 
    current &= irq_mask;
    outb(0x21, current);
}


void kb_init() {
    irq_enable(~(1 << 1));
}

void timer_init() {
    irq_enable(~(1 << 0));
}

void pic_send_eoi(){
    outb(0x20, 0x20);
}

// Обработчик divide by zero
void isr0_handler() {
    char *video = (char*)0xB8000;
    const char *msg = "DIVIDE BY ZERO!";

    for (int i = 0; msg[i]; i++) {
        video[i*2] = msg[i];
        video[i*2 + 1] = 0x4F; // красный на сером
    }

    while (1); // останавливаем выполнение
}

// Обработчик клавиатуры
void keyboard_handler() {

    
    char *video = (char*)0xB8000;
    uint8_t scancode = inb(0x60); // читаем сканкод

    // Пишем сканкод в верхний левый угол экрана
    video[0] = 'K';
    video[1] = 0x2F;
    video[2] = '0' + (scancode % 10);
    video[3] = 0x2F;

    pic_send_eoi();

}

void timer_handler() {
    static uint32_t ticks = 0;
    ticks++;

    char *video = (char*)0xB8000;
    // Вторая строка (строка 1, смещение 80 символов)
    video[80*2 + 0] = 'T';
    video[80*2 + 1] = 0x0F;
    video[80*2 + 2] = '0' + (ticks % 10);
    video[80*2 + 3] = 0x0F;

    pic_send_eoi();
}
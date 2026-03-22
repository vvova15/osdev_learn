#pragma once
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



void idt_set_gate(int n, uint32_t handler) ;
void idt_load();


void pic_remap() ;


void idt_init();

void irq_enable(uint8_t irq_mask) ;


void kb_init() ;

void timer_init() ;
void pic_send_eoi();

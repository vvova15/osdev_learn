//#include <stdint.h>
#include "idt.h"
#include "vga.h"
#include "console.h"
#include "pci.h"


void kernel_main() {
    vga_clear_screen();
    vga_print("Hello world!");
 // рисуем красный квадрат 50x50 пикселей в центре
    //draw_red_square(135, 75, 50);
    vga_init();
    draw_test_pattern();

   // idt_init();
   // timer_init();
  //  kb_init();

   // __asm__ volatile("sti");

 //   pci_scan();

    // вызовем divide by zero
   // int a = 1 / 0;

    // Останавливаем ядро
    while (1) {
        __asm__ volatile("hlt"); // экономим CPU
    }
}
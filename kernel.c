//#include <stdint.h>
void idt_init();

void vga_init();

void pci_scan();

void kernel_main() {
    char *video = (char*)0xB8000;

    const char *msg = "Hello, world!";

    for (int i = 0; msg[i]; i++) {
        video[i*2] = msg[i];
        video[i*2 + 1] = 0x07;
    }
 // рисуем красный квадрат 50x50 пикселей в центре
    //draw_red_square(135, 75, 50);
  //  vga_init();
 //   draw_test_pattern();

   // idt_init();
  //  timer_init();
  //  kb_init();

 //   __asm__ volatile("sti");

    pci_scan();

    // вызовем divide by zero
   // int a = 1 / 0;

    // Останавливаем ядро
    while (1) {
        __asm__ volatile("hlt"); // экономим CPU
    }
}
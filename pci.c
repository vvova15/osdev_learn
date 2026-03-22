#include <stdint.h>
#include "arch/x86/io.h"

#define PCI_CONFIG_ADDRESS 0xCF8
#define PCI_CONFIG_DATA    0xCFC

#define VGA_TEXT ((uint16_t*)0xB8000)

static int cursor = 0;

int row = 0;
int col = 0;

void vga_putc(char c) {
    if (c == '\n') {
        row++;
        col = 0;
        return;
    }

    VGA_TEXT[row * 80 + col] = (0x0F << 8) | c;
    col++;

    if (col >= 80) {
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

uint32_t pci_read(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset) {
    uint32_t address =
        (1 << 31) |                // enable bit
        ((uint32_t)bus << 16) |
        ((uint32_t)device << 11) |
        ((uint32_t)function << 8) |
        (offset & 0xFC);

    outl(PCI_CONFIG_ADDRESS, address);
    return inl(PCI_CONFIG_DATA);
}

int pci_device_exists(uint8_t bus, uint8_t device, uint8_t function) {
    uint16_t vendor = pci_read(bus, device, function, 0x00) & 0xFFFF;
    return vendor != 0xFFFF;
}

void pci_scan() {
    for (uint16_t bus = 0; bus < 256; bus++) {
        for (uint8_t device = 0; device < 32; device++) {
            for (uint8_t function = 0; function < 8; function++) {

                if (!pci_device_exists(bus, device, function))
                    continue;

                uint32_t data = pci_read(bus, device, function, 0x00);
                uint16_t vendor = data & 0xFFFF;
                uint16_t device_id = (data >> 16) & 0xFFFF;

                uint32_t class_data = pci_read(bus, device, function, 0x08);
                uint8_t class_code = (class_data >> 24) & 0xFF;
                uint8_t subclass   = (class_data >> 16) & 0xFF;

                // вывод
                vga_print("PCI: ");

                vga_print_hex8(bus);
                vga_putc(':');
                vga_print_hex8(device);
                vga_putc('.');
                vga_print_hex8(function);

                vga_print(" VID=");
                vga_print_hex16(vendor);

                vga_print(" DID=");
                vga_print_hex16(device_id);

                vga_print(" CLS=");
                vga_print_hex8(class_code);

                vga_print(" SUB=");
                vga_print_hex8(subclass);

                vga_putc('\n');
            }
        }
    }
}
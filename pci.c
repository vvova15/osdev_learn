#include "pci.h"


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

void pci_foreach(pci_callback_t cb) {
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

                cb(bus, device, function, vendor, device_id, class_code, subclass);
            }
        }
    }
}

void pci_print_cb(uint16_t bus, uint8_t device,
                                uint8_t function, 
                                uint16_t vendor, 
                                uint16_t device_id, 
                                uint8_t class_code, 
                                uint8_t subclass) {
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

void pci_scan() {
    pci_foreach(pci_print_cb);
}

uint32_t pci_read_bar(uint16_t bus, uint8_t device, uint8_t function, uint8_t bar) {
    return pci_read(bus, device, function, 0x10 + bar * 4);
}
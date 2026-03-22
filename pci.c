#include "pci.h"

pci_dev_t ide_dev;
ata_channel_t channels[2];


ata_device_t ata_devices[ATA_MAX_DEVICES];
int ata_device_count = 0;


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


int find_ide_controller() {
    for (uint16_t bus = 0; bus < 256; bus++) {
        for (uint8_t device = 0; device < 32; device++) {
            for (uint8_t function = 0; function < 8; function++) {

                if (!pci_device_exists(bus, device, function))
                    continue;

                uint32_t class_data = pci_read(bus, device, function, 0x08);
                uint8_t class_code = (class_data >> 24) & 0xFF;
                uint8_t subclass   = (class_data >> 16) & 0xFF;
                
                if (class_code == 0x01 && subclass == 0x01) {
                    ide_dev = (pci_dev_t){bus, device, function};
                    return 1;
                }
            }
        }
    }
    return 0;
}

uint32_t pci_read_bar(uint8_t bar) {
    return pci_read(ide_dev.bus, ide_dev.slot, ide_dev.func, 0x10 + bar * 4);
}

void ata_init_channels() {
    uint32_t bar0 = pci_read_bar(0);
    uint32_t bar1 = pci_read_bar(1);
    uint32_t bar2 = pci_read_bar(2);
    uint32_t bar3 = pci_read_bar(3);
    uint32_t bar4 = pci_read_bar(4);

    // legacy fallback
    channels[0].io_base = (bar0 & 0xFFFFFFFC) ? (bar0 & 0xFFFC) : 0x1F0;
    channels[0].ctrl    = (bar1 & 0xFFFFFFFC) ? (bar1 & 0xFFFC) : 0x3F6;

    channels[1].io_base = (bar2 & 0xFFFFFFFC) ? (bar2 & 0xFFFC) : 0x170;
    channels[1].ctrl    = (bar3 & 0xFFFFFFFC) ? (bar3 & 0xFFFC) : 0x376;

    channels[0].bmide = (bar4 & 0xFFFC);
    channels[1].bmide = (bar4 & 0xFFFC) + 8;
}

void ata_parse_model(uint16_t* data, char* model) {
    for (int i = 0; i < 40; i += 2) {
        model[i]     = data[27 + i/2] >> 8;
        model[i + 1] = data[27 + i/2] & 0xFF;
    }
    model[40] = 0;

    // убрать пробелы в конце
    for (int i = 39; i >= 0; i--) {
        if (model[i] == ' ')
            model[i] = 0;
        else
            break;
    }
}

int ata_identify(int ch, int dev, uint16_t* out_data) {
    uint16_t io = channels[ch].io_base;

    outb(io + 6, dev ? 0xB0 : 0xA0);

    inb(io + 7); inb(io + 7); inb(io + 7); inb(io + 7);

    outb(io + 2, 0);
    outb(io + 3, 0);
    outb(io + 4, 0);
    outb(io + 5, 0);

    outb(io + 7, 0xEC);

    if (inb(io + 7) == 0)
        return 0;

    while (inb(io + 7) & 0x80);

    uint8_t status = inb(io + 7);

    if (status & 0x01)
        return 0;

    while (!(inb(io + 7) & 0x08));

    insw(io + 0, out_data, 256);

    return 1;
}

void ata_scan() {
    ata_device_count = 0;

    for (int ch = 0; ch < 2; ch++) {
        for (int dev = 0; dev < 2; dev++) {

            uint16_t data[256];

            if (ata_identify(ch, dev, data)) {

                ata_device_t* d = &ata_devices[ata_device_count];

                d->reserved = 1;
                d->channel  = ch;
                d->drive    = dev;

                d->io_base  = channels[ch].io_base;
                d->ctrl_base= channels[ch].ctrl;

                // размер
                d->size = data[60] | (data[61] << 16);

                // модель
                ata_parse_model(data, d->model);

                ata_device_count++;
            }
        }
    }
}

void ata_init() {
    if (!find_ide_controller()) {
        // нет IDE
        return;
    }

    ata_init_channels();
    ata_scan();
}

void ata_print_devices() {
    for (int i = 0; i < ata_device_count; i++) {
        ata_device_t* d = &ata_devices[i];
        vga_clear_screen();
        vga_print("ATA device "); vga_print_dec8(i); vga_print("\n");
        vga_print("  model: "); vga_print(d->model); vga_print("\n");
        vga_print("  channel: "); vga_print(d->channel ? "secondary" : "primary"); vga_print("\n");
        vga_print("  drive:   "); vga_print(d->drive ? "slave" : "master"); vga_print("\n");
        vga_print("  size:    "); vga_print_dec32(d->size); vga_print(" sectors");
    }
}
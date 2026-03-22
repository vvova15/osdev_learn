#pragma once
#include <stdint.h>
#include "arch/x86/io.h"
#include "console.h"

#define PCI_CONFIG_ADDRESS 0xCF8
#define PCI_CONFIG_DATA    0xCFC

typedef struct {
    uint8_t bus, slot, func;
} pci_dev_t;

typedef struct {
    uint16_t io_base;
    uint16_t ctrl;
    uint16_t bmide;
} ata_channel_t;


#define ATA_MAX_DEVICES 4

typedef struct {
    uint8_t  reserved;   // есть ли устройство
    uint8_t  channel;    // 0 = primary, 1 = secondary
    uint8_t  drive;      // 0 = master, 1 = slave

    uint16_t io_base;
    uint16_t ctrl_base;

    uint32_t size;       // количество секторов
    char     model[41];  // строка модели (40 + \0)

} ata_device_t;


uint32_t pci_read(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset);

int pci_device_exists(uint8_t bus, uint8_t device, uint8_t function);

void pci_scan();

void ata_init();
void ata_print_devices();
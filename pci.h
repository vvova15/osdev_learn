#pragma once
#include <stdint.h>
#include "arch/x86/io.h"
#include "console.h"

#define PCI_CONFIG_ADDRESS 0xCF8
#define PCI_CONFIG_DATA    0xCFC

typedef struct{
    uint16_t bus;
    uint8_t device;
    uint8_t function;
    uint16_t vendor;
    uint16_t device_id;
    uint8_t class_code;
    uint8_t subclass;
} pci_device;

typedef void (*pci_callback_t)(uint16_t bus, uint8_t device,
                                uint8_t function, uint16_t vendor, 
                                uint16_t device_id, uint8_t class_code, 
                                uint8_t subclass);

uint32_t pci_read(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset);
int pci_device_exists(uint8_t bus, uint8_t device, uint8_t function);
void pci_scan();
void pci_foreach(pci_callback_t cb);
uint32_t pci_read_bar(uint16_t bus, uint8_t device, uint8_t function, uint8_t bar);
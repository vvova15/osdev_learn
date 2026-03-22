#pragma once
uint32_t pci_read(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset);

int pci_device_exists(uint8_t bus, uint8_t device, uint8_t function);

void pci_scan();
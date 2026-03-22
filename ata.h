#pragma once
#include <stdint.h>
#include "arch/x86/io.h"
#include "pci.h"

#define ATA_MAX_DEVICES 4
typedef struct {
    uint16_t io_base;
    uint16_t ctrl;
    uint16_t bmide;
} ata_channel_t;

typedef struct {
    uint8_t  reserved;   // есть ли устройство
    uint8_t  channel;    // 0 = primary, 1 = secondary
    uint8_t  drive;      // 0 = master, 1 = slave

    uint16_t io_base;
    uint16_t ctrl_base;

    uint32_t size;       // количество секторов
    char     model[41];  // строка модели (40 + \0)

} ata_device_t;

void ata_init();
void ata_print_devices();
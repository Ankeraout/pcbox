#ifndef __CORE_BIOS_H__
#define __CORE_BIOS_H__

#include <stdint.h>

uint8_t bios_read8(uint32_t address);
uint16_t bios_read16(uint32_t address);
void bios_write8(uint32_t address, uint8_t value);
void bios_write16(uint32_t address, uint16_t value);

#endif

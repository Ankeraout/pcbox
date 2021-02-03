#ifndef __CORE_MMU_H__
#define __CORE_MMU_H__

#include <stdint.h>

uint8_t mmu_read8(uint32_t address);
uint16_t mmu_read16(uint32_t address);
void mmu_write8(uint32_t address, uint8_t value);
void mmu_write16(uint32_t address, uint16_t value);

#endif

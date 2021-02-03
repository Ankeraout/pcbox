#include <stdint.h>

#include "core/bios.h"

uint8_t mmu_read8(uint32_t address);
uint16_t mmu_read16(uint32_t address);
void mmu_write8(uint32_t address, uint8_t value);
void mmu_write16(uint32_t address, uint16_t value);

uint8_t mmu_read8(uint32_t address) {
    if(address >= 0xf0000) {
        return bios_read8(address);
    } else {
        return 0;
    }
}

uint16_t mmu_read16(uint32_t address) {
    if(address >= 0xf0000) {
        return bios_read16(address);
    } else {
        return 0;
    }
}

void mmu_write8(uint32_t address, uint8_t value) {
    if(address >= 0xf0000) {
        bios_write8(address, value);
    }
}

void mmu_write16(uint32_t address, uint16_t value) {
    if(address >= 0xf0000) {
        bios_write16(address, value);
    }
}

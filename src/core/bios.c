#include <stdint.h>

uint8_t bios_read8(uint32_t address);
uint16_t bios_read16(uint32_t address);
void bios_write8(uint32_t address, uint8_t value);
void bios_write16(uint32_t address, uint16_t value);

uint8_t bios_read8(uint32_t address) {
    // Fix unused parameter warnings
    (void)address;

    return 0x00;
}

uint16_t bios_read16(uint32_t address) {
    // Fix unused parameter warnings
    (void)address;
    
    return 0x0000;
}

void bios_write8(uint32_t address, uint8_t value) {
    // Fix unused parameter warnings
    (void)address;
    (void)value;
}

void bios_write16(uint32_t address, uint16_t value) {
    // Fix unused parameter warnings
    (void)address;
    (void)value;
}

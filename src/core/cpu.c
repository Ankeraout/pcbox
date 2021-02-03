#include <stdbool.h>
#include <stdint.h>

#include "core/mmu.h"

typedef enum {
    CPU_SEGMENT_PREFIX_NONE,
    CPU_SEGMENT_PREFIX_CS,
    CPU_SEGMENT_PREFIX_DS,
    CPU_SEGMENT_PREFIX_ES,
    CPU_SEGMENT_PREFIX_SS
} cpu_segment_prefix_t;

typedef struct {
    uint16_t cs;
    uint16_t ds;
    uint16_t es;
    uint16_t ss;

    union {
        uint16_t ax;

        struct {
            uint8_t al;
            uint8_t ah;
        };
    };

    union {
        uint16_t bx;

        struct {
            uint8_t bl;
            uint8_t bh;
        };
    };

    union {
        uint16_t cx;

        struct {
            uint8_t cl;
            uint8_t ch;
        };
    };

    union {
        uint16_t dx;

        struct {
            uint8_t dl;
            uint8_t dh;
        };
    };

    uint16_t sp;
    uint16_t bp;
    uint16_t ip;
    uint16_t si;
    uint16_t di;

    union {
        uint16_t value;

        struct {
            uint16_t c : 1;
            uint16_t undefined_1 : 1;
            uint16_t p : 1;
            uint16_t undefined_2 : 1;
            uint16_t a : 1;
            uint16_t undefined_3 : 1;
            uint16_t z : 1;
            uint16_t s : 1;
            uint16_t t : 1;
            uint16_t i : 1;
            uint16_t d : 1;
            uint16_t o : 1;
            uint16_t undefined_4 : 4;
        } fields;
    } flags;

    cpu_segment_prefix_t segmentPrefix;
} cpu_state_t;

typedef enum {
    CPU_MODRM_REG_AL = 0,
    CPU_MODRM_REG_AX = 0,
    CPU_MODRM_REG_CL = 1,
    CPU_MODRM_REG_CX = 1,
    CPU_MODRM_REG_DL = 2,
    CPU_MODRM_REG_DX = 2,
    CPU_MODRM_REG_BL = 3,
    CPU_MODRM_REG_BX = 3,
    CPU_MODRM_REG_AH = 4,
    CPU_MODRM_REG_SP = 4,
    CPU_MODRM_REG_CH = 5,
    CPU_MODRM_REG_BP = 5,
    CPU_MODRM_REG_DH = 6,
    CPU_MODRM_REG_SI = 6,
    CPU_MODRM_REG_BH = 7,
    CPU_MODRM_REG_DI = 7
} cpu_modrm_reg_t;

typedef enum {
    CPU_OPERAND_REGISTER,
    CPU_OPERAND_MEMORY
} cpu_operand_type_t;

typedef struct {
    cpu_operand_type_t type;

    union {
        uint32_t address;
        cpu_modrm_reg_t reg;
    };
} cpu_operand_t;

typedef struct {
    cpu_operand_t reg;
    cpu_operand_t mod;
} cpu_modrm_t;

static cpu_modrm_t cpu_modrm;
static cpu_state_t cpu_state;

void cpu_reset();
void cpu_cycle();
static uint8_t cpu_fetch8();
static uint16_t cpu_fetch16();
static void cpu_decodeModRm();
static uint8_t cpu_readOperand8(cpu_operand_t operand);
static uint8_t cpu_readOperand16(cpu_operand_t operand);
static void cpu_writeOperand8(cpu_operand_t operand, uint8_t value);
static void cpu_writeOperand16(cpu_operand_t operand, uint16_t value);

static uint8_t cpu_fetch8() {
    return mmu_read8((cpu_state.cs << 4) + cpu_state.ip);
}

static uint16_t cpu_fetch16() {
    return mmu_read16((cpu_state.cs << 4) + cpu_state.ip);
}

static uint8_t cpu_readOperand8(cpu_operand_t operand) {
    switch(operand.type) {
        case CPU_OPERAND_REGISTER:
            switch(operand.reg) {
                case CPU_MODRM_REG_AL: return cpu_state.al;
                case CPU_MODRM_REG_CL: return cpu_state.cl;
                case CPU_MODRM_REG_DL: return cpu_state.dl;
                case CPU_MODRM_REG_BL: return cpu_state.bl;
                case CPU_MODRM_REG_AH: return cpu_state.ah;
                case CPU_MODRM_REG_CH: return cpu_state.ch;
                case CPU_MODRM_REG_DH: return cpu_state.dh;
                case CPU_MODRM_REG_BH: return cpu_state.bh;
            }

            break; // Useless, only here to remove a warning

        case CPU_OPERAND_MEMORY:
            return mmu_read8(operand.address);
    }

    return 0; // Useless, only here to remove a warning
}

static uint8_t cpu_readOperand16(cpu_operand_t operand) {
    switch(operand.type) {
        case CPU_OPERAND_REGISTER:
            switch(operand.reg) {
                case CPU_MODRM_REG_AX: return cpu_state.ax;
                case CPU_MODRM_REG_CX: return cpu_state.cx;
                case CPU_MODRM_REG_DX: return cpu_state.dx;
                case CPU_MODRM_REG_BX: return cpu_state.bx;
                case CPU_MODRM_REG_SP: return cpu_state.sp;
                case CPU_MODRM_REG_BP: return cpu_state.bp;
                case CPU_MODRM_REG_SI: return cpu_state.si;
                case CPU_MODRM_REG_DI: return cpu_state.di;
            }

            break; // Useless, only here to remove a warning

        case CPU_OPERAND_MEMORY:
            return mmu_read8(operand.address);
    }

    return 0; // Useless, only here to remove a warning
}

static void cpu_writeOperand8(cpu_operand_t operand, uint8_t value) {
    switch(operand.type) {
        case CPU_OPERAND_REGISTER:
            switch(operand.reg) {
                case CPU_MODRM_REG_AL: cpu_state.al = value; break;
                case CPU_MODRM_REG_CL: cpu_state.cl = value; break;
                case CPU_MODRM_REG_DL: cpu_state.dl = value; break;
                case CPU_MODRM_REG_BL: cpu_state.bl = value; break;
                case CPU_MODRM_REG_AH: cpu_state.ah = value; break;
                case CPU_MODRM_REG_CH: cpu_state.ch = value; break;
                case CPU_MODRM_REG_DH: cpu_state.dh = value; break;
                case CPU_MODRM_REG_BH: cpu_state.bh = value; break;
            }

            break;

        case CPU_OPERAND_MEMORY:
            mmu_write8(operand.address, value);
            break;
    }
}

static void cpu_writeOperand16(cpu_operand_t operand, uint16_t value) {
    switch(operand.type) {
        case CPU_OPERAND_REGISTER:
            switch(operand.reg) {
                case CPU_MODRM_REG_AX: cpu_state.ax = value; break;
                case CPU_MODRM_REG_CX: cpu_state.cx = value; break;
                case CPU_MODRM_REG_DX: cpu_state.dx = value; break;
                case CPU_MODRM_REG_BX: cpu_state.bx = value; break;
                case CPU_MODRM_REG_SP: cpu_state.sp = value; break;
                case CPU_MODRM_REG_BP: cpu_state.bp = value; break;
                case CPU_MODRM_REG_SI: cpu_state.si = value; break;
                case CPU_MODRM_REG_DI: cpu_state.di = value; break;
            }

            break;

        case CPU_OPERAND_MEMORY:
            mmu_write16(operand.address, value);
            break;
    }
}

static void cpu_decodeModRm() {
    cpu_segment_prefix_t segmentPrefix = CPU_SEGMENT_PREFIX_DS;

    uint8_t modrm = cpu_fetch8();

    uint8_t mod = modrm >> 6;
    uint8_t reg = (modrm >> 3) & 0x07;
    uint8_t rm = modrm & 0x07;

    cpu_modrm.reg.type = CPU_OPERAND_REGISTER;
    cpu_modrm.reg.reg = reg;

    switch(mod) {
        case 0:
            cpu_modrm.mod.type = CPU_OPERAND_MEMORY;

            switch(rm) {
                case 0: cpu_modrm.mod.address = cpu_state.bx + cpu_state.si; break;
                case 1: cpu_modrm.mod.address = cpu_state.bx + cpu_state.di; break;
                case 2: cpu_modrm.mod.address = cpu_state.bp + cpu_state.si; break;
                case 3: cpu_modrm.mod.address = cpu_state.bp + cpu_state.di; break;
                case 4: cpu_modrm.mod.address = cpu_state.si; break;
                case 5: cpu_modrm.mod.address = cpu_state.di; break;
                case 6: cpu_modrm.mod.address = cpu_fetch16(); break;
                case 7: cpu_modrm.mod.address = cpu_state.bx; break;
            }

            break;

        case 1:
            cpu_modrm.mod.type = CPU_OPERAND_MEMORY;

            switch(rm) {
                case 0: cpu_modrm.mod.address = cpu_state.bx + cpu_state.si; break;
                case 1: cpu_modrm.mod.address = cpu_state.bx + cpu_state.di; break;
                case 2: cpu_modrm.mod.address = cpu_state.bp + cpu_state.si; break;
                case 3: cpu_modrm.mod.address = cpu_state.bp + cpu_state.di; break;
                case 4: cpu_modrm.mod.address = cpu_state.si; break;
                case 5: cpu_modrm.mod.address = cpu_state.di; break;
                case 6: cpu_modrm.mod.address = cpu_state.bp; segmentPrefix = CPU_SEGMENT_PREFIX_SS; break;
                case 7: cpu_modrm.mod.address = cpu_state.bx; break;
            }

            cpu_modrm.mod.address += (int16_t)(int8_t)cpu_fetch8();

            break;
        case 2:
            cpu_modrm.mod.type = CPU_OPERAND_MEMORY;

            switch(rm) {
                case 0: cpu_modrm.mod.address = cpu_state.bx + cpu_state.si; break;
                case 1: cpu_modrm.mod.address = cpu_state.bx + cpu_state.di; break;
                case 2: cpu_modrm.mod.address = cpu_state.bp + cpu_state.si; break;
                case 3: cpu_modrm.mod.address = cpu_state.bp + cpu_state.di; break;
                case 4: cpu_modrm.mod.address = cpu_state.si; break;
                case 5: cpu_modrm.mod.address = cpu_state.di; break;
                case 6: cpu_modrm.mod.address = cpu_state.bp; segmentPrefix = CPU_SEGMENT_PREFIX_SS; break;
                case 7: cpu_modrm.mod.address = cpu_state.bx; break;
            }

            cpu_modrm.mod.address += cpu_fetch16();

            break;

        case 3:
            cpu_modrm.mod.type = CPU_OPERAND_REGISTER;
            cpu_modrm.mod.reg = rm;
            break;
    }

    if(cpu_modrm.mod.type == CPU_OPERAND_MEMORY) {
        if(cpu_state.segmentPrefix != CPU_SEGMENT_PREFIX_NONE) {
            segmentPrefix = cpu_state.segmentPrefix;
        }
        
        switch(segmentPrefix) {
            case CPU_SEGMENT_PREFIX_CS: cpu_modrm.mod.address += cpu_state.cs << 4; break;
            case CPU_SEGMENT_PREFIX_DS: cpu_modrm.mod.address += cpu_state.ds << 4; break;
            case CPU_SEGMENT_PREFIX_ES: cpu_modrm.mod.address += cpu_state.es << 4; break;
            case CPU_SEGMENT_PREFIX_SS: cpu_modrm.mod.address += cpu_state.ss << 4; break;
            default: break; // Useless, only here to remove a warning
        }
    }
}

void cpu_reset() {
    cpu_state.cs = 0xffff;
    cpu_state.ds = 0x0000;
    cpu_state.es = 0x0000;
    cpu_state.ss = 0x0000;
    cpu_state.ip = 0x0000;
    cpu_state.flags.value = 0x0000;
}

void cpu_cycle() {
    bool isSegmentPrefix = false;

    switch(cpu_fetch8()) {
        case 0x62: // ES:
            cpu_state.segmentPrefix = CPU_SEGMENT_PREFIX_ES;
            isSegmentPrefix = true;
            break;

        case 0x63: // SS:
            cpu_state.segmentPrefix = CPU_SEGMENT_PREFIX_SS;
            isSegmentPrefix = true;
            break;

        case 0xe2: // CS:
            cpu_state.segmentPrefix = CPU_SEGMENT_PREFIX_CS;
            isSegmentPrefix = true;
            break;

        case 0xe3: // DS:
            cpu_state.segmentPrefix = CPU_SEGMENT_PREFIX_DS;
            isSegmentPrefix = true;
            break;

        default: // UND
            break;
    }

    if(!isSegmentPrefix) {
        cpu_state.segmentPrefix = CPU_SEGMENT_PREFIX_NONE;
    }
}

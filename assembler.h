#ifndef ASSEMBLER_H
#define ASSEMBLER_H
#include "instruction.h"

enum operand_type{
    NO_OPERAND,
    S_REG,
    D_REG,
    REG_REG,
    D_IMM,
    S_IMM,
    IMM
};

struct Instruction_data{
    enum Opcode code;
    uint8_t length;
    enum operand_type optype; // 0 for no operand, 1 for one-reg, 2 for reg-reg, 3 for reg-imm, 4 for only imm
    uint8_t operand_cnt;
};

#endif
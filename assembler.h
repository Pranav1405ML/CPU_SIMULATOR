#ifndef ASSEMBLER_H
#define ASSEMBLER_H
#include "instruction.h"

struct Instruction_data{
    enum Opcode code;
    uint8_t length;
    uint8_t operand_type; // 0 for no operand, 1 for one-reg, 2 for reg-reg, 3 for reg-imm, 4 for only imm
};

struct Mnemonic_table{
    uint8_t opcode_no;
}

#endif
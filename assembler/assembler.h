#ifndef ASSEMBLER_H
#define ASSEMBLER_H
#include "../instruction/instruction.h"
#include <stdio.h>

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

int assemble_file(const char *in_filename, const char *out_hex_filename, uint8_t *program_buffer, int max_size);

#endif

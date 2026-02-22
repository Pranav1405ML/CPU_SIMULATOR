#ifndef INSTRUCTION_H
#define INSTRUCTION_H
#include <stdbool.h>

enum Register{
    R0,
    R1,
    R2,
    R3
};

enum Opcode{
    JUMP,
    HALT,
    MOV_IMM,
    NOP,
    MOV_REG,
    ADD,
    JC,
    JZ,
    LOAD,
    STORE,
    OPCODE_COUNT
};

struct Instruction_metadata{
    uint8_t length;
    bool has_immediate;
};

struct Decoded_instruction{
    enum Opcode opcode;
    enum Register dest_reg;
    enum Register source_reg;
    uint8_t immediate_value;
    uint8_t instr_length;
};

#endif

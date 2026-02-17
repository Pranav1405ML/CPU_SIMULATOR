#ifndef INSTRUCTION_H
#define INSTRUCTION_H

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
    STORE
};

#endif

#ifndef INSTRUCTION_H
#define INSTRUCTION_H

enum Register{
    R0,
    R1,
    R2,
    R3
};

enum Opcode{
    NOP,
    HALT,
    JUMP,
    MOV_IMM,
    MOV_REG,
    ADD,
    JC,
    JZ,
    LOAD,
    STORE
};

struct Instruction{
    enum Opcode opcode;  
    enum Register destination_operand;    
    enum Register source_operand;
    uint8_t data;
};

#endif

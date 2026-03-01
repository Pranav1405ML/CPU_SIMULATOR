# Byte-Encoded 8-Bit CPU — Instruction Set Architecture (ISA)

# Overview
  1. 8-bit CPU simulator
  2. Byte-encoded instructions
  3. Variable-length instruction model
  4. 4 general purpose registers
  5. Memory [256]


# Instruction Encoding Format
Bit  7 6 5 4  | 3 2  | 1 0
    ---------- ------ ------
    opcode      dest    src
    (4 bits)   (2 bits) (2 bits)

1. Opcode occupies upper 4 bits.
2. Destination register next 2 bits.
3. Source register lower 2 bits.


# Two-Byte Instructions
Byte 1 → Encoded instruction
Byte 2 → 8-bit immediate value
+ Immediate is interpreted either as literal data or absolute memory address depending on instruction.


# Register Model
1. 4 general-purpose registers: R0–R3
2. Each register is 8-bit unsigned
3. Encoded using 2 bits
_______________________
| Register | Encoding |
|----------|----------|
| R0       | 00       |
| R1       | 01       |
| R2       | 10       |
| R3       | 11       |
|__________|__________|


# Memory Model
1. 256 bytes total memory
2. Address range: 0–255
3. Byte-addressable
4. Unified memory (instructions and data share same space)
5. No segmentation or protection


# Flag Model
1. Zero Flag (Z)
A. Set when ALU result equals 0
B. Modified only by ADD instruction

2. Carry Flag (C)
A. Set when carry leaves most significant bit (bit 7)
B. Modified only by ADD instruction

+ Other instructions do not modify flags.


# Program Counter (PC) Rules
1. PC counts bytes
2. PC incremented by instruction length during ADVANCE stage
3. JUMP, JZ, JC override PC directly
4. When PC is overridden, ADVANCE is suppressed
5. If PC ≥ program size, machine halts


# Execution Model
Pipeline stages:
1. FETCH: Read instruction byte
2. DECODE: Extract fields + metadata
3. EXECUTE: Mutate machine state
4. ADVANCE: Increment PC


# Opcode Table
Mnemonic	Opcode(numeric)	Length	Immediate	Flags Affected    Description

NOP         3               1       0           0                 Does nothing
HALT        1               1       0           0                 Halts the CPU
MOV_IMM     2               2       1           0                 Move a value to a register 
MOV_REG     4               1       0           0                 Move a value from one register to another
ADD         5               1       0           2 (zero & carry)  Add values stored in two registers and store result in destination register along with flag updation
JUMP        0               2       1           0                 Jump to the specified byte address 
JZ          6               2       1           0                 Jump if zero flag is activated 
JC          7               2       1           0                 Jump if carry flag is activated
LOAD        8               2       1           0                 Load a value into a register from memory 
STORE       9               2       1           0                 Store a value from a register into the memory 

# Opcode numeric value is defined by enum ordering and must not be reordered without updating encoding.


# Error Handling Rules
1. Invalid opcode → halt
2. Missing immediate byte → halt
3. PC out of bounds → halt
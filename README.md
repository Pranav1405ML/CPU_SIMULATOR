# Tiny CPU & Assembler Project

Welcome to the **Tiny CPU & Assembler** project! This repository contains a custom 8-bit CPU simulator and its accompanying assembler, entirely written in C. It is designed to be an educational tool for understanding processor architecture, instruction pipelines, and assembly language translation.

---

## 🌟 Key Features

- **8-Bit Processor Simulation:**
  - 4 general-purpose 8-bit registers (`R0` to `R3`).
  - 256 bytes of unified RAM.
  - Zero (`Z`) and Carry (`C`) flags for conditional branching.
  - Downward-growing stack starting at memory address `255`.
- **4-Stage Cycle Pipeline:**
  - Accurately models the `FETCH` $\rightarrow$ `DECODE` $\rightarrow$ `EXECUTE` $\rightarrow$ `ADVANCE` instruction lifecycle.
- **Custom Assembler:**
  - Translates mnemonic code into 8-bit binary machine code.
  - Case-insensitive mnemonics and comment support (`;`).
- **Dynamic Binary Execution:**
  - The CPU runner natively assembles, loads, and executes the default assembly file in one seamless step, outputting binary to `output.txt`.

---

## 📂 Project Structure

- **`main.c`**: Entry point; orchestrates assembly of `programs/program.asm`, loading, and CPU execution.
- **`cpu/`**: Contains the CPU clock cycle engine (`cpu.c`, `machine.h`) and ALU (`alu.c`).
- **`assembler/`**: The assembler implementation for parsing and encoding instructions.
- **`instruction/`**: Opcode definitions, register enumerations, and structures.
- **`docs/`**: Documentation including the Instruction Set Architecture (`ISA.md`) and assembly syntax guides.
- **`programs/`**: Contains the assembly programs (currently `program.asm` is executed by default).

---

## 🛠 Instruction Set Architecture (ISA)

### Encoding Format
Each instruction uses a **1-byte header**, followed by an optional **1-byte immediate value** or address.

```text
Bit:   7  6  5  4 |  3  2 |  1  0
      [  Opcode   | Dest  |  Src  ]
```
- **Opcode** (Bits 7-4): The 4-bit identifier for the instruction.
- **Destination** (Bits 3-2): Register code (`00`=R0, `01`=R1, `10`=R2, `11`=R3).
- **Source** (Bits 1-0): Register code (same as Destination).

### Available Instructions
A quick reference of the supported instructions. For a deep dive, check out the full ISA reference in the docs.

| Mnemonic | Opcode (Hex) | Format | Description |
|---|---|---|---|
| `JUMP` | `0x0` | `JUMP addr` | Unconditional jump. |
| `HALT` | `0x1` | `HALT` | Stop execution. |
| `MOV_IMM`| `0x2` | `MOV_IMM dest, imm`| Load 8-bit immediate. |
| `NOP` | `0x3` | `NOP` | No operation. |
| `MOV_REG`| `0x4` | `MOV_REG dest, src`| Copy register to register. |
| `ADD` | `0x5` | `ADD dest, src` | Add and update flags. |
| `JC` | `0x6` | `JC addr` | Jump if Carry (C) set. |
| `JZ` | `0x7` | `JZ addr` | Jump if Zero (Z) set. |
| `LOAD` | `0x8` | `LOAD dest, addr` | Load from RAM. |
| `STORE` | `0x9` | `STORE src, addr` | Store to RAM. |
| `PUSH` | `0xA` | `PUSH src` | Push to stack. |
| `POP` | `0xB` | `POP dest` | Pop from stack. |
| `CALL` | `0xC` | `CALL addr` | Subroutine call. |
| `RET` | `0xD` | `RET` | Subroutine return. |

---

## 🚀 Getting Started & Execution Flow

The CPU simulation does **not** currently accept command-line arguments. Instead, it is hardcoded to read and assemble `programs/program.asm`.

### 1. Write your Assembly Code
Edit the file at `programs/program.asm` with your desired 8-bit instructions.

### 2. Build and Run
You can easily compile and run the project using the provided `makefile`. From the root directory, run:

```bash
mingw32-make run
# Or simply 'make run' on Unix-like systems
```

This command will:
1. Compile the C source files into an executable named `cpu`.
2. Automatically run the executable.
3. The executable will read `programs/program.asm`, assemble it, output the binary hex to `output.txt`, and execute the instructions step-by-step.

*(To only build the project without running, you can use `mingw32-make` or `make`.)*

---

## 💻 Example: Square Calculation (`program.asm`)

The default `program.asm` is configured to calculate the square of a number $N$ ($N^2$). Here is how it calculates $15^2 = 225$:

```assembly
; Byte 0 (len 2): Load N = 15 into R0
MOV_IMM R0, 15

; Byte 2 (len 2): Initialize result accumulator R1 = 0
MOV_IMM R1, 0

; Byte 4 (len 1): Copy loop counter N into R2
MOV_REG R2, R0

; Byte 5 (len 2): Load -1 (255 in 2's complement) into R3 for decrementing
MOV_IMM R3, 255

; --- LOOP START (Byte Address 7) ---
; Byte 7 (len 2): If loop counter R2 == 0 (Z flag active), jump to STORE at Byte 13
JZ 13

; Byte 9 (len 1): Accumulate: R1 = R1 + N (R0)
ADD R1, R0

; Byte 10 (len 1): Decrement loop counter (updates Z flag when R2 reaches 0)
ADD R2, R3

; Byte 11 (len 2): Jump back to loop condition check at Byte 7
JUMP 7
; --- LOOP END ---

; Byte 13 (len 2): Store squared result (225) into memory address 50
STORE R1, 50

; Byte 15 (len 1): Halt CPU execution
HALT
```

**Expected Output on Execution:**
```text
...
=== CPU EXECUTION HALTED ===

====================================================
FINAL MACHINE STATE:
----------------------------------------------------
PC: 15  | SP: 255 | Zero Flag (Z): 1 | Carry Flag (C): 1
Registers: R0 = 15, R1 = 225, R2 = 0, R3 = 255
Memory[50] = 225 (Result storage)
====================================================
```

Enjoy exploring the fundamentals of computer architecture with Tiny CPU!

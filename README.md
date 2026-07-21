# Tiny CPU & Assembler Project

An 8-bit byte-encoded CPU simulator and custom assembler written in C.

---

## Features
- **8-Bit Processor Simulation**: Features 4 general-purpose 8-bit registers (`R0`–`R3`), 256 bytes of unified RAM, Zero (`Z`) & Carry (`C`) flags, and a downward-growing stack starting at memory address `255`.
- **4-Stage Cycle Pipeline**: `FETCH` $\rightarrow$ `DECODE` $\rightarrow$ `EXECUTE` $\rightarrow$ `ADVANCE`.
- **Custom 2-Pass/1-Pass Assembler**: Translates assembly mnemonic code into 8-bit binary hex output. Supports comments (`;` or `#`), case-insensitive mnemonics, and custom labels/address offsets.
- **Dynamic Binary Loader**: The CPU directly loads and executes assembled hex output files (`output.txt`).

---

## Project Structure
| File | Description |
|---|---|
| [instruction.h](file:///C:/Users/ganes/OneDrive/Desktop/Tiny_CPU/instruction.h) | Opcode definitions, register enums, and instruction structures |
| [assembler.h](file:///C:/Users/ganes/OneDrive/Desktop/Tiny_CPU/assembler.h) | Assembler operand types and instruction metadata layout |
| [assembler.c](file:///C:/Users/ganes/OneDrive/Desktop/Tiny_CPU/assembler.c) | Assembler implementation (parses assembly & encodes bytes) |
| [machine.h](file:///C:/Users/ganes/OneDrive/Desktop/Tiny_CPU/machine.h) | CPU state structure, stage enum, and function declarations |
| [alu.c](file:///C:/Users/ganes/OneDrive/Desktop/Tiny_CPU/alu.c) | Bitwise 8-bit ALU adder with Carry & Zero flag generation |
| [cpu.c](file:///C:/Users/ganes/OneDrive/Desktop/Tiny_CPU/cpu.c) | CPU clock cycle engine (`FETCH`, `DECODE`, `EXECUTE`, `ADVANCE`) |
| [main.c](file:///C:/Users/ganes/OneDrive/Desktop/Tiny_CPU/main.c) | Main entry point; loads binary machine code from hex file and runs CPU |
| [ISA.md](file:///C:/Users/ganes/OneDrive/Desktop/Tiny_CPU/ISA.md) | Instruction Set Architecture reference specification |
| [assembly_syntax.md](file:///C:/Users/ganes/OneDrive/Desktop/Tiny_CPU/assembly_syntax.md) | Quick assembly language reference guide |
| [square.asm](file:///C:/Users/ganes/OneDrive/Desktop/Tiny_CPU/square.asm) | Assembly program that computes $N^2$ (e.g. $5^2 = 25$) |

---

## Instruction Set Architecture (ISA)

### Instruction Encoding Format
Each instruction consists of a **1-byte header**, optionally followed by a **1-byte immediate value**:

```
Bit:  7 6 5 4 | 3 2 | 1 0
     [ Opcode  | Dest| Src ]
```
- **Opcode** (Bits 7..4): 4-bit instruction opcode.
- **Destination Register** (Bits 3..2): 2-bit code (`00`=R0, `01`=R1, `10`=R2, `11`=R3).
- **Source Register** (Bits 1..0): 2-bit code (`00`=R0, `01`=R1, `10`=R2, `11`=R3).

### Opcode Summary Table
| Mnemonic | Opcode (Dec) | Opcode (Hex) | Length (Bytes) | Operand Format | Description |
|---|---|---|---|---|---|
| `JUMP` | 0 | `0x0` | 2 | `JUMP addr` | Unconditional jump to byte address |
| `HALT` | 1 | `0x1` | 1 | `HALT` | Stop machine execution |
| `MOV_IMM` | 2 | `0x2` | 2 | `MOV_IMM dest, imm` | Load 8-bit immediate into register |
| `NOP` | 3 | `0x3` | 1 | `NOP` | No operation |
| `MOV_REG` | 4 | `0x4` | 1 | `MOV_REG dest, src` | Copy source register to dest register |
| `ADD` | 5 | `0x5` | 1 | `ADD dest, src` | Add src to dest; updates Z & C flags |
| `JC` | 6 | `0x6` | 2 | `JC addr` | Jump if Carry flag (C) is set |
| `JZ` | 7 | `0x7` | 2 | `JZ addr` | Jump if Zero flag (Z) is set |
| `LOAD` | 8 | `0x8` | 2 | `LOAD dest, addr` | Load byte from RAM address into dest |
| `STORE` | 9 | `0x9` | 2 | `STORE src, addr` | Store byte from src into RAM address |
| `PUSH` | 10 | `0xA` | 1 | `PUSH src` | Push src register onto stack (`SP--`) |
| `POP` | 11 | `0xB` | 1 | `POP dest` | Pop from stack into dest register (`SP++`) |
| `CALL` | 12 | `0xC` | 2 | `CALL addr` | Push next PC onto stack and jump to address |
| `RET` | 13 | `0xD` | 1 | `RET` | Pop return address from stack into PC |

---

## Build & Execution Instructions

### 1. Build Unified Executable
Compile the integrated CPU runner using GCC:
```bash
gcc main.c assembler.c cpu.c alu.c -o tiny_cpu.exe
```

### 2. Single-Step Assembly & Execution Workflow
Pass any `.asm` file directly to `tiny_cpu.exe` (or use `run.bat`):

```bash
# Run square.asm
.\tiny_cpu.exe square.asm

# Or using the helper script
.\run.bat square.asm
```

This single command will:
1. Parse and assemble your `.asm` file into machine code in memory.
2. Save the machine code bytes to `output.txt` for inspection.
3. Automatically load the binary bytes into the 256-byte CPU memory.
4. Execute the CPU clock cycle loop until `HALT` is reached.
5. Print the step-by-step trace and final CPU state (registers, memory, flags).


---

## Example: Square Calculation (`square.asm`)
Below is an example assembly program that calculates the square of a number ($N^2$):

```assembly
; Load N = 5 into R0
MOV_IMM R0, 5

; Initialize accumulator R1 = 0
MOV_IMM R1, 0

; Copy loop counter N into R2
MOV_REG R2, R0

; Load -1 (255 in 2's complement) into R3
MOV_IMM R3, 255

; Loop Check (Byte 7): Jump to STORE (Byte 13) when R2 == 0
JZ 13

; R1 = R1 + N
ADD R1, R0

; Decrement loop counter R2 (updates Z flag when R2 reaches 0)
ADD R2, R3

; Jump back to loop start
JUMP 7

; Store result (25) into memory address 50
STORE R1, 50

; Halt execution
HALT
```

**Output on execution:**
```
=== CPU EXECUTION HALTED ===
--- Final Machine State ---
PC: 15 | SP: 255 | Z: 1 | C: 1
Registers: R0=5, R1=25, R2=0, R3=255
```

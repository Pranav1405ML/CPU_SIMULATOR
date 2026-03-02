#include <stdio.h>
#include "machine.h"

static const struct Instruction_metadata ISA[OPCODE_COUNT] = {
    [JUMP]    = {2, true},
    [HALT]    = {1, false},
    [MOV_IMM] = {2, true},
    [NOP]     = {1, false},
    [MOV_REG] = {1, false},
    [ADD]     = {1, false},
    [JC]      = {2, true},
    [JZ]      = {2, true},
    [LOAD]    = {2, true},
    [STORE]   = {2, true}
};

void step(uint8_t *program, struct Machine *machine, int num_of_instructions){
      struct Decoded_instruction decoded = {0};      
      int PC = machine -> program_counter;
      bool advance = true;

      if(machine -> program_counter < num_of_instructions){
            uint8_t fetched_instr_byte = FETCH(PC, num_of_instructions, program);
            decoded = DECODE(PC, program, fetched_instr_byte, num_of_instructions);
            EXECUTE(&decoded, machine, program, &advance);
            if(machine -> is_running && advance){
              ADVANCE(decoded.instr_length, machine);
            }
            // printf("Memory[20]: %d, Memory[21]: %d\n", machine -> memory[20], machine -> memory[21]);
        }
      else{
        machine -> is_running = false;
        printf("Program counter surpassed the program size\n");
      }
}

uint8_t FETCH(int PC, int num_of_instructions, uint8_t *program){
    uint8_t fetched_instr_byte = program[PC];
    return fetched_instr_byte;
}

struct Decoded_instruction DECODE(int PC, uint8_t *program, uint8_t fetched_instr_byte, int num_of_instructions){
    struct Decoded_instruction temp_decoded = {0};
    temp_decoded.opcode = (enum Opcode)((fetched_instr_byte >> 4) & 0x0F);
    temp_decoded.dest_reg = (enum Register)((fetched_instr_byte >> 2) & 0x03);
    temp_decoded.source_reg = (enum Register)(fetched_instr_byte & 0x03);
    temp_decoded.instr_length = ISA[temp_decoded.opcode].length;

    if(temp_decoded.opcode >= OPCODE_COUNT){
            printf("Invalid opcode at PC %d\n", PC);
            temp_decoded.opcode = HALT;
    }
    else if(ISA[temp_decoded.opcode].has_immediate){
        if(PC + 1 < num_of_instructions)
           temp_decoded.immediate_value = program[PC + 1];

        else{
           printf("Program counter surpassed the program size\n");
            temp_decoded.opcode = HALT;
        }
    }
    return temp_decoded;
}

void EXECUTE(struct Decoded_instruction *decoded, struct Machine *machine, uint8_t *program, bool *advance){
    struct ALU_result post_add;
    switch(decoded -> opcode){
        case NOP: 
        printf("PC = %d | Executing: NOP | R0=%d R1=%d R2=%d R3=%d | Z=%d | C=%d\n", machine->program_counter, machine -> registers[0], machine -> registers[1], machine -> registers[2], machine -> registers[3], machine -> zero_flag, machine -> carry_flag);
        break;

        case HALT:
        printf("PC = %d | Executing: HALT | R0=%d R1=%d R2=%d R3=%d | Z=%d | C=%d\n", machine->program_counter, machine -> registers[0], machine -> registers[1], machine -> registers[2], machine -> registers[3], machine -> zero_flag, machine -> carry_flag);
        machine -> is_running = false;
        break;

        case JUMP:
        printf("PC = %d | Executing: JUMP %d | R0=%d R1=%d R2=%d R3=%d | Z=%d | C=%d\n", machine->program_counter, decoded -> immediate_value, machine -> registers[0], machine -> registers[1], machine -> registers[2], machine -> registers[3], machine -> zero_flag, machine -> carry_flag);
        machine -> program_counter = decoded -> immediate_value;
        *advance = false;
        break;

        case MOV_IMM:
        machine -> Bus = decoded -> immediate_value;
        machine -> registers[decoded -> dest_reg] = machine -> Bus;
        printf("PC = %d | Executing: MOV_IMM R%d, %d | R0=%d R1=%d R2=%d R3=%d | Z=%d | C=%d\n", machine->program_counter, decoded ->dest_reg, decoded -> immediate_value, machine -> registers[0], machine -> registers[1], machine -> registers[2], machine -> registers[3], machine -> zero_flag, machine -> carry_flag);
        break;

        case MOV_REG:
        machine -> Bus = machine -> registers[decoded -> source_reg];
        machine -> registers[decoded -> dest_reg] = machine -> Bus;
        printf("PC = %d | Executing: MOV_REG R%d, R%d | R0=%d R1=%d R2=%d R3=%d | Z=%d | C=%d\n", machine->program_counter, decoded -> dest_reg, decoded -> source_reg ,  machine -> registers[0], machine -> registers[1], machine -> registers[2], machine -> registers[3], machine -> zero_flag, machine -> carry_flag);
        break;

        case ADD:
        post_add = add_8bit(machine -> registers[decoded -> dest_reg], machine -> registers[decoded -> source_reg]);
        machine -> Bus = post_add.result;
        machine -> registers[decoded -> dest_reg] = machine -> Bus;

        machine -> zero_flag = post_add.Z_flag;
        machine -> carry_flag = post_add.C_flag;
        printf("PC = %d | Executing: ADD R%d, R%d | R0=%d R1=%d R2=%d R3=%d | Z=%d | C=%d\n", machine->program_counter, decoded -> dest_reg, decoded -> source_reg ,  machine -> registers[0], machine -> registers[1], machine -> registers[2], machine -> registers[3], machine -> zero_flag, machine -> carry_flag);
        break;

        case JZ:
        printf("PC = %d | Executing: JZ %d | R0=%d R1=%d R2=%d R3=%d | Z=%d | C=%d\n", machine->program_counter, decoded -> immediate_value, machine -> registers[0], machine -> registers[1], machine -> registers[2], machine -> registers[3], machine -> zero_flag, machine -> carry_flag);
        if(machine -> zero_flag == true){
            machine -> program_counter = decoded -> immediate_value;
            *advance = false;
        }
        break;

        case JC:
        printf("PC = %d | Executing: JC %d | R0=%d R1=%d R2=%d R3=%d | Z=%d | C=%d\n", machine->program_counter, decoded -> immediate_value, machine -> registers[0], machine -> registers[1], machine -> registers[2], machine -> registers[3], machine -> zero_flag, machine -> carry_flag);
        if(machine -> carry_flag == true){
            machine -> program_counter = decoded -> immediate_value;
            *advance = false;
        }
        break;

        case LOAD:
        printf("PC = %d | Executing: LOAD R%d, %d | R0=%d R1=%d R2=%d R3=%d | Z=%d | C=%d\n", machine->program_counter, decoded -> dest_reg, decoded -> immediate_value, machine -> registers[0], machine -> registers[1], machine -> registers[2], machine -> registers[3], machine -> zero_flag, machine -> carry_flag);
        machine -> Bus = machine -> memory[decoded -> immediate_value];
        machine -> registers[decoded -> dest_reg] = machine -> Bus;

        break;

        case STORE:
        printf("PC = %d | Executing: STORE R%d, %d | R0=%d R1=%d R2=%d R3=%d | Z=%d | C=%d\n", machine->program_counter, decoded -> source_reg, decoded -> immediate_value, machine -> registers[0], machine -> registers[1], machine -> registers[2], machine -> registers[3], machine -> zero_flag, machine -> carry_flag);
        machine -> Bus = machine -> registers[decoded -> source_reg];
        machine -> memory[decoded -> immediate_value] = machine -> Bus;

        break;
    }
}

void ADVANCE(uint8_t instr_length, struct Machine *machine){
        machine -> program_counter += instr_length;
}


// gcc main.c alu.c cpu.c -o cpu
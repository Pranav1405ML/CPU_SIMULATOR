#include <stdio.h>
#include "machine.h"

void step(uint8_t *program, struct Machine *machine, int num_of_instructions){
    uint8_t fetched_instr_byte;

    struct ALU_result post_add;

    enum Opcode opcode;
    enum Register dest_reg, source_reg;
    uint8_t immediate_value; 

    int instr_length = 1;

    if(machine -> program_counter < num_of_instructions){
        fetched_instr_byte = program[machine -> program_counter];
        opcode = (enum Opcode)((fetched_instr_byte >> 4) & 0x0F);
        dest_reg = (enum Register)((fetched_instr_byte >> 2) & 0x03);
        source_reg = (enum Register)(fetched_instr_byte & 0x03);
    }
    // This is our decode 
    else{
        opcode = HALT;
        machine -> is_running = false;
    } 

    switch(opcode){
        // case NOP: 
        // printf("PC = %d | Executing: NOP | R0=%d R1=%d R2=%d R3=%d | Z=%d | C=%d\n", machine->program_counter, machine -> registers[0], machine -> registers[1], machine -> registers[2], machine -> registers[3], machine -> zero_flag, machine -> carry_flag);
        // break;

        case HALT:
        printf("PC = %d | Executing: HALT | R0=%d R1=%d R2=%d R3=%d | Z=%d | C=%d\n", machine->program_counter, machine -> registers[0], machine -> registers[1], machine -> registers[2], machine -> registers[3], machine -> zero_flag, machine -> carry_flag);
        machine -> is_running = false;
        break;

        // case JUMP:
        // printf("PC = %d | Executing: JUMP %d | R0=%d R1=%d R2=%d R3=%d | Z=%d | C=%d\n", machine->program_counter, fetched_instruction.data, machine -> registers[0], machine -> registers[1], machine -> registers[2], machine -> registers[3], machine -> zero_flag, machine -> carry_flag);
        // machine -> program_counter = fetched_instruction.data;
        // should_advance_pc = false;
        // break;

        case MOV_IMM:
        immediate_value = program[(machine -> program_counter) + 1];
        machine -> registers[dest_reg] = immediate_value;
        instr_length = 2;
        printf("PC = %d | Executing: MOV_IMM R%d, %d | R0=%d R1=%d R2=%d R3=%d | Z=%d | C=%d\n", machine->program_counter, dest_reg, immediate_value, machine -> registers[0], machine -> registers[1], machine -> registers[2], machine -> registers[3], machine -> zero_flag, machine -> carry_flag);
        break;

        // case MOV_REG:
        // machine -> registers[fetched_instruction.destination_operand] = machine -> registers[fetched_instruction.source_operand];
        // printf("PC = %d | Executing: MOV_REG R%d, R%d | R0=%d R1=%d R2=%d R3=%d | Z=%d | C=%d\n", machine->program_counter, fetched_instruction.destination_operand, fetched_instruction.source_operand ,  machine -> registers[0], machine -> registers[1], machine -> registers[2], machine -> registers[3], machine -> zero_flag, machine -> carry_flag);
        // break;

        case ADD:
        post_add = add_8bit(machine -> registers[dest_reg], machine -> registers[source_reg]);
        machine -> registers[dest_reg] = post_add.result;
        machine -> zero_flag = post_add.Z_flag;
        machine -> carry_flag = post_add.C_flag;
        printf("PC = %d | Executing: ADD R%d, R%d | R0=%d R1=%d R2=%d R3=%d | Z=%d | C=%d\n", machine->program_counter, dest_reg, source_reg ,  machine -> registers[0], machine -> registers[1], machine -> registers[2], machine -> registers[3], machine -> zero_flag, machine -> carry_flag);
        break;

    //     case JZ:
    //     printf("PC = %d | Executing: JZ %d | R0=%d R1=%d R2=%d R3=%d | Z=%d | C=%d\n", machine->program_counter, fetched_instruction.data, machine -> registers[0], machine -> registers[1], machine -> registers[2], machine -> registers[3], machine -> zero_flag, machine -> carry_flag);
    //     if(machine -> zero_flag == true){
    //         machine -> program_counter = fetched_instruction.data;
    //         should_advance_pc = false;
    //     }
    //     break;

    //     case JC:
    //     printf("PC = %d | Executing: JC %d | R0=%d R1=%d R2=%d R3=%d | Z=%d | C=%d\n", machine->program_counter, fetched_instruction.data, machine -> registers[0], machine -> registers[1], machine -> registers[2], machine -> registers[3], machine -> zero_flag, machine -> carry_flag);
    //     if(machine -> carry_flag == true){
    //         machine -> program_counter = fetched_instruction.data;
    //         should_advance_pc = false;
    //     }
    //     break;

    //     case LOAD:
    //     printf("PC = %d | Executing: LOAD R%d, %d | R0=%d R1=%d R2=%d R3=%d | Z=%d | C=%d\n", machine->program_counter, fetched_instruction.destination_operand, fetched_instruction.data, machine -> registers[0], machine -> registers[1], machine -> registers[2], machine -> registers[3], machine -> zero_flag, machine -> carry_flag);
    //     machine -> registers[fetched_instruction.destination_operand] = machine -> memory[fetched_instruction.data];
    //     break;

    //     case STORE:
    //     printf("PC = %d | Executing: STORE R%d, %d | R0=%d R1=%d R2=%d R3=%d | Z=%d | C=%d\n", machine->program_counter, fetched_instruction.source_operand, fetched_instruction.data, machine -> registers[0], machine -> registers[1], machine -> registers[2], machine -> registers[3], machine -> zero_flag, machine -> carry_flag);
    //     machine -> memory[fetched_instruction.data] = machine -> registers[fetched_instruction.source_operand];
    //     break;
    // }
    }
    if(machine -> is_running)
        machine -> program_counter += instr_length;
    // THIS IS OUR FUTURE ADVANCE()
 
}

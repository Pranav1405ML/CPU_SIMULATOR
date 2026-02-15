#include <stdio.h>
#include "machine.h"

void step(struct Instruction *program, struct Machine *machine, int num_of_instructions){
    struct Instruction fetched_instruction;
    struct ALU_result post_add;
    bool should_advance_pc = true;

    if(machine -> program_counter < num_of_instructions)
        fetched_instruction = program[machine -> program_counter];
    else{
        fetched_instruction.opcode = HALT;
        machine -> is_running = false;
    } 
    // This is our future FETCH()

    switch(fetched_instruction.opcode){
        case NOP: 
        printf("PC = %d | Executing: NOP | R0=%d R1=%d R2=%d R3=%d | Z=%d | C=%d\n", machine->program_counter, machine -> registers[0], machine -> registers[1], machine -> registers[2], machine -> registers[3], machine -> zero_flag, machine -> carry_flag);
        break;

        case HALT:
        printf("PC = %d | Executing: HALT | R0=%d R1=%d R2=%d R3=%d | Z=%d | C=%d\n", machine->program_counter, machine -> registers[0], machine -> registers[1], machine -> registers[2], machine -> registers[3], machine -> zero_flag, machine -> carry_flag);
        machine -> is_running = false;
        break;

        case JUMP:
        printf("PC = %d | Executing: JUMP %d | R0=%d R1=%d R2=%d R3=%d | Z=%d | C=%d\n", machine->program_counter, fetched_instruction.data, machine -> registers[0], machine -> registers[1], machine -> registers[2], machine -> registers[3], machine -> zero_flag, machine -> carry_flag);
        machine -> program_counter = fetched_instruction.data;
        should_advance_pc = false;
        break;

        case MOV_IMM:
        machine -> registers[fetched_instruction.destination_operand] = fetched_instruction.data;
        printf("PC = %d | Executing: MOV_IMM R%d, %d | R0=%d R1=%d R2=%d R3=%d | Z=%d | C=%d\n", machine->program_counter, fetched_instruction.destination_operand, fetched_instruction.data ,  machine -> registers[0], machine -> registers[1], machine -> registers[2], machine -> registers[3], machine -> zero_flag, machine -> carry_flag);
        break;

        case MOV_REG:
        machine -> registers[fetched_instruction.destination_operand] = machine -> registers[fetched_instruction.source_operand];
        printf("PC = %d | Executing: MOV_REG R%d, R%d | R0=%d R1=%d R2=%d R3=%d | Z=%d | C=%d\n", machine->program_counter, fetched_instruction.destination_operand, fetched_instruction.source_operand ,  machine -> registers[0], machine -> registers[1], machine -> registers[2], machine -> registers[3], machine -> zero_flag, machine -> carry_flag);
        break;

        case ADD:
        post_add = add_8bit(machine -> registers[fetched_instruction.destination_operand], machine -> registers[fetched_instruction.source_operand]);
        machine -> registers[fetched_instruction.destination_operand] = post_add.result;
        machine -> zero_flag = post_add.Z_flag;
        machine -> carry_flag = post_add.C_flag;
        printf("PC = %d | Executing: ADD R%d, R%d | R0=%d R1=%d R2=%d R3=%d | Z=%d | C=%d\n", machine->program_counter, fetched_instruction.destination_operand, fetched_instruction.source_operand ,  machine -> registers[0], machine -> registers[1], machine -> registers[2], machine -> registers[3], machine -> zero_flag, machine -> carry_flag);
        break;

        case JZ:
        printf("PC = %d | Executing: JZ %d | R0=%d R1=%d R2=%d R3=%d | Z=%d | C=%d\n", machine->program_counter, fetched_instruction.data, machine -> registers[0], machine -> registers[1], machine -> registers[2], machine -> registers[3], machine -> zero_flag, machine -> carry_flag);
        if(machine -> zero_flag == true){
            machine -> program_counter = fetched_instruction.data;
            should_advance_pc = false;
        }
        break;

        case JC:
        printf("PC = %d | Executing: JC %d | R0=%d R1=%d R2=%d R3=%d | Z=%d | C=%d\n", machine->program_counter, fetched_instruction.data, machine -> registers[0], machine -> registers[1], machine -> registers[2], machine -> registers[3], machine -> zero_flag, machine -> carry_flag);
        if(machine -> carry_flag == true){
            machine -> program_counter = fetched_instruction.data;
            should_advance_pc = false;
        }
        break;

        case LOAD:
        printf("PC = %d | Executing: LOAD R%d, %d | R0=%d R1=%d R2=%d R3=%d | Z=%d | C=%d\n", machine->program_counter, fetched_instruction.destination_operand, fetched_instruction.data, machine -> registers[0], machine -> registers[1], machine -> registers[2], machine -> registers[3], machine -> zero_flag, machine -> carry_flag);
        machine -> registers[fetched_instruction.destination_operand] = machine -> memory[fetched_instruction.data];
        break;

        case STORE:
        printf("PC = %d | Executing: STORE R%d, %d | R0=%d R1=%d R2=%d R3=%d | Z=%d | C=%d\n", machine->program_counter, fetched_instruction.source_operand, fetched_instruction.data, machine -> registers[0], machine -> registers[1], machine -> registers[2], machine -> registers[3], machine -> zero_flag, machine -> carry_flag);
        machine -> memory[fetched_instruction.data] = machine -> registers[fetched_instruction.source_operand];
        break;
    }
    // THIS IS OUR FUTURE DECODE()

    if(machine -> is_running && should_advance_pc)
        machine -> program_counter++;
    // THIS IS OUR FUTURE ADVANCE()
}

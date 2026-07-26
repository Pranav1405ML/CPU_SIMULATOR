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
    [STORE]   = {2, true},
    [PUSH]    = {1, false},
    [POP]     = {1, false},
    [CALL]    = {2, true},
    [RET]     = {1, false}
};

void step(uint8_t *program, struct Machine *machine, int num_of_instructions){

      if(machine -> program_counter < num_of_instructions){
        switch(machine -> current_stage){
            case fetch:
            machine -> advance = true;
            machine -> instruction_reg = FETCH(machine -> program_counter, program);
            machine -> current_stage = decode;
            break;
            case decode:
            machine -> decoded = DECODE(machine -> program_counter, program, machine -> instruction_reg, num_of_instructions);
            machine -> current_stage = execute;
            break;
            case execute:
            EXECUTE(&machine -> decoded, machine, program, &machine -> advance);
            machine -> current_stage = advance;
            break;
            case advance:
            if((machine -> is_running) && (machine -> advance))
                ADVANCE(machine -> decoded.instr_length, machine);
            machine -> current_stage = fetch;
            break;
           }
        }

      else{ 
         machine -> is_running = false;
         printf("Program counter surpassed the program size\n");
       }    
}


uint8_t FETCH(int PC, uint8_t *program){
    printf("\n==================================================\n\n");
    printf("[FETCH]\n");
    printf("PC : %-3d   IR : 0x%02X\n", PC, program[PC]);
    printf("--------------------------------------------------\n\n");
    return program[PC];
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

    printf("[DECODE]\n");
    printf("Opcode : %-2d   Dest : R%d   Src : R%d",
           temp_decoded.opcode,
           temp_decoded.dest_reg,
           temp_decoded.source_reg);

    if(ISA[temp_decoded.opcode].has_immediate)
        printf("   Imm : %d", temp_decoded.immediate_value);

    printf("\n");
    printf("--------------------------------------------------\n\n");

    return temp_decoded;
}

void EXECUTE(struct Decoded_instruction *decoded, struct Machine *machine, uint8_t *program, bool *advance){
    (void)program;

    struct ALU_result post_add;

    printf("[EXECUTE]\n");

    switch(decoded->opcode){

        case NOP:
            printf("Instruction : NOP\n");
            break;

        case HALT:
            printf("Instruction : HALT\n");
            machine->is_running = false;
            break;

        case JUMP:
            printf("Instruction : JUMP %d\n", decoded->immediate_value);
            machine->program_counter = decoded->immediate_value;
            *advance = false;
            break;

        case MOV_IMM:
            printf("Instruction : MOV_IMM R%d, %d\n",
                   decoded->dest_reg,
                   decoded->immediate_value);

            machine->Bus = decoded->immediate_value;
            machine->registers[decoded->dest_reg] = machine->Bus;
            break;

        case MOV_REG:
            printf("Instruction : MOV_REG R%d, R%d\n",
                   decoded->dest_reg,
                   decoded->source_reg);

            machine->Bus = machine->registers[decoded->source_reg];
            machine->registers[decoded->dest_reg] = machine->Bus;
            break;

        case ADD:
            printf("Instruction : ADD R%d, R%d\n",
                   decoded->dest_reg,
                   decoded->source_reg);

            post_add = add_8bit(machine->registers[decoded->dest_reg],
                                machine->registers[decoded->source_reg]);

            machine->Bus = post_add.result;
            machine->registers[decoded->dest_reg] = machine->Bus;
            machine->zero_flag = post_add.Z_flag;
            machine->carry_flag = post_add.C_flag;
            break;

        case JZ:
            printf("Instruction : JZ %d\n", decoded->immediate_value);

            if(machine->zero_flag){
                machine->program_counter = decoded->immediate_value;
                *advance = false;
            }
            break;

        case JC:
            printf("Instruction : JC %d\n", decoded->immediate_value);

            if(machine->carry_flag){
                machine->program_counter = decoded->immediate_value;
                *advance = false;
            }
            break;

        case LOAD:
            printf("Instruction : LOAD R%d, %d\n",
                   decoded->dest_reg,
                   decoded->immediate_value);

            machine->Bus = machine->memory[decoded->immediate_value];
            machine->registers[decoded->dest_reg] = machine->Bus;
            break;

        case STORE:
            printf("Instruction : STORE R%d, %d\n",
                   decoded->source_reg,
                   decoded->immediate_value);

            machine->Bus = machine->registers[decoded->source_reg];
            machine->memory[decoded->immediate_value] = machine->Bus;
            break;

        case PUSH:
            printf("Instruction : PUSH R%d\n", decoded->source_reg);

            if(machine->stack_pointer_reg == 0){
                printf("Stack overflow\n");
                machine->is_running = false;
            }
            else{
                machine->Bus = machine->registers[decoded->source_reg];
                machine->memory[machine->stack_pointer_reg] = machine->Bus;
                machine->stack_pointer_reg--;
            }
            break;

        case POP:
            printf("Instruction : POP R%d\n", decoded->dest_reg);

            if(machine->stack_pointer_reg == 255){
                printf("Stack underflow\n");
                machine->is_running = false;
            }
            else{
                machine->stack_pointer_reg++;
                machine->Bus = machine->memory[machine->stack_pointer_reg];
                machine->registers[decoded->dest_reg] = machine->Bus;
            }
            break;

        case CALL:
            printf("Instruction : CALL %d\n", decoded->immediate_value);

            machine->Bus = machine->program_counter + decoded->instr_length;
            machine->memory[machine->stack_pointer_reg] = machine->Bus;
            machine->stack_pointer_reg--;

            machine->program_counter = decoded->immediate_value;
            *advance = false;
            break;

        case RET:
            printf("Instruction : RET\n");

            machine->stack_pointer_reg++;
            machine->Bus = machine->memory[machine->stack_pointer_reg];
            machine->program_counter = machine->Bus;
            *advance = false;
            break;

        default:
            printf("Invalid opcode\n");
            break;
    }

    printf("PC  : %-3d   SP : %-3d   BUS : %-3d\n",
           machine->program_counter,
           machine->stack_pointer_reg,
           machine->Bus);

    printf("REG : R0=%-3d R1=%-3d R2=%-3d R3=%-3d\n",
           machine->registers[0],
           machine->registers[1],
           machine->registers[2],
           machine->registers[3]);

    printf("FLAGS : Z=%d  C=%d\n",
           machine->zero_flag,
           machine->carry_flag);

    printf("--------------------------------------------------\n\n");
}

void ADVANCE(uint8_t instr_length, struct Machine *machine){
    machine->program_counter += instr_length;

    printf("[ADVANCE]\n");
    printf("Next PC : %d\n", machine->program_counter);
    printf("\n==================================================\n");

}



// uint8_t FETCH(int PC, uint8_t *program){
//     printf("\nCycle stage: FETCH\n");
//     printf("PC = %d  ||  IR = %x\n", PC, program[PC]);
//     return program[PC];
// }

// struct Decoded_instruction DECODE(int PC, uint8_t *program, uint8_t fetched_instr_byte, int num_of_instructions){
//     struct Decoded_instruction temp_decoded = {0};
//     temp_decoded.opcode = (enum Opcode)((fetched_instr_byte >> 4) & 0x0F);
//     temp_decoded.dest_reg = (enum Register)((fetched_instr_byte >> 2) & 0x03);
//     temp_decoded.source_reg = (enum Register)(fetched_instr_byte & 0x03);
//     temp_decoded.instr_length = ISA[temp_decoded.opcode].length;

//     if(temp_decoded.opcode >= OPCODE_COUNT){
//             printf("Invalid opcode at PC %d\n", PC);
//             temp_decoded.opcode = HALT;
//     }
//     else if(ISA[temp_decoded.opcode].has_immediate){
//         if(PC + 1 < num_of_instructions)
//            temp_decoded.immediate_value = program[PC + 1];

//         else{
//            printf("Program counter surpassed the program size\n");
//             temp_decoded.opcode = HALT;
//         }
//     }
//     printf("\nCycle stage: DECODE\n");
//     printf("Opcode = %d  ||  Dest = R%d  ||  SRC = R%d\n", temp_decoded.opcode, temp_decoded.dest_reg, temp_decoded.source_reg);
//     return temp_decoded;
// }

// void EXECUTE(struct Decoded_instruction *decoded, struct Machine *machine, uint8_t *program, bool *advance){
//     (void)program;
//     struct ALU_result post_add;

//     printf("\nCycle stage: EXECUTE\n");
//     switch(decoded -> opcode){
//         case NOP: 
//         printf("PC = %d | Executing: NOP | R0=%d R1=%d R2=%d R3=%d | Z=%d | C=%d\n", machine->program_counter, machine -> registers[0], machine -> registers[1], machine -> registers[2], machine -> registers[3], machine -> zero_flag, machine -> carry_flag);
//         break;

//         case HALT:
//         printf("PC = %d | Executing: HALT | R0=%d R1=%d R2=%d R3=%d | Z=%d | C=%d\n", machine->program_counter, machine -> registers[0], machine -> registers[1], machine -> registers[2], machine -> registers[3], machine -> zero_flag, machine -> carry_flag);
//         machine -> is_running = false;
//         break;

//         case JUMP:
//         printf("PC = %d | Executing: JUMP %d | R0=%d R1=%d R2=%d R3=%d | Z=%d | C=%d\n", machine->program_counter, decoded -> immediate_value, machine -> registers[0], machine -> registers[1], machine -> registers[2], machine -> registers[3], machine -> zero_flag, machine -> carry_flag);
//         machine -> program_counter = decoded -> immediate_value;
//         *advance = false;
//         break;

//         case MOV_IMM:
//         machine -> Bus = decoded -> immediate_value;
//         machine -> registers[decoded -> dest_reg] = machine -> Bus;
//         printf("PC = %d | Executing: MOV_IMM R%d, %d | R0=%d R1=%d R2=%d R3=%d | Z=%d | C=%d\n", machine->program_counter, decoded ->dest_reg, decoded -> immediate_value, machine -> registers[0], machine -> registers[1], machine -> registers[2], machine -> registers[3], machine -> zero_flag, machine -> carry_flag);
//         break;

//         case MOV_REG:
//         machine -> Bus = machine -> registers[decoded -> source_reg];
//         machine -> registers[decoded -> dest_reg] = machine -> Bus;
//         printf("PC = %d | Executing: MOV_REG R%d, R%d | R0=%d R1=%d R2=%d R3=%d | Z=%d | C=%d\n", machine->program_counter, decoded -> dest_reg, decoded -> source_reg ,  machine -> registers[0], machine -> registers[1], machine -> registers[2], machine -> registers[3], machine -> zero_flag, machine -> carry_flag);
//         break;

//         case ADD:
//         post_add = add_8bit(machine -> registers[decoded -> dest_reg], machine -> registers[decoded -> source_reg]);
//         machine -> Bus = post_add.result;
//         machine -> registers[decoded -> dest_reg] = machine -> Bus;

//         machine -> zero_flag = post_add.Z_flag;
//         machine -> carry_flag = post_add.C_flag;
//         printf("PC = %d | Executing: ADD R%d, R%d | R0=%d R1=%d R2=%d R3=%d | Z=%d | C=%d\n", machine->program_counter, decoded -> dest_reg, decoded -> source_reg ,  machine -> registers[0], machine -> registers[1], machine -> registers[2], machine -> registers[3], machine -> zero_flag, machine -> carry_flag);
//         break;

//         case JZ:
//         printf("PC = %d | Executing: JZ %d | R0=%d R1=%d R2=%d R3=%d | Z=%d | C=%d\n", machine->program_counter, decoded -> immediate_value, machine -> registers[0], machine -> registers[1], machine -> registers[2], machine -> registers[3], machine -> zero_flag, machine -> carry_flag);
//         if(machine -> zero_flag == true){
//             machine -> program_counter = decoded -> immediate_value;
//             *advance = false;
//         }
//         break;

//         case JC:
//         printf("PC = %d | Executing: JC %d | R0=%d R1=%d R2=%d R3=%d | Z=%d | C=%d\n", machine->program_counter, decoded -> immediate_value, machine -> registers[0], machine -> registers[1], machine -> registers[2], machine -> registers[3], machine -> zero_flag, machine -> carry_flag);
//         if(machine -> carry_flag == true){
//             machine -> program_counter = decoded -> immediate_value;
//             *advance = false;
//         }
//         break;

//         case LOAD:
//         printf("PC = %d | Executing: LOAD R%d, %d | R0=%d R1=%d R2=%d R3=%d | Z=%d | C=%d\n", machine->program_counter, decoded -> dest_reg, decoded -> immediate_value, machine -> registers[0], machine -> registers[1], machine -> registers[2], machine -> registers[3], machine -> zero_flag, machine -> carry_flag);
//         machine -> Bus = machine -> memory[decoded -> immediate_value];
//         machine -> registers[decoded -> dest_reg] = machine -> Bus;
//         break;

//         case STORE:
//         printf("PC = %d | Executing: STORE R%d, %d | R0=%d R1=%d R2=%d R3=%d | Z=%d | C=%d\n", machine->program_counter, decoded -> source_reg, decoded -> immediate_value, machine -> registers[0], machine -> registers[1], machine -> registers[2], machine -> registers[3], machine -> zero_flag, machine -> carry_flag);
//         machine -> Bus = machine -> registers[decoded -> source_reg];
//         machine -> memory[decoded -> immediate_value] = machine -> Bus;
//         break;

//         case PUSH:
//         printf("PC = %d | Executing: PUSH R%d | R0=%d R1=%d R2=%d R3=%d | Z=%d | C=%d\n", machine->program_counter, decoded ->source_reg,  machine -> registers[0], machine -> registers[1], machine -> registers[2], machine -> registers[3], machine -> zero_flag, machine -> carry_flag);
//         if(machine -> stack_pointer_reg == 0){
//             printf("Stack overflow\n");
//             machine -> is_running = false;
//         }
//         else{
//             machine -> Bus = machine -> registers[decoded -> source_reg];
//             machine -> memory[machine -> stack_pointer_reg] = machine -> Bus;
//             printf("Value at address [%d] = %d\n", machine -> stack_pointer_reg, machine -> memory[machine -> stack_pointer_reg]);
//             machine -> stack_pointer_reg--;
//             printf("Stack pointer: %d\n", machine -> stack_pointer_reg);
//         }
//         break;

//         case POP:
//         printf("PC = %d | Executing: POP R%d | R0=%d R1=%d R2=%d R3=%d | Z=%d | C=%d\n", machine->program_counter, decoded ->dest_reg, machine -> registers[0], machine -> registers[1], machine -> registers[2], machine -> registers[3], machine -> zero_flag, machine -> carry_flag);
//         if(machine -> stack_pointer_reg == 255){
//             printf("Stack underflow\n");
//             machine -> is_running = false;
//         }
//         else{
//             machine -> stack_pointer_reg++;
//             machine -> Bus = machine -> memory[machine -> stack_pointer_reg];
//             machine -> registers[decoded -> dest_reg] = machine -> Bus;
//             printf("After POP:  R0=%d R1=%d R2=%d R3=%d\n", machine -> registers[0], machine -> registers[1], machine -> registers[2], machine -> registers[3]);
//             printf("Stack pointer: %d\n", machine -> stack_pointer_reg);
//         }
//         break;

//         case CALL:
//         printf("PC = %d | Executing: CALL %d | R0=%d R1=%d R2=%d R3=%d | Z=%d | C=%d\n", machine->program_counter, decoded ->immediate_value, machine -> registers[0], machine -> registers[1], machine -> registers[2], machine -> registers[3], machine -> zero_flag, machine -> carry_flag);
//         // push pc+1 on stack
//         machine -> Bus = (machine -> program_counter) + (decoded -> instr_length) ;
//         machine -> memory[machine -> stack_pointer_reg] = machine -> Bus;
//         machine -> stack_pointer_reg--;

//         // jump to the immediate value (return address)
//         machine -> program_counter = decoded -> immediate_value;

//         // Supress advance
//         *advance = false;
//         break;

//         case RET: 
//         printf("PC = %d | Executing: RET | R0=%d R1=%d R2=%d R3=%d | Z=%d | C=%d\n", machine->program_counter, machine -> registers[0], machine -> registers[1], machine -> registers[2], machine -> registers[3], machine -> zero_flag, machine -> carry_flag);
//         //Assume all push get their pop and we restore back to our stack pointer reg pointing the return address
//         machine -> stack_pointer_reg++;
//         machine -> Bus = machine -> memory[machine -> stack_pointer_reg];
//         machine -> program_counter = machine -> Bus;
//         *advance = false;
//         break;

//         default:
//         printf("Invalid opcode\n");
//         break;
//     }
//         printf("BUS = %d\n", machine -> Bus);
// }

// void ADVANCE(uint8_t instr_length, struct Machine *machine){
//         machine -> program_counter += instr_length;

//         printf("\nCycle stage: ADVANCE\n");
//         printf("PC updated = %d\n", machine -> program_counter);
// }



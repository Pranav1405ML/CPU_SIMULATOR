#include <stdio.h> 
#include <stdbool.h>
#include <stdint.h>

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
// Make sure enum is declared before making any use of it 
struct Instruction{
    enum Opcode opcode;  // This needs enum to be declared beforehand
    enum Register destination_operand;               // register index
    enum Register source_operand;
    uint8_t data;
};
struct Machine{
    bool is_running;
    int program_counter;
    uint8_t registers[4];
    bool zero_flag;
    bool carry_flag;
    uint8_t memory[256];
};
struct ALU_result{
    uint8_t result;
    bool C_flag;
    bool Z_flag;
};

void step(struct Instruction *program, struct Machine *machine, int num_of_instructions);
struct ALU_result add_8bit(uint8_t a, uint8_t b);

int main(){
    struct Machine machine = {0};       // sets everything to zero
    machine.is_running = true;          // changes is_running from zero(false) to true

    struct Instruction program[] = {{MOV_IMM, R1, 0, 1},
                                    {STORE, 0, R1, 0},

                                    {MOV_IMM, R1, 0, 2},
                                    {STORE, 0, R1, 1},

                                    {MOV_IMM, R1, 0, 3},
                                    {STORE, 0, R1, 2},

                                    {MOV_IMM, R1, 0, 4},
                                    {STORE, 0, R1, 3},

                                    {MOV_IMM, R1, 0, 5},
                                    {STORE, 0, R1, 4},

                                    {LOAD, R0, 0, 0},
                                    {LOAD, R1, 0, 1},
                                    {ADD, R0, R1, 0},

                                    {LOAD, R1, 0, 2},
                                    {ADD, R0, R1, 0},

                                    {LOAD, R1, 0, 3},
                                    {ADD, R0, R1, 0},

                                    {LOAD, R1, 0, 4},
                                    {ADD, R0, R1, 0},

                                    {HALT}
                                    };
    int num_of_instructions = (sizeof(program)/ sizeof(struct Instruction));

    while(machine.is_running){
        step(program, &machine, num_of_instructions);
    }
    return 0;
}

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
struct ALU_result add_8bit(uint8_t a, uint8_t b){
    struct ALU_result ALU = {0};
    uint8_t carry;
    while(b != 0){
        carry = a & b;           // Carry
        a = a ^ b;               // Sum
        if((carry & (1 << 7)) != 0) ALU.C_flag = true;
        b = carry << 1;          // add carry to next digit place 
    }
    ALU.result = a;
    if(a == 0) ALU.Z_flag = true;
    return ALU;
}

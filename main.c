#include "machine.h"

int main(){
    struct Machine machine = {0};       // sets everything to zero
    machine.is_running = true;          // changes is_running from zero(false) to true
    machine.advance = true;
    machine.stack_pointer_reg = 255;
    
    // uint8_t program[] = {
    //     0x20, 0x05,   // MOV_IMM R0, 5  
    //     0x90, 0x14,   // STORE R0, 20
    //     0x84, 0x14,   // LOAD R1, 20
    //     0x55,         // ADD R1, R1
    //     0x91, 0x15,   // STORE R1, 21
    //     0x28, 0xF6,   // MOV_IMM R2, 246
    //     0x56,         // ADD R1, R2
    //     0x70, 0x10,   // JZ, 16
    //     0x00, 0x04,   // JUMP, 4   
    //     0x10          // HALT
    // };

    // uint8_t program[] = {

    //     0x20, 0x03,   // MOV_IMM R0, 3
    //     0x24, 0x07,   // MOV_IMM R1, 7

    //     0xA0,         // PUSH R0
    //     0xA1,         // PUSH R1

    //     0xB8,         // POP R2
    //     0xBC,         // POP R3

    //     0x10          // HALT
    // };

    // Deep Stack Order
    // uint8_t program[] = {

    // 0x20,0x01,   // MOV_IMM R0,1
    // 0x24,0x02,   // MOV_IMM R1,2
    // 0x28,0x03,   // MOV_IMM R2,3

    // 0xA0,        // PUSH R0
    // 0xA1,        // PUSH R1
    // 0xA2,        // PUSH R2

    // 0xB8,        // POP R2
    // 0xB0,        // POP R0
    // 0xB4,        // POP R1

    // 0x10         // HALT
    // };

    // Register Mutation Test
    // uint8_t program[] = {

    // 0x20,0x05,   // MOV_IMM R0,5
    // 0xA0,        // PUSH R0

    // 0x20,0x63,   // MOV_IMM R0,99

    // 0xB4,        // POP R1

    // 0x10         // HALT
    // };

    // Stack Pointer Symmetry
    // uint8_t program[] = {

    // 0x20,0x04,   // MOV_IMM R0,4
    // 0x24,0x09,   // MOV_IMM R1,9

    // 0xA0,        // PUSH R0
    // 0xA1,        // PUSH R1

    // 0xB4,        // POP R1
    // 0xA1,        // PUSH R1

    // 0xB8,        // POP R2
    // 0xBC,        // POP R3

    // 0x10         // HALT
    // };


    // // Test 4 — Stack + ALU Interaction
    // uint8_t program[] = {

    // 0x20,0x0A,   // MOV_IMM R0,10
    // 0x24,0x14,   // MOV_IMM R1,20

    // 0xA0,        // PUSH R0
    // 0xA1,        // PUSH R1

    // 0x51,        // ADD R0,R1

    // 0xB8,        // POP R2
    // 0xBC,        // POP R3

    // 0x10         // HALT
    // };

uint8_t program[] = {

0x20,0x05,   // MOV_IMM R0,5
0xC0,0x05,   // CALL foo
0x10,        // HALT

0x50,        // ADD R0,R0
0xC0,0x09,   // CALL bar
0xD0,        // RET

0x50,        // ADD R0,R0
0x50,        // ADD R0,R0
0xD0         // RET
};

    int num_of_instructions = (sizeof(program)/ sizeof(uint8_t));

    while(machine.is_running){
        step(program, &machine, num_of_instructions);
    }
    return 0;
}
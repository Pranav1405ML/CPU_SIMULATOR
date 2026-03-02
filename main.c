#include "machine.h"

int main(){
    struct Machine machine = {0};       // sets everything to zero
    machine.is_running = true;          // changes is_running from zero(false) to true

    uint8_t program[] = {
        0x20, 0x05,   // MOV_IMM R0, 5  
        0x90, 0x14,   // STORE R0, 20
        0x84, 0x14,   // LOAD R1, 20
        0x55,         // ADD R1, R1
        0x91, 0x15,   // STORE R1, 21
        0x28, 0xF6,   // MOV_IMM R2, 246
        0x56,         // ADD R1, R2
        0x70, 0x10,   // JZ, 16
        0x00, 0x04,   // JUMP, 4   
        0x10          // HALT
    };

    int num_of_instructions = (sizeof(program)/ sizeof(uint8_t));

    while(machine.is_running){
        step(program, &machine, num_of_instructions);
    }
    return 0;
}
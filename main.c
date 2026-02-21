#include "machine.h"
#include <stdio.h>

int main(){
    struct Machine machine = {0};       // sets everything to zero
    machine.is_running = true;          // changes is_running from zero(false) to true

    uint8_t program[] = {
        0x20, 0x00,   // MOV_IMM R0, 0
        0x24, 0x03,   // MOV_IMM R1, 3
        0x28, 0xFF,   // MOV_IMM R2, 255  (-1)

        0x51,         // ADD R0, R1       ; sum += counter
        0x56,         // ADD R2, R1       ; counter--

        0x70, 0x0C,   // JZ 13            ; if zero jump to HALT
        0x00, 0x06,   // JUMP 6           ; jump back to ADD R0,R1

        0x10          // HALT
    };

    int num_of_instructions = (sizeof(program)/ sizeof(uint8_t));

    while(machine.is_running){
        step(program, &machine, num_of_instructions);
    }
    return 0;
}
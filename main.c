#include "cpu/machine.h"
#include "assembler/assembler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main(){

    printf("====================================================\n");    
    printf("         TINY CPU END-TO-END WORKFLOW               \n");
    printf("====================================================\n\n");

    struct Machine machine = {0};       // sets everything to zero
    machine.is_running = true;          // changes is_running from zero(false) to true
    machine.advance = true;
    machine.stack_pointer_reg = 255;

    uint8_t program[256];

    int num_of_instructions = assemble_file("programs/program.asm", "output.txt", program, 256);

    if (num_of_instructions <= 0) {
        printf("Assembly failed or source file was empty. Exiting execution.\n");
        return 1;
    }

    printf("\n=== STARTING CPU EXECUTION ===\n\n");
    while(machine.is_running){
        step(program, &machine, num_of_instructions);
    }

    printf("=== CPU EXECUTION HALTED ===\n\n");
    printf("====================================================\n");
    printf("FINAL MACHINE STATE:\n");
    printf("----------------------------------------------------\n");
    printf("PC: %-3d | SP: %-3d | Zero Flag (Z): %d | Carry Flag (C): %d\n", machine.program_counter, machine.stack_pointer_reg, machine.zero_flag, machine.carry_flag);
    printf("Registers: R0 = %d, R1 = %d, R2 = %d, R3 = %d\n", machine.registers[0], machine.registers[1], machine.registers[2], machine.registers[3]);
    printf("Memory[50] = %d (Result storage)\n", machine.memory[50]);
    printf("====================================================\n");

    return 0;
}

#include "machine.h"

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
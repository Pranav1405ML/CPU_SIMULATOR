#include "machine.h"

int main(){
    struct Machine machine = {0};       // sets everything to zero
    machine.is_running = true;          // changes is_running from zero(false) to true

    uint8_t program[] = {0x20, 0x05,
                         0x24, 0x0A,
                         0x51,
                         0x10
                         };

    int num_of_instructions = (sizeof(program)/ sizeof(uint8_t));

    while(machine.is_running){
        step(program, &machine, num_of_instructions);
    }
    return 0;
}
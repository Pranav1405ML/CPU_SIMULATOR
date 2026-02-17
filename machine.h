#include <stdbool.h>
#include <stdint.h>
#include "instruction.h"

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

void step(uint8_t *program, struct Machine *machine, int num_of_instructions);
struct ALU_result add_8bit(uint8_t a, uint8_t b);

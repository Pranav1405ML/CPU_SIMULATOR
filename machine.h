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
uint8_t FETCH(int PC, int num_of_instructions, uint8_t *program);
struct Decoded_instruction DECODE(int PC, uint8_t *program, uint8_t fetched_instr_byte, int num_of_instructions);
void EXECUTE(struct Decoded_instruction *decoded, struct Machine *machine, uint8_t *program, bool *advance);
void ADVANCE(uint8_t instr_length, struct Machine *machine);
struct ALU_result add_8bit(uint8_t a, uint8_t b);

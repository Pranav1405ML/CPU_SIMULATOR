#include <stdbool.h>
#include <stdint.h>
#include "instruction.h"

enum Stage{
    fetch,
    decode,
    execute,
    advance
};

struct Machine{
    bool is_running;
    bool zero_flag;
    bool carry_flag;
    bool advance;
    int program_counter;
    uint8_t registers[4];
    uint8_t memory[256];
    uint8_t Bus;
    uint8_t instruction_reg;
    enum Stage current_stage;
    struct Decoded_instruction decoded;
};

struct ALU_result{
    uint8_t result;
    bool C_flag;
    bool Z_flag;
};

void step(uint8_t *program, struct Machine *machine, int num_of_instructions);
uint8_t FETCH(int PC, uint8_t *program);
struct Decoded_instruction DECODE(int PC, uint8_t *program, uint8_t fetched_instr_byte, int num_of_instructions);
void EXECUTE(struct Decoded_instruction *decoded, struct Machine *machine, uint8_t *program, bool *advance);
void ADVANCE(uint8_t instr_length, struct Machine *machine);
struct ALU_result add_8bit(uint8_t a, uint8_t b);

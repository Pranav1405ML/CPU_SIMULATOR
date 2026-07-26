#include "machine.h"

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

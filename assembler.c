#include "assembler.h"
#include <stdio.h>
#include <string.h>

static const struct Instruction_data table[OPCODE_COUNT] = {
    [JUMP]    = {0, 2, IMM},
    [HALT]    = {1, 1, NO_OPERAND},
    [MOV_IMM] = {2, 2, REG_IMM},
    [NOP]     = {3, 1, NO_OPERAND},
    [MOV_REG] = {4, 1, REG_REG},
    [ADD]     = {5, 1, REG_REG},
    [JC]      = {6, 2, IMM},
    [JZ]      = {7, 2, IMM},
    [LOAD]    = {8, 2, REG_IMM},
    [STORE]   = {9, 2, REG_IMM},
    [PUSH]    = {10, 1, REG},
    [POP]     = {11, 1, REG},
    [CALL]    = {12, 2, IMM},
    [RET]     = {13, 1, NO_OPERAND}
};

static const char *mnemonics[OPCODE_COUNT] = {"JUMP", "HALT", "MOV_IMM", "NOP", "MOV_REG", "ADD", "JC", "JZ", "LOAD", "STORE", "PUSH", "POP", "CALL", "RET"};

void readline(){
    FILE *fp = fopen("program.asm", "r");
    if(fp == NULL){
        printf("Error opening the file.\n");
        return;
    }
    char line[128];
    char *token;
    uint8_t byte;

    while(fgets(line, 128, fp)){
        token = strtok(line, " ,");
        while(token != NULL){
            
            for(int i=0; i<OPCODE_COUNT; i++){
                if(!strcmp(mnemonics[i], token)){
                    // byte = i << 4;
                    byte = i;
                    continue;
                }
            }

            printf("Token: %s\n", token);
            printf("Byte: %x\n", byte);
            token = strtok(NULL, " ,");
        }
    }
    fclose(fp);
}

int main(){
    readline();
    return 0;
}

// gcc assembler.c -o a


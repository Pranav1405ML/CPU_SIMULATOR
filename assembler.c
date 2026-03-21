#include "assembler.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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
    [PUSH]    = {10, 1, S_REG},
    [POP]     = {11, 1, D_REG},
    [CALL]    = {12, 2, IMM},
    [RET]     = {13, 1, NO_OPERAND}
};

static const char *mnemonics[OPCODE_COUNT] = {"JUMP", "HALT", "MOV_IMM", "NOP", "MOV_REG", "ADD", "JC", "JZ", "LOAD", "STORE", "PUSH", "POP", "CALL", "RET"};

void tokenize(){
    FILE *fp = fopen("program.asm", "r");
    if(fp == NULL){
        printf("Error opening the file\n");
        return;
    }

    FILE *fp2 = fopen("output.txt", "w");
    if (fp2 == NULL) {
        printf("Error opening file for writing\n");
        return;
    }

    char line[128];
    const char *token;
    char token_array[5][8];
    uint8_t token_num = 0;

    uint8_t first_byte, second_byte;
    uint8_t opcode, dest_reg, src_reg, imm;
    dest_reg = src_reg = imm = 0;

    while(fgets(line, 128, fp)){
        token = strtok(line, " ,");
        while(token != NULL){
            char *ptr = strchr(token, '\n');       // strchr gives the address of first occurence of that \n
            if(ptr != NULL){
                *ptr = '\0';
            }

            strcpy(token_array[token_num], token);

            token_num++;
            token = strtok(NULL, " ,");
        } 
        printf("Opcode = %s, dest_reg = %s, src_reg = %s\n", token_array[0], token_array[1], token_array[2]);

        for(int i=0; i<OPCODE_COUNT; i++){
                if(!strcmp(mnemonics[i], token_array[0])){
                    opcode = i;
                    break;
                }
            }

        switch(table[opcode].optype){
            case NO_OPERAND:
            break;

            case D_REG: // PUSH has src_reg while POP has dest_reg
            dest_reg = token_array[1][1] - '0';
            break;

            case S_REG: // PUSH has src_reg while POP has dest_reg
            src_reg = token_array[1][1] - '0';
            break;

            case REG_REG:
            dest_reg = token_array[1][1] - '0';
            src_reg = token_array[2][1] - '0';
            break;

            case REG_IMM:
            dest_reg = token_array[1][1] - '0';
            imm = atoi(token_array[2]);
            break;

            case IMM:
            imm = atoi(token_array[1]);
            break;
         }

        first_byte = ((opcode << 4) | (dest_reg << 2) | (src_reg));
        second_byte = imm;

        printf("First-Byte: 0x%x || Second-Byte: 0x%x\n", first_byte, second_byte);

        fprintf(fp2, "0x%x",first_byte);
        if(table[opcode].length == 2){
            fprintf(fp2, " 0x%x",second_byte);
        }
        fprintf(fp2, "\n");

        token_num = 0;
        dest_reg = src_reg = imm = 0;
    }
    fclose(fp2);
    fclose(fp);
}

int main(){
    tokenize();
    return 0;
}

// gcc assembler.c -o a


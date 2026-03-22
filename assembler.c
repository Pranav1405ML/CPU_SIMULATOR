#include "assembler.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static const struct Instruction_data table[OPCODE_COUNT] = {
    [JUMP]    = {0, 2, IMM, 1},
    [HALT]    = {1, 1, NO_OPERAND, 0},
    [MOV_IMM] = {2, 2, REG_IMM, 2},
    [NOP]     = {3, 1, NO_OPERAND, 0},
    [MOV_REG] = {4, 1, REG_REG, 2},
    [ADD]     = {5, 1, REG_REG, 2},
    [JC]      = {6, 2, IMM, 1},
    [JZ]      = {7, 2, IMM, 1},
    [LOAD]    = {8, 2, REG_IMM, 2},
    [STORE]   = {9, 2, REG_IMM, 2},
    [PUSH]    = {10, 1, S_REG, 1},
    [POP]     = {11, 1, D_REG, 1},
    [CALL]    = {12, 2, IMM, 1},
    [RET]     = {13, 1, NO_OPERAND, 0}
};

static const char *mnemonics[OPCODE_COUNT] = {"JUMP", "HALT", "MOV_IMM", "NOP", "MOV_REG", "ADD", "JC", "JZ", "LOAD", "STORE", "PUSH", "POP", "CALL", "RET"};

int tokenize(char *line, char token_array[][8]);
int get_opcode(char *mnemonic);
void parse_operands(uint8_t opcode, uint8_t *dest_reg, uint8_t *src_reg, uint8_t *imm, char token_array[][8]);
void encode_and_write(uint8_t opcode, uint8_t dest_reg, uint8_t src_reg, uint8_t imm, FILE *fp2);
void assemble();

int main(){
    assemble();
    return 0;
}

void assemble(){
    FILE *fp1 = fopen("program.asm", "r");
    if(fp1 == NULL){
        printf("Error opening the file\n");
        return;
    }

    FILE *fp2 = fopen("output.txt", "w");
    if (fp2 == NULL) {
        printf("Error opening file for writing\n");
        return;
    }

    char line[128];
    char token_array[5][8]; 

    while(fgets(line, 128, fp1)){
        uint8_t dest_reg=0, src_reg=0, imm=0;

        int token_num = tokenize(line, token_array);
        if(token_num == 0) continue;

        printf("Opcode = %s, dest = %s, src = %s\n", token_array[0], token_array[1], token_array[2]);

        int opcode = get_opcode(token_array[0]);
        if(opcode == -1){
            printf("OPCODE NOT FOUND\n");
            continue;
        }

        if(!(token_num == (table[opcode].operand_cnt + 1))){
            printf("Invalid No. of Operands\n\n");
            continue;
        }

        parse_operands(opcode, &dest_reg, &src_reg, &imm, token_array);
        encode_and_write(opcode, dest_reg, src_reg, imm, fp2);     
    }
    fclose(fp2);
    fclose(fp1);
}
int tokenize(char *line, char token_array[][8]){
    const char *token = strtok(line, " ,");
    int cnt = 0;
    while(token != NULL){
        char *ptr = strchr(token, '\n');                  // strchr gives the address of first occurence of that \n
        if(ptr != NULL)  *ptr = '\0';
        
        strcpy(token_array[cnt], token);

        cnt++;
        token = strtok(NULL, " ,");
    }
 return cnt;
}
int get_opcode(char *mnemonic){
    for(int i=0; i<OPCODE_COUNT; i++){
        if(!strcmp(mnemonics[i], mnemonic)){
            return i;
        }
    }
    return -1;
}
void parse_operands(uint8_t opcode, uint8_t *dest_reg, uint8_t *src_reg, uint8_t *imm, char token_array[][8]){
    switch(table[opcode].optype){
        case NO_OPERAND:
        break;

        case D_REG: // PUSH has src_reg while POP has dest_reg
        if((token_array[1][1] - '0') <= 3 && (token_array[1][1] - '0') >= 0)
            *dest_reg = token_array[1][1] - '0';
        else printf("Invalid Register\n");
        break;

        case S_REG: // PUSH has src_reg while POP has dest_reg
        if((token_array[1][1] - '0') <= 3 && (token_array[1][1] - '0') >= 0)
            *src_reg = token_array[1][1] - '0';
        else printf("Invalid Register\n");
        break;

        case REG_REG:
        if((token_array[1][1] - '0') <= 3 && (token_array[1][1] - '0') >= 0)
            *dest_reg = token_array[1][1] - '0';
        else printf("Invalid Register\n");

        if((token_array[2][1] - '0') <= 3 && (token_array[2][1] - '0') >= 0)
            *src_reg = token_array[2][1] - '0';
        else printf("Invalid Register\n");
        break;

        case REG_IMM:
        if((token_array[1][1] - '0') <= 3 && (token_array[1][1] - '0') >= 0)
            *dest_reg = token_array[1][1] - '0';
        else printf("Invalid Register\n");

        if(atoi(token_array[2]) < 256  && atoi(token_array[2]) >= 0)
            *imm = atoi(token_array[2]);
        else printf("Invalid Immediate value\n");
        break;

        case IMM:
        if(atoi(token_array[1]) < 256  && atoi(token_array[1]) >= 0)
            *imm = atoi(token_array[1]);
        else printf("Invalid Immediate value\n");
        break;
        }
}
void encode_and_write(uint8_t opcode, uint8_t dest_reg, uint8_t src_reg, uint8_t imm, FILE *fp2){
    uint8_t first_byte = ((opcode << 4) | (dest_reg << 2) | (src_reg));
    uint8_t second_byte = imm;

    printf("First-Byte: 0x%x || Second-Byte: 0x%x\n\n", first_byte, second_byte);

    fprintf(fp2, "0x%x",first_byte);
    if(table[opcode].length == 2){
        fprintf(fp2, " 0x%x",second_byte);
    }
    fprintf(fp2, "\n");
}

// gcc assembler.c -o a

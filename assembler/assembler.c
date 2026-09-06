#ifdef __MINGW32__
int stricmp(const char *, const char *);
#endif

#include "assembler.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

static const struct Instruction_data table[OPCODE_COUNT] = {
    [JUMP]    = {0, 2, IMM, 1},
    [HALT]    = {1, 1, NO_OPERAND, 0},
    [MOV_IMM] = {2, 2, D_IMM, 2},
    [NOP]     = {3, 1, NO_OPERAND, 0},
    [MOV_REG] = {4, 1, REG_REG, 2},
    [ADD]     = {5, 1, REG_REG, 2},
    [JC]      = {6, 2, IMM, 1},
    [JZ]      = {7, 2, IMM, 1},
    [LOAD]    = {8, 2, D_IMM, 2},
    [STORE]   = {9, 2, S_IMM, 2},
    [PUSH]    = {10, 1, S_REG, 1},
    [POP]     = {11, 1, D_REG, 1},
    [CALL]    = {12, 2, IMM, 1},
    [RET]     = {13, 1, NO_OPERAND, 0}
};

static const char *mnemonics[OPCODE_COUNT] = {
    "JUMP", "HALT", "MOV_IMM", "NOP", "MOV_REG", "ADD",
    "JC", "JZ", "LOAD", "STORE", "PUSH", "POP", "CALL", "RET"
};

int tokenize(char *line, char token_array[5][16]);
int get_opcode(char *mnemonic);
int parse_register(const char *reg_str);
int parse_immediate(const char *imm_str);
int parse_operands(int opcode, int *dest_reg, int *src_reg, int *imm, char token_array[5][16]);
void encode_and_write(int opcode, int dest_reg, int src_reg, int imm, FILE *fp2, uint8_t *buffer, int *buf_idx, int max_size);
int assemble_file(const char *in_filename, const char *out_hex_filename, uint8_t *program_buffer, int max_size);


int tokenize(char *line, char token_array[5][16]) {
    const char *token = strtok(line, " ,\t\r\n");
    int cnt = 0;
    while (token != NULL && cnt < 5) {
        strncpy(token_array[cnt], token, 15);
        token_array[cnt][15] = '\0';
        cnt++;
        token = strtok(NULL, " ,\t\r\n");
    }
    return cnt;
}

int get_opcode(char *mnemonic) {
    for (int i = 0; i < OPCODE_COUNT; i++) {
        if (stricmp(mnemonics[i], mnemonic) == 0) {
            return i;
        }
    }
    return -1;
}

int parse_register(const char *reg_str) {
    if (!reg_str || strlen(reg_str) < 2) return -1;
    if (toupper((unsigned char)reg_str[0]) != 'R') return -1;
    int reg_num = reg_str[1] - '0';
    if (reg_num >= 0 && reg_num <= 3 && reg_str[2] == '\0') {
        return reg_num;
    }
    return -1;
}

int parse_immediate(const char *imm_str) {
    if (!imm_str || *imm_str == '\0') return -1;
    char *endptr;
    long val = strtol(imm_str, &endptr, 0);
    if (*endptr != '\0') return -1;
    if (val >= 0 && val <= 255) {
        return (int)val;
    }
    return -1;
}

int parse_operands(int opcode, int *dest_reg, int *src_reg, int *imm, char token_array[5][16]) {
    switch (table[opcode].optype) {
        case NO_OPERAND:
            break;

        case D_REG:
            *dest_reg = parse_register(token_array[1]);
            break;

        case S_REG:
            *src_reg = parse_register(token_array[1]);
            break;

        case REG_REG:
            *dest_reg = parse_register(token_array[1]);
            *src_reg = parse_register(token_array[2]);
            break;

        case D_IMM:
            *dest_reg = parse_register(token_array[1]);
            *imm = parse_immediate(token_array[2]);
            break;

        case S_IMM:
            *src_reg = parse_register(token_array[1]);
            *imm = parse_immediate(token_array[2]);
            break;

        case IMM:
            *imm = parse_immediate(token_array[1]);
            break;
    }

    if (*dest_reg == -1 || *src_reg == -1 || *imm == -1) {
        return -1;
    }
    return 1;
}

void encode_and_write(int opcode, int dest_reg, int src_reg, int imm, FILE *fp2, uint8_t *buffer, int *buf_idx, int max_size) {
    uint8_t first_byte = ((opcode << 4) | ((dest_reg & 0x03) << 2) | (src_reg & 0x03));
    uint8_t second_byte = (uint8_t)imm;

    if (buffer != NULL && *buf_idx < max_size) {
        buffer[(*buf_idx)++] = first_byte;
    }

    if (fp2) {
        fprintf(fp2, "0x%02x", first_byte);
    }

    if (table[opcode].length == 2) {
        if (buffer != NULL && *buf_idx < max_size) {
            buffer[(*buf_idx)++] = second_byte;
        }
        if (fp2) {
            fprintf(fp2, " 0x%02x", second_byte);
        }
    }

    if (fp2) {
        fprintf(fp2, "\n");
    }
}

int assemble_file(const char *in_filename, const char *out_hex_filename, uint8_t *program_buffer, int max_size) {
    FILE *fp1 = fopen(in_filename, "r");
    if (fp1 == NULL) {
        printf("Error: Could not open input file '%s'\n", in_filename);
        return -1;
    }

    FILE *fp2 = NULL;
    if (out_hex_filename != NULL) {
        fp2 = fopen(out_hex_filename, "w");
        if (fp2 == NULL) {
            printf("Warning: Could not open output hex file '%s' for writing\n", out_hex_filename);
        }
    }

    char line[128];
    char token_array[5][16]; 
    int line_num = 0;
    int buf_idx = 0;
    int error_count = 0;

    printf("--> Assembling '%s'...\n", in_filename);

    while (fgets(line, sizeof(line), fp1)) {
        line_num++;
        int dest_reg = 0, src_reg = 0, imm = 0;

        // Strip comments starting with ';' or '#'
        char *comment_pos = strpbrk(line, ";#");
        if (comment_pos) {
            *comment_pos = '\0';
        }

        int token_count = tokenize(line, token_array);
        if (token_count == 0) continue; // Skip blank or comment-only lines

        int opcode = get_opcode(token_array[0]);
        if (opcode == -1) {
            printf("[Line %d] Error: Unknown opcode '%s'\n", line_num, token_array[0]);
            error_count++;
            continue;
        }

        if (token_count != (table[opcode].operand_cnt + 1)) {
            printf("[Line %d] Error: Opcode '%s' expects %d operands, got %d\n",
                   line_num, token_array[0], table[opcode].operand_cnt, token_count - 1);
            error_count++;
            continue;
        }

        int check = parse_operands(opcode, &dest_reg, &src_reg, &imm, token_array);
        if (check == -1) {
            printf("[Line %d] Error: Failed to parse operands for '%s'\n", line_num, token_array[0]);
            error_count++;
            continue;
        }

        encode_and_write(opcode, dest_reg, src_reg, imm, fp2, program_buffer, &buf_idx, max_size);
    }

    if (fp2) fclose(fp2);
    fclose(fp1);

    if (error_count > 0) {
        printf("--> Assembly FAILED: %d error(s) found.\n", error_count);
        return -1;
    }

    printf("--> Assembly completed successfully: %d bytes generated.\n", buf_idx);
    return buf_idx;
}
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define MAG_0 0xFE
#define MAG_1 0xED
#define MAG_2 0xCA
#define MAG_3 0xFE

typedef enum instr {
	HLT_INSTR = 0x00,
	MOV_INSTR,
	ADD_INSTR,
	SUB_INSTR,
	MUL_INSTR,
	DIV_INSTR,
} instr;

#define MAX_ARGS 3

typedef enum instruction_type {
	REG_TYPE = 0,
	IMM_TYPE,
} instr_type;

typedef struct instr_arg {
	instr_type type;
	short val;
} instr_arg;

typedef struct instruction {
	short opcode;
	short regs[MAX_ARGS];
	short immediate;
} instruction;

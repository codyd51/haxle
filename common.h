#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

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
	//instr_arg regs[MAX_ARGS - 1];
	short regs[MAX_ARGS];
	short immediate;
	//instr_arg immediate;
} instruction;

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define HLT_INSTR  0x00
#define MOV_INSTR  0x01
#define ADD_INSTR  0x02

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

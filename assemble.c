#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ARGS 3

typedef enum instruction_type {
	REG_TYPE = 0,
	IMM_TYPE,
} instr_type;

typedef struct instr_arg {
	instr_type type;
	int val;
} instr_arg;

typedef struct instruction {
	unsigned opcode;
	instr_arg args[MAX_ARGS];
} instruction;

unsigned parse_op(char* op) {
	if (!strcmp(op, "hlt") || !strcmp(op, "HLT")) {
		return 0xCC;
	}

	printf("Parsing error: Unknown instruction %s\n", op);
	exit(1);
}

instr_arg* parse_arg(char* arg_str) {
	printf("parsing arg %s\n", arg_str);
	instr_arg* arg = malloc(sizeof(instr_arg));
	switch (arg_str[0]) {
		case '#':
			arg->type = IMM_TYPE;
			break;
		case 'r':
		case 'R':
			arg->type = REG_TYPE;
			break;
		default:
			printf("Parsing error: Unknown argument %s\n", arg_str);
			exit(1);
			break;
	}

	//hax
	//get substring(1, arg.len)
	char* val = (char*)&(arg_str[1]);
	int raw = atoi(val);
	arg->val = raw;

	return arg;
}

instruction* parse_instruction(char** tokens, int count) {
	instruction* ret = malloc(sizeof(instruction));
	ret->opcode = parse_op(tokens[0]);
	for (int i = 0; i < count - 1; i++) {
		ret->args[i] = *parse_arg(tokens[i + 1]);
	}
	return ret;
}

void dump_instruction(instruction* instr) {
	printf("0x%x\n", instr->opcode);
}

#define MAX_INSTRUCTIONS 256
int main(int argc, char** argv) {
	char* prog = "hlt";

	instruction** ast = malloc(sizeof(instruction) * MAX_INSTRUCTIONS);
	unsigned linenum = 0;
	
	//split by each line
	char* line = strtok(prog, "\n");
	while (line) {
		//split this line into tokens
		//TODO remove this constant
		char** tokens = malloc(sizeof(char*) * 32);
		char* token = strtok(line, " ");
		int token_count = 0;
		while (token) {
			tokens[token_count++] = token;
			token = strtok(NULL, " ");
		}

		//now that we've split the line into individual tokens,
		//parse the instruction this represents and add to instruction list
		ast[linenum++] = parse_instruction(tokens, token_count);

		line = strtok(NULL, "\n");
	}

	//program assembled!
	//print out opcodes
	for (int i = 0; i <	MAX_INSTRUCTIONS; i++) {
		if (!ast[i]) break;

		dump_instruction(ast[i]);
	}
	
	return 0;
}

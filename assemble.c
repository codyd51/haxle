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
	short val;
} instr_arg;

typedef struct instruction {
	short opcode;
	instr_arg args[MAX_ARGS];
} instruction;

short parse_op(char* op) {
	if (!strcmp(op, "hlt") || !strcmp(op, "HLT")) {
		return 0x00;
	}
	else if (!strcmp(op, "mov") || !strcmp(op, "MOV")) {
		return 0x01;
	}
	else if (!strcmp(op, "add") || !strcmp(op, "ADD")) {
		return 0x02;
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

	//count is not zero indexed
	//subtract 1 to fix that
	for (int i = 0; i < count - 1; i++) {
		//don't include instruction, only arguments
		//so use tokens[i + 1] to get index of arg
		//(index 0 is instruction)
		ret->args[i] = *parse_arg(tokens[i + 1]);
	}

	return ret;
}

void dump_instruction(instruction* instr) {
	printf("0x%x\n", instr->opcode);
}

void write_instruction(FILE* fp, instruction* instr) {	
	/*
	ret->instruction = (instr & 0xF000) >> 12;
	ret->reg_op0	 = (instr & 0xF00 ) >> 8;
	ret->reg_op1	 = (instr & 0xF0  ) >> 4;
	ret->reg_op2	 = (instr & 0xF   );
	ret->immediate	 = (instr & 0xFF  );
	*/
	int result = 0;
	result |= (instr->opcode) << 12;
	result |= (instr->args[0].val) << 8;
	result |= (instr->args[1].val) << 4;
	result |= (instr->args[2].val & 0xF) ;
	result |= (instr->args[3].val & 0xFF);

	printf("writing 0x%x\n", result);

	fwrite(&result, sizeof(result), 1, fp);
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

		char* copy = strdup(line);
		char* token = strtok(copy, " ");
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

	//assembled!
	//write assembly to binary
	FILE* fp = fopen("a.out", "w");
	if (!fp) {
		printf("Assembler error: Couldn't open output file for writing\n");
		exit(1);
	}

	//is using linenum here unsafe?
	for (int i = 0; i < linenum; i++) {
		write_instruction(fp, ast[i]);
	}
	fclose(fp);

	//program assembled!
	//print out opcodes
	/*
	for (int i = 0; i <	MAX_INSTRUCTIONS; i++) {
		if (!ast[i]) break;
		dump_instruction(ast[i]);
	}
	*/

	for (int i = 0; i < linenum; i++) {
		free(ast[i]);
	}
	free(ast);
	
	return 0;
}

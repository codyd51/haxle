#include "common.h"

typedef struct instr_p {
	short opcode;
	instr_arg args[MAX_ARGS];
} instr_p;

short parse_op(char* op) {
	if (!strcmp(op, "hlt") || !strcmp(op, "HLT")) {
		return HLT_INSTR;
	}
	else if (!strcmp(op, "mov") || !strcmp(op, "MOV")) {
		return MOV_INSTR;
	}
	else if (!strcmp(op, "add") || !strcmp(op, "ADD")) {
		return ADD_INSTR;
	}
	else if (!strcmp(op, "sub") || !strcmp(op, "SUB")) {
		return SUB_INSTR;
	}
	else if (!strcmp(op, "mul") || !strcmp(op, "MUL")) {
		return MUL_INSTR;
	}
	else if (!strcmp(op, "div") || !strcmp(op, "DIV")) {
		return DIV_INSTR;
	}

	printf("Parsing error: Unknown instruction %s\n", op);
	exit(1);
}

instr_arg* parse_arg(char* arg_str) {
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

instr_p* parse_instruction(char** tokens, int count) {
	instr_p* ret = malloc(sizeof(instr_p));
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

void dump_instruction(instr_p* instr) {
	printf("0x%x ", instr->opcode);
	for (int i = 0; i < MAX_ARGS; i++) {
		printf("0x%x ", instr->args[i].val);
	}
	printf("\n");
}

void write_instruction(FILE* fp, instr_p* instr) {	
	int result = 0;
	result |= (instr->opcode	 ) << 12;
	result |= (instr->args[0].val) << 8;
	result |= (instr->args[1].val) << 4;
	result |= (instr->args[2].val & 0xF);

	fwrite(&result, sizeof(result), 1, fp);
}

#define MAX_INSTRUCTIONS 256
int main(int argc, char** argv) {
	if (argc != 2) {
		printf("Please provide an input file.\n");
		return 1;
	}
	char* input = argv[1];
	FILE* input_file = fopen(input, "r");
	if (!input_file) {
		printf("Couldn't open file %s for reading.\n", input);
		return 1;
	}
	char prog[2048];
	fread(&prog, sizeof(char), sizeof(prog), input_file);
	fclose(input_file);

	instr_p** ast = malloc(sizeof(instr_p) * MAX_INSTRUCTIONS);
	unsigned linenum = 0;
	//spit by each line 
	char* line, *string, *tofree;
	line = string = prog;
	while ((line = strsep(&string, "\n")) != NULL) {
		if (!strlen(line)) continue;

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
		
		free(tokens);
		free(copy);
	}
	
	free(tofree);

	//assembled!
	//write assembly to binary
	FILE* fp = fopen("a.out", "w");
	if (!fp) {
		printf("Assembler error: Couldn't open output file for writing\n");
		exit(1);
	}

	//write magic to file
	char magic[] = {MAG_0, MAG_1, MAG_2, MAG_3};
	fwrite(magic, sizeof(char), 4, fp);

	//is using linenum here unsafe?
	for (int i = 0; i < linenum; i++) {
		write_instruction(fp, ast[i]);
	}
	fclose(fp);

	//program assembled!
	//print out opcodes
	printf("instr dump\n-------------\n");
	for (int i = 0; i <	MAX_INSTRUCTIONS; i++) {
		if (!ast[i]) break;
		dump_instruction(ast[i]);
	}

	for (int i = 0; i < linenum; i++) {
		free(ast[i]);
	}
	free(ast);
	
	return 0;
}

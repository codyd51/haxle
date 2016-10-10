#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define NUM_REGS 4
typedef struct cpu_state {
	unsigned regs[NUM_REGS];
} cpu_state;

typedef struct instruction {
	int instruction;
	int reg_op0, reg_op1, reg_op2;
	int immediate;
} instruction;

int fetch(int* prog) {
	//start at PC = 0;
	static unsigned pc = 0;
	return prog[pc++];
}

#define HLT_INSTR  0x00
#define LOAD_INSTR 0x01
#define ADD_INSTR  0x02

instruction* decode(unsigned instr) { 
	instruction* ret = malloc(sizeof(ret)); 
	ret->instruction = (instr & 0xF000) >> 12; 
	ret->reg_op0	 = (instr & 0xF00 ) >> 8;
	ret->reg_op1	 = (instr & 0xF0  ) >> 4;
	ret->reg_op2	 = (instr & 0xF   );
	ret->immediate	 = (instr & 0xFF  );
	return ret;
}

static bool running = true;
void eval(cpu_state* state, instruction* instr) {
	switch (instr->instruction) {
		case LOAD_INSTR:
			printf("LOAD R%d, #%d", instr->reg_op0, instr->immediate);
			state->regs[instr->reg_op0] = instr->immediate;
			break;
		case ADD_INSTR:
			printf("ADD R%d, R%d + R%d", instr->reg_op0, instr->reg_op1, instr->reg_op2);
			state->regs[instr->reg_op0] = instr->reg_op1 + instr->reg_op2;
			break;
		default:
			printf("Unknown instruction %x!\n", instr->instruction);
			//fall through to HLT case
		case HLT_INSTR:
			printf("HLT");
			running = false;
			break;
	}
	printf("\n");
}

void dump_cpu_state(cpu_state* state) {
	for (int i = 0; i < NUM_REGS; i++) { 
		printf("R%d = %d\t", i, state->regs[i]);
	}
	printf("\n");
}

void run(int* prog) {
	cpu_state* state = malloc(sizeof(cpu_state));
	memset(state, 0, sizeof(cpu_state));

	while (running) {
		unsigned instr = fetch(prog);
		instruction* real = decode(instr);
		eval(state, real);
		dump_cpu_state(state);
	}
}

int main(int argc, char** argv) {
	if (argc != 2) {
		printf("Please provide an executable file.\n");
		return 1;
	}

	//int prog[] = {0x1064, 0x11C8, 0x2201, 0x0000};
	char* name = argv[1];
	FILE* fp = fopen(name, "r");
	if (!fp) {
		printf("Couldn't open file %s for execution.\n", name);
		return 1;
	}

	int prog[2048];
	fread(&prog, sizeof(int), sizeof(prog), fp);

	for (int i = 0; i < 10; i++) {
		printf("%x ", prog[i]);
	}
	printf("\n");

	run(prog);

	return 0;
}


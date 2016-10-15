#include "common.h"

#define NUM_REGS 4
typedef struct cpu_state {
	unsigned regs[NUM_REGS];
} cpu_state;

typedef struct haxle_instance {
	bool running; //still executing if set

	int* bytecode; //bytecode to execute in this instance
	unsigned pc; //program counter

	cpu_state state; //register state
} haxle_instance;

int fetch(haxle_instance* vm) {
	return vm->bytecode[vm->pc++];
}

instruction* decode(int instr) { 
	printf("(0x%x) ", instr);
	
	instruction* ret = malloc(sizeof(ret)); 
	ret->opcode	   = (instr & 0xF000) >> 12;
	ret->regs[0]   = (instr & 0xF00 ) >> 8;
	ret->regs[1]   = (instr & 0xF0  ) >> 4;
	ret->regs[2]   = (instr & 0xF	);
	ret->immediate = (instr & 0xF0  ) >> 4;
	
	return ret;
}

void eval(haxle_instance* vm, instruction* instr) {
	cpu_state* state = &vm->state;

	switch (instr->opcode) {
		case MOV_INSTR:
			printf("MOV R%d, #%d", instr->regs[0], instr->immediate);
			state->regs[instr->regs[0]] = instr->immediate;
			break;
		case ADD_INSTR:
			printf("ADD R%d, R%d + R%d", instr->regs[0], instr->regs[1], instr->regs[2]);
			state->regs[instr->regs[0]] = state->regs[instr->regs[1]] + state->regs[instr->regs[2]];
			break;
		case SUB_INSTR:
			printf("SUB R%d, R%d - R%d", instr->regs[0], instr->regs[1], instr->regs[2]);
			state->regs[instr->regs[0]] = state->regs[instr->regs[1]] - state->regs[instr->regs[2]];
			break;
		case MUL_INSTR:
			printf("MUL R%d, R%d * R%d", instr->regs[0], instr->regs[1], instr->regs[2]);
			state->regs[instr->regs[0]] = state->regs[instr->regs[1]] * state->regs[instr->regs[2]];
			break;
		case DIV_INSTR:
			printf("DIV R%d, R%d / R%d", instr->regs[0], instr->regs[1], instr->regs[2]);
			state->regs[instr->regs[0]] = state->regs[instr->regs[1]] / state->regs[instr->regs[2]];
			break;
		default:
			printf("Unknown instruction %x!\n", instr->opcode);
			//fall through to HLT case
		case HLT_INSTR:
			printf("HLT");
			vm->running = false;
			break;
	}
	printf("\n");
}

void dump_cpu_state(cpu_state state) {
	printf("--- CPU state: ");
	for (int i = 0; i < NUM_REGS; i++) { 
		printf("R%d = %d\t", i, state.regs[i]);
	} printf(" ---\n"); 
}

void run(haxle_instance* vm) {
	while (vm->running) {
		unsigned instr = fetch(vm);
		instruction* real = decode(instr);
		eval(vm, real);
	}
	dump_cpu_state(vm->state);
}

haxle_instance* haxle_init(int* bytecode) {
	haxle_instance* vm = malloc(sizeof(haxle_instance));
	memset(vm, 0, sizeof(haxle_instance));
	vm->running = true;
	vm->bytecode = bytecode;
	vm->pc = 0;
	return vm;
}

int main(int argc, char** argv) {
	if (argc != 2) {
		printf("Please provide an executable file.\n");
		return 1;
	}

	char* name = argv[1];
	FILE* fp = fopen(name, "r");
	if (!fp) {
		printf("Couldn't open file %s for execution.\n", name);
		return 1;
	}

#define MAX_SIZE 2048
	int* file_data = malloc(sizeof(int) * MAX_SIZE);
	fread(file_data, sizeof(int), MAX_SIZE, fp);

	//verify magic of file
	//cast to uint8_t to read individual bytes
	uint8_t* magic = (uint8_t*)file_data;
	printf("magic: %x\n", (int)*file_data);
	if (magic[0] != MAG_0 || magic[1] != MAG_1 || magic[2] != MAG_2 || magic[3] != MAG_3) {
		printf("File %s contained invalid magic. (%x)\n", name, (int)*file_data);
		exit(1);
	}

	//strip magic from VM bytecode
	//do so by dropping first 4 bytes
	//an int is exactly 4 bytes, so add 1 int size to original pointer 
	int* prog = file_data + 1;

	haxle_instance* vm = haxle_init(prog);
	run(vm);

	free(file_data);

	return 0;
}

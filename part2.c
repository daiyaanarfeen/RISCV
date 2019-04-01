#include <stdio.h> // for stderr
#include <stdlib.h> // for exit()
#include "types.h"
#include "utils.h"
#include "riscv.h"

void execute_rtype(Instruction, Processor *);
void execute_itype_except_load(Instruction, Processor *);
void execute_branch(Instruction, Processor *);
void execute_jal(Instruction, Processor *);
void execute_load(Instruction, Processor *, Byte *);
void execute_store(Instruction, Processor *, Byte *);
void execute_ecall(Processor *, Byte *);
void execute_lui(Instruction, Processor *);

void execute_instruction(Instruction instruction,Processor *processor,Byte *memory) {    
	/* YOUR CODE HERE: COMPLETE THE SWITCH STATEMENTS */
	// decode_instruction(instruction);
	unsigned int opcode = instruction.opcode;
	switch(opcode) { // What do we switch on?
		case 3:
			execute_load(instruction, processor, memory);
			processor->PC = processor->PC + 4;
			break;
		case 19:
			execute_itype_except_load(instruction, processor);
			processor->PC = processor->PC + 4;
			break;
		case 51:
			execute_rtype(instruction, processor);
			processor->PC = processor->PC + 4;
			break;
		case 55:
			execute_lui(instruction, processor);
            processor->PC = processor->PC + 4;
			break;
		case 111:
			execute_jal(instruction, processor);
			break;
		case 35:
			execute_store(instruction, processor, memory);
            processor->PC = processor->PC + 4;
			break;
		case 99:
			execute_branch(instruction, processor);
			break;
		case 115:
			execute_ecall(processor, memory);
            processor->PC = processor->PC + 4;
			break;
		default: // undefined opcode
			handle_invalid_instruction(instruction);
			exit(-1);
			break;
	}
}

void execute_rtype(Instruction instruction, Processor *processor) {
	unsigned int f3 = instruction.rtype.funct3;
	unsigned int f7 = instruction.rtype.funct7;
	switch(f3) { // What do we switch on?
		/* YOUR CODE HERE */
		case 0:
			switch(f7) {
				case 0:
					processor->R[instruction.rtype.rd] = processor->R[instruction.rtype.rs1] + processor->R[instruction.rtype.rs2];
					break;
				case 1:
					processor->R[instruction.rtype.rd] = processor->R[instruction.rtype.rs1] * processor->R[instruction.rtype.rs2];
					break;
				case 32:
					processor->R[instruction.rtype.rd] = processor->R[instruction.rtype.rs1] - processor->R[instruction.rtype.rs2];
					break;
			}
			break;
		case 1:
			if (f7 == 0) {
				processor->R[instruction.rtype.rd] = processor->R[instruction.rtype.rs1] << processor->R[instruction.rtype.rs2];
			} else {
				int64_t val = ((int64_t) processor->R[instruction.rtype.rs1]) * ((int64_t) processor->R[instruction.rtype.rs2]);
				val = val >> 32;
				processor->R[instruction.rtype.rd] = (int32_t) val;
			}
			break;
		case 2: 
			processor->R[instruction.rtype.rd] = ((int) processor->R[instruction.rtype.rs1]) < ((int) processor->R[instruction.rtype.rs2]);
			break;
		case 4:
			if (f7 == 0) {
				processor->R[instruction.rtype.rd] = processor->R[instruction.rtype.rs1] ^ processor->R[instruction.rtype.rs2];
			} else {
				processor->R[instruction.rtype.rd] = processor->R[instruction.rtype.rs1] / processor->R[instruction.rtype.rs2];
			}
			break;
		case 5:
			if (f7 == 0) {
				processor->R[instruction.rtype.rd] = processor->R[instruction.rtype.rs1] >> processor->R[instruction.rtype.rs2];
			} else {
				processor->R[instruction.rtype.rd] = ((int) processor->R[instruction.rtype.rs1]) >> processor->R[instruction.rtype.rs2];
			}
			break;
		case 6:
			if (f7 == 0) {
				processor->R[instruction.rtype.rd] = processor->R[instruction.rtype.rs1] | processor->R[instruction.rtype.rs2];
			} else {
				processor->R[instruction.rtype.rd] = processor->R[instruction.rtype.rs1] % processor->R[instruction.rtype.rs2];
			}
			break;
		case 7:
			processor->R[instruction.rtype.rd] = processor->R[instruction.rtype.rs1] & processor->R[instruction.rtype.rs2];
			break;
		default:
			handle_invalid_instruction(instruction);
			break;
	}
}

void execute_itype_except_load(Instruction instruction, Processor *processor) {
	// int shiftOp;
	// shiftOp = -1;
	int imm = 0;
	switch(instruction.itype.funct3) { // What do we switch on?
		/* YOUR CODE HERE */
		case 0x0:
			imm = bitSigner(instruction.itype.imm, 12);
			processor->R[instruction.itype.rd] = processor->R[instruction.itype.rs1] + imm;
			break;
		case 0x1:
			imm = bitSigner(instruction.itype.imm, 12);
			processor->R[instruction.itype.rd] = processor->R[instruction.itype.rs1] << imm;
			break;
		case 0x2:
			imm = bitSigner(instruction.itype.imm, 12);
			processor->R[instruction.itype.rd] = ((int) processor->R[instruction.itype.rs1]) < imm;
			break;
		case 0x4:
			imm = bitSigner(instruction.itype.imm, 12);
			processor->R[instruction.itype.rd] = processor->R[instruction.itype.rs1] ^ imm;
			break;
		case 0x5:
			if (instruction.itype.imm < 1024) {
				imm = bitSigner(instruction.itype.imm, 6);
				processor->R[instruction.rtype.rd] = processor->R[instruction.rtype.rs1] >> imm;
			} else {
				imm = bitSigner(instruction.itype.imm & 3071, 6);
				processor->R[instruction.rtype.rd] = ((int) processor->R[instruction.rtype.rs1]) >> imm;
			}
			break;
		case 0x6:
			imm = bitSigner(instruction.itype.imm, 12);
			processor->R[instruction.itype.rd] = processor->R[instruction.itype.rs1] | imm;
			break;
		case 0x7:
			imm = bitSigner(instruction.itype.imm, 12);
			processor->R[instruction.itype.rd] = processor->R[instruction.itype.rs1] & imm;
			break;
		default:
			handle_invalid_instruction(instruction);
			break;  
	}
}

void execute_ecall(Processor *p, Byte *memory) {
	int a0 = p->R[10];
	switch(a0) { // What do we switch on?
		/* YOUR CODE HERE */
		case 1:
			printf("%d", p->R[11]);
			break;
		case 10:
			printf("exiting the simulator\n");
			exit(0);
			break;
		default: // undefined ecall
			printf("Illegal ecall number %d\n", -1); // What stores the ecall arg?
			exit(-1);
			break;
	}
}

void execute_branch(Instruction instruction, Processor *processor) {
	// int branchaddr;
	// branchaddr = 0;
	/* Remember that the immediate portion of branches
	   is counting half-words, so make sure to account for that. */
	switch(instruction.sbtype.funct3) { // What do we switch on?
		/* YOUR CODE HERE */
		case 0x0:
			if (((int) processor->R[instruction.sbtype.rs1]) == ((int) processor->R[instruction.sbtype.rs2])) {
				processor->PC += get_branch_offset(instruction);
			} else {
				processor->PC += 4;
			}
			break;
		case 0x1:
			if (processor->R[instruction.sbtype.rs1] != processor->R[instruction.sbtype.rs2]) {
				processor->PC += get_branch_offset(instruction);
			} else {
				processor->PC += 4;
			}
			break;
		default:
			handle_invalid_instruction(instruction);
			break;
	}
}

void execute_load(Instruction instruction, Processor *processor, Byte *memory) {
	int funct3 = instruction.itype.funct3;
	int address = processor->R[instruction.itype.rs1] + instruction.itype.imm;
	int loadedByte = 0;
	Alignment alignment = 0;
	switch(funct3) { // What do we switch on?
		/* YOUR CODE HERE */
		case 0x0:
			//sb
			alignment = LENGTH_BYTE;
			loadedByte = load(memory, address, alignment, 0);
			processor->R[instruction.itype.rd] = loadedByte;
			break;
		case 0x1:
			//sh
			alignment = LENGTH_HALF_WORD;
			loadedByte = load(memory, address, alignment, 0);
			processor->R[instruction.itype.rd] = loadedByte;
			break;
		case 0x2:
			//sw
			alignment = LENGTH_WORD;
			loadedByte = load(memory, address, alignment, 0);
			processor->R[instruction.itype.rd] = loadedByte;
			break;
		default:
			handle_invalid_instruction(instruction);
			break;
	}
}

void execute_store(Instruction instruction, Processor *processor, Byte *memory) {
	// int byte = 0xff;
	// int half = 0xffff;
	// int word = 0xffffffff;
	int funct3 = instruction.stype.funct3;
	int address = processor->R[instruction.stype.rs1] + get_store_offset(instruction);
	int storedByte = processor->R[instruction.stype.rs2];
	Alignment alignment = 0;
	switch(funct3) { // What do we switch on?
		/* YOUR CODE HERE */
		case 0x0:
			//sb
			alignment = LENGTH_BYTE;
			store(memory, address, alignment, storedByte, 0);
			break;
		case 0x1:
			//sh
			alignment = LENGTH_HALF_WORD;
			store(memory, address, alignment, storedByte, 0);
			break;
		case 0x2:
			//sw
			alignment = LENGTH_WORD;
			store(memory, address, alignment, storedByte, 0);
			break;
		default:
			handle_invalid_instruction(instruction);
			exit(-1);
			break;
	}
}

void execute_jal(Instruction instruction, Processor *processor) {
	/* Remember that the immediate and offset are counting half-words.
	   So make sure to plan accordingly to accomodate that. */
	// int nextPC;
	// nextPC = 0;
	/* YOUR CODE HERE */
	processor->R[instruction.ujtype.rd] = processor->PC + 4;
	processor->PC += get_jump_offset(instruction);
}

void execute_lui(Instruction instruction, Processor *processor) {
	// int imm;
	// imm = 0;
	/* YOUR CODE HERE */
	int imm = instruction.utype.imm;
	imm = imm << 12;
	processor->R[instruction.utype.rd] = (Word) imm;
}

/* Checks that the address is aligned correctly */
int check(Address address,Alignment alignment) {
	if(address>0 && address < MEMORY_SPACE){
		if(alignment == LENGTH_BYTE){
			return 1;
		}
		else if( alignment == LENGTH_HALF_WORD ){
			return address%2 == 0;
		}
		else if (alignment == LENGTH_WORD){
			return address%4 ==0;
		}
	}
	
	return 0;
	
}

void store(Byte *memory,Address address, Alignment alignment, Word value, int check_align) {
	if((check_align && !check(address,alignment)) || (address >= MEMORY_SPACE)) {
		handle_invalid_write(address);
	}
	/* YOUR CODE HERE */
	// Bytes are read little endian, if bytes are 0xabcd, first byte is d.
	Byte fourth = (Byte) (value >> 24) & 0xff;
	// value = value >> 8;
	Byte third = (Byte) (value >> 16) & 0xff;
	// value = value >> 8;
	Byte second = (Byte) (value >> 8) & 0xff;
	// value = value >> 8;
	Byte first = (Byte) value & 0xff;
	// printf("STORE:\n");
	// printf("first %d\n", first);
	// printf("second %d\n", second);
	// printf("third %d\n", third);
	// printf("fourth %d\n", fourth);
	switch (alignment) {
		case LENGTH_BYTE:
			memory[address] = first;
			break;
		case LENGTH_HALF_WORD:
			memory[address] = first;
			memory[address + 1] = second;
			break;
		case LENGTH_WORD:
			memory[address] = first;
			memory[address + 1] = second;
			memory[address + 2] = third;
			memory[address + 3] = fourth;
			break;
		default:
			printf("Invalid alignment of size %d\n", (int) alignment);
			exit(0);
			break;
	}
}

Word load(Byte *memory,Address address,Alignment alignment, int check_align) {
	if((check_align && !check(address,alignment)) || (address >= MEMORY_SPACE)) {
	handle_invalid_read(address);
	}
	
	/* YOUR CODE HERE */
	uint32_t data = 0; // initialize our return value to zero
	Byte first = memory[address];
	Byte second = memory[address + 1];
	Byte third = memory[address + 2];
	Byte fourth = memory[address + 3];
	// printf("LOAD:\n");
	// printf("first %d\n", first);
	// printf("second %d\n", second);
	// printf("third %d\n", third);
	// printf("fourth %d\n", fourth);
	switch (alignment) {
		case LENGTH_BYTE:
			data += first;
			data = bitSigner(data, LENGTH_BYTE * 8);
			return data;
			break;
		case LENGTH_HALF_WORD:
			data += second;
			data = data << 8;
			data += first;
			data = bitSigner(data, LENGTH_HALF_WORD * 8);
			return data;
			break;
		case LENGTH_WORD:
			data += fourth;
			data = data << 8;
			data += third;
			data = data << 8;
			data += second;
			data = data << 8;
			data += first;
			return data;
			break;
		default:
			printf("Invalid alignment of size %d\n", (int) alignment);
			exit(0);
			return data;
			break;
	}
}



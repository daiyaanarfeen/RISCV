#include <stdio.h> // for stderr
#include <stdlib.h> // for exit()
#include "types.h"
#include "utils.h"

void print_rtype(char *, Instruction);
void print_itype_except_load(char *, Instruction, int);
void print_load(char *, Instruction);
void print_store(char *, Instruction);
void print_branch(char *, Instruction);
void write_rtype(Instruction);
void write_itype_except_load(Instruction); 
void write_load(Instruction);
void write_store(Instruction);
void write_branch(Instruction);
void write_lui(Instruction);
void write_jal(Instruction);
void write_ecall(Instruction);


void decode_instruction(Instruction instruction) {
    /* YOUR CODE HERE: COMPLETE THE SWITCH STATEMENTS */
    unsigned int opcode = instruction.opcode;
    switch(opcode) { // What do we switch on?
        case 3:
            write_load(instruction);
            break;

        case 19:
            write_itype_except_load(instruction);
            break;

        case 51:
            write_rtype(instruction);
            break;

        case 55:
            write_lui(instruction);
            break;

        case 111:
            write_jal(instruction);
            break;

        case 35:
            write_store(instruction);
            break;

        case 99:
            write_branch(instruction);
            break;

        case 115:
            write_ecall(instruction);
            break;

    	default: // undefined opcode
            handle_invalid_instruction(instruction);
    	    break;
    }
}

void write_rtype(Instruction instruction) {
    char *name = "";
    unsigned int f3 = instruction.rtype.funct3;
    unsigned int f7 = instruction.rtype.funct7;
	switch(f3) { // What do we switch on?
        /* YOUR CODE HERE */
        case 0:
            switch(f7) {
                case 0:
                    name = "add";
                    break;
                case 1:
                    name = "mul";
                    break;
                case 32:
                    name = "sub";
                    break;
            }
            break;
        case 1:
            if (f7 == 0) {
                name = "sll";
            } else {
                name = "mulh";
            }
            break;
        case 2: 
            name = "slt";
            break;
        case 4:
            if (f7 == 0) {
                name = "xor";
            } else {
                name = "div";
            }
            break;
        case 5:
            if (f7 == 0) {
                name = "srl";
            } else {
                name = "sra";
            }
            break;
        case 6:
            if (f7 == 0) {
                name = "or";
            } else {
                name = "rem";
            }
            break;
        case 7:
            name = "and";
            break;
	    default:
            handle_invalid_instruction(instruction);
            break;
    }
    print_rtype(name, instruction);
}

void write_itype_except_load(Instruction instruction) {
    char *name = "";
    // int shiftOp;
    // shiftOp = -1;
    int imm = 0;
    switch(instruction.itype.funct3) { // What do we switch on?
        /* YOUR CODE HERE */
        case 0x0:
            name = "addi";
            imm = bitSigner(instruction.itype.imm, 12);
            break;
        case 0x1:
            name = "slli";
            imm = bitSigner(instruction.itype.imm, 12);
            break;
        case 0x2:
            name = "slti";
            imm = bitSigner(instruction.itype.imm, 12);
            break;
        case 0x4:
            name = "xori";
            imm = bitSigner(instruction.itype.imm, 12);
            break;
        case 0x5:
            if (instruction.itype.imm < 1024) {
                name = "srli";
                imm = bitSigner(instruction.itype.imm, 6);
            } else {
                name = "srai";
                imm = bitSigner(instruction.itype.imm & 3071, 6);
            }
            break;
        case 0x6:
            name = "ori";
            imm = bitSigner(instruction.itype.imm, 12);
            break;
        case 0x7:
            name = "andi";
            imm = bitSigner(instruction.itype.imm, 12);
            break;
        default:
            handle_invalid_instruction(instruction);
            break;  
    }
    print_itype_except_load(name, instruction, imm);
}

void write_load(Instruction instruction) {
    char *name = "";
    switch(instruction.itype.funct3) { // What do we switch on?
        /* YOUR CODE HERE */
        case 0x0:
            name = "lb";
            break;
        case 0x1:
            name = "lh";
            break;
        case 0x2:
            name = "lw";
            break;
        default:
            handle_invalid_instruction(instruction);
            break;
    }
    print_load(name, instruction);
}

void write_store(Instruction instruction) {
    char *name = "";
    switch(instruction.stype.funct3) { // What do we switch on?
        /* YOUR CODE HERE */
        case 0x0:
            name = "sb";
            break;
        case 0x1:
            name = "sh";
            break;
        case 0x2:
            name = "sw";
            break;
        default:
            handle_invalid_instruction(instruction);
            break;
    }
    print_store(name, instruction);
}

void write_branch(Instruction instruction) {
    char *name = "";
    switch(instruction.sbtype.funct3) { // What do we switch on?
        /* YOUR CODE HERE */
        case 0x0:
            name = "beq";
            break;
        case 0x1:
            name = "bne";
            break;
        default:
            handle_invalid_instruction(instruction);
            break;
    }
    print_branch(name, instruction);
}

/* For the writes, probably a good idea to take a look at utils.h */

void write_lui(Instruction instruction) {
    /* YOUR CODE HERE */
    unsigned int rd = instruction.utype.rd;
    unsigned int imm = instruction.utype.imm;
    printf(LUI_FORMAT, rd, imm);
}

void write_jal(Instruction instruction) {
    /* YOUR CODE HERE */
    unsigned int rd = instruction.ujtype.rd;
    unsigned int imm = get_jump_offset(instruction);
    printf(JAL_FORMAT, rd, imm);
}

void write_ecall(Instruction instruction) {
    /* YOUR CODE HERE */
    printf(ECALL_FORMAT);
}

void print_rtype(char *name, Instruction instruction) {
    /* YOUR CODE HERE */
    unsigned int rd = instruction.rtype.rd;
    unsigned int rs1 = instruction.rtype.rs1;
    unsigned int rs2 = instruction.rtype.rs2;
    printf(RTYPE_FORMAT, name, rd, rs1, rs2);
}

void print_itype_except_load(char *name, Instruction instruction, int imm) {
    /* YOUR CODE HERE */
    unsigned int rd = instruction.itype.rd;
    unsigned int rs1 = instruction.itype.rs1;
    printf(ITYPE_FORMAT, name, rd, rs1, imm);
}

void print_load(char *name, Instruction instruction) {
    /* YOUR CODE HERE */
    unsigned int rd = instruction.itype.rd;
    unsigned int rs1 = instruction.itype.rs1;
    int imm = bitSigner(instruction.itype.imm, 12);
    printf(MEM_FORMAT, name, rd, imm, rs1);
}

void print_store(char *name, Instruction instruction) {
    /* YOUR CODE HERE */
    unsigned int rs1 = instruction.stype.rs1;
    unsigned int rs2 = instruction.stype.rs2;
    int imm = get_store_offset(instruction);
    printf(MEM_FORMAT, name, rs2, imm, rs1);
}

void print_branch(char *name, Instruction instruction) {
    /* YOUR CODE HERE */
    unsigned int rs1 = instruction.sbtype.rs1;
    unsigned int rs2 = instruction.sbtype.rs2;
    int imm = get_branch_offset(instruction);
    printf(BRANCH_FORMAT, name, rs1, rs2, imm);
}

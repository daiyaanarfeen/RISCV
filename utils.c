#include "utils.h"
#include <stdio.h>
#include <stdlib.h>

//sign extends a bitfield with given size
/* You may find implementing this function helpful */
int bitSigner(unsigned int field, unsigned int size){
    /* YOUR CODE HERE */
    unsigned int sign_bit = (field & ( 1 << (size - 1)));
    unsigned int sign_extender = sign_bit;
    int i;
    for (i = 0; i < (32 - size); i++) {
        sign_bit = sign_bit << 1;
        sign_extender += sign_bit;
    }
    int extended = field | sign_extender;
    return extended;  
}

int get_branch_offset(Instruction instruction) {
    /* YOUR CODE HERE */
    unsigned int eleven = (instruction.sbtype.imm5 & 1) << 10;
    unsigned int first_four = instruction.sbtype.imm5 >> 1;
    unsigned int five_through_ten = (instruction.sbtype.imm7 & 63) << 4;
    unsigned int twelve = (instruction.sbtype.imm7 & 64) << 5;
    return bitSigner(first_four + five_through_ten + eleven + twelve, 12) << 1;
}

int get_jump_offset(Instruction instruction) {
    /* YOUR CODE HERE */
    unsigned int scrambled = instruction.ujtype.imm;
    unsigned int first_ten = (scrambled & 523776) >> 9;
    unsigned int eleven = (scrambled & 256) << 2;
    unsigned int twelve_through_nineteen = (scrambled & 255) << 11;
    unsigned int twenty = (scrambled & 524288);
    return bitSigner(first_ten + eleven + twelve_through_nineteen + twenty, 20) << 1;
}

int get_store_offset(Instruction instruction) {
    /* YOUR CODE HERE */
    unsigned int first_five = instruction.stype.imm5;
    unsigned int five_through_eleven = instruction.stype.imm7 << 5;
    return bitSigner(first_five + five_through_eleven, 12);
}

void handle_invalid_instruction(Instruction instruction) {
    printf("Invalid Instruction: 0x%08x\n", instruction.bits);
    printf("Opcode found: 0x%08x\n", instruction.opcode);
    printf("Funct3 found: 0x%08x\n", instruction.sbtype.funct3);
}

void handle_invalid_read(Address address) {
    printf("Bad Read. Address: 0x%08x\n", address);
    exit(-1);
}

void handle_invalid_write(Address address) {
    printf("Bad Write. Address: 0x%08x\n", address);
    exit(-1);
}


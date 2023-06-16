#include <stdio.h>
#include <math.h>
#include "main.c"

struct RegisterFile {
    int registers [16]; //16
};

struct MemoryFile{
    int * mem_file ;
};


//initializing registerFile
struct RegisterFile registerFile = {
        { 0 ,2 , 2, 4 , 34 ,21 ,
                5 , 10 , 4 , 22 , 11 , 23
                ,12 , 12 , 12 ,4}
};

//initializing memory
struct MemoryFile memFile;


void initial_memories(struct Program * runningProgram);
boolean fetch(struct Program * runningProgram, int PC);
void decode(int rs , int rt , int rd ,int imm,  int PC);
void exe(int rsData , int rtDAta ,int rt , int rd ,int imm, int PC);

void initial_memories(struct Program * runningProgram){

    memFile.mem_file = malloc(sizeof(int)*16000000000);


    for (int i = 0; i < 16000000000 ; ++i) {
        memFile.mem_file[i] = 0;
    }

    fetch(runningProgram , 0);
}



boolean fetch(struct Program * runningProgram, int PC){

    while (1) {

        if(PC == -1)
            break;
        //read the current address using pc and load it here
        struct Instruction new_instruction_line = runningProgram->instructions[PC];
        decode(new_instruction_line.rs , new_instruction_line.rt , new_instruction_line.rd , new_instruction_line.imm ,PC+4);
    }

    return 1;
}

void decode(int rs , int rt , int rd ,int imm, int PC){

    int readBus1 = registerFile.registers[rs];
    int readBus2 = registerFile.registers[rt];

    exe(readBus1, readBus2 ,rt ,  rd , imm,PC);
}

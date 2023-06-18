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
void decode(int rs , int rt , int rd, int opCode ,int imm, enum Types type , int PC);
void exe(int rsData , int rtDAta ,int rt , int rd , int opCode ,int imm, enum Types type, int PC );
int ALU(int opCode ,int typeSecond , int firstBus , int secondBus);
int decToBinary(int n , int type);
void memory(int aluRes , int opCode , int rt);


//functions ======================================================================================

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
        decode(new_instruction_line.rs , new_instruction_line.rt , new_instruction_line.rd,
                new_instruction_line.opCode , new_instruction_line.imm ,new_instruction_line.insType ,PC+1);
    }

    return 1;
}

void decode(int rs , int rt , int rd , int opCode ,int imm,enum Types type, int PC){

    int readBus1 = registerFile.registers[rs];
    int readBus2 = registerFile.registers[rt];

    exe(readBus1, readBus2 ,rt ,  rd ,opCode, imm, type ,PC);
}

void exe(int rsData , int rtDAta ,int rt , int rd, int opCOde ,int imm ,enum Types type, int PC){

    int firstBus = rsData;
    int secondBus;
    int secondType;

    switch (type) {

        case Itype:
            secondBus = imm;
            secondType=1;
            break;
        case Rtype:
            secondBus = rt;
            secondType= 0;
            break;
    }

    int output = ALU(opCOde , secondType , firstBus , secondBus);
    if (opCOde == 11) {
        branching();
        return;
    }

    memory(output , opCOde , rtDAta);
}

int ALU(int opCode , int type , int firstBus , int secondBus) {

    switch (opCode) {

        case 0:
            return firstBus + secondBus;
        case 1:
            return firstBus - secondBus;
        case 2:
            if (secondBus - firstBus > 0)
                return 1;
            return 0;
        case 3:
            return decToBinary(firstBus , 0) | decToBinary(secondBus ,type );
        case 4:
            return !(decToBinary(firstBus ,0 ) & decToBinary(secondBus , type));
        case 5:
            return firstBus + secondBus;
        case 6:
            return decToBinary(firstBus , 0) | decToBinary(secondBus , type);
        case 7:
            if (secondBus - firstBus > 0)
                return 1;
            return 0;
        case 8:
            return decToBinary(firstBus , 0) >> 16u;
        case 9:
            return firstBus + secondBus;

        case 10:
            return firstBus + secondBus;
        case 11:
            if (secondBus-firstBus ==0)
                return 0;
            return 1;
        default:
            return 0;
    }
}


void memory(int aluRes , int opCode , int rt){

}



int decToBinary(int n , int type)
{
    // array to store binary number
    int binaryNum[32];

    switch (type) {

        case 1 :
            //imm should be extended unsigned
            for (int i= 0 ; i<32 ;++i)
                binaryNum[i] = 0;
            break;
        default:
            //rs , rt should be extended signed
            if (n <0) {
                for (int i = 0; i < 32; ++i)
                    binaryNum[i] = 1;
            } else
                for (int i= 0 ; i<32 ;++i)
                    binaryNum[i] = 0;
            break;
    }



    // counter for binary array
    int i = 0;

    while (n > 0) {
        // storing remainder in binary array
        binaryNum[i] = n % 2;
        n = n / 2;
        i++;
    }

    int answer = 0;
    int power = 0;
    // printing binary array in reverse order
    for (int j =0 ; j < 32 ; ++j){

        int temp = binaryNum[j] * ((int)pow(10 , power));
        ++power;
        answer += temp;
    }

    return  answer;
}
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
void decode(struct Instruction instruction);
void exe(int rsData , int rtDAta ,struct Instruction instruction );
int ALU(int opCode ,int typeSecond , int firstBus , int secondBus);
int * decToBinary(int number);
void memory(int aluRes , struct Instruction instruction  , int rtData);
void writeBack(int aluResult , int memoryResult , struct Instruction instruction );
int binaryToDec(int * binaryNum);
void branching();

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
        decode(new_instruction_line);
    }

    return 1;
}

void decode(struct Instruction instruction){

    int readBus1 = registerFile.registers[instruction.rs];
    int readBus2 = registerFile.registers[instruction.rt];

    exe(readBus1, readBus2 ,instruction);
}

void exe(int rsData , int rtDAta , struct Instruction instruction){

    int firstBus = rsData;
    int secondBus;
    int secondType;

    switch (instruction.insType) {

        case Itype:
            secondBus = instruction.imm;
            secondType=1;
            break;
        case Rtype:
            secondBus = instruction.rt;
            secondType= 0;
            break;
        default:
            break;
    }

    int output = ALU(instruction.opCode , secondType , firstBus , secondBus);
    if (instruction.opCode == 11) {
        branching();
        return;
    }

    memory(output , instruction , rtDAta);
}

int ALU(int opCode , int type , int firstBus , int secondBus) {


    int * first ;
    int * second;
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
             first = decToBinary(firstBus);
            second = decToBinary(secondBus);

            for(int i = 0; i < 32; i++)
                first[i] = first[i] | second[i];

            return binaryToDec(first);
        case 4:
            first = decToBinary(firstBus);
            second = decToBinary(secondBus);

            for(int i = 0; i < 32; i++)
                first[i] = first[i] & second[i];

            return binaryToDec(first);

        case 5:
            return firstBus + secondBus;
        case 6:
            first = decToBinary(firstBus);
            second = decToBinary(abs(secondBus)); //ori should be unsigned

            for(int i = 0; i < 32; i++)
                first[i] = first[i] | second[i];

            return binaryToDec(first);
        case 7:
            if (secondBus - firstBus > 0)
                return 1;
            return 0;
//        case 8:
//            return binaryToDec(decToBinary(firstBus , 0) >> 16u);
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


void memory(int aluRes ,struct Instruction instruction , int rtData){

    int lw_result = 0;
    if (instruction.opCode == 9) //lw
        lw_result = memFile.mem_file[aluRes];
    else if (instruction.opCode == 10) //sw
        memFile.mem_file[aluRes] = rtData;

    if (instruction.opCode != 10)
        writeBack(aluRes ,lw_result ,instruction);
}


void writeBack(int aluResult , int memoryResult ,struct Instruction instruction ){

    int result = 0;

    if(instruction.opCode == 9)//if memory result by lw should be chosen
        result = memoryResult;
    else
        result = aluResult;

    if (0 <= instruction.opCode && instruction.opCode <= 4)//we have rType
        registerFile.registers[instruction.rd] = result;
    else
        registerFile.registers[instruction.rt] = result;

}
//===================================================== PC


void branching(){

}


//=======================================================================================
int * decToBinary(int number)
{
    // array to store binary number
    int binaryNum[32];

    for(int i = 0 ; i < 32 ; i++)
        binaryNum[i] = 0;

    int tmpNum = abs(number);

    // counter for binary array
    int i = 0;
    while (tmpNum > 0) {
        binaryNum[i] = tmpNum % 2;
        tmpNum = tmpNum / 2;
        i++;
    }
    if(number < 0)
    {
        boolean zeroIgnorFlag = 0;
        for(int i = 31 ; i >= 0 ; i--)
        {
            if(!binaryNum[i] && !zeroIgnorFlag)
                continue;
            else if(binaryNum[i] && !zeroIgnorFlag)
            {
                zeroIgnorFlag = 1;
                continue;
            }
            else if(zeroIgnorFlag)
                binaryNum[i] = !binaryNum[i];
        }
    }

    return binaryNum;

}

int binaryToDec(int * binaryNum)
{
    int decimal = 0;
    int base = 1;
    boolean isNegetiveFlag = 0;
    if(binaryNum[0])
    {
        isNegetiveFlag = 1;
        boolean zeroIgnorFlag = 0;
        for(int i = 31 ; i >= 0 ; i--)
        {
            if(!binaryNum[i] && !zeroIgnorFlag)
                continue;
            else if(binaryNum[i] && !zeroIgnorFlag)
            {
                zeroIgnorFlag = 1;
                continue;
            }
            else if(!binaryNum[i] && zeroIgnorFlag)
                binaryNum[i] = 1;
            else if(binaryNum[i] && zeroIgnorFlag)
                binaryNum[i] = 0;
        }
    }

    for (int i = 31 ; i >= 0 ; i--)
    {
        decimal += binaryNum[i] * base;
        base *= 2;
    }
    if(isNegetiveFlag)
        decimal *= -1;
    return decimal;
}
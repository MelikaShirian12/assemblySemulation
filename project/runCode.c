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
int * decToBinary(int number);
void memory(int aluRes , int opCode , int rt);
void writeBack(int aluResult , int memoryResult , int opCode);
int binaryToDec(int * binaryNum);

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
        default:
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


void memory(int aluRes , int opCode , int rtData){

    int lw_result = 0;
    if (opCode == 9) //lw
        lw_result = memFile.mem_file[aluRes];
    else if (opCode == 10) //sw
        memFile.mem_file[aluRes] = rtData;

    if (opCode != 10)
        writeBack(aluRes ,lw_result ,opCode);
}


void writeBack(int aluResult , int memoryResult , int opCode){

    int result = 0;

    if(opCode == 9)//if memory result by lw should be chosen
        result = memoryResult;
    else
        result = aluResult;

    if (0 <= opCode && opCode <= 4)//we have rType
        registerFile[]

}

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
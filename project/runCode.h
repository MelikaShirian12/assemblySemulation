#include <stdio.h>
#include <math.h>
#include <malloc.h>
#include "main.h"

struct RegisterFile {
    int registers [16]; //16
};

struct MemoryFile{
    int * mem_file ;
};

struct ControlUnit{
    int RegDest;
    int jump;
    int branch;
    int MemRead;
    int MemToReg;
    int ALUOp;
    int MemWrite;
    int ALUSrc;
    int RegWrite;
    int upper;
    int halt;
    int jalr;
};

struct ALUControl{
    int operation;
    int zero;
};


//initializing registerFile
struct RegisterFile registerFile = {
        { 0 ,1 , 2, 4 , 34 ,-2 ,
                5 , 10 , 4 , 22 , 11 , 23
                ,12 , 12 , 12 ,4}
};

//initializing memory
struct MemoryFile memFile;

int PC = 0;

//initializing control units
struct ControlUnit controlUnit;
struct ALUControl aluControl;

void initial_memories(struct Program * runningProgram);
bool fetch(struct Program * runningProgram, int PC);
void decode(struct Instruction instruction);
void exe(int rsData , int rtDAta ,struct Instruction instruction );
int ALU(int opCode ,int typeSecond , int firstBus , int secondBus);
int * decToBinary(int number);
void memory(int aluRes , struct Instruction instruction  , int rtData , int rsData);
void writeBack(int aluResult , int memoryResult , struct Instruction instruction , int rsData);
int binaryToDec(int * binaryNum);
void branching();

//functions ======================================================================================

void initial_memories(struct Program * runningProgram){

    memFile.mem_file = malloc(sizeof(int)*16000);

    for (int i = 0; i < 16000 ; ++i) {
        memFile.mem_file[i] = 0;
    }

    fetch(runningProgram , 0);
}

void initial_controls(struct Instruction ins){

    //===============halt
    if(ins.opCode == 14)
        controlUnit.halt = 1;
    else controlUnit.halt = 0;

    //=============RegDest
    if (ins.opCode >=0 && ins.opCode<5)
        controlUnit.RegDest = 1;

    else if ((ins.opCode >=5 && ins.opCode<13) && ins.opCode != 11 && ins.opCode != 10)
        controlUnit.RegDest = 0;
    else controlUnit.RegDest = 1111; //stands for 'dont care'

    //=============Branch
    if (ins.opCode == 11 || ins.opCode == 12 || ins.opCode == 13)
        controlUnit.branch = 1;
    else controlUnit.branch = 0;

    //====================MemRead
    if (ins.opCode == 9)
        controlUnit.MemRead = 1;
    else controlUnit.MemRead = 0;

    //===================MemToReg and Upper
    if (ins.opCode == 9) {
        controlUnit.upper = 0;
        controlUnit.MemToReg = 1;
    }
    else if (ins.opCode == 8) {
        controlUnit.MemToReg = 0;
        controlUnit.upper = 1;
    }
    else {
        controlUnit.MemToReg = 0;
        controlUnit.upper = 0;
    }

    //=======================AluOp

    controlUnit.ALUOp = ins.opCode;


    //======================MemWrite
    if (ins.opCode == 10)
        controlUnit.MemWrite = 1;
    else controlUnit.MemWrite = 0;

    //======================ALUSrc
    if ((ins.opCode >=0 && ins.opCode<5) || ins.opCode == 11)
        controlUnit.ALUSrc = 0;
    else if (ins.opCode == 12)
        controlUnit.ALUSrc = 1111;
    else controlUnit.ALUSrc = 1;

    //====================RegWrite
    if (ins.opCode == 10 || ins.opCode == 11)
        controlUnit.RegWrite = 0;
    else controlUnit.RegWrite = 1;

    //=====================Jalr
    if (ins.opCode == 12)
        controlUnit.jalr = 1;
    else controlUnit.jalr = 0;

    // ------------ ALU UNIT --------------
    aluControl.operation = ins.opCode;

}

int Adder(int bus1 , int bus2){

    return  bus1+bus2;
}


bool fetch(struct Program * runningProgram , int pc){

    PC = pc;
    while (1) {
        for(int i= 0 ; i<16 ; ++i)
            printf("[register %d]: %d\n" , i , registerFile.registers[i]);

        printf("====================================\n");

        if (PC == -1)//if it was halt then the program should be finished after that
            break;

        //read the current address using pc and load it here
        struct Instruction new_instruction_line = runningProgram->instructions[PC];


        //control unit -----> pc = pc+1
        PC = Adder(PC , 1) ;

        initial_controls(new_instruction_line);

        decode(new_instruction_line);
    }

    return 1;
}

void decode(struct Instruction instruction){


    //CONTROL UNIT ---------> choosing between next line or halt
    if (controlUnit.halt == 1)
        PC = -1;

    //DECODE path

    int readBus1 = registerFile.registers[instruction.rs];
    int readBus2 = registerFile.registers[instruction.rt];

    exe(readBus1, readBus2 ,instruction);
}

void exe(int rsData , int rtDAta , struct Instruction instruction){

    int firstBus = rsData;
    int secondBus;
    int secondType;

    //SIGN EXTENDED imm for branching
    int AluResult = Adder(instruction.imm , PC);

    switch (controlUnit.ALUSrc) {

        case 1:
            secondBus = instruction.imm;
            if (instruction.opCode == 10 || instruction.opCode == 11) // for jal and beq
                secondBus = rtDAta;
            secondType=1;
            break;
        case 0:
            secondBus = rtDAta;
            secondType= 0;
            break;
        default:
            break;
    }

    int output = ALU(instruction.opCode , secondType , firstBus , secondBus);

    //choose branching or pc+4
    if ((aluControl.zero & controlUnit.branch ) == 1)
        PC = AluResult;

    memory(output , instruction , rtDAta , rsData);
}

int ALU(int opCode , int type , int firstBus , int secondBus) {


    int * first ;
    int * second;

    aluControl.zero = 0;

    switch (aluControl.operation) {

        case 0:
            return firstBus + secondBus;
        case 1:
            aluControl.zero = 1;
            return firstBus - secondBus;
        case 2:
            aluControl.zero = 1;
            if (secondBus - firstBus > 0)
                return 1;
            return 0;
        case 3:
            return firstBus | secondBus;
        case 4:
            return !(firstBus & secondBus);

        case 5:
            return firstBus + secondBus;
        case 6:
            return firstBus | secondBus;
        case 7:
            aluControl.zero = 1;
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
            aluControl.zero = 1;
            if (secondBus-firstBus ==0)
                return 1;
            return 0;
        default:
            return 0;
    }
}


void memory(int aluRes ,struct Instruction instruction , int rtData , int rsData){


    //choosing next pc
    if (controlUnit.jump == 1)
        PC = instruction.imm;

    int lw_result = 0;
    if (controlUnit.MemRead == 1) //lw
        lw_result = memFile.mem_file[aluRes];
    else if (controlUnit.MemWrite == 1) //sw
        memFile.mem_file[aluRes] = rtData;

    if (instruction.opCode != 10) //sw doesnt have write back
        writeBack(aluRes ,lw_result ,instruction ,rsData);
}


void writeBack(int aluResult , int memoryResult ,struct Instruction instruction ,int rsData){

    //final result of pc
    if (controlUnit.jalr == 1)
        PC = rsData;


    int result = 0;

    //==============first MUX 2 to 3
    if(controlUnit.upper == 0 && controlUnit.MemToReg == 1)//if memory result by lw should be chosen
        result = memoryResult;
    else if (controlUnit.upper == 0 && controlUnit.MemToReg == 0)
        result = aluResult;
    else if(controlUnit.upper == 1 && controlUnit.MemToReg == 0)
        //then we have lui
        result = instruction.imm << 16u;


   //======================= second MUX 1 to 2
   if(controlUnit.jalr == 1)
       result = PC +1;

    //======================writing to register MUX 1 to 2

    if (controlUnit.RegDest == 1 && controlUnit.RegWrite == 1)//we have rType
        registerFile.registers[instruction.rd] = result;
    else if(controlUnit.RegDest == 0 && controlUnit.RegWrite == 1)
        registerFile.registers[instruction.rt] = result;

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
        bool zeroIgnorFlag = 0;
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
    bool isNegetiveFlag = 0;
    if(binaryNum[0])
    {
        isNegetiveFlag = 1;
        bool zeroIgnorFlag = 0;
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
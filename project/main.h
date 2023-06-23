#include <stdio.h>
#include <math.h>
#include <stdbool.h>


//=================structs===============

enum Types{
    Itype , Rtype , Jtype , Null, DOTtype
};


struct MachineCode{

    char hexMachineCode[9];
    long int  decimalMachineCode;
};

struct Instruction{

    enum Types insType;
    struct MachineCode machineCode;
    char inst[50];
    int rs;
    int rt;
    int rd;
    int imm; //and target address
    int PC;
    int opCode;
    int directory;
    int diCode;
};


struct Error{
    int address;
    //char lineOfError[250];
    char errorInfo[60];
};

struct MyMap{
    char each_label[20];
    int address;
};



struct Program{
    char inputProgram [100][250];
    struct MyMap label[100];
    struct Instruction instructions [100];
    int labels_num;
    int inputSize;

};


struct Program readingFiles(char * address);

bool find_Labels(struct Program * structProgram);

bool check_line_by_line(struct Program * structProgram);

long int hex_to_decimal (char * hexdecnumber);

bool makeMachineCode(struct Program *structProgram);

void writing_errors(struct Error error);

void write_output(char * fileName , struct Program * structProgram);

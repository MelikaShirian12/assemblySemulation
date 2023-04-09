#include <stdio.h>
#include <math.h>

#ifdef __unix__                    /* __unix__ is usually defined by compilers targeting Unix systems */

#define OS_Windows 0
    #include <unistd.h>
    #include <stdlib.h>
    #include <stdio.h>
    #include <string.h>

#elif defined(_WIN32) || defined(WIN32)     /* _Win32 is usually defined by compilers targeting 32 or   64 bit Windows systems */

#define OS_Windows 1
#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#define DIV 1048576
#define WIDTH 7

#endif

char R_TYPE [][5]= {"add","sub" ,"slt","or","nand"}; //5
char I_TYPE [][5] ={"addi" , "ori" , "slti", "lui", "lw" , "sw" , "beq", "jalr"};//8
char J_TYPE [][5] = {"j" , "halt"};//2

//=================structs===============


enum Types{
    Itype , Rtype , Jtype , Null
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


struct Program readingFiles();

boolean find_Labels(struct Program * structProgram);

boolean check_line_by_line(struct Program * structProgram);

long int hex_to_decimal (char * hexdecnumber);

boolean makeMachineCode(struct Program *structProgram);

void writing_errors(struct Error error);


//=================functions==================

int main() {

    if(OS_Windows)
        printf("Hello, Welcome to windows!\n");
    else
        printf("Hello, linux!\n");

    struct Program programLines = readingFiles();

    boolean true_labels = find_Labels(& programLines);
    if ( !true_labels )
        return 1;

    boolean true = check_line_by_line(& programLines);
    if (!true)
        return 1;

    makeMachineCode(& programLines);


    return 0;
}


//============================files=================================================

struct Program readingFiles(){

    int bufferLength = 250;
    char buffer [bufferLength];

    FILE *fptr;
    fptr = fopen("testReading.txt" , "r");

    if (fptr == NULL)
        printf("could not find the address !");


    struct Program structProgram;

    int i=0;
    while(fgets(buffer, bufferLength, fptr)) {

        // it reads every line and writes it down to buffer
        strcpy( structProgram.inputProgram[i] ,buffer);
        ++i;
    }

    structProgram.inputSize = i;


    fclose(fptr);

    return structProgram;

}

void make_the_error(char * errorString , int address){

    struct Error error ;
    error.address = address;
    strcpy(error.errorInfo , errorString);

    writing_errors(error);

}

void writing_errors(struct Error error){

    FILE * fptr;
    fptr = fopen("testWriting.txt" , "a");

   // struct Error error2 = {4, "this is an error "};
    fwrite(& error , sizeof(struct Error) ,1 , fptr );
    fprintf(fptr , "\n");



    fclose(fptr);

}

void writing_assembly(){

}


//============================labels===============================================

int check_duplication(char * token , struct Program * structProgram , int row_place){

    for (int i = 0; i <structProgram->labels_num ; ++i)
        if (strcmp(structProgram->label[i].each_label , token) == 0)
            return i;

     strcpy(structProgram->label[structProgram->labels_num].each_label , token);
    structProgram->label[structProgram->labels_num].address = row_place;
     ++structProgram->labels_num;

     return -1;

}

boolean check_label(char * token){

    for (int i = 0; i <5 ; ++i)
        if (strcmp(R_TYPE[i] , token) == 0)
            return 0;


    for (int i = 0; i <8 ; ++i)
        if (strcmp(I_TYPE[i] , token) == 0)
            return 0;

    for (int i = 0; i <2 ; ++i)
        if (strcmp(J_TYPE[i] , token) == 0)
            return 0;

    return 1;

}

boolean find_Labels(struct Program * structProgram){


    structProgram->labels_num = 0;
    for (int i=0 ; i<structProgram->inputSize ; ++i) {

        char *backup = malloc(250);
        strcpy(backup,structProgram->inputProgram[i]);

        char *token = strtok(structProgram->inputProgram[i], "\t");

        int  checking = 0;
        if(check_label(token) != 0 ) {
            checking = check_duplication(token, structProgram, i); //i is for address place

            if (checking != -1) {
                //this means that we had a duplicated label and we have an exception
                printf("You have duplicated labels in the code");
                make_the_error("You have duplicated labels in the code", i);
                return 0;
            }
        }


        strcpy(structProgram->inputProgram[i] , backup);

    }

    //we have founded all the labels , and they all are different from one another
    return 1;

}

int  find_label_value(char * token , struct Program * structProgram) {

    if(token[strlen(token) - 1] == '\n')
        token[strlen(token) - 1] = '\0';

    for (int i = 0; i < structProgram->labels_num; ++i)
        if (strcmp(structProgram->label[i].each_label, token) == 0)
            return structProgram->label[i].address;

    return -1;
}

//==================================op code ======================================


enum Types turnType(char * token , int * opCode){

    //finding the opCode at the same time as well
    *opCode = 0;

    for (int i = 0; i <5 ; ++i) {
        *opCode = i;
        if (strcmp(R_TYPE[i], token) == 0)
            return Rtype;
    }


    for (int i = 0; i <8 ; ++i) {
        *opCode = i;
        if (strcmp(I_TYPE[i], token) == 0) {
            *opCode+=5;
            return Itype;
        }
    }

    for (int i = 0; i <2 ; ++i) {
        *opCode = i;
        if (strcmp(J_TYPE[i], token) == 0) {
            *opCode+=13;
            return Jtype;
        }
    }

     opCode = 0 ;
     return Null;

}

boolean makeOpInstruction(char * token ,struct Program * structProgram , int size){

    int opCode = 0;
    enum Types type = turnType(token  , & opCode);

    if (type == Null) {
        printf("There is no such instruction !");
        make_the_error("There is no such instruction" , size);

        return 0;//which means we don't have that sort of inst
    }
     struct Instruction new_instruction ;
     new_instruction.opCode = opCode;
     new_instruction.insType = type;
     new_instruction.PC= size; // address of that line of instruction

     //adding the new instruction
     structProgram->instructions[size] = new_instruction;

     if (opCode == 14){
         structProgram->instructions[size].imm = 0;
         structProgram->instructions[size].rs = -1;
         structProgram->instructions[size].rt = -1;
         structProgram->instructions[size].rd = -1;
     }

     return 1;
}

int calculateRegister(char * my_register){

    int number = 0;
    //========================find each of the register numbers =======================
    if(my_register[0] >= '0'  && my_register[0] <= '9')
        number = (my_register[0] - 48);
    else
        return -1;//wrong input
    if (my_register[1] != 0 && my_register[1]!= '\n') {
        if (my_register[1] >= '0' && my_register[1] <= '4') {
            number *= 10;
            number += (my_register[1] - 48);
        }
        else
            return -1; //wrong input
        if (my_register[2] != 0 && my_register[2]!= '\n')
            return -1;
    }

    return number;
}

boolean makeRegiInstruction(char * token, struct Program * structProgram , int size){


    //split the registers
    char  *my_register =  strtok(token, ",");

    if(structProgram->instructions[size].insType == Jtype) {
        if (structProgram->instructions[size].opCode != 14) {//if it was not halt

            if ((my_register[0] <= '9' && my_register[0] >= '0') || my_register[0] == '-') {

                char *tmp;
                structProgram->instructions[size].imm = strtol(my_register, &tmp, 10);
                if (abs(structProgram->instructions[size].imm) > 65535){
                    //because 2 ^16 is in 7 bits (10000000000000000)that we have 16 zeros
                    printf("imm is greater than 16 bits :%d",size);
                    make_the_error("imm is greater than 16 bits" , size);
                    return 0;
                }
            } else {
                int address = find_label_value(my_register, structProgram);
                if (address < 0) {

                    printf("couldn't find the label: %d",size);
                    make_the_error("couldn't find the label", size);
                    return 0;
                }
                structProgram->instructions[size].imm = address;
                structProgram->instructions[size].rs = -1;
                structProgram->instructions[size].rt = -1;
                structProgram->instructions[size].rd = -1;

                return 1;
            }
        }

    }

    int num_of_register = 1;
    while( my_register != NULL ){

        int result;
        if (num_of_register != 3) {
            result = calculateRegister(my_register);
            if (result < 0) {
                printf("wrong register input");
                make_the_error("wrong register input" , size);
                return 0;
            }
        }
        //this means we have to fill in the rd
        if(num_of_register == 1)
            structProgram->instructions[size].rt= result;
        else if (num_of_register == 2) //this means we are on the second register -> rs
            structProgram->instructions[size].rs = result;
        else
            if(structProgram->instructions[size].insType == Rtype) { //we have an r type so we should fill in the rt

                result = calculateRegister(my_register);
                if (result < 0) {//exception

                    printf("wrong register input :%d",size);
                    make_the_error("wrong register input" , size);

                    return 0;
                }
                //and make immediate value , null
                structProgram->instructions[size].rt = result;
                structProgram->instructions[size].imm = -100;
            }else{
                structProgram->instructions[size].rd = -1;
                if ((my_register[0] <= '9' && my_register[0] >= '0') || my_register[0] == '-' ){

                    char * tmp ;
                    structProgram->instructions[size].imm = strtol(my_register , & tmp , 10);
                    if (abs(structProgram->instructions[size].imm) > 65535){
                        //because 2 ^16 is in 7 bits (10000000000000000)that we have 16 zeros
                        printf("imm is greater than 16 bits :%d",size);
                        make_the_error("imm is greater than 16 bits" , size);
                        return 0;
                    }
                } else{
                    int address = find_label_value(my_register , structProgram);
                    if (address < 0) {

                        printf("could not find the label :%d",size);
                        make_the_error("wrong register input" , size);
                        return 0;
                    }
                    structProgram->instructions[size].imm = address;
                }


            }


        //=====================adding the registers into the right place======================

        ++num_of_register;
        my_register = strtok(NULL , ",");
    }
    return 1;
}


boolean check_line_by_line(struct Program * structProgram){

    for (int i=0 ; i<structProgram->inputSize ; ++i) {

        char *backup = malloc(250);
        strcpy(backup,structProgram->inputProgram[i]);

        char *token = strtok(structProgram->inputProgram[i], "\t");

        int spaceNum = 0;
        int checking = 0;
        while( token != NULL ) {


            if(token[strlen(token) - 1] == '\n')
                token[strlen(token) - 1] = '\0';


            if (token[0] == '#')
                break;

            switch (spaceNum) {

                boolean check;
                case 0:
                    check = check_label(token);
                    if (check == 0) {
                        check = makeOpInstruction(token, structProgram , i);
                        ++checking;//which means we already check the instruction
                        if (check == 0 ){
                            //exception
                            return 0;
                        }
                    }
                    break;

                case 1:
                    if (checking == 0) {
                        check = makeOpInstruction(token, structProgram , i);
                        ++checking;//which means we already check the instruction
                        --spaceNum;
                        if (check == 0 ){
                            //exception
                            return 0;
                        }
                        break;
                    }
                    check = makeRegiInstruction(token, structProgram , i);
                    if (check == 0){
                        //exception
                        return 0;
                    } //else //structProgram->instructions[i].inst = structProgram->inputProgram[i]; fill
                    break;



            }

            ++spaceNum;
            token = strtok(NULL, "\t");
        }

        strcpy(structProgram->inputProgram[i] , backup);
    }

    return 1;

}


//===================================decimal and hex==================================


void swap(char * str) {

    int j = ((int) strlen(str))-1;
    for (int i = 0; i < (int) strlen(str) / 2; i++) {
        char tmp = str[i];
        str[i] = str[strlen(str) -j];
        str[strlen(str) -j] = tmp;

        --j;
    }
}

char* decimal_to_hex(long int decimalNumber){
     long int remainder,quotient;
    int i=0,j,temp;
    char * hexadecimalNumber = malloc(sizeof(char)*100);
    quotient = decimalNumber;

    while(quotient!=0) {
        temp = quotient % 16;
        //To convert integer into character
        if( temp < 10)
            temp =temp + 48; else
            temp = temp + 55; //for making A B C ...
        hexadecimalNumber[i++]= temp;
        quotient = quotient / 16;
    }

    if(i != 0)
        hexadecimalNumber[i] = '\0';
    else {
        hexadecimalNumber[i] = '0';
        hexadecimalNumber[i + 1] = '\0';
    }

    swap (hexadecimalNumber);
    return hexadecimalNumber;
}


long int hex_to_decimal (char * hexdecnumber){

    long long decimalnumber, place;
    int i = 0, val, len;

    decimalnumber = 0;
    place = 1;

    // Find the length of total number of hex digit
    // finding the length of hexa decimal number
    len = strlen(hexdecnumber);
    len--;

    // for loop iterates the hexa decimal number digits
    for (i = 0; hexdecnumber[i] != '\0'; i++) {

        // finding the equivalent decimal digit for each
        // hexa decimal digit
        if (hexdecnumber[i] >= '0'
            && hexdecnumber[i] <= '9') {
            val = hexdecnumber[i] - 48;
        }
        else if (hexdecnumber[i] >= 'a'
                 && hexdecnumber[i] <= 'f') {
            val = hexdecnumber[i] - 97 + 10;
        }
        else if (hexdecnumber[i] >= 'A'
                 && hexdecnumber[i] <= 'F') {
            val = hexdecnumber[i] - 65 + 10;
        }

        decimalnumber += val * pow(16, len);
        len--;
    }
    return decimalnumber;
}


//=====================================make the machine code ==============================

 boolean makeMachineCode(struct Program *structProgram){

    for (int i = 0; i <structProgram->inputSize ; ++i) {
        char eachMachineCode [9];


        char opCode = decimal_to_hex(structProgram->instructions[i].opCode)[0];

        eachMachineCode[1] = opCode;
        eachMachineCode[0]='0';
        eachMachineCode[8] = '\0';

        switch (structProgram->instructions[i].insType) {

            case Rtype:
                eachMachineCode[7]='0';
                eachMachineCode[6]='0';
                eachMachineCode[5]='0';

                char rt = decimal_to_hex(structProgram->instructions[i].rt)[0];
                char rs = decimal_to_hex(structProgram->instructions[i].rs)[0];
                char rd = decimal_to_hex(structProgram->instructions[i].rd)[0];

                eachMachineCode[4]= rd;
                eachMachineCode[3] = rt;
                eachMachineCode[2] = rs;

                break;

            case Itype:


                rt = decimal_to_hex(structProgram->instructions[i].rt)[0];
                rs = decimal_to_hex(structProgram->instructions[i].rs)[0];

                eachMachineCode[3] = rt;
                eachMachineCode[2] = rs;


                char *tmpImm = decimal_to_hex(structProgram->instructions[i].imm);


                int tmpDif = 4 - (int)strlen(tmpImm);

                for(int j = 0 ; j  < tmpDif; j++)
                    eachMachineCode[j+4] = '0';

                for(int j = 0 ; j < 4 ; j++)
                    eachMachineCode[j+4+tmpDif] = tmpImm[j];

                break;

            case Jtype:

                tmpImm = decimal_to_hex(structProgram->instructions[i].imm);



                tmpDif = 4 - (int)strlen(tmpImm);

                for(int j = 0 ; j  < tmpDif; j++)
                    eachMachineCode[j+4] = '0';

                for(int j = 0 ; j < 4 ; j++)
                    eachMachineCode[j+4+tmpDif] = tmpImm[j];


                eachMachineCode[3] = '0';
                eachMachineCode[2] = '0';

                break;

        }

        strcpy((char *) structProgram->instructions[i].machineCode.hexMachineCode, eachMachineCode);
        structProgram->instructions[i].machineCode.decimalMachineCode = hex_to_decimal(eachMachineCode);

    }

    return 1;
}

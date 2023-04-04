#include <stdio.h>

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

struct Program readingFiles();

boolean find_Labels(struct Program * structProgram);

enum J_TYPE {
    j , halt
};

//=================structs===============

struct MyMap{
    char each_label[20];
    int address;
};



struct Program{
    char inputProgram [100][250];
    struct MyMap label[100];
    int labels_num;
    int inputSize;
};


//=================functions==================

int main() {

    if(OS_Windows)
        printf("Hello, Welcome to windows!\n");
    else
        printf("Hello, linux!\n");

    struct Program programLines = readingFiles();

    boolean true_labels = find_Labels(& programLines);
    if ( !true_labels ){
        //write the error
    }




    return 0;
}


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


boolean check_duplication(char * token , struct Program * structProgram , int row_place){

    for (int i = 0; i <structProgram->labels_num ; ++i)
        if (strcmp(structProgram->label[i].each_label , token) != 0)
            return 1;

     strcpy(structProgram->label[structProgram->labels_num].each_label , token);
    structProgram->label[structProgram->labels_num].address = row_place;
     ++structProgram->labels_num;

     return 0;

}


boolean check_label(char * token){

    for (int i = 0; i <5 ; ++i)
        if (strcmp(R_TYPE[i] , token) != 0)
            return 0;


    for (int i = 0; i <8 ; ++i)
        if (strcmp(I_TYPE[i] , token) != 0)
            return 0;

    for (int i = 0; i <2 ; ++i)
        if (strcmp(J_TYPE[i] , token) != 0)
            return 0;

    return 1;

}

boolean find_Labels(struct Program * structProgram){

    structProgram->labels_num = 0;
    for (int i=0 ; i<structProgram->inputSize ; ++i) {

        char *token = strtok(structProgram->inputProgram[i], " ");

        boolean checking;
        if(check_label(token) != 0 )
            checking = check_duplication(token , structProgram , i); //i is for address place

         if (checking != 0)
             //this means that we had a duplicated label and we have an exception
             return 0;
    }

    //we have founded all the labels , and they all are different from one another
    return 1;

}



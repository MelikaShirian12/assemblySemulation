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

struct Program readingFiles();

struct Program{

    char inputProgram [100][250];
};

int main() {


    if(OS_Windows)
        printf("Hello, Welcome to windows!\n");
    else
        printf("Hello, linux!\n");

    struct Program programLines = readingFiles();



    return 0;
}


struct Program readingFiles(){

    int bufferLength = 250;
    char buffer [bufferLength];

    FILE *fptr;
    fptr = fopen("address" , "r");

    if (fptr == NULL)
        printf("could not find the address !");


    struct Program structProgram;

    int i=0;
    while(fgets(buffer, bufferLength, fptr)) {

        // it reads every line and writes it down to buffer
        strcpy( structProgram.inputProgram[i] ,buffer);
        ++i;
    }


    fclose(fptr);

    return structProgram;

}
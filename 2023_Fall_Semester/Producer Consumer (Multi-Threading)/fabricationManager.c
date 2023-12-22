// fabricationManager.c : the parent process file that opens all the .txt files and calls fork() and execv() to create and execute the child process. The parent process is ensured to wait for the child process to finish first using wait().

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <pthread.h>
#include "fabricationPlant.h"

// github link: https://github.com/CIS-3207-F23/project-2-f23-tun82434

// int main() : calls fork() to fork the child process. if childProcess < 0 then fork() error. else if childProcess == 0 then open all .txt files and set them in char *args[] and execute the child process while passing args[] as arguments. else, the parent process waits for the child process to finish before finishing.
int main(){
    int childProcess = fork();
    if (childProcess < 0){
        printf("Error: childProcess could not fork()\n");
    } else if (childProcess == 0){
        //umask(0);
        int railway = open("railwayCars.txt", O_RDONLY);
        int blue = open("BLUE_delivery.txt", O_WRONLY | O_CREAT , S_IRWXU);
        int red = open("RED_delivery.txt", O_WRONLY | O_CREAT , S_IRWXU);
        if (railway == -1 || blue == -1 || red == -1){
            printf("Error: open() for either input or output\n");
            exit(1);
        }
        char railwayStr[4], blueStr[4], redStr[4];
        sprintf(railwayStr, "%d", railway);
        sprintf(blueStr, "%d", blue);
        sprintf(redStr, "%d", red);
        char *assemProgram = "./assemManager";
        char *args[] = {assemProgram, railwayStr, blueStr, redStr, (char *) NULL};
        execv(assemProgram, args);
    } else {
        int childProcessWait;
        childProcessWait = wait(NULL);
        if (childProcessWait == -1){
            printf("Error: wait() error.\n");
        }
    }
    return 0;
}
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <getopt.h>
#include "MYps.h"


// this function within the getPIDVmem.c file takes in the struct PIDinfo and the pid, it then goes through /proc/[PID]/statm to find the virtual memory size info and store it in the struct then prints out the virtual memory info and returns 0 to end.
// the function initializes the FILE pathStatm and opens it, it then goes through and scans until it reaches the 1st item in the statm file, the virtual memory size. It then stores the item in pidData.procVmemSize
int getPIDVmem(struct PIDinfo pidData,char *pid){
    FILE *pathStatm;
    char buffer[128];
    size_t pathLen = strlen(pid) + 14;
    char *path = (char *)malloc(pathLen + 1);
    strcpy(path, "/proc/");
    strcat(path, pid);
    strcat(path, "/statm");
    pathStatm = fopen(path,"rb");
    if (pathStatm == NULL){
        printf("Error: Invalid PID, please enter a valid PID.\n");
        exit(1);
    }
    if (fscanf(pathStatm, "%s", buffer) == 1){
        pidData.procVMemSize = atoi(buffer);
        printf("%dkB ", pidData.procVMemSize);
    }
    fclose(pathStatm);
    free(path);
    return 0;
}
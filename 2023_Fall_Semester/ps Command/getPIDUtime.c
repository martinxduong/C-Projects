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

// this function within the getPIDUtime.c file takes in the struct PIDinfo and the pid, it then goes through /proc/[PID]/stat to find the stime info and store it in the struct then prints out the utime and returns 0 to end.
// the function initializes the FILE pathStat and opens it, it then goes through and scans until it reaches the 14th item in the stat file, the utime. It then stores the item in pidData.procUtime

int getPIDUtime(struct PIDinfo pidData,char *pid){
    FILE *pathStat;
    char buffer[128];
    size_t pathLen = strlen(pid) + 13;
    char *path = (char *)malloc(pathLen + 1);
    strcpy(path, "/proc/");
    strcat(path, pid);
    strcat(path, "/stat");
    pathStat = fopen(path,"rb");
    if (pathStat == NULL){
        printf("Error: Invalid PID, please enter a valid PID.\n");
        exit(1);
    }
    int fileCounter = 0;
    while (fscanf(pathStat, "%s", buffer) != EOF){
        fileCounter++;
        if (fileCounter == 14){
            pidData.procUtime = atoi(buffer);
            printf("utime=%d ", pidData.procUtime);
            break;
        }
    }
    fclose(pathStat);
    free(path);
    return 0;
}
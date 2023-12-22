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

// this function within the getPIDcmdline.c file takes in the struct PIDinfo and the pid, it then goes through /proc/[PID]/cmdline to find the cmdline that is used to call this process and store it in the struct then prints out the state and returns 0 to end.
// the function initializes the FILE pathCmdline and opens it, it then goes through and gets each line. If each character in the line has a null terminating value "\0" but the character after is not a \0 then set pidData.cmdline[i] equal a space. If both has \0 then then set pidData.cmdline[i] equal to \0. If neither of those if statements are true just copy to pidDtat.cmdline[i]


int getPIDcmdline(struct PIDinfo pidData,char *pid){
    FILE *pathCmdline;
    char buffer[1000];
    size_t pathLen = strlen(pid) + 16;
    char *path = (char *)malloc(pathLen + 1);
    strcpy(path, "/proc/");
    strcat(path, pid);
    strcat(path, "/cmdline");
    pathCmdline = fopen(path,"rb");
    if (pathCmdline == NULL){
        printf("Error: Invalid PID, please enter a valid PID.\n");
        exit(1);
    }
    while(fgets(buffer, sizeof(buffer), pathCmdline) != NULL){
        for(int i = 0; i < sizeof(buffer); i++){
            if (buffer[i] == '\0' && buffer[i+1] != '\0'){
                pidData.cmdline[i] = ' ';
            } else if (buffer[i] == '\0' && buffer[i+1] == '\0'){
                pidData.cmdline[i] = '\0';
                break;
            } else{
                pidData.cmdline[i] = buffer[i];
            }
        }
    }
    printf("%s ",pidData.cmdline);
    fclose(pathCmdline);
    free(path);
    return 0;
}


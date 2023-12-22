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

int getPIDs(char ***matchingPIDs){
    int pidCount = 0;
    struct dirent **namelist;
    int n;
    n = scandir("/proc", &namelist, NULL, alphasort); // scandir returns the number of items in the directory
    if (n < 0){ // if scandir fails, n will be less than 0
        printf("Error: cannot open /proc\n");
        exit(1);
    }
    __uid_t  myUID = getuid();
    while (n--){
        FILE *pathStatus;
        char buffer[128];
        const char uidSTR[] = "Uid:";
        int isNumeric = 0;
        for (int i = 0; namelist[n]->d_name[i] != '\0'; i++){
            char ch = namelist[n]->d_name[i];
            if (!isdigit(ch)){
                isNumeric = 1;
            }
        }
        if (isNumeric == 0){
            char myUidStr[20];
            size_t pathLen = strlen(namelist[n]->d_name) + 13;
            char *path = (char *)malloc(pathLen + 1);
            strcpy(path,"/proc/");
            strcat(path,namelist[n]->d_name);
            strcat(path,"/status");
            pathStatus = fopen(path,"rb");
            if (pathStatus == NULL){
                printf("Error: Unable to open status file.\n");
                exit(1);
            }
            while (fgets(buffer, sizeof(buffer), pathStatus)){
                snprintf(myUidStr,sizeof(myUidStr), "%llu", (unsigned long long) myUID); //store myUID in myUIDStr
                char *uidNumLoc = strstr(buffer, uidSTR);
                char *sameUID = strstr(buffer, myUidStr);
                if(uidNumLoc != NULL && sameUID != NULL){
                    *matchingPIDs = realloc(*matchingPIDs, (pidCount + 1) * sizeof(char *));
                    (*matchingPIDs)[pidCount] = strdup(namelist[n]->d_name);
                    pidCount++;
                }
            }
            fclose(pathStatus);
            free(path);
        }
    }
    return(pidCount);
}
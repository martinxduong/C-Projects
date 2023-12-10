#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h> // getcwd

// Function Declaration
int cInfo();
int mInfo();
char* grabNum(char *text);


int main(){
    cInfo();
    mInfo();
    return 0;
}

int cInfo(){
    FILE *cpuInfo = fopen("/proc/cpuinfo","rb");
    char buffer[128];
    char *processNum = (char *)malloc(128 * sizeof(char));
    strcpy(processNum,"0");
    char *processSize = NULL;
    while (fgets(buffer,sizeof(buffer),cpuInfo)){
        const char cpuStr[] = "processor";
        char *processNumLoc = strstr(buffer,cpuStr);
        if (processNumLoc != NULL){
            strcpy(processNum,grabNum(processNumLoc));
            continue;
        } 
        const char *sizeStr = "cache size";
        processSize = strstr(buffer,sizeStr);
        if (processSize != NULL){
            processSize = grabNum(processSize);
            printf("Processor %s with a case size of %s KB\n", processNum, processSize);
        }
    }
    free(processNum);
    return 0;
}

int mInfo(){
    FILE *memInfo = fopen("/proc/meminfo","rb");
    char buffer[128];
    while (fgets(buffer,sizeof(buffer),memInfo)){
        const char freeStr[] = "MemFree";
        char *memNum = strstr(buffer,freeStr);
        if (memNum != NULL){
            strcpy(memNum,grabNum(memNum));
            printf("Free memory %s kB\n",memNum);
            continue;
        }
        const char buffStr[] = "Buffers";
        char *buffNum = strstr(buffer, buffStr);
        if (buffNum != NULL){
            strcpy(buffNum,grabNum(buffNum));
            printf("Buffers: %s kB\n",buffNum);
            break;
        }
    }
    return 0;
}

char* grabNum(char *text){
    int j = 0;
    for(int i = 0; i < strlen(text); i++){
        if (!isspace(text[i]) && text[i] != ':' && !isalpha(text[i])){
            text[j++] = text[i];
        }
    }
    text[j] = '\0';
    return text;
}

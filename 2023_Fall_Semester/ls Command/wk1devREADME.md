# README.md


### wk1dev.c :
- cInfo() : Reads through /proc/cpuinfo and prints out the list of CPUs as well as the cache size for each CPU.
  - Open "/proc/cpuinfo" & initalize all variables.
  ```
    FILE *cpuInfo = fopen("/proc/cpuinfo","rb");
    char buffer[128];
    char *processNum = (char *)malloc(128 * sizeof(char));
    strcpy(processNum,"0");
    char *processSize = NULL;
  ```
  - While loop condition is while there is still something to read in the cpuInfo file. Check if there is "processor" in buffer (line in cpuInfo). If there is, copy grabNum(processNumloc) to processSize and continue. Check if "there is cache size" in buffer (line in cpuInfo). If there is, copy grabNum(processSize) to itself, then print out the current processor string as well as the current processor size. After the while loop exits, free processNum and return 0.
  ```
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
  ```
- mInfo() : Reads through /proc/meminfo and prints out the amount of free memory and buffer size.
  - Open "/proc/meminfo" and initialize variable.
  ```
   FILE *memInfo = fopen("/proc/meminfo","rb");
   char buffer[128];
  ```
  - While loop conidition is while there is still something to read in the memInfo file. Check if there is "MemFree" in buffer. If there is, copy grabNum(memNum) to memNum. Print out the free memory and continue. Check if there is "buffNum" in buffer. If ther is, copy grabNum(buffNum) to buffNum and print out the buffers and break out of the while loop. Return 0.
  ```
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
  ```
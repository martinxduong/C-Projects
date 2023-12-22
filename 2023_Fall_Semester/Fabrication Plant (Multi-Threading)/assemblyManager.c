// assemblyManager.c : this is the child process. This is where the 2 worker threads get parts from railwayCars.txt and place them into the conveyor belt where the 2 consumer threads get the parts and write them into the output files.

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
#include <stdbool.h>
#include "fabricationPlant.h"



// Function Declarations: 
void *workerThread(void *arg);
void *consumerThread(void *arg);
int put(Parts part, char conveyor);
int get(char conveyor);
char* dataOutput(int part, int sequence);

// Struct Declaration:
Sync_Struct syncData = {.fullBlue = PTHREAD_COND_INITIALIZER,
                        .fullRed = PTHREAD_COND_INITIALIZER,
                        .emptyBlue = PTHREAD_COND_INITIALIZER,
                        .emptyRed = PTHREAD_COND_INITIALIZER,
                        .condL= PTHREAD_COND_INITIALIZER,
                        .condR = PTHREAD_COND_INITIALIZER,
                        .buffer_lockBlue = PTHREAD_MUTEX_INITIALIZER,
                        .buffer_lockRed = PTHREAD_MUTEX_INITIALIZER,
                        .lockReading = PTHREAD_MUTEX_INITIALIZER,
                        .railway = 0,
                        .blue = 0,
                        .red = 0,
                        .readEOF = false,
                        .sequence = 0,
                        .shutdown = 0,
                        .blueHead = 0,
                        .blueTail = 0,
                        .maxLenBlue = 10, 
                        .redHead = 0,
                        .redTail = 0,
                        .maxLenRed = 15,
};

// int main() : converts all the .txt files from type char to type int and store them into the global struct. Create all the threads and join them to ensure the main thread does not finish before the worker and consumer threads. Once worker and consumer threads are finished, close the .txt files.
int main(int argc, char *argv[]){
    syncData.railway = atoi(argv[1]);
    syncData.blue = atoi(argv[2]);
    syncData.red = atoi(argv[3]);
    pthread_t threadL;
    pthread_t threadR;
    pthread_t threadX;
    pthread_t threadY;
    pthread_create(&threadL, NULL, workerThread, "L");
    pthread_create(&threadR, NULL, workerThread, "R");
    pthread_create(&threadX, NULL, consumerThread, "B");
    pthread_create(&threadY, NULL, consumerThread, "R");
    pthread_join(threadL, NULL);
    pthread_join(threadR, NULL);
    pthread_join(threadX, NULL);
    pthread_join(threadY, NULL);
    close(syncData.railway);
    close(syncData.blue);
    close(syncData.red);
    return 0;
}

// workerThread() : initialize a bool eof to false and check which worker thread it is (L or R). For L, it initializes some values and enters a while eof is false, grab the lock and place all the items that it read into its own private buffer and then onto the conveyor belt. If it read EOF then it would put -1 in both sequence and parts. It then releases the lock and signals any that are waiting for the lock. For R, it does a similar thing with a few changes due to specifications.
void *workerThread(void *arg){
    bool eof = false;
    if (strcmp(arg, "L") == 0){
        Parts bufferL[25];
        size_t bufferLNum;
        while(!eof){
            pthread_mutex_lock(&syncData.lockReading);
            bufferLNum = 0;
            for(size_t l = 0; l < 25; l++){
                ssize_t bytes;
                char c;
                char partStr[3];
                int cIdx = 0;
                while ((bytes = read(syncData.railway, &c, sizeof(char))) > 0){
                    if (c == ' '){
                        partStr[cIdx] = '\0';
                        cIdx = 0;
                        break;
                    }
                    partStr[cIdx++] = c;
                }
                if (bytes == 0 && cIdx == 0){
                    bufferL[l].sequence_number = -1;
                    bufferL[l].part_number = -1;
                    bufferLNum++;
                    eof = true;
                    printf("workerThreadL read EOF\n");
                    break;
                } else {
                    bufferLNum++;
                    bufferL[l].sequence_number = ++syncData.sequence;
                    bufferL[l].part_number = atoi(partStr);
                    printf("workerThreadL read | part: %d\tsequence: %d\n",bufferL[l].part_number,bufferL[l].sequence_number);
                }
            }
            for(size_t l = 0; l < bufferLNum; l++){
                if (bufferL[l].part_number >= 1 && bufferL[l].part_number <= 12){
                    printf("workerThreadL putting into Blue | part: %d\tsequence: %d\n",bufferL[l].part_number,bufferL[l].sequence_number);
                    put(bufferL[l], 'b');
                } else if (bufferL[l].part_number >= 13 && bufferL[l].part_number <= 25){
                    printf("workerThreadL putting into Red | part: %d\tsequence: %d\n",bufferL[l].part_number,bufferL[l].sequence_number);
                    put(bufferL[l], 'r');
                } else if (bufferL[l].part_number == -1){
                    printf("workerThreadL putting EOF into Blue | part: %d\tsequence: %d\n",bufferL[l].part_number,bufferL[l].sequence_number);
                    put(bufferL[l], 'b');
                } else{
                    printf("threadL Buffer ERROR | Part: %d | Seq: %d \n",bufferL[l].part_number,bufferL[l].sequence_number);
                }
                printf("workerThreadL delaying for .25 seconds...\n");
                usleep(250000);
                printf("workerThreadL delay done!\n");
            }
            pthread_cond_signal(&syncData.condR);
            if (!eof){
                pthread_cond_wait(&syncData.condL, &syncData.lockReading);
            }
            pthread_mutex_unlock(&syncData.lockReading);
        }
        char *success = "Worker Thread L exit successfully";
        pthread_exit(success);
    } else if (strcmp(arg, "R") == 0){
        Parts bufferR[15];
        size_t bufferRNum;
        while(!eof){
            bufferRNum = 0;
            pthread_mutex_lock(&syncData.lockReading);
            for(size_t r = 0; r < 15; r++){
                ssize_t bytes;
                char c;
                char partStr[3];
                int cIdx = 0;
                while ((bytes = read(syncData.railway, &c, sizeof(char))) > 0){
                    if (c == ' '){
                        partStr[cIdx] = '\0';
                        cIdx = 0;
                        break;
                    }
                    partStr[cIdx++] = c;
                }
                if (bytes == 0 && cIdx == 0){
                    bufferR[r].sequence_number = -1;
                    bufferR[r].part_number = -1;
                    bufferRNum++;
                    eof = true;
                    printf("workerThreadR read EOF\n");
                    break;
                } else {
                    bufferRNum++;
                    bufferR[r].sequence_number = ++syncData.sequence;
                    bufferR[r].part_number = atoi(partStr);
                    printf("workerThreadR read | part: %d\tsequence: %d\n",bufferR[r].part_number,bufferR[r].sequence_number);
                }
            }
            for(size_t r = 0; r < bufferRNum; r++){
                if (bufferR[r].part_number >= 1 && bufferR[r].part_number <= 12){
                    printf("workerThreadR putting into Blue | part: %d\tsequence: %d\n",bufferR[r].part_number,bufferR[r].sequence_number);
                    put(bufferR[r], 'b');
                } else if (bufferR[r].part_number >= 13 && bufferR[r].part_number <= 25){
                    printf("workerThreadR putting into Red | part: %d\tsequence: %d\n",bufferR[r].part_number,bufferR[r].sequence_number);
                    put(bufferR[r], 'r');
                } else if (bufferR[r].part_number == -1){
                    printf("workerThreadR putting EOF into Blue | part: %d\tsequence: %d\n",bufferR[r].part_number,bufferR[r].sequence_number);
                    put(bufferR[r], 'b');
                } else {
                    printf("threadR Buffer ERROR | Part: %d | Seq: %d \n",bufferR[r].part_number,bufferR[r].sequence_number);
                }
                printf("workerThreadR delaying for .5 seconds...\n");
                usleep(500000);
                printf("workerThreadR delay done!\n");
            }
            pthread_cond_signal(&syncData.condL);
            if (!eof){
                pthread_cond_wait(&syncData.condR, &syncData.lockReading);
            }
            pthread_mutex_unlock(&syncData.lockReading);
        }
        char *success = "Worker Thread R exit successfully";
        pthread_exit(success);
    } else {
        printf("Invalid use of workerThread. Valid parameters are either \"L\" or \"R\"\n");
        exit(1);
    }
}

// consumerThread() : while readEOF is false, get() and then delay.
void *consumerThread(void *arg){
     if(strcmp(arg, "B") == 0){
        while (syncData.readEOF == false){
            get('b');
            printf("consumerThreadB delaying for .2 seconds...\n");
            usleep(200000);
            printf("consumerThreadB delay done!\n");
        }
        char *success = "Consumer Thread B exit successfully";
        pthread_exit(success);
        
    } else if (strcmp(arg, "R") == 0){
        while (syncData.readEOF == false){
            get('r');
            printf("consumerThreadR delaying for .2 seconds...\n");
            usleep(200000);
            printf("consumerThreadB delay done!\n");
        }
        char *success = "Consumer Thread B exit successfully";
        pthread_exit(success);
    } else {
        printf("Invalid use of consumerThread. Valid parameters are either \"B\" or \"R\"\n");
        exit(1);
    }
}

// int put() : grabs lock, checks if next is maxLen, if it is set next to 0. while next == tail, it is full so wait. After, signal it is filled.
int put(Parts part, char conveyor){
    if (conveyor == 'b'){
        pthread_mutex_lock(&syncData.buffer_lockBlue);
        int next;
        next = syncData.blueHead + 1;
        if (next >= syncData.maxLenBlue){
            next = 0;
        }
        while (next == syncData.blueTail){
            pthread_cond_wait(&syncData.emptyBlue, &syncData.buffer_lockBlue);
        }
        syncData.partsBlue[syncData.blueHead] = part;
        syncData.blueHead = next;
        pthread_cond_signal(&syncData.fullBlue);
        pthread_mutex_unlock(&syncData.buffer_lockBlue);
    } else if (conveyor == 'r'){
        pthread_mutex_lock(&syncData.buffer_lockRed);
        int next;
        next = syncData.redHead + 1;
        if (next >= syncData.maxLenRed){
            next = 0;
        }
        while (next == syncData.redTail){
            pthread_cond_wait(&syncData.emptyRed, &syncData.buffer_lockRed);
        }
        syncData.partsRed[syncData.redHead] = part;
        syncData.redHead = next;
        pthread_cond_signal(&syncData.fullRed);
        pthread_mutex_unlock(&syncData.buffer_lockRed);
    } else {
        printf("Error: Invalid usage of put(Parts *part, char conveyor). char conveyor only takes 'b' or 'r'.\n");
        exit(1);
    }
    return 0;
}

// int get() : grabs lock, while head == tail, wait. if next is max, then set next to 0. If read not -1, then write to file, if it did read -1, set readEOF to true and signal.
int get(char conveyor){
    if (conveyor == 'b'){
        pthread_mutex_lock(&syncData.buffer_lockBlue);
        int next;
        while (syncData.blueHead == syncData.blueTail){
            pthread_cond_wait(&syncData.fullBlue, &syncData.buffer_lockBlue);
        }
        next = syncData.blueTail + 1;
        if (next >= syncData.maxLenBlue){
            next = 0;
        }
        if (syncData.partsBlue[syncData.blueTail].part_number != -1){
            printf("consumerThreadB getting from Blue | Part: %d\tSequence: %d\n",syncData.partsBlue[syncData.blueTail].part_number, syncData.partsBlue[syncData.blueTail].sequence_number);
            char *data = dataOutput(syncData.partsBlue[syncData.blueTail].part_number, syncData.partsBlue[syncData.blueTail].sequence_number);
            ssize_t bytes = write(syncData.blue, data, strlen(data));
            if (bytes == -1){
                printf("Error: Could not write().\n");
            }
        }
        if (syncData.partsBlue[syncData.blueTail].part_number == -1){
            syncData.readEOF = true;
            pthread_cond_signal(&syncData.fullRed);
        }
        syncData.blueTail = next;
        pthread_cond_signal(&syncData.emptyBlue);
        pthread_mutex_unlock(&syncData.buffer_lockBlue);
    } else if (conveyor == 'r'){
        pthread_mutex_lock(&syncData.buffer_lockRed);
        int next;
        while (syncData.redHead == syncData.redTail){
            if (syncData.readEOF == true){
                break;
            }
            pthread_cond_wait(&syncData.fullRed, &syncData.buffer_lockRed);
        }
        if (syncData.readEOF == true){
            return 0;
        }
        next = syncData.redTail + 1;
        if (next >= syncData.maxLenRed){
            next = 0;
        }
        if (syncData.partsRed[syncData.redTail].part_number != -1){
            printf("consumerThreadR getting from Red | Part: %d\tSequence: %d\n",syncData.partsRed[syncData.redTail].part_number, syncData.partsRed[syncData.redTail].sequence_number);
            char *data = dataOutput(syncData.partsRed[syncData.redTail].part_number, syncData.partsRed[syncData.redTail].sequence_number);
            ssize_t bytes = write(syncData.red, data, strlen(data));
            if (bytes == -1){
                printf("Error: Could not write().\n");
            }
        }
        if (syncData.partsRed[syncData.redTail].part_number == -1){
            syncData.readEOF = true;
        }
        syncData.redTail = next;
        pthread_cond_signal(&syncData.emptyRed);
        pthread_mutex_unlock(&syncData.buffer_lockRed);
    } else {
        printf("Error: Invalid usage of get(char conveyor). char conveyor only takes 'b' or 'r'.\n");
        exit(1);
    }
    return 0;
}

// char* dataOutput() : converts the parts and sequence into a string that can be written to the file.
char* dataOutput(int part, int sequence){
    char partStr[3];
    char sequenceStr[4];
    snprintf(partStr, sizeof(partStr), "%d", part);
    snprintf(sequenceStr, sizeof(sequenceStr), "%d", sequence);
    int dataLen = sizeof(partStr) + sizeof(sequenceStr) + 21;
    char *data = (char *)malloc(dataLen + 1);
    strcpy(data, "Part: ");
    strcat(data, partStr);
    strcat(data, " \tSequence:");
    strcat(data, sequenceStr);
    strcat(data, "\n");
    return data;
}


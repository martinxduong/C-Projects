#ifndef fabricationPlant_H
#define fabricationPlant_H
#include <stdio.h>
#include <pthread.h>
#include <stdbool.h>

// fabricationManager.h : the header file for all of the C files and where I declared my structs and my declarations

struct __parts{
    int sequence_number;
    int part_number;
};
typedef struct __parts Parts;

struct __synchronization{ // if you contain all information in one struct you can easily pass it around to your threads since they only accept one argument
    pthread_cond_t fullBlue; // signaling a pause
    pthread_cond_t fullRed; // signaling a pause
    pthread_cond_t emptyBlue; // signaling a continue
    pthread_cond_t emptyRed; // signaling a continue
    pthread_cond_t condL;
    pthread_cond_t condR;
    pthread_mutex_t buffer_lockBlue; // for blocking critical section access
    pthread_mutex_t buffer_lockRed; // for blocking critical section access
    pthread_mutex_t lockReading; // for writing to the sequence number
    int railway;
    int blue;
    int red;
    bool readEOF;
    size_t sequence;
    size_t shutdown;
    size_t blueHead; 
    size_t blueTail; 
    size_t maxLenBlue; 
    size_t redHead; 
    size_t redTail; 
    size_t maxLenRed; 
    Parts partsBlue[10];
    Parts partsRed[15];
};
typedef struct __synchronization Sync_Struct;
//sync.parts = (Parts*)(malloc(sizeof(Parts));

// Function Declarations

#endif
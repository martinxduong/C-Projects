#ifndef spellChecker_H
#define spellChecker_H
#include <stdio.h>
#include <pthread.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <arpa/inet.h>

struct flags{
    int dFlag;
    int pFlag;
    int tFlag;
    int cFlag;
    int sFlag;
    char dictionaryPath[1000];
    int portNumber;
    int numThreads;
    char scheduleType[16];
    
    
};

struct connection{
    int connectionSocket;
    char *timeSocketReceived;
    int priority; // random number 1-10
};

struct __synchronization{
    // condition and mutexes
    pthread_cond_t connectionQueueFull;
    pthread_cond_t connectionQueueIsEmpty;

    pthread_cond_t logQueueFull;
    pthread_cond_t logQueueIsEmpty;

    pthread_mutex_t connectionQueueLock;
    pthread_mutex_t logQueueLock;

    // to implement connecton Queue in FIFO (use same code from project 2)
    int numItems;
    int putIndexConnection;
    int getIndexConnection;
    int putIndexLog;
    int getIndexLog;

    int numCells;
    char schedule;

    char **dictionary;
    int dictionaryCount;

    char **logQueue;
    struct connection *connectionBuffer;
};

typedef struct __synchronization Sync;

#endif
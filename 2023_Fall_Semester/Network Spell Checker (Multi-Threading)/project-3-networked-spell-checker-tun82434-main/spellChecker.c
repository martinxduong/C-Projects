#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <dirent.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <pthread.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <getopt.h>
#include "spellChecker.h"


void *workerThread(void *arg);
void *logThread(void *arg);
int putConnection(struct connection connect);
struct connection getConnection();
int putLog(char *string);
char *getLog();
int compare(const void *a, const void *b);

Sync syncData = {
        .connectionQueueFull = PTHREAD_COND_INITIALIZER,
        .connectionQueueFull= PTHREAD_COND_INITIALIZER,
        .logQueueFull = PTHREAD_COND_INITIALIZER,
        .logQueueIsEmpty = PTHREAD_COND_INITIALIZER,
        .connectionQueueLock = PTHREAD_MUTEX_INITIALIZER,
        .logQueueLock = PTHREAD_MUTEX_INITIALIZER,
        .numItems = 0,
        .putIndexConnection = 0,
        .getIndexConnection = 0, 
        .getIndexLog = 0,
        .putIndexLog = 0,
    };

/*
int main(int argc, char *argv[])
  - Sets the default dictionary, default port. Initialize struct flags flag and opt. Process all of the flags and determine the schedule type, dictionary, and port.
  - Initialize both log and connection buffer 
  - Initialize server and open dictionary file and extract to array.
  - Once binded, wait for incoming connections, and initialize all the threads.
  - While (1), accept all incoming connection and place into a struct with a priority and send to connectionQueue.
*/

int main(int argc, char *argv[]){
    char *DEFAULT_DICTIONARY = "/usr/share/dict/american-english";
    int DEFAULT_PORT = 8888;
    struct flags flag = {0, 0, 0, 0, 0, "", 0, 0, ""};
    int opt;
        while ((opt = getopt(argc, argv, ":d:p:t:c:s:")) != -1) {
            switch (opt) {
                case 'd':
                    flag.dFlag = 1;
                    strcpy(flag.dictionaryPath, optarg);
                    break;
                case 'p':
                    flag.pFlag = 1;
                    flag.portNumber = atoi(optarg);
                    break;
                case 't':
                    flag.tFlag = 1;
                    flag.numThreads = atoi(optarg);
                    break;
                case 'c':
                    flag.cFlag = 1;
                    syncData.numCells = atoi(optarg);
                    break;
                case 's':
                    flag.sFlag = 1;
                    strcpy(flag.scheduleType, optarg);
                    break;
                case '?':
                    printf("Unknown option: -%c\n", optopt);
                    break;
        }
    }
    if (strcmp(flag.scheduleType, "FIFO") == 0){
        syncData.schedule = 'F';
    } else if (strcmp(flag.scheduleType, "PRIORITY") == 0){
        syncData.schedule = 'P';
    } else {
        printf("Error: Valid Usage for Scheduling Type: [-s] FIFO or [-s] PRIORITY.\n");
        exit(1);
    }
    if (flag.dFlag == 0){ // use default dictionary
        strcpy(flag.dictionaryPath, DEFAULT_DICTIONARY);
    } 
    if (flag.pFlag == 0){ // use default port
        flag.portNumber = DEFAULT_PORT;
    }
    printf("Dictionary File: %s\n", flag.dictionaryPath);
    printf("Port Number: %d\n", flag.portNumber);
    printf("Threads: %d\n", flag.numThreads);
    printf("Cells: %d\n", syncData.numCells);
    printf("Scheduling Type: %c\n", syncData.schedule);
    syncData.connectionBuffer = malloc(syncData.numCells * sizeof(struct connection));
    syncData.logQueue = (char **)malloc(syncData.numCells * sizeof(char *)); // used same num as connection queue.
    if (syncData.connectionBuffer == NULL || syncData.logQueue == NULL){
        printf("Error: connectionBuffer or logQueue memory allocation failed.\n");
        exit(1);
    }
    int socketDesc, newSocket, c;
    struct sockaddr_in server, client;
    FILE *dictionaryFile;
    char line[100];
    dictionaryFile = fopen(flag.dictionaryPath, "r");
    if (dictionaryFile == NULL){
        printf("Error: opening dictionaryFile.\n");
    }
    while (fgets(line, sizeof(line), dictionaryFile) != NULL){
        size_t wordLen = strlen(line);
        if (wordLen > 0 && line[wordLen - 1] == '\n'){
            line[wordLen - 1] = '\0';
        }
        char *dictionaryWord = strdup(line);

        if (dictionaryWord == NULL){
            printf("Error: dictionaryWord memory allocation failed.\n");
            exit(1);
        }
       syncData.dictionary = realloc(syncData.dictionary, (syncData.dictionaryCount + 1) * sizeof(char *));
        if (syncData.dictionary == NULL){
            printf("Error: dictionary memory realocation failed.\n");
            exit(1);
        }
        syncData.dictionary[syncData.dictionaryCount++] = dictionaryWord;
    }

    // Create Socket (create active socket descriptor)
    socketDesc = socket(AF_INET, SOCK_STREAM, 0);
    if (socketDesc == -1){
        printf("Error: could not create socket.\n");
        exit(1);
    }
    // Prepare sockaddr_instructure
    server.sin_family = AF_INET; 
    server.sin_addr.s_addr = INADDR_ANY; // defaults to 127.0.0.1 
    server.sin_port = htons(flag.portNumber);
    // Bind (connect the server's socket address to the socket descriptor)
    int bindResult = bind(socketDesc, (struct sockaddr *)&server, sizeof(server));
    if (bindResult < 0){
        printf("Error: could not bind.\n");
        exit(1);
    }
    printf("Bind Completed.\n");
    // Listen (converts active socket to a LISTENING socket which can accept connections)
    listen(socketDesc, flag.numThreads);
    printf("Waiting for incoming connections...\n");
    c = sizeof(struct sockaddr_in);
    pthread_t threadLog;
    pthread_t threadWorkers[flag.numThreads];
    for (size_t i = 0; i < flag.numThreads; i++){
        if (pthread_create(&threadWorkers[i], NULL, workerThread, NULL) != 0){
            printf("Error: creating workerThreads.\n");
            exit(1);
        }
    }
    pthread_create(&threadLog, NULL, logThread, NULL);
    while (1){
        // Accept an incoming connection; create a new CONNECTED descriptor
        newSocket = accept(socketDesc, (struct sockaddr *)&client, (socklen_t*)&c);
        if (newSocket < 0){
            printf("Error: Could not accept.\n");
            continue;
        }
        struct connection connect = {
            .connectionSocket = newSocket,
            .priority = rand() % 10 + 1,
        };
        printf("Connection Accepted.\n");
        char *queueMessage = "You have entered the queue, please wait..\n";
        write(newSocket, queueMessage, strlen(queueMessage));
        putConnection(connect);
        pthread_mutex_unlock(&syncData.connectionQueueLock);
    }
    return 0;
}

/*
void *workerThread(void *arg)
  - While (1), call getConnection, once connection retrieved, that is the start time.
  - While client is still sending words, calculate elapsed time, and "clean up" the client message.
  - From what I observed, once data is retrieved by the server from the client, there is an additional 2 extra bytes at the end. I also "cleaned up" the client's message by detecting "\" for special characters and made the client's word lowercase.
  - Then, check if the clean word is in the dictionary and create a string of the results and send to the log queue.
*/

void *workerThread(void *arg){
    while (1){
        struct connection connect = getConnection();
        time_t startTime;
        time(&startTime);
        char *message;
        message = "Handler has accepted your connection.\nPlease enter a word for spell checking:\n";
        write(connect.connectionSocket, message, strlen(message));
        ssize_t read_size;
        char client_message[2000];
        while ((read_size = recv(connect.connectionSocket, client_message, sizeof(client_message) - 1, 0)) > 0){
            time_t endTime;
            endTime = time(NULL);
            double elapsedTime = difftime(endTime,startTime);
            char elapsedTimeStr[32];
            snprintf(elapsedTimeStr, sizeof(elapsedTimeStr), "%d", (int)elapsedTime);
            connect.timeSocketReceived = (char *)malloc(strlen(elapsedTimeStr) + 1);
            strcpy(connect.timeSocketReceived, elapsedTimeStr);
            client_message[read_size] = '\0';
            char userInput[read_size - 1];
            int i = 0;
            for (; i < read_size - 2; i++){
                if (client_message[i] == ' ' || client_message[i] == '\\'){
                    break;
                }
                userInput[i] = tolower(client_message[i]);
            }
            userInput[i] = '\0';
            bool wordFound = false;
            for (size_t i = 0; i < syncData.dictionaryCount; i++){
                if (strcmp(userInput, syncData.dictionary[i]) == 0){
                    wordFound = true;
                }
            }
            char *responseMessage;
            if (wordFound == true){
                responseMessage = (char *)malloc(strlen(userInput) + strlen(" + OK\n") + 1);// t= s
                char priorityString[20];
                sprintf(priorityString, "%d", connect.priority);
                strcat(responseMessage, userInput);
                strcat(responseMessage, " + OK\n\0");
                write(connect.connectionSocket, responseMessage, strlen(responseMessage));
                char *logMessage = (char *)malloc(strlen("T= s Priority=  ") + strlen(connect.timeSocketReceived) + strlen(responseMessage)+1);
                strcpy(logMessage,"T=");
                strcat(logMessage, connect.timeSocketReceived);
                strcat(logMessage, "s Priority=");
                strcat(logMessage, priorityString);
                strcat(logMessage, " ");
                strcat(logMessage, responseMessage);
                putLog(logMessage);
            } else {
                responseMessage = (char *)malloc(strlen(userInput) + strlen(" + MISSPELLED\n") + 1);
                char priorityString[20];
                sprintf(priorityString, "%d", connect.priority);
                strcat(responseMessage, userInput);
                strcat(responseMessage, " + MISSPELLED\n\0");
                write(connect.connectionSocket, responseMessage, strlen(responseMessage));
                char *logMessage = (char *)malloc(strlen("T= s Priority=  ") + strlen(connect.timeSocketReceived) + strlen(responseMessage)+1);
                strcpy(logMessage,"T=");
                strcat(logMessage, connect.timeSocketReceived);
                strcat(logMessage, "s Priority=");
                strcat(logMessage, priorityString);
                strcat(logMessage, " ");
                strcat(logMessage, responseMessage);
                putLog(logMessage);
            }
        }
        if (read_size == 0){
            printf("Client Disconnected.\n");
            fflush(stdout);
            continue;
        } else if (read_size == -1){
            printf("Error: recv failed.\n");
        }
    }
    return 0;
}

/*
int putConnection(struct connection connect)
  - If FIFO:
    - Lock, set next = head + 1 and if next is >= numCells then next = 0.
    - While next == getIndexConnection, wait. Once out, put the connect into the buffer and set the index = next and broadcast and unlock.
  - If PRIORITY:
    - Lock, if next = head + 1 and if next >= numCells then wait. 
    - Once out, add connect to connection buffer, run qsort on the priority of the connections and set head = next and broadcast and unlock.
*/

int putConnection(struct connection connect){
    if (syncData.schedule  == 'F'){
        pthread_mutex_lock(&syncData.connectionQueueLock);
        char *queueMessage = "You are currently in a queue, please wait...\n";
        write(connect.connectionSocket, queueMessage, strlen(queueMessage));
        int next = syncData.putIndexConnection + 1;
        if (next >= syncData.numCells){
            next = 0;
        }
        while (next == syncData.getIndexConnection){
            printf("Connection Queue is Full.\n");
            pthread_cond_wait(&syncData.connectionQueueIsEmpty, &syncData.connectionQueueLock);
        }
        //printf("Put Index Connection: %d\n",syncData.putIndexConnection);
        syncData.connectionBuffer[syncData.putIndexConnection] = connect;
        syncData.putIndexConnection = next;
        pthread_cond_broadcast(&syncData.connectionQueueFull);
        pthread_mutex_unlock(&syncData.connectionQueueLock);
    } else if (syncData.schedule  == 'P'){
        //printf("putConnection() Before: getIndex %d putIndex %d\n", syncData.getIndexConnection, syncData.putIndexConnection);
        pthread_mutex_lock(&syncData.connectionQueueLock);
        char *queueMessage = "You are currently in a queue, please wait...\n";
        write(connect.connectionSocket, queueMessage, strlen(queueMessage));
        int next = syncData.putIndexConnection + 1;
        if (next >= syncData.numCells){
            printf("Connection Queue is Full.\n");
            pthread_cond_wait(&syncData.connectionQueueIsEmpty, &syncData.connectionQueueLock); // if next == eg. 3 then once it gets, syncData.putIndex will be deincremented. put index will never reach 3 only 2.
        }
        syncData.connectionBuffer[syncData.putIndexConnection] = connect;
        qsort(syncData.connectionBuffer, syncData.putIndexConnection + 1, sizeof(struct connection), compare);
        syncData.putIndexConnection = next;
        pthread_cond_broadcast(&syncData.connectionQueueFull);
        pthread_mutex_unlock(&syncData.connectionQueueLock);
        //printf("putConnection() After: getIndex %d putIndex %d\n", syncData.getIndexConnection, syncData.putIndexConnection);
    }
    return 0;
}

/*
int getConnection()
  - If FIFO:
    - Lock and if head == tail then wait. Once out, set next = tail + 1 and if next >= numCells then next = 0.
    - Save current index, set tail to next
    - signal, unlock, and return the connection.
  - If PRIORITY:
    - lock and while head == tail, wait. Once out, connect == the tail (for priority it will always be 0 (first index).). Deincrement head and shift all the numbers in the array back and sort.
    - Signal unlock, and return highest priority connection.
*/

struct connection getConnection(){
    if (syncData.schedule == 'F'){
        pthread_mutex_lock(&syncData.connectionQueueLock);
        int next;
        while (syncData.putIndexConnection == syncData.getIndexConnection){
            printf("Connection Queue is Empty.\n");
            pthread_cond_wait(&syncData.connectionQueueFull, &syncData.connectionQueueLock);
        }
        next = syncData.getIndexConnection + 1;
        if (next >= syncData.numCells){
            next = 0;
        }
        //printf("Get Index Connection: %d\n",syncData.getIndexConnection);
        int currentIdx = syncData.getIndexConnection;
        syncData.getIndexConnection = next;
        pthread_cond_signal(&syncData.connectionQueueIsEmpty);
        pthread_mutex_unlock(&syncData.connectionQueueLock);
        return syncData.connectionBuffer[currentIdx];
    } else if (syncData.schedule  == 'P'){
        pthread_mutex_lock(&syncData.connectionQueueLock);
        //printf("getConnection() Before: getIndex %d putIndex %d\n", syncData.getIndexConnection, syncData.putIndexConnection);
        while (syncData.getIndexConnection == syncData.putIndexConnection){
            printf("Connection Queue is Empty.\n");
            pthread_cond_wait(&syncData.connectionQueueFull, &syncData.connectionQueueLock);
        }
        struct connection connect = syncData.connectionBuffer[syncData.getIndexConnection];
        syncData.putIndexConnection--;
        for (int i = 0; i <= syncData.putIndexConnection; i++){
            syncData.connectionBuffer[i] = syncData.connectionBuffer[i+1];
        }
        qsort(syncData.connectionBuffer, syncData.putIndexConnection + 1, sizeof(struct connection), compare);
        
        pthread_cond_signal(&syncData.connectionQueueIsEmpty);
        pthread_mutex_unlock(&syncData.connectionQueueLock);
        //printf("getConnection() After: getIndex %d putIndex %d\n", syncData.getIndexConnection, syncData.putIndexConnection);
        return connect;
    } else {
        printf("Error: schedule type is invalid.\n");
        exit(1);
    }
}

/*
int compare(const void *a, const void *b)
  - This is the compare algorithm for qsort.
  - typecast a and b to const struct connection and compare the priority.
  - b - a because we want it in descending order.
*/

int compare(const void *a, const void *b){
    return ( (*(const struct connection*)b).priority - (*(const struct connection *)a).priority);
}

/*
void *logThread(void *arg)
  - While (1) open the log file, if it exists, then append, if it doesn't then create.
  - get the string from log queue and write to the log file and close.
*/

void *logThread(void *arg){
    while (1){
        int log = open("log.txt", O_WRONLY | O_APPEND | O_CREAT , S_IRWXU);
        if (log == -1){
            printf("Error: could not create log file.\n");
            exit(1);
        }
        char *logInfo = getLog();
        ssize_t bytesWritten = write(log, logInfo, strlen(logInfo));
        if (bytesWritten == -1){
            printf("Error: could not write to log file.\n");
            exit(1);
        }
        close(log);
    }
    return 0;
}

/*
int putLog(char *string)
  - lock and do the normal put for circular queues as previously in the code and in the past labs.
*/

int putLog(char *string){
    pthread_mutex_lock(&syncData.logQueueLock);
    int next;
    next = syncData.putIndexLog + 1;
    if (next >= syncData.numCells){
        next = 0;
    } while (next == syncData.getIndexLog){
        printf("Log Queue is full.\n");
        pthread_cond_wait(&syncData.logQueueIsEmpty, &syncData.logQueueLock);
    }
    printf("Put Index Log: %d\n", syncData.putIndexLog);
    syncData.logQueue[syncData.putIndexLog] = (char *)malloc(strlen(string) + 1);
    strcpy(syncData.logQueue[syncData.putIndexLog],string); //strcpy??? or equal
    syncData.putIndexLog = next;
    pthread_cond_broadcast(&syncData.logQueueFull);
    pthread_mutex_unlock(&syncData.logQueueLock);
    return 0;
}

/*
int getLog(char *string)
  - lock and do the normal get for circular queues as previously in the code in the past labs.
*/

char *getLog(){
    pthread_mutex_lock(&syncData.logQueueLock);
    int next;
    while (syncData.putIndexLog == syncData.getIndexLog){
        printf("Log Queue is Empty.\n");
        pthread_cond_wait(&syncData.logQueueFull, &syncData.logQueueLock);
    }
    next = syncData.getIndexLog + 1;
    if (next >= syncData.numCells){
        next = 0;
    }
    printf("Get Index Log: %d\n", syncData.getIndexLog);
    int currentIdx = syncData.getIndexLog;
    syncData.getIndexLog = next;
    pthread_cond_broadcast(&syncData.logQueueIsEmpty);
    pthread_mutex_unlock(&syncData.logQueueLock);
    return syncData.logQueue[currentIdx];
}

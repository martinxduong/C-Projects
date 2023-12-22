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
#include <sys/socket.h>
#include <arpa/inet.h>
#include <getopt.h>
#include "spellChecker.h"

int main (int argc, char **argv){
    int sock_fd;
    struct sockaddr_in server;
    char *message;

    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd == -1){
        puts("Couldn't create socket.");
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_port = htons( 8888 );
    
    if ((connect(sock_fd, (struct sockaddr *)&server, sizeof(server)))<0){
        puts("Connect failed\n");
        exit(1);
    }
    puts("Connected");
    
    message = "MESSAGE TEST";
    if (send(sock_fd, message, strlen(message), 0) < 0){
        puts("Send failed\n");
        exit(1);
    }
    puts("Data Send\n");
    return 0;

}
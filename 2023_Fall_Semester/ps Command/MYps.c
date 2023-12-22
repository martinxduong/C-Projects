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

/*
Option p: PID for the specific process; if -p is not present, print for all processes with same UID
Option s: Displays the single-character state infromation about the process 3 state %c
Option U: DO NOT display the amount of user time consumed by this process 14 utime %lu
Option S: Display the amount of system time consuemd by this process 15 stime %lu
Option v: Display the amount of virtual memory current used statm first field
Option c: DO NOT display the command-line that started this program proc/pid/cmdline
*/
int main(int argc, char *argv[]){
    char **pids = NULL;
    struct flags flag = {0,0,0,0,0,0,""};
    int opt;
    while((opt = getopt(argc,argv,":p:sUSvc")) != -1){
        switch(opt){
            case 'p':
                int pARG = atoi(optarg);
                if (pARG == 0) {
                    printf("The -p option requires a PID\n");
                    exit(1);
                }
                flag.pFlag = 1;
                strcpy(flag.pid,optarg);
                break;
            case 's':
                flag.sFlag = 1;
                break;
            case 'U':
                flag.UFlag = 1;
                break;
            case 'S': 
                flag.SFlag = 1;
                break;
            case 'v':
                flag.vFlag = 1;
                break;
            case 'c':
                flag.cFlag = 1;
                break;
            default:
                printf("Invalid usage: ./MYps [-p] <PID> [-s] [-U] [-S] [-v] [-c]\n");
                exit(1);
        }
    }
    if (flag.pFlag == 1){ // p flag was used
        struct PIDinfo pidData;
        printf("%s: ", flag.pid);
        if(flag.sFlag){ 
            getPIDState(pidData,flag.pid);
        } if (!flag.UFlag){ 
            getPIDUtime(pidData,flag.pid);
        } if (flag.SFlag){ 
            getPIDStime(pidData,flag.pid);
        } if (flag.vFlag){ 
            getPIDVmem(pidData,flag.pid);
        } if (!flag.cFlag){ 
            getPIDcmdline(pidData,flag.pid);
        }
        printf("\n");
    } else{ // p flag was not used
        int pidCount = getPIDs(&pids);
        for(int i = 0; i < pidCount; i++){
            struct PIDinfo pidData;
            printf("%s: ", pids[i]);
            if(flag.sFlag){ 
                getPIDState(pidData,pids[i]);
            } if (!flag.UFlag){ 
                getPIDUtime(pidData,pids[i]);
            } if (flag.SFlag){ 
                getPIDStime(pidData,pids[i]);
            } if (flag.vFlag){ 
                getPIDVmem(pidData,pids[i]);
            } if (!flag.cFlag){ 
                getPIDcmdline(pidData,pids[i]);
            }
            printf("\n");
        }
    }
    free(pids);
    return 0;
}

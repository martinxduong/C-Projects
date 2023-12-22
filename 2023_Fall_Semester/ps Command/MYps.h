// MYps.h : the header file for all of the C files and where I declared my structs and my declarations

#ifndef MYps_H
#define MYps_h

struct flags{
    int pFlag;
    int sFlag;
    int UFlag;
    int SFlag;
    int vFlag;
    int cFlag;
    char pid[1000];
};

struct PIDinfo{
    char procState;
    unsigned int procUtime;
    unsigned int procStime;
    int procVMemSize;
    char cmdline[4096];
};

extern int pidCount;

// Function Declarations

int getPIDs(char ***matchingPIDs);
int getPIDState(struct PIDinfo pidData, char *pid);
int getPIDUtime(struct PIDinfo pidData, char *pid);
int getPIDStime(struct PIDinfo pidData,char *pid);
int getPIDVmem(struct PIDinfo pidData,char *pid);
int getPIDcmdline(struct PIDinfo pidData,char *pid);

#endif
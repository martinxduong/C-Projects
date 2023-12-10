#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>

// https://github.com/tun82434/3207-Project-0B

// Function Declaration:
int twoARGC(char **argv);
int nARGC(int argc, char **argv);

int main(int argc, char **argv){
    if (argc <= 2){ // less than 2 arguments
        printf("Error: Insufficient arguments, two arguments are required.\n");
        exit(1);
    } else if (argc == 3){ // 2 arguments
        twoARGC(argv);
    } else{ // greater than 2 arguments (n arguments)
        nARGC(argc, argv);
    }
    return 0;
}

int twoARGC(char **argv){
    struct stat path1, path2;
    FILE *sFile, *dFile; 
    if(stat(argv[1], &path1) == 0 && S_ISDIR(path1.st_mode)){ 
        printf("Error: Source file cannot be a directory.\n"); // argv[1] is a directory
        exit(1);
    } else if(stat(argv[1], &path1) == -1){
        printf("Error: Invalid source file path. stat() failure.\n"); // argv[1] is an invalid path
        exit(1);
    }
    sFile = fopen(argv[1],"rb");
    if (sFile == NULL){
        printf("Error: Invalid source file path. fopen() failure.\n"); // argv[1] is either a directory or an invalid path 
        exit(1);
    }
    if (stat(argv[2], &path2) == 0 && S_ISDIR(path2.st_mode)){ // argv[2] is a destination directory
        char *fileName = strrchr(argv[1], '/');
        if (fileName != NULL){
            fileName += 1;
        } else{
            fileName = argv[1];
        }
        size_t dPathLen = strlen(argv[2]) + strlen(fileName) + 1;
        char *dPath = (char *)malloc(dPathLen + 1); // allocate space
        strcpy(dPath,argv[2]);
        strcat(dPath, "/");
        strcat(dPath, fileName);
        dFile = fopen(dPath, "wb"); // dFile open with write binary
        if(dFile == NULL){
            printf("Error: Invalid destination file path. fopen() failure.\n");
            exit(1);
        }
        char line[100]; // initalize line for parsing through sFile
        size_t bytes;
        while ((bytes = fread(line, 1, sizeof(line), sFile)) > 0){
            fwrite(line, 1, bytes, dFile);
        }
        fclose(sFile);
        fclose(dFile);
        free(dPath);
        return 0;
    }
    // if dFile is a file
    dFile = fopen(argv[2], "wb");
    if (dFile == NULL){
        printf("Error: Invalid destination file path. fopen() failure.\n"); // argv[2] is a invalid path
        exit(1);
    }
    char line[100];
    size_t bytes;
    while ((bytes = fread(line, 1, sizeof(line), sFile)) > 0){
        fwrite(line, 1, bytes, dFile);
    }
    fclose(sFile);
    fclose(dFile); 
    return 0;
}

int nARGC(int argc, char **argv){
    int arguments = argc - 1; // Remove the ./tuls argument from the count
    for (int i = 1; i <= arguments; i++){
        if(i != arguments){ // if argv[i] are supposed to be source files
            FILE *sFile, *dFile;
            struct stat file;
            if(stat(argv[i], &file) == 0 && S_ISDIR(file.st_mode)){ // directory
                printf("Error: Argument [%d] must be a source file.\n", i);
                exit(1);
            } else if (stat(argv[i], &file) == -1){ // could not open
                printf("Error: Argument [%d] is an invalid source file path.\n", i);
                exit(1);
            }
            char *fileName = strrchr(argv[i], '/');
            if (fileName != NULL){
                fileName += 1;
            } else{
                fileName = argv[i];
            }
            size_t pathLen = strlen(argv[arguments]) + strlen(fileName) + 1;
            char *dPath = (char *)malloc(pathLen + 1);
            strcpy(dPath, argv[arguments]);
            strcat(dPath, "/");
            strcat(dPath, fileName);
            sFile = fopen(argv[i],"rb");
            if(sFile == NULL){
                printf("Error: Argument [%d]. fopen() failure.\n", i);
                exit(1);
            }
            dFile = fopen(dPath, "wb");
            if(dFile == NULL){
                printf("Error: Invalid Directory. fopen() failure.\n");
                exit(1);
            }
            char line[100];
            size_t bytes;
            while ((bytes = fread(line, 1, sizeof(line), sFile)) > 0){
                fwrite(line, 1, bytes, dFile);
            }
            fclose(sFile);
            fclose(dFile); 
        } else if (i == arguments){
            struct stat directory;
            if(stat(argv[i], &directory) != 0 || !(S_ISDIR(directory.st_mode))){ // argv[arguments] is in invalid path or is not a directory
                printf("Error: Argument [%d] is either an invalid destination path or is not a directory.\n", i);
                exit(1);
            }
        }
    }
    return 0;
}



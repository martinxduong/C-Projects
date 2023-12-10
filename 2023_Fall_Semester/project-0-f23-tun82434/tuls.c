#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>

// Function Declaration
int zeroARGC();
int oneARGC(char *path, int tabs);

int main(int argc, char **argv){
    if(argc == 1){ // 0 argument (excluding ./tuls)
        zeroARGC();
    } else if(argc == 2){ // 1 argument (excluding ./tuls)
        zeroARGC();
        oneARGC(argv[1],1); // argv[0] = ./tuls argv[1] = parameter ; ./
    } else{ // too many arguments : exit with an error code
        printf("Error: Too many arguments\n");
        exit(1); // Exit with a non-zero status means error; zero status means successful
    }
    return 0;
}

int zeroARGC(){
    struct dirent **namelist;
    int n;
    char buffer[1024];
    getcwd(buffer, sizeof(buffer));
    n = scandir(buffer, &namelist, NULL, alphasort);
    if (n < 0){
        printf("tuls: cannot open directory\n");
        exit(1);
    } else{
        while(n--){
            if((namelist[n]->d_name[0]) == '.'){ // skips over hidden files
                continue;
            } else{ // prints files & directories in current directory
                printf("%s\n", namelist[n]->d_name);
                free(namelist[n]);
            }
        }
    }
    free(namelist);
    return 0;
}

int oneARGC(char *path, int tabs){
    struct dirent **namelist;
    int n;
    n = scandir(path, &namelist, NULL, alphasort); // scandir returns the number of items in the directory
    if (n < 0){ // if scandir fails, n will be less than 0
        printf("tuls: cannot open directory\n");
        exit(1);
    } else{
        while(n--){ // decrease by 1 while n != 0
            if((namelist[n]->d_name[0]) == '.'){ // skips over hidden files
                continue;
            }
            else if(namelist[n]->d_type == DT_DIR){ // if it is a directory
                size_t pathLen = strlen(path) + 1 + namelist[n]->d_reclen; // New Path Size = path length + 1 (for the slash) + namelist[n] (directory)
                char *newPath = (char *)malloc(pathLen + 1); // Allocate memory for new path (+1 for '\0')
                strcpy(newPath,strcat(path, "/"));
                strcat(newPath,namelist[n]->d_name);
                for(int i = 0; i < tabs; i++){ // printing tabs (indicating levels in the directory)
                    printf("\t");
                }
                printf("[%s]\n", namelist[n]->d_name); // directories have [] to indicate their type
                oneARGC(newPath,tabs+1); // calls itself but with the parameter of the newpath and tabs +1
                free(namelist[n]);
                free(newPath);
            } else{ // if it is a folder
                for(int i = 0; i < tabs; i++){ 
                    printf("\t");
                }
                printf("%s\n", namelist[n]->d_name);
                free(namelist[n]);
            }
        }
    }
    free(namelist);
    return 0;
}

// alphasort: https://pubs.opengroup.org/onlinepubs/9699919799/functions/alphasort.html
// getcwd: https://pubs.opengroup.org/onlinepubs/007904975/basedefs/unistd.h.html
// opendir: https://pubs.opengroup.org/onlinepubs/009604599/functions/opendir.html
// DT_DIR: https://www.gnu.org/software/libc/manual/html_node/Directory-Entries.html

// GitHub Link: https://github.com/CIS-3207-F23/project-0-f23-tun82434




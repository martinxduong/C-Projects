Objective: recreate the ls command in linux. List information about the files in a directory.

Usage:
- ./tuls : when there are no parameters for the command, the program prints a list of files and directories in the current working directory.
- ./tuls [directory path] : when a directory path is included with the command, the program prints a list of files in the current working directory and recursively prints the list of files in all directories specified by the included directory path parameter.



int main(int argc, char **argv)
  - Has the parameter of int argc and char **argv. argc is the number of arguments, and char **argv is pointer to a pointer to an array of arguments
  - When argc == 1, then there is only the ./tuls argument. zeroARGC() function is called
  - When argc == 2, then there is both the ./tuls argument as well as the path. zeroARGC() and the oneARGC() function is called. oneARGC is called with the parameter of the path as well as 1 tab for spacing.
  - Else, there are too many arguments and the program returns and error and exits with 1.

int zeroARGC()
  - struct dirent **namelist is a pointer to a pointer to a struct dirent.
  - char buffer[1024] initializes the buffer for the getcwd command.
  - getcwd(buffer, sizeof(buffer)) gets the cwd and puts it in buffer.
  - scandir with the path (buffer) and put all the files and directories into namelist.
  - If n (the number of items in the cwd) is less than 0 then print error and exit with 1.
  - While n-- and != 0, skip over hidden files and prints files & directories in the cwd.
  - Once the program finishes, it free(namelist) and returns 0, indicating success in the function.

int oneARGC(char *path, int tabs)
  - Takes in a path, which when it is called in main() is argument with the path.
  - scandir with the path and if it cannot open print error and exit with 1.
  - While n-- and != 0, skip over hidden files and if it is a directory, then allocate memeory for the newPath and then concatenate and copy into the newPath string. The function then prints the directory with the tabs and brackets 
    to indicate it is a directory. Then call oneARGC with the newPath and tabs+1.
  - Once oneARGC with the parameters of newPath and tabs+1 finishes, then it frees up the namelist[n].
  - If it is a folder, then just print out the name and then free(namelist[n]).
  - Once the program finishes, it free(namelist) and returns 0, indicating success in the function.

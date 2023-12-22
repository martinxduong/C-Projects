MYPS.c README:

Compile and Run:
  - run command "make" to compile all of the C files.
  - ./MYps [-p] <PID> [-s] [-U] [-S] [-v] [-c]
  - [-p] <PID> : inputs a PID value into ./MYps; [-p] will not run if there is not PID
  - [-s] : displays the state of the PID(s)
  - [-U] : DOES NOT display the utime of the PID(s)
  - [-S] : displays the stime of the PID(s)
  - [-v] : displays the amount of the virtual memory sized currently being used for the PID(s)
  - [-c] : DOES NOT display the cmdline argument to run the PID(s)

Design Section:
  - I decided to split my C files into 7 different C files: MYps.c getPIDs.c getPIDcmdline.c getPIDState.c getPIDStime.c getPIDUtime.c getPIDVmem.c 
  - My "main" file is the MYps.c file and for every C file I passed the parameter pid and the struct PIDinfo.
  - Myps.c parameters is int argc (the number of arguments) and the arguments. My MYps.c file is where the program begins when they execute the code, it initializes an array of pids. I also initialize my flags struct and set all the flags to 0 (meaning flag is not used). Afterwards I go through the all of the flags from the input and if the p flag is present but the PID is not ,then it would print that the PID is required and exit with 1 (failure). But if a PID is present, set the pFlag to 1 (meaning flag is used), and copy the PID to flag.pid. Afterwards, it would go through all of the cases if they are present then set them to 1. Once the while loop through the flags is finished, check if flag.pFlag == 1 to see if the p flag was used, if it was print the pid and go through a series of if statements to see which flag was used and within those functions, print out the onces that were used. If flag.pFlag == 0 (else), go through a for loop for each of the pids in the array of pids.
  - int getPIDs(char ***matchingPIDs) : this function within the getPIDs.c file takes in the char **pids from MYps.c and go through /proc and go through all of the PIDs and adds only the PIDs with matching UID as my machine to the array of PIDs and returns the count of PIDs.
  - int getPIDState(struct PIDinfo pidData, char *pid) : this function within the getPIDState.c file takes in the struct PIDinfo and the pid, it then goes through /proc/[PID]/stat to find the state info and store it in the struct then prints out the state and returns 0 to end.
  - int getPIDStime(struct PIDinfo pidData, char *pid) : this function within the getPIDStime.c file takes in the struct PIDinfo and the pid, it then goes through /proc/[PID]/stat to find the stime info and store it in the struct then prints out the stime and returns 0 to end.
  - int getPIDUtime(struct PIDinfo pidData, char *pid) : this function within the getPIDUtime.c file takes in the struct PIDinfo and the pid, it then goes through /proc/[PID]/stat to find the utime info and store it in the struct then prints out the utime and returns 0 to end.
  - int getPIDVmem(struct PIDinfo pidData, char *pid) : this function within the getPIDVmem.c file takes in the struct PIDinfo and the pid, it then goes through /proc/[PID]/statm to find the Vmem info and store it in the struct then prints out the virtual memory size and returns 0 to end.
  - int getPIDcmdline(struct PIDinfo pidData, char *pid) : this function within the getPIDcmdline.c file takes in the struct PIDinfo and the pid, it then goes through /proc/[PID]/cmdline to find the stime info and store it in the struct then prints out the commandline to run the process and returns 0 to end.
  - MYps.h : this header file is where I declared all of my functions from all of my C files as well as the struct flags and the struct PIDinfo.

Testing Plan:
At first I was having a difficult time starting this project and visualizing it, I had the struct for all the flags but I couldn't figure out where to start. I then made a struct for the info on the specific PID and managed to get my MYps.c to get flags by testing it with if statements. I then started with my getPIDs function which at first started with just by printing it out, and then I adjusted it to put it in a pointer array. Once I had that, I coded the rest of my functions pretty much using the same format for each of them and then I made them store the values back into the struct and print them. Once I had the functions working I differentiated between when the PID is present or not for the input and if it was then the program would find all the valid PIDs and run a series of if statements to print out the information on the commandline.








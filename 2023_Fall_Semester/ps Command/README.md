MYPS.c README:

Objective: recreate the ps command in Linux. myps will read a variety of information about one or more running programs/processes on the computer.

Usage:
  - ./MYps [-p] <PID> [-s] [-U] [-S] [-v] [-c]
  - [-p] <PID> : inputs a PID value into ./MYps; [-p] will not run if there is not PID
  - [-s] : displays the state of the PID(s)
  - [-U] : DOES NOT display the utime of the PID(s)
  - [-S] : displays the stime of the PID(s)
  - [-v] : displays the amount of the virtual memory sized currently being used for the PID(s)
  - [-c] : DOES NOT display the cmdline argument to run the PID(s)









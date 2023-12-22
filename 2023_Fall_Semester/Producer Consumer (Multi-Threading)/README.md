fabricationManager.c assemblyManager.c fabricationPlant.hREADME:

Layout: fabricationManager.c contains the parent process that runs the child process, assemblyManager.c. Both .c files contain fabricationPlant.h header.

Compile and Run:
  - run command "make" to compile all of the C files.
  - make will compile 2 files, fabManager and assemManager.
  - ./fabManager to run the parent process.
  - There are no arguments needed to run, the program will print out the actions of the threads.

Design Section:
  - fabricationManager.c : parent process that fork() and exec() assemblyManager.c
  - assemblyManager.c : child process that creates 4 threads (2 worker threads, 2 consumer threads).
  - workerThread(void* arg) : arguments for workerThread is either "L" or "R" for workerThreadL or workerThreadR.
  - consumerThread(void* arg) : arguments for consumerThread is either "B" or "R" for consumerThreadB or consumerThreadR.
  - put(Parts part, char conveyor) : given a Parts struct and a character, put will put the part into the conveyorbelt. Argument for char conveyor is either 'r' or 'b' to put into redBuffer or blueBuffer.
  - get(char conveyor) : Argument for char conveyor is either 'r' or 'b' to get from the redBuffer or blueBuffer
  - dataOutput(int part, int sequence) : dataOutput takes in two integers, parts and sequence. dataOutput() will then return a char* which will be written into the files

Testing Plan:
  - I first created fabricationManager.c and assemblyManager.c to get the layout of the parent and child process.
  - I then ensured that parent process finishes once child process completes. After that, I fully coded and completed fabricationManager.c.
  - For assemblyManager.c I first only did the workerThread() and put() so that I can be able to ensure that it read and put correctly.
  - Afterwards, I worked on consumerThread() and get() so that I can be sure that it will get the item from the conveyor and write it correctly.
  - I then realized that I should put the parts and sequences as string, so then I created a separate function to do so. After testing that everything worked correctly with no deadlock, race condition, fairness, etc. I added in all the delay specifications.
  

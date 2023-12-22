spellChecker.c README:

Objective: create a networked spell checker server. Gain additional exposure and experience with multi-threading programming and synchronization problems along with it.
Gain experience writing programs that communicate across networks using sockets for communication.

Usage:
  - run command "make" to compile all of the C files.
  - ./MYps [-d] <dictionary> [-p] <port number> [-t] <number of threads> [-c] <number of cells> [-s] <scheduling type>
  - [-d] <dictionary> : (string) path to dictionary file.
  - [-p] <port number>: (int) port number.
  - [-t] <number of threads>: (int) number of threads.
  - [-c] <number of cells>: (int) number of cells.
  - [-s] <scheduling type>: (string) "FIFO" or "PRIORITY"

Specifications:
- If a dictionary argument is not provided, DEFAULT_DICTIONARY is used.
- If a port argument is not provided, DEFAULT_PORT is used.
- The server has two primary functions:
  1) Accept and distribute connection requests from clients.
  2) Construct a log file of all spell check activites.
- The server places all incoming connections into the specified queue and the threads will accept the connection and listens for the client to enter a word for spell checking.

Function Declarations & Global Variable:
  - void *workerThread(void *arg);
  - void *logThread(void *arg);
  - int putConnection(struct connection connect);
  - struct connection getConnection();
  - int putLog(char *string);
  - char *getLog();
  - int compare(const void *a, const void *b);
  - Sync syncData {}; // I declared the struct as a global struct so all threads have access

int main(int argc, char *argv[])
  - Sets the default dictionary, default port. Initialize struct flags flag and opt. Process all of the flags and determine the schedule type, dictionary, and port.
  - Initialize both log and connection buffer 
  - Initialize server and open dictionary file and extract to array.
  - Once binded, wait for incoming connections, and initialize all the threads.
  - While (1), accept all incoming connection and place into a struct with a priority and send to connectionQueue.

void *workerThread(void *arg)
  - While (1), call getConnection, once connection retrieved, that is the start time.
  - While client is still sending words, calculate elapsed time, and "clean up" the client message.
  - From what I observed, once data is retrieved by the server from the client, there is an additional 2 extra bytes at the end. I also "cleaned up" the client's message by detecting "\" for special characters and made the client's word lowercase.
  - Then, check if the clean word is in the dictionary and create a string of the results and send to the log queue.

int putConnection(struct connection connect)
  - If FIFO:
    - Lock, set next = head + 1 and if next is >= numCells then next = 0.
    - While next == getIndexConnection, wait. Once out, put the connect into the buffer and set the index = next and broadcast and unlock.
  - If PRIORITY:
    - Lock, if next = head + 1 and if next >= numCells then wait. 
    - Once out, add connect to connection buffer, run qsort on the priority of the connections and set head = next and broadcast and unlock.

int getConnection()
  - If FIFO:
    - Lock and if head == tail then wait. Once out, set next = tail + 1 and if next >= numCells then next = 0.
    - Save current index, set tail to next
    - signal, unlock, and return the connection.
  - If PRIORITY:
    - lock and while head == tail, wait. Once out, connect == the tail (for priority it will always be 0 (first index).). Deincrement head and shift all the numbers in the array back and sort.
    - Signal unlock, and return highest priority connection.

int compare(const void *a, const void *b)
  - This is the compare algorithm for qsort.
  - typecast a and b to const struct connection and compare the priority.
  - b - a because we want it in descending order.

void *logThread(void *arg)
  - While (1) open the log file, if it exists, then append, if it doesn't then create.
  - get the string from log queue and write to the log file and close.
  
int putLog(char *string)
  - lock and do the normal put for circular queues as previously in the code and in the past labs.

int getLog(char *string)
  - lock and do the normal get for circular queues as previously in the code in the past labs.

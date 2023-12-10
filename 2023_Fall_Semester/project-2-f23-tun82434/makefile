CFLAGS = -lpthread -Wall -Werror
TXTFILES = BLUE_delivery.txt RED_delivery.txt

all: fabManager assemManager

fabManager: fabricationManager.c
	gcc -o fabManager fabricationManager.c $(CFLAGS)

assemManager: assemblyManager.c
	gcc -o assemManager assemblyManager.c $(CFLAGS) 


clean:
	rm -rf .o fabManager assemManager $(TXTFILES)
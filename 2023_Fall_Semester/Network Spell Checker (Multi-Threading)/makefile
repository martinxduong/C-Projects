CFLAGS = -lpthread -Wall -Werror

all: spellChecker client

spellChecker: spellChecker.c
	gcc -o spellChecker spellChecker.c $(CFLAGS)

client: client.c
	gcc -o client client.c -lpthread $(CFLAGS)

clean:
	rm -rf .o spellChecker client log.txt

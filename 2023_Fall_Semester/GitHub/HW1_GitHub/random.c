#include "random.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


char randchar(){
    int charNum = rand() % 26; // charNum: a number between 0 - 25
    return ('a' + charNum); // returns a char a-z
}


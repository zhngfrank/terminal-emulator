
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
//so c evidently doesn't give me any basic data structures, 
//so instead of coding up a bunch of data structures 
//I've opted to just scanning each string twice, the first time to find
//the lengths of the arrays I'll need, and the second time to create the arrays

//this is highly inneficient but effective


int getlength(char* input) {
    int i = 0;
    while(input[i] != '\0') {
        i++;
    }
    return i;
}

//get positions of &, <, >, |
//pos[0] = number of following positions
//returned array should be freed
//scanning twice is inneficient but means I don't have to grow the array
int* getspecialpositions(char* input, int length) {
    int size = 0;

    //first scan: find size of positions
    for(int i = 0; i < length; i++) {
        if(input[i] == '&') {
            if (input[i+1] == '&') {
                i++;
            }
            size++;
        }
        else if (input[i] == '|' || input[i] == '<' 
            || input[i] == '>' || input[i] == ';') {
                size++;
        }
    }//end first scan

    //second scan: populate positions

    int* positions = (int*)malloc((size+1)*sizeof(int));
    positions[0] = size;
    int pos = 1;
    for(int i = 0; i < length; i++) {
        if(input[i] == '&') {
            positions[pos] = i;
            if (input[i+1] == '&') {
                i++;
            }
            pos++;
        }
        else if (input[i] == '|' || input[i] == '<' 
            || input[i] == '>' || input[i] == ';') {
                positions[pos] = i;
                pos++;
        }
    }//end second scan

    return positions;
}

char* stripspaces (char * input, int length) {
    //so this function bugs out when it doesn't start or end with spaces so here's my fix
    char* str = (char*)malloc((length+3)*sizeof(char));
    str[0] = ' ';
    int i = 0;
    for(i; i < length; i++) {
        str[i+1] = input[i];
    }
    str[i+1] = ' ';
    str[i+2] = '\0';

    length = length + 3;
    int spacestoremove = 0;
    int spaces[length]; //figure out indices where spaces exist
    spaces[0] = 0;
    for(int i = 0; i < length; i++) {
        if(str[i] == ' ') {
            int j = i + 1;
            while(str[j] == ' ') {
                j++;
                spacestoremove++;
            }
            if(j != i) {
                int offset = spaces[0]*2 + 1;
                spaces[offset] = i;
                spaces[offset + 1] = j-1; //i did something in my math wrong and this
                                        //fixes it
                spaces[0] = spaces[0] + 1;
                i = j;
            }
        }
    }

    int newstrlength = length - spacestoremove;
    char* stripped = (char*)malloc((newstrlength + 1)*sizeof(char));
    int start = 0;
    int offset = 0;
    for(int i = 1; i < spaces[0]*2 +1; i+=2) {
        for(int j = start; j < spaces[i]; j++) {
            stripped[offset] = str[j];
            offset++;
        }
        stripped[offset] = ' ';
        offset++;
        start = spaces[i+1] + 1;
    }
    stripped[newstrlength] = '\0';
    //now remove leading or trailing whitespace
    int front = 0, back = offset - 1;
    if(stripped[0] = ' ') { 
        front++;
    }
    if(stripped[back]){
        back--;
    }
    if(front != 0 || back != offset - 1) {
        char* actuallystripped = (char*)malloc((back + 2)*sizeof(char));
        for(int i = front; i < back + 1; i++) {
            actuallystripped[i-1] = stripped[i];
        }
        actuallystripped[back] = '\0';
        return actuallystripped;
    }
    else {
        return stripped;
    }
}

int numberofwords(char* str) {
    int i = 0, numwords = 1;
    while(str[i] != '\0') {
        if(str[i] == ' ') {
            numwords++;
        }
        i++;
    }
    return numwords;
}

//return argv formatted words
//return structure: 
//[argv][special char]
//...
//[argv][special char]
//NULL
//str to free
char*** getargs(char* input) {
    int origlength = getlength(input);
    char* str = stripspaces(input, origlength);
    int length = getlength(str);

    int numwords = numberofwords(str);
    char** tokens = (char**)malloc((numwords + 1)*sizeof(char*));
    
    char* token = strtok(str, " ");
    int i = 0;
    while(token != NULL){
        tokens[i] = token;
        i++;
        token = strtok(NULL, " ");
    }
    tokens[numwords] = NULL;
    int* pos = getspecialpositions(str, length); 
    int numargvs = 2*pos[0] + 3;
    char*** allargs = (char***)malloc(numargvs*sizeof(char**));
    allargs[numargvs-2] = NULL;
    allargs[numargvs - 1] = tokens;
    int start = 0, argoffset = 0;
    for(int j = 0; j < numwords; j++) {
        if(tokens[j][0] == '<' || tokens[j][0] == '>' || tokens[j][0] == '|' ||
         tokens[j][0] == ';' || tokens[j][0] == '&') {
             allargs[argoffset] = (char**)malloc((j-start + 1)*sizeof(char*));
             int h = 0;
             for(int k = start; k < j; k++) {
                 allargs[argoffset][h] = tokens[k];
                 h++;
             }
             allargs[argoffset][h] = NULL;
             argoffset++;
             allargs[argoffset] = (char**)malloc(sizeof(char*));
             allargs[argoffset][0] = tokens[j];
             argoffset++;
             start = j+1;
        }
        else if(j == numwords - 1) {
            allargs[argoffset] = (char**)malloc((j-start + 1)*sizeof(char*));
            int h = 0;
            for(int k = start; k < numwords; k++) {
                allargs[argoffset][h] = tokens[k];
                h++;
            }
             allargs[argoffset][h] = NULL;
        }
    }
    return allargs;
}

void printtriplestar(char*** args) {
    int i = 0, j =0;
    while(args[i] != NULL) {
        j = 0;
        while(args[i][j] != NULL) {
            printf("layer %d word %d: ", i, j);
            printf("<%s>\n", args[i][j]); 
            j++;
        }
        i++;
    }
}

int gettriplestarlength(char*** args) {
    int i = 0;
    while(args[i] != NULL) {
        i++;
    }
    return i;
}

void deleteargvs(char*** argvs) {
    int i = 0;
    while(argvs[i] != NULL) {
        free(argvs[i]);
        i++;
    }
    free(argvs);
}

int gettriplestardepth(char*** args, int i) {
    int length = 0;
    while(args[i][length] != '\0') {
        length++;
    }
    return length;
}

//if the input is valid, return -1
//else return the index of the first error
//input should be [argv][symbol][arg][symbol]
//input can end on an argv or on an ampersand
int verifyinput(char*** args) {
    int i = 0;
    const char* symbols[] = {">", "<", ";", "&", "&&", ";", "|"};
    const int num = 7;
    bool symflag = false;
    while(args[i] != NULL) {
        for(int j = 0; j < num; j++) {
            if(strcmp(args[i][0], symbols[j]) == 0) {
                symflag = true;

                continue;
            }
        }
        if(symflag) {
            symflag = false;
            if(i%2 == 0) {
                return i; //only args should be in even indices
            }
            else if(args[i+1] == NULL) {
                if(strcmp(args[i][0], "&") != 0) {
                    return i;
                }//if the last index is a symbol that isn't &, return an error
            }
        }
        else {
            if(i%2 == 1) {
                return i; //only symbols should be in odd indices
            }
        }
        i++;
    }

    return -1;
}



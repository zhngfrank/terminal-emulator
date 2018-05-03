#ifndef PARSER_H_
#define PARSER_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>



//get length of string 
int getlength(char* input);

int verifyinput(char*** args);

//get number of special characters (';', '<', '&', etc) and their positions in the array
//returns an array, return[0] is the number of statements
int* getspecialpositions(char* input, int length);

char* stripspaces (char * input, int length);

int numberofwords(char* str);

char*** getargs(char* input);

void printtriplestar(char*** args);

int gettriplestarlength(char*** args);

int gettriplestardepth(char*** args, int i);

void deleteargvs(char*** argvs);

#endif
#ifndef HISTORY_H_
#define HISTORY_H_

#include <stdio.h>
#include <stdlib.h>

//const unsigned int MAXSIZE = 100;

typedef struct node node;
struct node {
    node * next;
    char* str;
};

typedef struct linkedlist ll;
struct linkedlist {
    node* head; //oldest member of the list
    node* tail; //newest member of the list
    unsigned int count;
    int maxsize;
};

ll* createll(ll* list);

ll* pop_front(ll* list);

//debugging function
void printall(ll* list);

ll* push_back(ll* list, char* str);

node* accessLL(ll* list, int i);

void empty(ll* list);

char* copystr(char* input);

#endif
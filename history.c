#include <stdio.h>
#include <stdlib.h>
#include "history.h"


ll* createll(ll* list) {
    list->head = NULL;
    list->tail = NULL;
    list->count = 0;
    list->maxsize = 100;
    return list;
}

ll* pop_front(ll* list) {
    if(list->head != NULL) {
        node* todelete = list->head;
        list->head = todelete->next;
        free(todelete->str);
        free(todelete);
        list->count--;
    }
    else {
        printf("List is empty, nothing to pop\n");
    }
}

//print in history format
void printall(ll* list) {
    if(list->head == NULL) {
        printf("cannot print, list is empty\n");
        return;
    }
    node* iterator = list->head;
    int i = 0;
    while(iterator != NULL){
        printf("%5d  %s\n", i, iterator->str);
        iterator = iterator->next;
        i++;
    }
}

//deep copy a char array
char* copystr(char* input) {
    int i = 0; 
    while(input[i] != '\0') {
        i++;
    }
    
    char* toreturn = (char*)malloc((i+1)*sizeof(char));
    i = 0;
    while(input[i] != '\0') {
        toreturn[i] = input[i];
        i++;
    }
    toreturn[i] = '\0';

    return toreturn;
}

ll* push_back(ll* list, char* str) {
    if(list->head == NULL) {
        list->head = (node*)malloc(sizeof(node));
        list->head->str = str;
        list->head->next = NULL;
        list->tail = list->head; 
        list->count++;
    }
    else if(list->count >= list->maxsize) {
        pop_front(list);
        node* new = (node*)malloc(sizeof(node));
        new->str = str;
        new->next = NULL;
        list->tail->next = new;
        list->tail = new;
        list->count++; //count is decremented in pop_back for consistency
    }
    else {
        node* new = (node*)malloc(sizeof(node));
        new->str = str;
        new->next = NULL;
        list->tail->next = new;
        list->tail = new;
        list->count++;
    }
    return list;
}

node* accessLL(ll* list, int i) {
    if(i > list->count)
        return NULL; //return null as an outofbounds error

    int j = 0;
    node* iterator = list->head;
    while(iterator != NULL) {
        if( j ==  i)
            return iterator; //with any luck this is actually the right one
        else  {
            iterator = iterator->next;
            j++;
        }
    }
    return NULL;
}

void empty(ll* list) {
    while( list->count != 0) {
        pop_front(list);
    }
}

// }
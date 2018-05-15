#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/queue.h>
#include "./mt19938.h"

//shorthand for generating a random number with MT
#define random(n) ((randomMT() % n) + 1)

typedef struct Workers {
    int searchers;
    int inserters;
    int deleters;
    int total;
} Workers;

typedef struct Node {
    int value;
    STAILQ_ENTRY(Node) nodes;
} Node;

sem_t search, delete, insert;

//head pointer for list
struct HeadPtr *headptr;

//a tail queue
STAILQ_HEAD(HeadPtr, Node) head = STAILQ_HEAD_INITIALIZER(head);

//function for inserters
void* inserter(void* x){
    int a = 0, b = 0;
    Node* node;
    while(1){
        sem_wait(&insert);
        sleep(1);
        
        //make node and assign a random value
        node = malloc(sizeof(Node));
        a = random(1000);
        node->value = a;
        
        //put node in list
        printf("putting %d in list\n", a);
        STAILQ_INSERT_TAIL(&head, node, nodes);
        
        //let everyone know
        sem_post(&insert);
        
        //check on the others
        while(a != 0 && b != 0){ sem_getvalue(&deleters, &a); sem_getvalue(&inserters, &b); }
    }
}
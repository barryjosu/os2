//James Barry
//for OS2 @ OSU, S2018
//This program creates a group of inserting, searching, and deleting threads that all operate on one linked list. 
//For execution instructions, check the readme. Compile with make.

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
void* inserter(void *arg){
    int a = 1, b = 1, r;
    Node* node;
    while(1){
        sem_wait(&insert);
        sleep(random(3));
        
        //make node and assign a random value
        node = malloc(sizeof(Node));
        r = random(1000);
        node->value = r;
        
        //put node in list
        printf("putting %d in list\n", r);
        STAILQ_INSERT_TAIL(&head, node, nodes);
        
        //let everyone know
        sem_post(&insert);
        
        //check on the others
        while(a != 0 && b != 0){ sem_getvalue(&delete, &a); sem_getvalue(&insert, &b); }
    }
}

//function for deleters
void* deleter(void *arg){
    int a = 1, b = 1, c = 1, r;
    Node* node;
    
    while(1){
        sem_wait(&delete);
        sleep(random(3));
        
        r = random(1000);
        STAILQ_FOREACH(node, &head, nodes){
            //make sure its okay to delete
            if(node->value % 2 == ((r %2 == 0) ? 0 : 1)){
                printf("deleting %d from list\n", node->value);
                STAILQ_REMOVE(&head, node, Node, nodes);
                free(node);
                break;
            }
        }
        
        //let everyone know and check on the others
        sem_post(&delete);
        while(a != 0 && b != 0 && c != 0){ sem_getvalue(&search, &a); sem_getvalue(&insert, &b); sem_getvalue(&delete, &c); }
        
    }
}

void* searcher(void* arg){
    int a = 1, b = 1, r, f;
    Node* node;
    
    while(1){
        sem_wait(&search);
        sleep(random(3));
        
        r = random(1000);
        f = 0;
        STAILQ_FOREACH(node, &head, nodes){
            if(node->value == r){ printf("found %d\n", r); f = 1; break;}
        }
        
        if(!f) printf("could not find %d\n", r);
        
        sem_post(&search);
        while(a != 0 && b != 0){ sem_getvalue(&search, &a); sem_getvalue(&delete, &b); }
    }
}

int main(int argc, char** argv){
    pthread_t* threads;
    int s, d, i, t, a;
    
    if(argc == 4){
        STAILQ_INIT(&head);
        s = atoi(argv[1]);
        d = atoi(argv[2]);
        i = atoi(argv[3]);
        t = s + d + i;
    
        //initialize our semaphores
        sem_init(&search, 0, s);
        sem_init(&delete, 0, 1);
        sem_init(&insert, 0, 1);
        
        threads = malloc(t*sizeof(pthread_t));
        
        //initialize our threads
        for(a = 0; a < s; a++) pthread_create(&threads[a], NULL, searcher, (void*) &t);
        for(a = a; a < s+d; a++) pthread_create(&threads[a], NULL, deleter, (void*) &t);
        for(a = a; a < t; a++) pthread_create(&threads[a], NULL, inserter, (void*) &t);
        
        for(a = 0; a < t; a++) pthread_join(threads[a], NULL);
    }
    
    else printf ("usage: ./problem2 <numsearchers> <numdeleters> <numinserters>\n");
    return 0;
}
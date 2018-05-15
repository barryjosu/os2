#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <unistd.h>
#include "./mt19938.h"

//shorthand for generating a random number with MT
#define random(n) ((randomMT() % n) + 1)

//for keeping track of busy and locked states
typedef struct Resource {
    sem_t busy;
    sem_t lock;
} Resource;

void* useResource(void* x){
    //some temp holders
    int a, b;
    //a resource
    Resource *resource = (Resource*) x;
    while(1){
        //use resource
        sem_wait(&resource->busy);
        //arbitrary 10 limited time to use resource
        a = random(10);
        printf("Working for %d seconds\n", a);
        sleep(a);
        
        //all done
        sem_post(&resource->busy)
        //get the sem value so we can see if resource is full yet
        sem_getvalue(&resource->busy, &b)
        while(b!=3){
            sem_getvalue(&resource->busy, &b)
        }
    }
}

int main(){
    int a = 0;
    //intialize our three workers
    pthread_t threads[3];
    //malloc a resource
    Resource *resource = (Resource*) malloc(sizeof(Resource*));
    seedMT(time(NULL));
    
    //make our busy semaphore and check for success
    if(sem_init(&resource->busy, 0, 3) < 0) perror("busy semaphore failed\n");
    //make our lock semaphore
    if(sem_init(&resource->lock, 0, 0) < 0) perror("lock semaphore failed\n");
    //initialize threads
    for(a = 0; a < 3; a++) pthread_create(&threads[a], NULL, useResource, resource);
    //join threads
    for(a = 0; a < 3; a++) pthread_join(threads[i], NULL);
    
    return 0;
}
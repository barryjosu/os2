//James Barry
//for OS2 @ OSU, S2018

//To run, make and input "./concurrency1 <numproducers> <numconsumers>"

#include <stdio.h>
#include <stdlib.h> 
#include <pthread.h>
#include <errno.h>
#include <fcntl.h>
#include <semaphore.h>
#include <unistd.h>
#include <assert.h>
#include "./mt19938.h"
#include "./sem.c"

#define LEN 32

//shorthand
typedef unsigned long long ulint;

//for determining what random gen to use
typedef enum RandType {
    MT,
    RDRD
} RandType; 

//for storing produced info
typedef struct Consumable {
    ulint time, value;
} Consumable;

struct sem_t sem_producer; //slots available
struct sem_t sem_consumer; //slots filled

//for locking buffer
pthread_mutex_t mut_buf = PTHREAD_MUTEX_INITIALIZER;

Consumable buf[LEN]; //buffer
int focc = 0, femp = 0; //first occupied, first empty

//given function for rdrand
ulint asmRdRand(void){
    register ulint rand_num;
    __asm__ volatile ( "rdrand %0;" : "=r" ( rand_num ) );
    return rand_num;
}

//given function for determing what random gen to use
RandType getRandType(){
	unsigned int eax;
	unsigned int ebx;
	unsigned int ecx;
	unsigned int edx;
	eax = 0x01;
	__asm__ __volatile__("cpuid;": "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx): "a"(eax));
	if(ecx & 0x40000000){
		return RDRD;
	}
	else{
        return MT;
	}
}

//add to buffer
void push_buffer(Consumable x){
    buf[femp] = x;
    femp++;
    if (femp >= LEN){ femp = 0; }
}

//take from buffer
Consumable take_buffer(){
    Consumable x = buf[focc];
    focc++;
    if (focc >= LEN){ focc = 0; };
    return x;
}

//create producer
void *producer(void *arg){
    Consumable item;
    ulint wait;
    
    while(1){
        if(getRandType() == MT){
            item.time = (randomMT() % 8) + 2;
            item.value = randomMT();
            wait = (randomMT() % 5) + 3;
        }
        else{
            item.time = (asmRdRand() % 8) + 2;
            item.value = asmRdRand();
            wait = (asmRdRand() % 5) + 3;
        }
        
        my_sem_wait(&sem_producer);
        pthread_mutex_lock(&mut_buf);
            push_buffer(item);
        pthread_mutex_unlock(&mut_buf);
        my_sem_post(&sem_consumer);
        
        printf("<<%u put on buffer, sleeping %u seconds>>\n", item.value, wait);
        
        sleep(wait);
        
    }
}

//create consumer
void *consumer(void *arg){
    while (1){
        Consumable item;
        
        my_sem_wait(&sem_consumer); //wait for consume
        
        pthread_mutex_lock(&mut_buf); //lock once available
            item = take_buffer(); //take from buffer
        pthread_mutex_unlock(&mut_buf); //unlock
        
        my_sem_post(&sem_producer); //tell producer item was consumed
        
        sleep(item.time); //take time to consume
        
        printf("%u consumed, took %u seconds\n", item.value, item.time); //print item
        fflush(stdout);
    }
}

//create all threads
int run(int p, int c){
    //create threads
    my_sem_init(&sem_producer, 0, LEN);
    my_sem_init(&sem_consumer, 0, 0);
    
    int check;
    int i;
    
    //create producers p times
    for (i = 0; i < p; i++){
        pthread_t pro;
        check = pthread_create(&pro, NULL, &producer, NULL);
        assert(check == 0);
    }
    
    //create consumers c times
    for (i = 0; i < c; i++){
        pthread_t con;
        check = pthread_create(&con, NULL, &consumer, NULL);
        assert(check == 0);
    }
    
    while(1){
        sleep(1); //don't exit until process is killed externally
    }
    
}

int main(int argc, char** argv){
    
    if(argc == 3){
    
        time_t t;
        int p = atoi(argv[1]);
        int c = atoi(argv[2]);
        if(getRandType() == MT) seedMT((uint32)&t);
        
        run(p, c);
        
        return 1;
    
    }
    
    else{
        printf("usage: ./concurrency1 numproducers numconsumers\n");
        return 1;
    }
}
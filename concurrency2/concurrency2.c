//James Barry
//for OS2 @ OSU, S2018
//This program creates five threads representing philosophers, who must share five forks to balance thinking and eating.
//Keep in mind: if philosopher 1 is eating, then 2 and 5 should not be.

//To run, make and input "./concurrency2"

#include <stdio.h>
#include <stdlib.h> 
#include <pthread.h>
#include <errno.h>
#include <fcntl.h>
#include <semaphore.h>
#include <unistd.h>
#include "./mt19938.h"

//number of philosophers (changing this can make the code a bit wacky)
#define PHILOSOPHERS 5

//state handlers
#define THINKING 0
#define HUNGRY 1
#define EATING 2

//some globals
sem_t sem[PHILOSOPHERS];
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int state[PHILOSOPHERS];
const char* names[] = {"Khalil Gibran", "Alan Watts", "Albert Einstein", "Plato", "Lao Tzu"};
const char* hungryquotes[] = {"You eat when you cease to be at peace with your thoughts.", "Saints need sinners. And the sated need the hungry...", "Two things are infinite: the universe and my stomach. And I'm not sure about the universe...", "Human behavior flows from three main sources: emotion, knowledge, and hunger.", "Simplicity, patience, food. These three are your greatest treasures."};
const char* donequotes[] = {"In the sweetness of dinner, let there be laughter and sharing of pleasures.", "I owe my solitude to this delicious spaghetti.", "Once we accept our limits, we go beyond them.", "Let parents bequeath to their children not spaghetti, but the spirit of reverence.", "Spaghetti is a source of great strength."};

//prototypes
void philosopher(int n);
void getForks(int n);
void putForks(int n);
void test(int n);

typedef enum RandType {
    MT,
    RDRD
} RandType; 

//given function for rdrand
unsigned long long asmRdRand(void){
    register unsigned long long rand_num;
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

//global for random type, so we only have to determine it once
//an upgrade over my concurrency1 work, which calculated type every time a random gen was done
//RandType randomtype = getRandType();

//loop for the philosophers
void philosopher(int n){
    int sleept, eatt;
    
    while(1){
        
        //determine random algorithm
        if(getRandType() == MT){
            sleept = (randomMT() % 8) + 2;
            eatt = (randomMT() % 20) + 1;
        }
        else{
            sleept = (asmRdRand() % 8) + 2;
            eatt = (asmRdRand() % 20) + 1;
        }
        
        sleep(sleept);
        getForks(n);
        sleep(eatt);
        putForks(n);
        
        
    }
}

void getForks(int n){
    pthread_mutex_lock(&mutex);
    state[n] = HUNGRY;
    pthread_mutex_unlock(&mutex);
    while(1){
        test(n);
        if(state[n] == EATING) break;
    }
    //sem_wait(&sem[n]);
}

void putForks(int n){
    pthread_mutex_lock(&mutex);
    printf("Philosopher %d, %s: %s\n", n+1, names[n], donequotes[n]);
    state[n] = THINKING;
    pthread_mutex_unlock(&mutex);
    //sem_post(&sem[n]);
}

//calculation for determing if forks are available for a philosopher to eat
void test(int n){
    if(state[n] == HUNGRY && state[(n + PHILOSOPHERS - 1) % PHILOSOPHERS] != EATING && state[(n + 1) % PHILOSOPHERS] != EATING){
        pthread_mutex_lock(&mutex);
        printf("Philosopher %d, %s: %s\n", n+1, names[n], hungryquotes[n]);
        state[n] = EATING;
        pthread_mutex_unlock(&mutex);
        return 1;
    }
}

int main(){
    
    printf("\nFor reference, the status of the thinkers is printed every two seconds.\n0 means thinking, 1 means hungry, 2 means eating.\nThe philosophers also mention when they start and stop eating.\nToday, we shall join %s, %s, %s, %s, and %s for dinner.\n\n", names[0], names[1], names[2], names[3], names[4]);
    sleep(5);
    
    int i;
    
    pthread_t tid[PHILOSOPHERS];
    
    for(i = 0;i < PHILOSOPHERS; i++) sem_init(&sem[i], 0, 1);
    
    printf("Starting threads.\n");
    
    for(i = 0; i < PHILOSOPHERS; i++) pthread_create(&tid[i], NULL, philosopher, i);
    
    while(1){ 
        printf("%d, %d, %d, %d, %d\n", state[0], state[1], state[2], state[3], state[4]);
        sleep(2);
    }
    
    return 0;
    
}


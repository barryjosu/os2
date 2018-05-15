CC = gcc
CFLAGS  = -g -Wall

all: problem1 problem2 mt19938
	$(CC) $(CFLAGS) -o problem1 problem1.o mt19938.o -lpthread	
	$(CC) $(CFLAGS) -o problem2 problem2.o mt19938.o -lpthread

problem1:  problem1.c  
	$(CC) $(CFLAGS) -c problem1.c

problem2:  problem2.c  
	$(CC) $(CFLAGS) -c problem2.c

mt19938:  mt19938.c mt19938.h
	$(CC) $(CFLAGS) -c mt19938.c

clean:
	rm -rf *.out *.o problem1 problem2
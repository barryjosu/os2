CC = gcc
CFLAGS  = -g -Wall

all: concurrency1 mt19938 sem.c
	$(CC) $(CFLAGS) -o concurrency1 concurrency1.o mt19938.o -lpthread

concurrency1:  concurrency1.c
	$(CC) $(CFLAGS) -c concurrency1.c

mt19938:  mt19938.c mt19938.h
	$(CC) $(CFLAGS) -c mt19938.c

clean:
	rm -rf *.out *.o concurrency1 
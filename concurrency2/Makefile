CC = gcc
CFLAGS  = -g -Wall

all: concurrency2 mt19938
	$(CC) $(CFLAGS) -o concurrency2 concurrency2.o mt19938.o -lpthread

concurrency2:  concurrency2.c
	$(CC) $(CFLAGS) -c concurrency2.c

mt19938:  mt19938.c mt19938.h
	$(CC) $(CFLAGS) -c mt19938.c

clean:
	rm -rf *.out *.o concurrency2 
CC=gcc
CFLAGS=-std=c99 -Wall -pedantic -g3

all: compositor

compositor: compositor.c
	$(CC) $(CFLAGS) -o $@ $^
	
test: compositor
	./compositor

	


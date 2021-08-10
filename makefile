
CC = gcc
CC_FLAGS = -Wall -pedantic -O2 -c

PROG_FILENAME = digest256

all: ./obj/main.o ./obj/sha256.o
	$(CC) -o $(PROG_FILENAME) $^

./obj/main.o: main.c
	$(CC) $(CC_FLAGS) -o $@ $^

./obj/sha256.o: ./lib/sha256.c
	$(CC) $(CC_FLAGS) -o $@ $^ 

clean:
	rm $(PROG_FILENAME) ./obj/*


CC = gcc
CC_FLAGS = -Wall -pedantic -O2 -c

.PHONY: clean all test

all:
	@echo "Command options:"
	@echo " make test     --> build and run test for library"
	@echo " make filehash --> build program to hash files"
	@echo " make clean    --> delete build files"

# Link and run test program
test: test.o sha256.o prog_bar.o
	$(CC) -o testprog $^
	@echo
	@echo "##################### RUNING TEST ##################################"
	@echo
	@./testprog
	@rm *.o testprog

# Link "filehash" program
filehash: filehash.o sha256.o prog_bar.o
	$(CC) -o $@ $^

# Compile objects
test.o: ./test/test.c
	$(CC) $(CC_FLAGS) -o $@ $^

filehash.o: ./test/filehash.c
	$(CC) $(CC_FLAGS) -o $@ $^
	
sha256.o: sha256.c
	$(CC) $(CC_FLAGS) -o $@ $^ 

prog_bar.o: prog_bar.c
	$(CC) $(CC_FLAGS) -o $@ $^


clean:
	@rm *.o filehash

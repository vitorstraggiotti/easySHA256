
CC = gcc
CC_FLAGS = -Wall -pedantic -O2 -c
CC_DFLAGS = -Wall -pedantic -g -c

.PHONY: clean all

all:
	@echo "Command options:"
	@echo " make test    --> build optimized version"
	@echo " make test_d  --> build debug version"
	@echo " make clean   --> erase build files and executable"

# Build optimized version
test: test.o sha256.o prog_bar.o
	$(CC) -o $@ $^

test.o: test.c
	$(CC) $(CC_FLAGS) -o $@ $^

sha256.o: sha256.c
	$(CC) $(CC_FLAGS) -o $@ $^ 

prog_bar.o: prog_bar.c
	$(CC) $(CC_FLAGS) -o $@ $^

# Build debug version
test_d: test_d.o sha256_d.o prog_bar_d.o
	$(CC) -o $@ $^

test_d.o: test.c
	$(CC) $(CC_DFLAGS) -o $@ $^

sha256_d.o: sha256.c
	$(CC) $(CC_DFLAGS) -o $@ $^

prog_bar_d.o: prog_bar.c
	$(CC) $(CC_DFLAGS) -o $@ $^

clean:
	rm *.o test test_d

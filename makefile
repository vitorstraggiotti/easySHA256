
CC = gcc
CC_FLAGS = -Wall -pedantic -O2 -c
CC_DFLAGS = -Wall -pedantic -g -c

.PHONY: clean all

all:
	@echo "Command options:"
	@echo " make test    --> build optimized version"
	@echo " make test2   --> build optimized version"
	@echo " make test_d  --> build debug version"
	@echo " make test2_d --> build debug version"
	@echo " make clean   --> erase build files and executable"

# Build optimized version for "test"
test: test.o sha256.o prog_bar.o
	$(CC) -o $@ $^

test.o: test.c
	$(CC) $(CC_FLAGS) -o $@ $^

sha256.o: sha256.c
	$(CC) $(CC_FLAGS) -o $@ $^ 

prog_bar.o: prog_bar.c
	$(CC) $(CC_FLAGS) -o $@ $^

# Build optimized version for "test2"
test2: test2.o sha256.o prog_bar.o
	$(CC) -o $@ $^

test2.o: test2.c
	$(CC) $(CC_FLAGS) -o $@ $^

# Build debug version for "test"
test_d: test_d.o sha256_d.o prog_bar_d.o
	$(CC) -o $@ $^

test_d.o: test.c
	$(CC) $(CC_DFLAGS) -o $@ $^

sha256_d.o: sha256.c
	$(CC) $(CC_DFLAGS) -o $@ $^

prog_bar_d.o: prog_bar.c
	$(CC) $(CC_DFLAGS) -o $@ $^

# Build debug version for "test2"
test2_d: test2_d.o sha256_d.o prog_bar_d.o
	$(CC) -o $@ $^

test2_d.o: test2.c
	$(CC) $(CC_DFLAGS) -o $@ $^

########################################################
# Clean routine

clean:
	rm *.o test test_d test2 test2_d




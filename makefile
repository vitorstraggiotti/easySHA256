
all:
	gcc -g sha256.c test.c -lm -o test

clean:
	rm test

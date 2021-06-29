
all:
	gcc -g sha256.c test.c -lm -o digest256

clean:
	rm digest256

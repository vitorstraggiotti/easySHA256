
CC = gcc

SOURCE_FILES = sha256.c file_digest256.c
PROG_FILENAME = digest256

all:
	$(CC) -lm -o $(PROG_FILENAME) $(SOURCE_FILES)

release:
	$(CC) -O2 -lm -o $(PROG_FILENAME) $(SOURCE_FILES)

debug:
	$(CC) -g -lm -o $(PROG_FILENAME) $(SOURCE_FILES)

clean:
	rm $(PROG_FILENAME)

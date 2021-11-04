# easySHA256
A tiny and simple C library without dependencies that compute SHA256 digest from a file.
Comes with a test program that compute the SHA256 disgest of an input file.

To use this library you need to add these files to your project: `prog_bar.c`,`prog_bar.h`,
`sha256.c` and `sha256.h`

These are the functions available:
```c
//Return 32 bytes digest of Data array on success. Return NULL if fail.
//VerboseStatus = SHA256_VERBOSE --> Will print progress
//VerboseStatus = SHA256_NOT_VERBOSE --> Will not print progress
uint8_t *sha256_data(uint8_t *Data, uint64_t DataSizeByte, uint8_t VerboseStatus);

//Return 32 bytes digest of file on success. Return NULL if fail.
//VerboseStatus = SHA256_VERBOSE --> Will print progress
//VerboseStatus = SHA256_NOT_VERBOSE --> Will not print progress
uint8_t *sha256_file(const char *Filename, uint8_t VerboseStatus);
```

## Example programs

The function `sha256_data()` are implemented on example program `test.c`

The function `sha256_file()` are implemented on example program `test2.c`

The example programs can be compiled by runing `$ make` on terminal. For compilation
you need to have `gcc` and `make` instralled on your machine. If you do not have it, 
you can install with the command `$ sudo apt install build-essential` on Ubuntu.

To build you need make and gcc copiler. After just run: `$ make`

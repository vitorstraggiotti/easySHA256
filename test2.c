/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	Simple program to compute the SHA256 digest of given file

	Author: Vitor Henrique Andrade Helfensteller Straggiotti
	Date: 04/11/2021 (DD/MM/YYYY)
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/stat.h>

#include "sha256.h"

#define DEBUG_FLAG		0

int main(int argc, char *argv[])
{
	uint8_t *Digest;

	//input arguments validation
	if(argc != 2)
	{
		printf("Error: invalid number of arguments.\n");
		printf("Use: %s <path_to_file>\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	
	//Computing digest
	Digest = sha256_file(argv[1], SHA256_VERBOSE);
	
	//Printing digest
	printf("\nDigest in hex:\n");
	for(uint32_t i = 0; i < 32; i++)
	{
		printf("%02x ", Digest[i]);
	}
	printf("\n\n");
	
	return 0;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	Simple program to compute the SHA256 digest of given file
	
	Author: Vitor Henrique Andrade Helfensteller Straggiotti
	Date: 29/06/2021 (DD/MM/YYYY)
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/stat.h>
#include "sha256.h"

#define DEBUG_FLAG		0

int main(int argc, char *argv[])
{
	FILE *File;
	uint8_t Tmp, *Digest, *Data;
	uint64_t FileSizeByte = 0;
	
	//Hold file informations
	struct stat FileStatus;
	
	//input arguments validation
	if(argc != 2)
	{
		printf("Error: invalid number of arguments.\n");
		printf("Use: %s <path_to_file>\n", argv[0]);
		exit(EXIT_FAILURE);
	}
		
	//Open file on read only mode and find size in bytes
	if((File = fopen(argv[1], "rb")) == NULL)
	{
		printf("Error: Could not open file or it does not exist.\n");
		exit(EXIT_FAILURE);
	}
	stat(argv[1], &FileStatus);
	FileSizeByte = FileStatus.st_size;

	/* Need Rewrite to compute digest without geting the entire file into RAM */
	
	//Copying file into memory
	Data = (uint8_t *)malloc(FileSizeByte * sizeof(uint8_t));
	fread(Data, sizeof(uint8_t), FileSizeByte, File);
	fclose(File);
	
	//Computing digest
	Digest = sha256(Data, FileSizeByte);
	
	//Printing digest
	printf("\nDigest in hex: ");
	for(uint32_t i = 0; i < 32; i++)
	{
		printf("%02x ", Digest[i]);
	}
	printf("\n\n");
	
	return 0;
}


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>
#include "sha256.h"


int main(int argc, char *argv[])
{
	FILE *File;
	uint8_t Tmp, *Digest, *Data;
	uint64_t FileSizeByte = 0;
	

	if(argc != 2)
	{
		printf("Error: invalid number of arguments.\n");
		printf("Use: %s <path_to_file>\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	
	File = fopen(argv[1], "rb");
	
	while(1)
	{
		fread(&Tmp, sizeof(uint8_t), 1, File);
		if(ferror(File)) break;
		if(feof(File)) break;
		FileSizeByte++;
	}
	fclose(File);
	
	File = fopen(argv[1], "r");
	
	Data = (uint8_t *)malloc(FileSizeByte * sizeof(uint8_t));
	
	fread(Data, sizeof(uint8_t), FileSizeByte, File);
	
	Digest = sha256(Data, FileSizeByte);
	
	printf("\nDigest: ");
	for(uint32_t i = 0; i < 32; i++)
	{
		printf("%02x ", Digest[i]);
	}
	printf("\n\n");
	
//print constants
#if 0	
	printf("\n\n");
	for(int i = 0; i < 8; i++)
	{
		printf("%d: %x\n", i, Hash[i]);
	}
	printf("\n\n");
	for(int i = 0; i < 64; i++)
	{
		printf("%d: %x\n", i, K_const[i]);
	}
#endif
	return 0;
}

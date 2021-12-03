/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	Simple program to compute the SHA256 digest of given file

	Author: Vitor Henrique Andrade Helfensteller Straggiotti
	Date: 29/06/2021 (DD/MM/YYYY)
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/stat.h>

#include "../sha256.h"

#define DEBUG_FLAG		0

int main(int argc, char *argv[])
{
	//sha256 hash values of files
	char *FileHash[] = {
							"fd 95 28 b9 20 d6 d3 95 6e 9e 16 11 45 23 e1 88 9c 75 1e 8c 1e 04 01 82 11 6d 4c 90 6b 43 f5 58",
							"38 ef c8 0b bd 34 3e 87 38 08 9d e1 cc 0e 69 55 4b 1e 08 0e f1 cf f8 c0 f7 81 02 4a 36 e4 f5 a0",
							"d3 4c d9 cb ac 7f 66 ab 30 1a 23 92 6c 0e bb 1c d3 08 fe b4 c9 a6 6d 96 97 4f 89 42 a5 d5 9a a7",
							"1a 25 e1 71 9e 99 ca 3c 54 68 32 3d 17 da a1 8a 03 24 c3 f5 ca ec 1a 82 e2 cb 37 ee 79 6f 7b f8",
							"82 a5 56 04 88 30 03 dc 4f 59 9a 4d f0 9a 7f 79 49 b2 7a 6d ac 71 b3 9c f0 31 41 c0 10 a0 c4 0d",
							"e7 9b 53 c6 c7 54 e8 f4 ad 6c d8 5a 4c 90 7b 77 90 f3 78 66 43 fd 72 af c9 06 87 de c3 3f e1 d3",
							"86 57 0a 4c 83 80 8b 1f 01 65 8c 7f e7 56 f1 0f 7c c0 6a b0 52 ea bb 15 fa f4 01 84 ed 2a f9 9c",
							"f3 1f 96 ba 76 55 42 4b 6a 6b 7f 5a 11 3e 1e 4e 7d 35 33 55 3c 17 43 fc f2 26 b6 eb b7 ca 7a a6",
							"94 8a c7 42 b7 50 72 ad ae 07 70 18 aa 63 b5 fa d8 c3 63 92 26 ee c5 72 40 a9 28 03 40 e7 f8 17",
							"91 62 cd f8 cb 9a 4f 65 26 ea 8a f2 2e b8 73 e4 61 bb ba a7 9b da 7b 97 1d 32 b7 4c a9 ef 38 08",
							"e9 12 2b 96 a5 92 af e2 2f a2 3d 5b f9 77 e2 41 85 e7 a8 8b 3e 15 a7 23 eb e4 67 79 dc bc a1 c2",
							"bc 86 f3 50 13 bc f4 67 9b e4 78 11 14 19 c7 7f 23 c0 07 75 77 2b 11 0c cc 3d c5 c9 13 dc 7b 83",
							"82 82 e5 71 ab cf 5e b5 47 45 65 87 06 05 9b 2c 20 72 14 d9 93 26 76 67 fb 72 a4 3d 90 73 ba ed",
							"c7 8e ac 5f 7b 20 75 47 13 65 fc 1b 0e 3a 7d 2b 8b 3f 55 6f 58 15 46 e0 e3 04 e9 ae 2a f7 cc eb",
							"08 19 34 81 ad 9a d5 29 5d 3b 2d 93 ce 96 6b 6f 9a 09 e4 77 f1 2d 53 2c a0 d7 02 fd 6f 3a 9b bc",
							"66 25 59 06 3c 1c 45 7d 24 2c 7a 3f c1 a7 d2 ba 44 27 fe 66 c0 9d 6b 9e c5 21 4d 92 2a e0 a9 00",
							"82 eb 04 f3 f2 f0 00 32 46 ee d6 9a 46 0f 82 dc 2a ec 56 52 eb a8 4d 2c af 11 39 87 06 5f f9 8c",
							"a2 8a a3 bf bf 3b 23 80 5b 96 1f da 18 d2 b7 4a 33 cb 0b 4f b6 23 4b 89 c5 38 a4 83 64 fb 53 02",
							"f0 9f 91 e4 81 c8 27 d6 b6 cb f6 6e b9 bf ef 6c c5 f5 d2 21 a4 8e 16 15 44 22 e8 36 19 7d db 5d"};
	
	char *Filename[] = {
							"./test/file01.data",
							"./test/file02.data",
							"./test/file03.data",
							"./test/file04.data",
							"./test/file05.data",
							"./test/file06.data",
							"./test/file07.data",
							"./test/file08.data",
							"./test/file09.data",
							"./test/file10.data",
							"./test/file11.data",
							"./test/file12.data",
							"./test/file13.data",
							"./test/file14.data",
							"./test/file15.data",
							"./test/file16.data",
							"./test/file17.data",
							"./test/file18.data",
							"./test/file19.data"};
	
	char *Data[] = {
					"Vitor Henrique",
					"Este e um teste para validar o algoritmo de hash criptografico ",
					"Este e um teste para validar o algoritmo de hash criptografico. ",
					"Este e um teste para validar o algoritmo de hash criptografico.. "};
						
	uint8_t DataSize[] = {14, 63, 64, 65};
	
	char *DataHash[] = {
						"75 94 8c 39 d9 f5 26 f6 74 84 bb 20 36 da 19 dc ef 93 1e 11 c1 f7 da f3 8c 40 0b f0 12 50 91 ef",
						"40 a6 41 7d f9 f1 9b 02 c9 c6 d1 f2 3b 91 53 e7 15 99 d8 25 5c eb d1 37 98 ac a3 f5 b7 68 56 56",
						"37 70 18 63 0a 51 e2 6c f0 ba 07 3f 40 f6 3b 3e f0 83 42 14 d0 a8 d4 a6 de 56 76 6c 93 b1 73 00",
						"19 df 46 1e 75 9d 58 0f 41 21 8e 87 6b bb 99 ac 77 18 04 dd 4c 9b 0c 87 1a a7 a0 4e 2d 95 c3 e5"};
						
	uint8_t			*Digest;
	unsigned int	ConvertedHash[32];

	/*==================== TESTING FUNCTION: sha256_file() ===================*/
	printf("Testing function: sha256_file()\n");
	printf("Number of tests: 19\n");
	for(int i = 0; i < 19; i++)
	{	
		//Computing digest
		Digest = sha256_file(Filename[i], NOT_VERBOSE);
		if(Digest == NULL)
		{
			printf("Test %d : FAILURE (bad return from function)\n", i+1);
			continue;
		}
		
		//Verify digest and print result
		sscanf(FileHash[i], "%x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x",
								&ConvertedHash[0],&ConvertedHash[1],&ConvertedHash[2],&ConvertedHash[3],
								&ConvertedHash[4],&ConvertedHash[5],&ConvertedHash[6],&ConvertedHash[7],
								&ConvertedHash[8],&ConvertedHash[9],&ConvertedHash[10],&ConvertedHash[11],
								&ConvertedHash[12],&ConvertedHash[13],&ConvertedHash[14],&ConvertedHash[15],
								&ConvertedHash[16],&ConvertedHash[17],&ConvertedHash[18],&ConvertedHash[19],
								&ConvertedHash[20],&ConvertedHash[21],&ConvertedHash[22],&ConvertedHash[23],
								&ConvertedHash[24],&ConvertedHash[25],&ConvertedHash[26],&ConvertedHash[27],
								&ConvertedHash[28],&ConvertedHash[29],&ConvertedHash[30],&ConvertedHash[31]);
		for(int j = 0; j < 32; j++)
		{
			if(Digest[j] != ConvertedHash[j])
			{
				printf("Test %d : FAILURE (hash do not match)\n", i+1);
				break;
			}
			else if(j == 31) //last element
			{
				printf("Test %d : SUCCESS\n", i+1);
			}
		}
		
		free(Digest);
	}
	
	/*=================== TESTING FUNCTION: sha256_data() ====================*/
	printf("\nTesting function: sha256_data()\n");
	printf("Number of tests: 4\n");
	for(int i = 0; i < 4; i++)
	{
		//Computing digest
		Digest = sha256_data(Data[i], DataSize[i], NOT_VERBOSE);
		if(Digest == NULL)
		{
			printf("Test %d : FAILURE (bad return from function)\n", i+1);
			continue;
		}
		
		//Verify digest and print result
		sscanf(DataHash[i], "%x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x",
								&ConvertedHash[0],&ConvertedHash[1],&ConvertedHash[2],&ConvertedHash[3],
								&ConvertedHash[4],&ConvertedHash[5],&ConvertedHash[6],&ConvertedHash[7],
								&ConvertedHash[8],&ConvertedHash[9],&ConvertedHash[10],&ConvertedHash[11],
								&ConvertedHash[12],&ConvertedHash[13],&ConvertedHash[14],&ConvertedHash[15],
								&ConvertedHash[16],&ConvertedHash[17],&ConvertedHash[18],&ConvertedHash[19],
								&ConvertedHash[20],&ConvertedHash[21],&ConvertedHash[22],&ConvertedHash[23],
								&ConvertedHash[24],&ConvertedHash[25],&ConvertedHash[26],&ConvertedHash[27],
								&ConvertedHash[28],&ConvertedHash[29],&ConvertedHash[30],&ConvertedHash[31]);
		for(int j = 0; j < 32; j++)
		{
			if(Digest[j] != ConvertedHash[j])
			{
				printf("Test %d : FAILURE (hash do not match)\n", i+1);
				break;
			}
			else if(j == 31) //last element
			{
				printf("Test %d : SUCCESS\n", i+1);
			}
		}
		
		free(Digest);
	}
	
	return 0;
}









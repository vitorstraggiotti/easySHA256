/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	Calculate the sha256 digest of some data
	Author: Vitor Henrique Andrade Helfensteller Straggiotti Silva
	Date: 26/06/2021 (DD/MM/YYYY)
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
 
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>

#include "sha256.h"
#include "prog_bar.h"

#define DEBUG_FLAG			0

#define BLOCK_SIZE_BYTE		64


//==============================================================================
//                 HELPER FUNCTIONS
//==============================================================================
//Calculate the rotations and shifts of sigma1 function
static uint32_t sigma1(uint32_t x)
{
	uint32_t RotateRight17, RotateRight19, ShiftRight10;
	
	RotateRight17 = (x >> 17) | (x << 15);
	RotateRight19 = (x >> 19) | (x << 13);
	ShiftRight10 = x >> 10;
	
	return RotateRight17 ^ RotateRight19 ^ ShiftRight10;
}
/******************************************************************************/
//Calculate the rotations and shifts of sigma0 function
static uint32_t sigma0(uint32_t x)
{
	uint32_t RotateRight7, RotateRight18, ShiftRight3;

	RotateRight7 = (x >> 7) | (x << 25);
	RotateRight18 = (x >> 18) | (x << 14);
	ShiftRight3 = x >> 3;
	
	return RotateRight7 ^ RotateRight18 ^ ShiftRight3;
}
/******************************************************************************/
//if bit of x is 1 return bit y else return z
static uint32_t choice(uint32_t x, uint32_t y, uint32_t z)
{
	return (x & y) ^ ((~x) & z);
}
/******************************************************************************/
//Calculate the rotations of BigSigma1 function
static uint32_t BigSigma1(uint32_t x)
{
	uint32_t RotateRight6, RotateRight11, RotateRight25;
	
	RotateRight6 = (x >> 6) | (x << 26);
	RotateRight11 = (x >> 11) | (x << 21);
	RotateRight25 = (x >> 25) | (x << 7);
	
	return RotateRight6 ^ RotateRight11 ^ RotateRight25;
}
/******************************************************************************/
//Calculate the rotations of BigSigma0 function
static uint32_t BigSigma0(uint32_t x)
{
	uint32_t RotateRight2, RotateRight13, RotateRight22;
	
	RotateRight2 = (x >> 2) | (x << 30);
	RotateRight13 = (x >> 13) | (x << 19);
	RotateRight22 = (x >> 22) | (x << 10);
	
	return RotateRight2 ^ RotateRight13 ^ RotateRight22;
}
/******************************************************************************/
//return the bit value that is majority
static uint32_t major(uint32_t x, uint32_t y, uint32_t z)
{
	return (x & y) ^ (x & z) ^ (y & z);
}
/******************************************************************************/
//Take data and partially fills the schedule array (w[0] to w[15]). Return 1 on
// success and 0 if it ends or fail.
static uint8_t create_schedule_array_data(uint8_t *Data, uint64_t DataSizeByte, uint64_t *RemainingDataSizeByte, uint32_t *W)
{	
	//Checking for file/data size limit
	if((0xFFFFFFFFFFFFFFFF / 8) < DataSizeByte)
	{
		printf("Error! File/Data exceeds size limit of 20097152 TiB");
		exit(EXIT_FAILURE);
	}

	//Starting with all data + 1 ending byte + 8 size byte
	uint8_t			TmpBlock[64];
	uint8_t			IsFinishedFlag = 0;
	static uint8_t	SetEndOnNextBlockFlag = 0;
	
	//Clear schedule array before use
	for(uint8_t i = 0; i < 64; i++)
	{
		W[i] = 0x0;
		TmpBlock[i] = 0x0; //Necessary for 0 padding on last block
	}
	
	//Creating 512 bits (64 bytes, 16 uint32_t) block with ending byte, padding
	// and data size
	for(uint8_t i = 0; i < 64; i++)
	{
		if(*RemainingDataSizeByte > 0)
		{
			TmpBlock[i] = Data[DataSizeByte - *RemainingDataSizeByte];
			*RemainingDataSizeByte = *RemainingDataSizeByte - 1;
			
			if(*RemainingDataSizeByte == 0) //Data ends before the end of the block
			{
				if(i < 63)
				{
					i++;
					TmpBlock[i] = 0x80;
					if(i < 56)
					{
						//64 bits data size in bits with big endian representation
						uint64_t DataSizeBits = DataSizeByte * 8;
						TmpBlock[56] = (DataSizeBits >> 56) & 0x00000000000000FF;
						TmpBlock[57] = (DataSizeBits >> 48) & 0x00000000000000FF;
						TmpBlock[58] = (DataSizeBits >> 40) & 0x00000000000000FF;
						TmpBlock[59] = (DataSizeBits >> 32) & 0x00000000000000FF;
						TmpBlock[60] = (DataSizeBits >> 24) & 0x00000000000000FF;
						TmpBlock[61] = (DataSizeBits >> 16) & 0x00000000000000FF;
						TmpBlock[62] = (DataSizeBits >> 8) & 0x00000000000000FF;
						TmpBlock[63] = DataSizeBits & 0x00000000000000FF;
						IsFinishedFlag = 1;
						goto outside1;
					}
					else //Block canot hold 64 bits data size value
						goto outside1;
				}
				else //Last element of data is the last element on block
				{
					SetEndOnNextBlockFlag = 1;
				}
			}
		}
		else
		{
			if((SetEndOnNextBlockFlag == 1) && (i == 0))
			{
				TmpBlock[i] = 0x80;
				SetEndOnNextBlockFlag = 0;
			}
			uint64_t DataSizeBits = DataSizeByte * 8;
			TmpBlock[56] = (DataSizeBits >> 56) & 0x00000000000000FF;
			TmpBlock[57] = (DataSizeBits >> 48) & 0x00000000000000FF;
			TmpBlock[58] = (DataSizeBits >> 40) & 0x00000000000000FF;
			TmpBlock[59] = (DataSizeBits >> 32) & 0x00000000000000FF;
			TmpBlock[60] = (DataSizeBits >> 24) & 0x00000000000000FF;
			TmpBlock[61] = (DataSizeBits >> 16) & 0x00000000000000FF;
			TmpBlock[62] = (DataSizeBits >> 8) & 0x00000000000000FF;
			TmpBlock[63] = DataSizeBits & 0x00000000000000FF;
			IsFinishedFlag = 1;
			goto outside1;
		}
	}
	outside1:

	//Filling the schedule array
	for(uint8_t i = 0; i < 64; i += 4)
	{
		W[i/4] = (((uint32_t)TmpBlock[i]) << 24) |
				 (((uint32_t)TmpBlock[i + 1]) << 16) |
				 (((uint32_t)TmpBlock[i + 2]) << 8) |
				 ((uint32_t)TmpBlock[i + 3]);
	}

	if(IsFinishedFlag == 1)
		return 0;
	else
		return 1;
}
/******************************************************************************/
//Take data and partially fills the schedule array (w[0] to w[15]). Return 1 on
// success and 0 if it ends or fail.
static uint8_t create_schedule_array_file(FILE *File_fp, uint64_t DataSizeByte, uint32_t *W)
{
	//Checking for file/data size limit
	if((0xFFFFFFFFFFFFFFFF / 8) < DataSizeByte)
	{
		printf("Error! File/Data exceeds size limit of 20097152 TiB");
		exit(EXIT_FAILURE);
	}

	//Starting with all data + 1 ending byte + 8 size byte
	uint8_t			TmpBlock[64];
	uint8_t			IsFinishedFlag = 0;
	static uint8_t	SetEndOnNextBlockFlag = 0;
	static uint8_t	RemainingDataFlag = 1;
	
	uint8_t	BytesRead;
	
	//Clear schedule array before use
	for(uint8_t i = 0; i < 64; i++)
	{
		W[i] = 0x0;
		TmpBlock[i] = 0x0; //Necessary for 0 padding on last block
	}	
	
	//Creating 512 bits (64 bytes, 16 uint32_t) block with ending byte, padding
	// and data size
	for(int8_t i = 0; i < 64; i++)
	{
		if(RemainingDataFlag == 1)
		{
			if((BytesRead = fread(TmpBlock, sizeof(uint8_t), BLOCK_SIZE_BYTE, File_fp)) != BLOCK_SIZE_BYTE)
			{
				RemainingDataFlag = 0;
				i = BytesRead - 1;
			}
			else
				goto outside1;
			
			if(RemainingDataFlag == 0) //Data ends before the end of the block
			{
				if(i < 63)
				{
					i++;
					TmpBlock[i] = 0x80;
					if(i < 56)
					{
						//64 bits data size in bits with big endian representation
						uint64_t DataSizeBits = DataSizeByte * 8;
						TmpBlock[56] = (DataSizeBits >> 56) & 0x00000000000000FF;
						TmpBlock[57] = (DataSizeBits >> 48) & 0x00000000000000FF;
						TmpBlock[58] = (DataSizeBits >> 40) & 0x00000000000000FF;
						TmpBlock[59] = (DataSizeBits >> 32) & 0x00000000000000FF;
						TmpBlock[60] = (DataSizeBits >> 24) & 0x00000000000000FF;
						TmpBlock[61] = (DataSizeBits >> 16) & 0x00000000000000FF;
						TmpBlock[62] = (DataSizeBits >> 8) & 0x00000000000000FF;
						TmpBlock[63] = DataSizeBits & 0x00000000000000FF;
						RemainingDataFlag = 1;
						IsFinishedFlag = 1;
						goto outside1;
					}
					else //Block canot hold 64 bits data size value
						goto outside1;
				}
				else //Last element of data is the last element on block
				{
					SetEndOnNextBlockFlag = 1;
				}
			}
		}
		else
		{
			if((SetEndOnNextBlockFlag == 1) && (i == 0))
			{
				TmpBlock[i] = 0x80;
				SetEndOnNextBlockFlag = 0;
			}
			uint64_t DataSizeBits = DataSizeByte * 8;
			TmpBlock[56] = (DataSizeBits >> 56) & 0x00000000000000FF;
			TmpBlock[57] = (DataSizeBits >> 48) & 0x00000000000000FF;
			TmpBlock[58] = (DataSizeBits >> 40) & 0x00000000000000FF;
			TmpBlock[59] = (DataSizeBits >> 32) & 0x00000000000000FF;
			TmpBlock[60] = (DataSizeBits >> 24) & 0x00000000000000FF;
			TmpBlock[61] = (DataSizeBits >> 16) & 0x00000000000000FF;
			TmpBlock[62] = (DataSizeBits >> 8) & 0x00000000000000FF;
			TmpBlock[63] = DataSizeBits & 0x00000000000000FF;
			RemainingDataFlag = 1;
			IsFinishedFlag = 1;
			goto outside1;
		}
	}
	outside1:

	//Filling the schedule array
	for(uint8_t i = 0; i < 64; i += 4)
	{
		W[i/4] = (((uint32_t)TmpBlock[i]) << 24) |
				 (((uint32_t)TmpBlock[i + 1]) << 16) |
				 (((uint32_t)TmpBlock[i + 2]) << 8) |
				 ((uint32_t)TmpBlock[i + 3]);
	}

	if(IsFinishedFlag == 1)
		return 0;
	else
		return 1;
}
/******************************************************************************/
static void complete_schedule_array(uint32_t *W)
{
	//add more 48 words of 32bit [w16 to w63]
	for(uint8_t i = 16; i < 64; i++)
	{
		W[i] = sigma1(W[i-2]) + W[i-7] + sigma0(W[i-15]) + W[i-16];
	}
}
/******************************************************************************/
//SHA256 compression process
static void compression(uint32_t *Hash, uint32_t *W)
{	
	enum TmpH {a, b, c, d, e, f, g, h};
	//create round constants (K)
	const uint32_t K_const[64] = {0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5,
								  0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
								  0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
								  0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
								  0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
								  0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
								  0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,
								  0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
								  0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
								  0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
								  0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3,
								  0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
								  0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5,
								  0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
								  0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
								  0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2};
	
	uint32_t TmpHash[8] = {0};
	uint32_t Temp1 = 0, Temp2 = 0;
	
	//inicialize variables a, b, c, d, e, f, g, h to h[0::7] respectively
	TmpHash[a] = Hash[0];
	TmpHash[b] = Hash[1];
	TmpHash[c] = Hash[2];
	TmpHash[d] = Hash[3];
	TmpHash[e] = Hash[4];
	TmpHash[f] = Hash[5];
	TmpHash[g] = Hash[6];
	TmpHash[h] = Hash[7];
	
	//Compression of the message schedule (W[0::63]) -----------------------
	for(uint32_t i = 0; i < 64; i++)
	{
		Temp1 = BigSigma1(TmpHash[e]) + choice(TmpHash[e], TmpHash[f], TmpHash[g]) + 
				K_const[i] + W[i] + TmpHash[h];
		Temp2 = BigSigma0(TmpHash[a]) + major(TmpHash[a], TmpHash[b], TmpHash[c]);

		TmpHash[h] = TmpHash[g];
		TmpHash[g] = TmpHash[f];
		TmpHash[f] = TmpHash[e];
		TmpHash[e] = TmpHash[d] + Temp1;
		TmpHash[d] = TmpHash[c];
		TmpHash[c] = TmpHash[b];
		TmpHash[b] = TmpHash[a];
		TmpHash[a] = Temp1 + Temp2;
	}
	//Update hash values for current block
	Hash[0] += TmpHash[a];
	Hash[1] += TmpHash[b];
	Hash[2] += TmpHash[c];
	Hash[3] += TmpHash[d];
	Hash[4] += TmpHash[e];
	Hash[5] += TmpHash[f];
	Hash[6] += TmpHash[g];
	Hash[7] += TmpHash[h];	
}
/******************************************************************************/
static uint8_t *extract_digest(uint32_t *Hash)
{
	uint8_t *Digest;
	
	//Allocate memory for digest pointer
	Digest = (uint8_t *)malloc(32 * sizeof(uint8_t));
	
	//Prepare digest for return
	for(uint32_t i = 0; i < 32; i += 4)
	{
		Digest[i]   = (uint8_t)((Hash[i/4] >> 24) & 0x000000FF);
		Digest[i+1] = (uint8_t)((Hash[i/4] >> 16) & 0x000000FF);
		Digest[i+2] = (uint8_t)((Hash[i/4] >> 8) & 0x000000FF);
		Digest[i+3] = (uint8_t)(Hash[i/4] & 0x000000FF);
	}
	
	return Digest;
}


								
//==============================================================================
//                 MAIN FUNCTIONS
//==============================================================================
//Return 32 bytes digest of Data on success. Return NULL if fail.
//VerboseStatus = SHA256_VERBOSE --> Will print progress
//VerboseStatus = SHA256_NOT_VERBOSE --> Will not print progress
uint8_t *sha256_data(uint8_t *Data, uint64_t DataSizeByte, uint8_t VerboseStatus)
{	
	bar_graph_t	*BarGraph;
	bar_t		*Bar;
	uint64_t	CurrProgressState = 0;
	
	//schedule array
	uint32_t	W[64];
	
	//H -> Block hash ; TmpH -> temporary hash in compression loop
	//Temp1 and Temp2 are auxiliar variable to calculate TmpH[]
	uint32_t	Hash[8] = {0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
						   0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19};
	
	//Hashed data
	uint8_t *Digest;
	
	uint64_t RemainingDataSizeByte = DataSizeByte;
	
	if(VerboseStatus == VERBOSE)
	{
		Bar = init_bar(0, DataSizeByte/BLOCK_SIZE_BYTE, 70, 1);
		BarGraph = init_bar_graph('|','#',' ','|');
		printf("\n");
	}
	
	while(create_schedule_array_data(Data, DataSizeByte, &RemainingDataSizeByte, W) == 1)
	{
		complete_schedule_array(W);
		compression(Hash, W);
		
		if(VerboseStatus == VERBOSE)
		{
			update_bar(Bar, BarGraph, CurrProgressState);
			CurrProgressState++;
		}
	}
	complete_schedule_array(W);
	compression(Hash, W);
	if(VerboseStatus == VERBOSE)
		update_bar(Bar, BarGraph, CurrProgressState);
	
	Digest = extract_digest(Hash);
	
	if(VerboseStatus == VERBOSE)
	{
		destroy_bar(Bar);
		destroy_graph(BarGraph);
	}
	
	return Digest;
}
/******************************************************************************/
//Return 32 bytes digest of file on success. Return NULL if fail.
//VerboseStatus = SHA256_VERBOSE --> Will print progress
//VerboseStatus = SHA256_NOT_VERBOSE --> Will not print progress
uint8_t *sha256_file(const char *Filename, uint8_t VerboseStatus)
{	
	bar_graph_t	*BarGraph;
	bar_t		*Bar;
	uint64_t	CurrProgressState = 0;
	
	//schedule array
	uint32_t	W[64];
	
	//H -> Block hash ; TmpH -> temporary hash in compression loop
	//Temp1 and Temp2 are auxiliar variable to calculate TmpH[]
	uint32_t	Hash[8] = {0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
						   0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19};
	
	//Hashed data
	uint8_t *Digest;
	
	uint64_t DataSizeByte;
	FILE *File_fp;
	
	
	if((File_fp = fopen(Filename, "rb")) == NULL)
	{
		printf("Could not read file!");
		exit(EXIT_FAILURE);
	}
	
	//Find input file size
	struct stat Status;
	stat(Filename, &Status);
	DataSizeByte = Status.st_size;
	
	if(VerboseStatus == VERBOSE)
	{
		Bar = init_bar(0, DataSizeByte/BLOCK_SIZE_BYTE, 70, 1);
		BarGraph = init_bar_graph('|','#',' ','|');
		printf("\n");
	}
	
	while(create_schedule_array_file(File_fp, DataSizeByte, W) == 1)
	{
		complete_schedule_array(W);
		compression(Hash, W);
		
		if(VerboseStatus == VERBOSE)
		{
			update_bar(Bar, BarGraph, CurrProgressState);
			CurrProgressState++;
		}
	}
	complete_schedule_array(W);
	compression(Hash, W);
	if(VerboseStatus == VERBOSE)
		update_bar(Bar, BarGraph, CurrProgressState);
	
	Digest = extract_digest(Hash);
	

	if(VerboseStatus == VERBOSE)
	{
		destroy_bar(Bar);
		destroy_graph(BarGraph);
	}
	
	fclose(File_fp);
	
	return Digest;
}













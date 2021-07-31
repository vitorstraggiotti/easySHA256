/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	Calculate the sha256 digest of some data
	Author: Vitor Henrique Andrade Helfensteller Straggiotti Silva
	Date: 26/06/2021 (DD/MM/YYYY)
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
 
#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "sha256.h"

#define DEBUG_FLAG		0

//inicialize hash values (H)
const uint32_t HashStart[8] =  {0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
								0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19};
		
//create round constants (K)
const uint32_t K_const[64] =   {0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5,
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

enum TmpH {a, b, c, d, e, f, g, h};

/******************************************************************************/
#if DEBUG_FLAG
//Print the pre-processed data in 512bits blocks (debug use)
static void print_block(uint8_t **DataBlock, uint32_t NumOfBlocks)
{
	for(uint32_t Block = 0; Block < NumOfBlocks; Block++)
	{
		printf("Block %u:\n", Block);
		for(uint32_t ByteOnBlock = 0; ByteOnBlock < 64; ByteOnBlock++)
		{
			if((ByteOnBlock % 8) == 0) printf("\n");
			printf("%02X\t", DataBlock[Block][ByteOnBlock]);
		}
		printf("\n\n");
	}
}
#endif
/******************************************************************************/
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
/*******************************************************************************/
//Ceil function of a division
static uint32_t int_ratio_ceil(uint64_t Numerator, uint64_t Denominator)
{
	if((Numerator % Denominator) != 0)
	{
		return (Numerator / Denominator) + 1;
	}
	else
	{
		return Numerator / Denominator;
	}
}
/******************************************************************************/
static void print_progress(uint64_t CurrState, uint64_t Min, uint64_t Max, uint32_t BarSize)
{
	uint32_t BarPosition;
	float Percentage;
	
	//Divide by zero error
	if(Min == Max)
		return;
	
	//Calculating progress
	BarPosition = (uint32_t)(((uint64_t)BarSize * (CurrState - Min)) / (Max - Min));
	Percentage = 100.0 * ((float)(CurrState - Min) / (float)(Max - Min));
	
	//Drawing progress bar
	fputs("\r[", stdout);
	for(unsigned int i = 0; i < BarSize; i++)
	{
		if(i < BarPosition)
		{
			fputc('=', stdout);
		}else
		{
			fputc('.', stdout);
		}
	}
	if(CurrState > (Max - 1500)) Percentage = 100.0;
	printf("] %.3f %%", Percentage);
	fflush(stdout);

}
/*******************************************************************************/
//Calculates sha256 of Data 
uint8_t *sha256(uint8_t *Data, uint64_t DataSizeByte)
{
	uint64_t DataSizeBits;
	uint32_t BytesForPadding, NumOfBlocks;
	uint8_t **DataBlock;
	
	//schedule array
	uint32_t W[64];
	
	//H -> Block hash ; TmpH -> temporary hash in compression loop
	//Temp1 and Temp2 are auxiliar variable to calculate TmpH[]
	uint32_t H[8], TmpH[8], Temp1, Temp2;
	
	//Hashed data
	uint8_t *Digest;
	
	//Pre-processing Data ------------------------------------------------------

	//Calculating the data size in bits
	DataSizeBits = DataSizeByte * 8;
	
	//Calculating the quantity of zero bytes to append to data
	BytesForPadding = 64 - ((DataSizeByte + 9)%64);
	
	//Calculating the quantity of 512bits data blocks
	NumOfBlocks = int_ratio_ceil(DataSizeByte, 64);
	
	//Allocating 512bits blocks
	printf("Allocating blocks in memory...\n");
	DataBlock = (uint8_t **)malloc(NumOfBlocks * sizeof(uint8_t *));
	for(uint32_t i = 0; i < NumOfBlocks; i++)
	{
		DataBlock[i] = (uint8_t *)malloc(64 * sizeof(uint8_t));
	}
	
	printf("Pre-processing data into blocks...\n");
	for(uint32_t Block = 0; Block < NumOfBlocks; Block++)
	{
		if(Block == NumOfBlocks - 1) //if it's in the last block
		{
			//copy the last bytes from Data
			for(uint32_t ByteOnBlock = 0; ByteOnBlock < (DataSizeByte % 64); ByteOnBlock++)
			{
				DataBlock[Block][ByteOnBlock] = Data[(Block * 64) + ByteOnBlock];
			}
			//Append bit 1 at the end of data
			DataBlock[Block][DataSizeByte % 64] = 0x80;
			
			//Pad with 0's until data is multiple of (512-64)=448
			for(uint32_t ByteOnBlock = (DataSizeByte % 64) + 1; ByteOnBlock < 56; ByteOnBlock++)
			{
				DataBlock[Block][ByteOnBlock] = 0x0;
			}
			//append 64bits of data size value in bits as big-endian
			DataBlock[Block][56] = (DataSizeBits >> 56) & 0x00000000000000FF;
			DataBlock[Block][57] = (DataSizeBits >> 48) & 0x00000000000000FF;
			DataBlock[Block][58] = (DataSizeBits >> 40) & 0x00000000000000FF;
			DataBlock[Block][59] = (DataSizeBits >> 32) & 0x00000000000000FF;
			DataBlock[Block][60] = (DataSizeBits >> 24) & 0x00000000000000FF;
			DataBlock[Block][61] = (DataSizeBits >> 16) & 0x00000000000000FF;
			DataBlock[Block][62] = (DataSizeBits >> 8) & 0x00000000000000FF;
			DataBlock[Block][63] = DataSizeBits & 0x00000000000000FF;
		}
		else
		{
			//copy bytes from Data to block
			for(uint32_t ByteOnBlock = 0; ByteOnBlock < 64; ByteOnBlock++)
			{
				DataBlock[Block][ByteOnBlock] = Data[(Block * 64) + ByteOnBlock];
			}
		}
		
		/* Need better implementation for progress bar */
		
		if(NumOfBlocks > 150000)
		{
			if(((Block % 1500) == 0) | (Block == NumOfBlocks - 1))
				print_progress((uint64_t)Block, 0, (uint64_t)(NumOfBlocks - 1), 50);
		}
		else
		{
			print_progress((uint64_t)Block, 0, (uint64_t)(NumOfBlocks - 1), 50);
		}
	}

#if DEBUG_FLAG
	print_block(DataBlock, NumOfBlocks);
#endif

	//Inicialize current hash table
	H[0] = HashStart[0];  
	H[1] = HashStart[1];  
	H[2] = HashStart[2];  
	H[3] = HashStart[3];  
	H[4] = HashStart[4];  
	H[5] = HashStart[5];  
	H[6] = HashStart[6];  
	H[7] = HashStart[7];  
	
	//Create message schedule loop 512bit block
	printf("\nData compression...\n");
	for(uint32_t Block = 0; Block < NumOfBlocks; Block++)
	{
		//(divide 512bits block into 16 32bit words [w0 t0 w15])
		for(uint32_t ByteOnBlock = 0; ByteOnBlock < 64; ByteOnBlock += 4)
		{
			W[ByteOnBlock / 4] = (((uint32_t)DataBlock[Block][ByteOnBlock]) << 24) |
								 (((uint32_t)DataBlock[Block][ByteOnBlock + 1]) << 16) |
								 (((uint32_t)DataBlock[Block][ByteOnBlock + 2]) << 8) |
								 ((uint32_t)DataBlock[Block][ByteOnBlock + 3]);
			
#if DEBUG_FLAG
			printf("w(%d): %08x\n", ByteOnBlock/4, W[ByteOnBlock/4]);
#endif
		}
		
		//add more 48 32bit words [w16 to w63]
		for(uint32_t i = 16; i < 64; i++)
		{
			W[i] = sigma1(W[i-2]) + W[i-7] + sigma0(W[i-15]) + W[i-16];
			
#if DEBUG_FLAG
			printf("w(%d): %08x\n", i, W[i]);
#endif
		}
		
		//inicialize variables a, b, c, d, e, f, g, h to h[0::7] respectively
		TmpH[a] = H[0];
		TmpH[b] = H[1];
		TmpH[c] = H[2];
		TmpH[d] = H[3];
		TmpH[e] = H[4];
		TmpH[f] = H[5];
		TmpH[g] = H[6];
		TmpH[h] = H[7];
		
		//Compression of the message schedule (W[0::63]) -----------------------
		for(uint32_t i = 0; i < 64; i++)
		{
			Temp1 = BigSigma1(TmpH[e]) + choice(TmpH[e], TmpH[f], TmpH[g]) + 
					K_const[i] + W[i] + TmpH[h];
			Temp2 = BigSigma0(TmpH[a]) + major(TmpH[a], TmpH[b], TmpH[c]);

			TmpH[h] = TmpH[g];
			TmpH[g] = TmpH[f];
			TmpH[f] = TmpH[e];
			TmpH[e] = TmpH[d] + Temp1;
			TmpH[d] = TmpH[c];
			TmpH[c] = TmpH[b];
			TmpH[b] = TmpH[a];
			TmpH[a] = Temp1 + Temp2;
		}
		//Update hash values for current block
		H[0] += TmpH[a];
		H[1] += TmpH[b];
		H[2] += TmpH[c];
		H[3] += TmpH[d];
		H[4] += TmpH[e];
		H[5] += TmpH[f];
		H[6] += TmpH[g];
		H[7] += TmpH[h];
		
		if(NumOfBlocks > 150000)
		{
			if((Block % 1500) == 0)
				print_progress((uint64_t)Block, 0, (uint64_t)(NumOfBlocks - 1), 50);
		}
		else
		{
			print_progress((uint64_t)Block, 0, (uint64_t)(NumOfBlocks - 1), 50);
		}
		
	}
	printf("\n");
	
	//Deallocate DataBlock
	for(uint32_t i = 0; i < NumOfBlocks; i++)
	{
		free(DataBlock[i]);
	}
	free(DataBlock);
	
	//Allocate memory for digest pointer
	Digest = (uint8_t *)malloc(32 * sizeof(uint8_t));
	
	//Prepare digest for return
	for(uint32_t i = 0; i < 32; i += 4)
	{
		Digest[i]   = (uint8_t)((H[i/4] >> 24) & 0x000000FF);
		Digest[i+1] = (uint8_t)((H[i/4] >> 16) & 0x000000FF);
		Digest[i+2] = (uint8_t)((H[i/4] >> 8) & 0x000000FF);
		Digest[i+3] = (uint8_t)(H[i/4] & 0x000000FF);
	}
	
	return Digest;
	
}














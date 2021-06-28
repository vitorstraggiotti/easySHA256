/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	Header file of the sha256.c source file
	Author: Vitor Henrique Andrade Helfensteller Straggiotti Silva
	Date: 26/06/2021 (DD/MM/YYYY)
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef SHA256_H
#define SHA256_H

extern const uint32_t HashStart[8];
extern const uint32_t K_const[64];
/******************************************************************************/
//Calculates sha256 of Data
uint8_t *sha256(uint8_t *Data, uint64_t DataSizeByte);

//Print the pre-processed data in 512bits blocks
void print_block(uint8_t **DataBlock, uint32_t NumOfBlocks);

//Calculate the rotations and shifts of sigma1 function
uint32_t sigma1(uint32_t x);

//Calculate the rotations and shifts of sigma0 function
uint32_t sigma0(uint32_t x);

//if bit of x is 1 return bit y else return z
uint32_t choice(uint32_t x, uint32_t y, uint32_t z);

//Calculate the rotations of BigSigma1 function
uint32_t BigSigma1(uint32_t x);

//Calculate the rotations of BigSigma0 function
uint32_t BigSigma0(uint32_t x);

//return the bit value thai is majority
uint32_t major(uint32_t x, uint32_t y, uint32_t z);

#endif

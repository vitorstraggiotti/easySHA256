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


#endif

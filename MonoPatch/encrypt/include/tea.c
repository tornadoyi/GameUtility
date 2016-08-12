#include "tea.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

const size_t Tea_Block_Size = 8; 

void _do_tea_encrypt(uint32_t* v, const uint32_t* k)
{
	uint32_t v0 = v[0], v1 = v[1], sum = 0, i;           /* set up */
	uint32_t delta = 0x9e3779b9;                     /* a key schedule constant */
	uint32_t k0 = k[0], k1 = k[1], k2 = k[2], k3 = k[3];   /* cache key */
	for (i = 0; i < 32; i++) {                       /* basic cycle start */
		sum += delta;
		v0 += ((v1 << 4) + k0) ^ (v1 + sum) ^ ((v1 >> 5) + k1);
		v1 += ((v0 << 4) + k2) ^ (v0 + sum) ^ ((v0 >> 5) + k3);
	}                                              /* end cycle */
	v[0] = v0; v[1] = v1;
}

void _do_tea_decrypt(uint32_t* v, const uint32_t* k)
{
	uint32_t v0 = v[0], v1 = v[1], sum = 0xC6EF3720, i;  /* set up */
	uint32_t delta = 0x9e3779b9;                     /* a key schedule constant */
	uint32_t k0 = k[0], k1 = k[1], k2 = k[2], k3 = k[3];   /* cache key */
	for (i = 0; i < 32; i++) {                         /* basic cycle start */
		v1 -= ((v0 << 4) + k2) ^ (v0 + sum) ^ ((v0 >> 5) + k3);
		v0 -= ((v1 << 4) + k0) ^ (v1 + sum) ^ ((v1 >> 5) + k1);
		sum -= delta;
	}                                              /* end cycle */
	v[0] = v0; v[1] = v1;
}

void tea_encrypt(const char* key, unsigned char* rawMsg, int encMsgLen)
{
	uint32_t i = Tea_Block_Size;
	
	for (; i <= encMsgLen; i = i + Tea_Block_Size)
	{
		_do_tea_encrypt((uint32_t *)&rawMsg[i - Tea_Block_Size], (const uint32_t *)key);
	}
}

void tea_decrypt(const char* key, unsigned char* encMsg, int encMsgLen)
{
	uint32_t i = Tea_Block_Size;

	for (; i <= encMsgLen; i = i + Tea_Block_Size)
	{
		_do_tea_decrypt((uint32_t*)&encMsg[i - Tea_Block_Size], (const uint32_t *)key);
	}
}
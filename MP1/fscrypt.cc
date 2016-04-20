#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "openssl/blowfish.h"

/* 
implement the encrypt / decrypt function using the openSSL library

1.	void BF_set_key(BF_KEY *key, int len, const unsigned char *data)
2.	void BF_cbc_encrypt(const unsigned char *in, unsigned char *out, long length, BF_KEY *schedule, unsigned char *ivec, int enc)
*/


const int BLOCKSIZE = 8;

void *fs_encrypt(void *plaintext, int bufsize, char *keystr, int *resultlen)
{
	// put your code here:

	unsigned char* ptext = (unsigned char *)plaintext;
	
	// Define Initialization Vector
	unsigned char ivec[8];
	int i;
	for (i=0; i<8; i++) ivec[i] = '0';

	// Set up key
	BF_KEY* key = (BF_KEY*)malloc(sizeof(BF_KEY));
	BF_set_key(key, 8, (const unsigned char*)keystr);

	// Encrypt
	unsigned char *out = (unsigned char *)malloc(bufsize * sizeof(unsigned char));
	BF_cbc_encrypt(ptext, out, bufsize, key, ivec, BF_ENCRYPT);

	// Get length after encryption
	*resultlen = (int)strlen((const char*)out);

	return (void *)out;
}

void *fs_decrypt(void *ciphertext, int bufsize, char *keystr, int *resultlen)
{
	// put your code here:

	unsigned char* ctext = (unsigned char *)ciphertext;
	
	// Define Initialization Vector
	unsigned char ivec[8];
	int i;
	for (i=0; i<8; i++) ivec[i] = '0';

	// Set up key
	BF_KEY* key = (BF_KEY*)malloc(sizeof(BF_KEY));
	BF_set_key(key, 8, (const unsigned char*)keystr);

	// Decrypt
	unsigned char *out = (unsigned char *)malloc(bufsize * sizeof(unsigned char));
	BF_cbc_encrypt(ctext, out, bufsize, key, (unsigned char*)ivec, BF_DECRYPT);

	// Get length after decryption
	*resultlen = (int)strlen((const char*)out) + 1;

	return (void *)out;
}
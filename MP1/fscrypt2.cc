#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "openssl/blowfish.h"
#include <vector>

/* 
implement the encrypt / decrypt function using the openSSL library

1.	void BF_set_key(BF_KEY *key, int len, const unsigned char *data)
2.	void BF_ecb_encrypt(const unsigned char *in, unsigned char *out, BF_KEY *key, int enc)

implement your own CBC operation mode
*/
const int BLOCKSIZE = 8;

int getNBlocks(int bytes);
int getPadding(int bytes);

void *fs_encrypt(void *plaintext, int bufsize, char *keystr, int *resultlen)
{
	// put your code here:
	
	unsigned char* ptext = (unsigned char *)plaintext;

	// Define Initialization Vector
	unsigned char ivec[8];
	int i;
	for (i=0; i<8; i++) ivec[i] = '0';

	// Get number of blocks and padding bytes
	int nBlocks = getNBlocks(bufsize);
	printf("%s %d\n", "Number of blocks: ", nBlocks);
	int padding = getPadding(bufsize);
	printf("%s %d\n", "Padding: ", padding);

	// Break the plaintext into blocks and add padding
	std::vector<unsigned char*> *ptextBlocks = new std::vector<unsigned char*> ();
	for(int i = 0; i < nBlocks; i++){
		int index = i * BLOCKSIZE * sizeof(unsigned char);
		for(int j = 0; j < BLOCKSIZE; j++){
			if(ptext[index + j] == '\0'){
				for(int k=1; k <= padding; k++){
					// PKCS#5 Padding
					ptext[index + j + k] = (unsigned char)(((int)'0') + padding);
				}
			}
		}
		// Temporal buffer to push blocks into vector
		unsigned char* buffer = (unsigned char *)malloc(BLOCKSIZE);
		strncpy((char *)buffer, (const char *)ptext + index, BLOCKSIZE);
		ptextBlocks->push_back(buffer);
	}

	// Set up the key
	BF_KEY* key = (BF_KEY*)malloc(sizeof(BF_KEY));
	BF_set_key(key, 8, (const unsigned char*)keystr);
	
	// Start encryption function
	std::vector<unsigned char *> *ctextBlocks = new std::vector<unsigned char *> ();
	for(int i = 0; i < ptextBlocks->size(); i++){
		if(i == 0){
			// If first block...
			unsigned char* firstBlock = ptextBlocks->at(i);
			unsigned char* xorIvec = (unsigned char *)malloc(BLOCKSIZE* sizeof(unsigned char));
			unsigned char* buffer = (unsigned char *)malloc(BLOCKSIZE * sizeof(unsigned char));
			
			// Xor with IV
			for(int j = 0; j < BLOCKSIZE; j++){
				xorIvec[j] = ivec[j] ^ firstBlock[j];
			}
			// Push first block into vector
			ctextBlocks->push_back(buffer);

			// Encrypt in EBC mode
			BF_ecb_encrypt(xorIvec, ctextBlocks->at(i), key, BF_ENCRYPT);
		}
		else{
			// Rest of blocks
			unsigned char* pastBlock = ctextBlocks->at(i-1);
			unsigned char* presentBlock = ptextBlocks->at(i);
			unsigned char* xorOut = (unsigned char *)malloc(BLOCKSIZE * sizeof(unsigned char));
			unsigned char* buffer = (unsigned char *)malloc(BLOCKSIZE * sizeof(unsigned char));

			// XOR with previous encrypted block
			for(int j = 0; j < BLOCKSIZE; j++){
				xorOut[j] = presentBlock[j] ^ pastBlock[j];
			}

			// Push into vector and encrypt
			ctextBlocks->push_back(buffer);
			BF_ecb_encrypt(xorOut, ctextBlocks->at(i), key, BF_ENCRYPT);
		}
	}

	// Format vector into convenient output
	int nBytesOut = BLOCKSIZE * sizeof(unsigned char) * nBlocks;
	int iterate = 0;
	unsigned char* out = (unsigned char *)malloc(nBytesOut);
	for(int i = 0; i < nBlocks; i++){
		unsigned char* encrypted = ctextBlocks->at(i);
		for(int j = 0; j < BLOCKSIZE; j++){
			out[iterate] = encrypted[j];
			iterate++;
		}
	}

	// Get length after encryption
	*resultlen = nBytesOut;

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

	// Get number of blocks
	int nBlocks = getNBlocks(bufsize);

	// Break ciphertext into blocks	
	std::vector<unsigned char *> *ctextBlocks = new std::vector<unsigned char *> ();
	for(int i = 0; i < nBlocks; i++){
		int index = i * BLOCKSIZE * sizeof(unsigned char);

		// Temporal buffer to push blocks into vector
		unsigned char* buffer = (unsigned char *)malloc(BLOCKSIZE);
		strncpy((char *)buffer, (const char *)ctext + index, BLOCKSIZE);
		ctextBlocks->push_back(buffer);
	}

	// Set up the key
	BF_KEY* key = (BF_KEY*)malloc(sizeof(BF_KEY));
	BF_set_key(key, 8, (const unsigned char*)keystr);

	// Start decryption function
	std::vector<unsigned char *> *ptextBlocks = new std::vector<unsigned char *> ();
	for(int i = 0; i < ctextBlocks->size(); i++){
		if(i == 0){
			// If first block...
			unsigned char* firstBlock = (unsigned char *)malloc(BLOCKSIZE * sizeof(unsigned char));
			unsigned char* xorIvec = (unsigned char *)malloc(BLOCKSIZE* sizeof(unsigned char));

			// Decryption and Xor with IV
			BF_ecb_encrypt(ctextBlocks->at(i), firstBlock, key, BF_DECRYPT);
			for(int j = 0; j < BLOCKSIZE; j++){
				xorIvec[j] = ivec[j] ^ firstBlock[j];
			}
			
			ptextBlocks->push_back(xorIvec);
		}
		else{
			// Rest of blocks
			unsigned char* presentBlock = ctextBlocks->at(i);
			unsigned char* pastBlock = ctextBlocks->at(i-1);
			unsigned char* decrypted = (unsigned char *)malloc(BLOCKSIZE * sizeof(unsigned char));
			unsigned char* xorOut = (unsigned char *)malloc(BLOCKSIZE * sizeof(unsigned char));

			// Decryption and Xor with previous block
			BF_ecb_encrypt(presentBlock, decrypted, key, BF_DECRYPT);
			for(int j = 0; j< BLOCKSIZE; j++){
				xorOut[j] = decrypted[j]^pastBlock[j];
			}

			ptextBlocks->push_back(xorOut);
		}
	}

	// Format vector into convenient output
	int nBytesOut = BLOCKSIZE * nBlocks * sizeof(unsigned char);
	int iterate = 0;
	unsigned char* out = (unsigned char *)malloc(nBytesOut);
	for(int i = 0; i < nBlocks; i++){
		unsigned char* block = ptextBlocks->at(i);
		for(int j = 0; j < BLOCKSIZE; j++){
			out[iterate] = block[j];
			iterate++;
		}
	}

	// Get length after decryption
	*resultlen = (int)strlen((const char*)out) + 1;
	
	return (void *)out;

}

// Get the number of blocks from an input
int getNBlocks(int bytes){
	int blocks = bytes/BLOCKSIZE;
	//Adjust when bytes is not an 8 multiple
	if(bytes%BLOCKSIZE !=0){
		blocks++;
	}
	return blocks;
}

// Get the number of padding bytes needed
int getPadding(int bytes){
	int padding = 0;
	if(bytes % BLOCKSIZE != 0){
		padding = bytes % BLOCKSIZE;
	}
	return padding;
}
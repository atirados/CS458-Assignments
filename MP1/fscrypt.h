#include "openssl/blowfish.h"

// bufsize = lenth of plaintext/ciphertext
// keystr = the key
// resultlen = number of valid bytes in the buffer

// the function return a pointer to a buffer
// the buffer stores the result, it's size is as least the required size

const int BLOCKSIZE = 8;

void *fs_encrypt(void *plaintext, int bufsize, char *keystr, int *resultlen);	// encryption function

void *fs_decrypt(void *ciphertext, int bufsize, char *keystr, int *resultlen);	// decryption function
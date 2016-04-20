Adrian Tirados Mata

To run the code, simply execute the following in the code folder:

-- First program
g++ main.cc fscrypt.cc -lcrypto
./a.out

-- Second program
g++ main.cc fscrypt2.cc -lcrypto
./a.out

Both programs will give the output of both the ciphertext and plaintext, being equal for both programs.
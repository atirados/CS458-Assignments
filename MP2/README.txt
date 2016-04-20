--------------
     MP2
--------------

Adrian Tirados Mata

--------------
sserver.cc
--------------

This program defines the behavior of the SSL server. It reads an input buffer from the client, adds 1 to the random number received, and then writes the response back to the client. I have used long long integers to avoid overflows if the nonce received is too big for the type int.

To compile the code, simply execute:

g++ sserver.cc -lssl -lcrypto -o server.out

To run the code, execute:

./server.out [path-to-cert] [path-to-key] [port number]

For example:
./server.out certs/cert.pem certs/key.pem 3000

--------------
sclient.cc
--------------

This program defines the behavior of the SSL client. It takes an input from the user and, after validating it, sends it to the server. Then it waits for a response and checks if the buffer read from the server is equal to the user input + 1. Currently, the program only accepts positive integer nonces for convenience, although this could be easily modified in the input checking part of the code.

To compile the code, simply execute:

g++ sclient.cc -lssl -lcrypto -o client.out

To run the code, execute:

./client.out [server name] [server port]

For example, if executing locally:
./client.out 127.0.0.1 3000

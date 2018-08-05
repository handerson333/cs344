#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

/*
* Carlos Leonard
* Program 4
* 11/29/2016
* The otp_enc_d program that will receive the plaint text and key from client and perform the encryption
*/


int search(char list[], char letter)
{
	int y = 0;
	while (y < strlen(list)){
		if (list[y] == letter)
		{
			return y;
		}
	}
	y++;
}

int main(int argc, char *argv[])
{
	// Our server/program endpoint
	int socketFD;

	// Our client socket
	int clientFD;

	// Our port number
	int port;

	struct sockaddr_in serv_addr, cli_addr;

	// Checking that we include our port as one of the command line arguments
	if (argc < 2) {
		fprintf(stderr, "Error: no port provided\n");
		exit(1);
	}

	// Opening the server socket
	socketFD = socket(AF_INET, SOCK_STREAM, 0);

	if (socketFD < 0)
	{
		fprintf(stderr, "Error: no port provided\n");
		exit(1);
	}

	// Assigning our port number
	port = atoi(argv[1]);

	// Server settings
	memset((char *)&serv_addr, '\0', sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(port);

	// Binding our server
	if (bind(socketFD, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
	{
		fprintf(stderr, "Error: unable to bind\n");
		exit(1);
	}

	listen(socketFD, 5);
	int length = sizeof(cli_addr);
	clientFD = accept(socketFD, (struct sockaddr *) &cli_addr, &length);
	if (clientFD < 0)
	{
		fprintf(stderr, "Error: unable to accept\n");
		exit(1);
	}

	char buffer[1024];
	int bytes;

	// Reading information from the client
	memset(buffer, '\0', 1024);
	bytes = recv(clientFD, buffer, 1023, 0);
	if (bytes < 0)
	{
		fprintf(stderr, "Error: unable to read\n");
		exit(1);
	}

	char buffer2[1024];
	int bytes2;
	memset(buffer2, '\0', 1024);
	bytes2 = recv(clientFD, buffer2, 1023, 0);

	// Ciphering our text
	char list[] = { "ABCDEFGHIJKLMNOPQRSTUVWXYZ " };
	char cipherText[70000];
	int x =0;
	while (x < bytes -1){
		// Getting the value of our current character from our message
		int sourceValue = search(list, buffer[x]);

		// Getting the value of our current character from our key
		int keyValue = search(list, buffer2[x]);

		// Message + Key
		int cipherValue = sourceValue + keyValue;

		// Condition in case the number is greater than 27
		while (cipherValue >= 27)
		{
			cipherValue -= 27;
		}

		// Getting the proper character for our cipher text based on the cipher value
		cipherText[x] = list[cipherValue];
		x++;
	}

	// Writing information to the client
	bytes = send(clientFD, cipherText, strlen(cipherText), 0);
	if (bytes < 0)
	{
		fprintf(stderr, "Error: unable to write\n");
		exit(1);
	}

	close(clientFD);
	close(socketFD);
	return 0;
}
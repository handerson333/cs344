#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 



#define h_addr h_addr_list[0]

int main(int argc, char *argv[])
{
	// Length of our files
	int plaintextLength = 0, keyLength = 0;

	// Condition to check for command line arguments
	if (argc < 4)
	{
		fprintf(stderr, "Error: Not enough arguments provided\n");
		exit(1);
	}

	// Analyzing the plaintText and Key files
	FILE * plainText = fopen(argv[1], "r");
	FILE * key = fopen(argv[2], "r");
	char letter;
	char text[100000];
	char keyText[100000];

	// Iterating through the plainText file
	// Will keep track of its lengths and also assure we only work with valid characters
	while ((letter = fgetc(plainText)) != EOF)
	{
		text[plaintextLength] =  letter;
		
		// Condition to check that we have only valid characters
		if (letter >= 'A' && letter <= 'Z' || letter == ' ' || letter == '\n')
		{
			// Keep track of the plaintext file length
			plaintextLength++;
		}
		else
		{
			fprintf(stderr, "Error: input contains bad characters\n");
			exit(1);
		}
	}
	// Iterating through the key file
	while ((letter = fgetc(key)) != EOF)
	{
		keyText[keyLength] = letter;

		// Condition to check that we have only valid characters
		if (letter >= 'A' && letter <= 'Z' || letter == ' ' || letter == '\n')
		{
			// Keep track of the plaintext file length
			keyLength++;
		}
		else
		{
			fprintf(stderr, "Error: input contains bad characters\n");
			exit(1);
		}
	}

	// Condition to make sure the length of our key is the same size or longer as our plaintext
	if (keyLength < plaintextLength)
	{
		fprintf(stderr, "Error: key '%s' is too short\n", argv[2]);
		exit(1);
	}
	
	// Client socket
	int socketFD;
	
	// Will contain our port number
	int port;

	// Build in structures from netinet
	struct sockaddr_in serv_addr;
	struct hostent *server;

	// Assigning our port number
	port = atoi(argv[3]);

	// Opening our client socket
	socketFD = socket(AF_INET, SOCK_STREAM, 0);
	if (socketFD < 0)
	{
		fprintf(stderr, "Error: Unable to open socket\n");
		exit(1);
	}

	// Assigning our target host
	server = gethostbyname("localhost");
	if (server == NULL)
	{
		fprintf(stderr, "Error: no such host\n");
		exit(1);
	}

	// Server Settings
	memset((char*)&serv_addr, '\0', sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr,(char *)&serv_addr.sin_addr.s_addr,server->h_length);
	serv_addr.sin_port = htons(port);

	// Connecting to the server
	if (connect(socketFD, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
	{
		fprintf(stderr, "Error: could not contact otp_enc_d on port %d\n", port);
		exit(2);
	}


	char buffer[1024];
	int bytes;

	// Sending information to our server
	bytes = send(socketFD, text, strlen(text), 0);
	memset(text,'\0',sizeof(text));
	if (bytes < 0)
	{
		fprintf(stderr, "Error: Unable to write to the server\n");
		exit(1);
	}
	
	int bytes2;
	bytes2 = send(socketFD, keyText, strlen(keyText), 0);

	// Receiving information from our server
	memset(buffer, '\0', sizeof(buffer));
	bytes = recv(socketFD, buffer, sizeof(buffer) - 1, 0);
	if (bytes < 0)
	{
		fprintf(stderr, "Error: Unable to read from the server\n");
		exit(1);
	}
	fprintf(stdout, "%s", buffer);
	fprintf(stdout, "\n");

	close(socketFD);
	
	return 0;
}
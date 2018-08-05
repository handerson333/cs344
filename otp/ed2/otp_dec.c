#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

void error(const char *msg) { perror(msg); exit(1); } // Error function used for reporting issues
char* readfile(char*);


int main(long argc, char *argv[])
{
	long socketFD, portNumber, charsWritten, charsRead;
	struct sockaddr_in serverAddress;
	struct hostent* serverHostInfo;
	char buffer[100000];
	
	//check it has all CLA's
	if (argc < 4) { fprintf(stderr,"USAGE: %s plaintext key port\n", argv[0]); exit(0); } // Check usage & args

	//get content inside files
	char* plaintext_filename = argv[1];
    	char* key_filename = argv[2];
    	//printf("pt = %s\nkey = %s\n", plaintext_filename, key_filename);    	
    	char* plaintext = readfile(plaintext_filename);
	//printf("pt = %s\n", plaintext);
	char* key = readfile(key_filename);
	//printf("pt = %s\n", key);
	if (strlen(key) < strlen(plaintext)){
		fprintf(stderr, "error: mykey is too short");
		exit(1);
	}

	// Set up the server address struct
	memset((char*)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
	portNumber = atoi(argv[3]); // Get the port number, convert to an integer from a string
	serverAddress.sin_family = AF_INET; // Create a network-capable socket
	serverAddress.sin_port = htons(portNumber); // Store the port number
	serverHostInfo = gethostbyname("localhost"); // Convert the machine name into a special form of address
	
	
	//check for host
	if (serverHostInfo == NULL) { fprintf(stderr, "CLIENT: ERROR, no such host\n"); exit(0); }
	memcpy((char*)&serverAddress.sin_addr.s_addr, (char*)serverHostInfo->h_addr, serverHostInfo->h_length); // Copy in the address




	// Set up the socket
	socketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
	if (socketFD < 0) error("CLIENT: ERROR opening socket");
	
	// Connect to server
	if (connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) // Connect socket to address
		error("CLIENT: ERROR connecting");







	// Send plaintext to daemon
	charsWritten = send(socketFD, plaintext, strlen(plaintext), 0); // Write to the server
	if (charsWritten < 0) error("CLIENT: ERROR writing to socket");
	if (charsWritten < strlen(plaintext)) printf("CLIENT: WARNING: Not all data written to socket!\n");
	
	// Get return message confirmation from daemon
	memset(buffer, '\0', sizeof(buffer)); // Clear out the buffer again for reuse
	charsRead = recv(socketFD, buffer, sizeof(buffer) - 1, 0); // Read data from the socket, leaving \0 at end
	if (charsRead < 0) error("CLIENT: ERROR reading from socket");
	//printf("CLIENT: I received this from the server: \"%s\"\n", buffer);
	
	
	// Send key to daemon
	charsWritten = send(socketFD, key, strlen(key), 0); // Write to the server
	if (charsWritten < 0) error("CLIENT: ERROR writing to socket");
	if (charsWritten < strlen(key)) printf("CLIENT: WARNING: Not all data written to socket!\n");
	
	// Get return message confirmation
	memset(buffer, '\0', sizeof(buffer)); // Clear out the buffer again for reuse
	charsRead = recv(socketFD, buffer, sizeof(buffer) - 1, 0); // Read data from the socket, leaving \0 at end
	if (charsRead < 0) error("CLIENT: ERROR reading from socket");
	
	//print the encrypted text to stdout
	fprintf(stdout, "%s\n", buffer);
	


	close(socketFD); // Close the socket
	return 0;
}



char *readfile(char* textfile){

	FILE *fp;
  	char *text = malloc(200000);
  	fp=fopen(textfile,"r");

	//read file until you get to newline
 	fscanf(fp, "%[^\n]", text);
	fclose(fp);
	return text;
	
}






#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

void error(const char *msg) { perror(msg); exit(2); } // Error function used for reporting issues
void encode(char*,char*);
void fork_it(long listenSocketFD, long establishedConnectionFD, long charsRead, char* buffer, char* key);

int main(long argc, char *argv[])
{

	long j =0;
	
	
	long listenSocketFD, establishedConnectionFD, portNumber, charsRead;
	socklen_t sizeOfClientInfo;
	char *buffer = malloc(200000);
	char *key = malloc(200000);
	struct sockaddr_in serverAddress, clientAddress;



	//check CLA's
	if (argc < 2) { fprintf(stderr,"USAGE: %s port\n", argv[0]); exit(1); } // Check usage & args



	// Set up the address struct for this process (the server)
	memset((char *)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
	portNumber = atoi(argv[1]); // Get the port number, convert to an integer from a string
	serverAddress.sin_family = AF_INET; // Create a network-capable socket
	serverAddress.sin_port = htons(portNumber); // Store the port number
	serverAddress.sin_addr.s_addr = INADDR_ANY; // Any address is allowed for connection to this process




	// Set up the socket
	listenSocketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
	if (listenSocketFD < 0) error("ERROR opening socket");




	// Enable the socket to begin listening
	if (bind(listenSocketFD, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) // Connect socket to port
		error("ERROR on binding");
	listen(listenSocketFD, 5); // Flip the socket on - it can now receive up to 5 connections



	
	// Accept a connection, blocking if one is not available until one connects
	
	do{
	sizeOfClientInfo = sizeof(clientAddress); // Get the size of the address for the client that will connect
	establishedConnectionFD = accept(listenSocketFD, (struct sockaddr *)&clientAddress, &sizeOfClientInfo); // Accept
	
	fork_it(listenSocketFD, establishedConnectionFD, charsRead, buffer, key);
	
	}while(1);

	
	//free variables
	free(buffer);	
	free(key);
	close(establishedConnectionFD); // Close the existing socket which is connected to the client
	
	close(listenSocketFD); // Close the listening socket
	
	
	return 0; 
}

void fork_it(long listenSocketFD, long establishedConnectionFD, long charsRead, char* buffer, char* key){
	pid_t pid = fork();
	
	
	if (pid == -1){//error
		error("forking error\n");
		exit(2);
	}
	else if(pid == 0){//child
		//close(listenSocketFD); // Close the listening socket
		// Get the message from the client and display it
		
		
		
		memset(buffer, '\0', 100000);
		char small_read[100000];
		memset(small_read, '\0', sizeof(small_read));
		charsRead = recv(establishedConnectionFD, buffer, sizeof(buffer)-1, 0);
		charsRead = recv(establishedConnectionFD, small_read, sizeof(small_read)-1, 0);
		//while(strstr(buffer, "@@") == NULL){
		//	memset(small_read, '\0', sizeof(small_read));//clear buiffer
			//charsRead = recv(establishedConnectionFD, small_read, sizeof(small_read)-1, 0); // Read the client's message from the socket
			//strcat(buffer, small_read);//add what we have so far
		//	if (charsRead <= 0){ error("ERROR reading from socket\n"); exit(1);}
	
		//}
		//int terminalLocation = strstr(buffer, "@@") - buffer; // Where is the terminal
		//buffer[terminalLocation] = '\0';
	
		// Send a Success message back to the client
		charsRead = send(establishedConnectionFD, "I am the server, and I got your message", 39, 0); // Send success back
	
		if (charsRead < 0) error("ERROR writing to socket");
		
		
		// Get the message from the client and display it
		
		
		//memset(key, '\0', 100000);
		//charsRead = recv(establishedConnectionFD, key, 99999, 0); // Read the client's message from the socket
		//if (charsRead < 0) error("ERROR reading from socket");
		
		memset(key, '\0', 100000);
		char small_key[100000];
		memset(small_key, '\0', sizeof(small_key));
		charsRead = recv(establishedConnectionFD, key, sizeof(key)-1, 0);
		charsRead = recv(establishedConnectionFD, small_key, sizeof(small_key)-1, 0);
		strcat(key, small_key);
		
		
		//while(strstr(key, "@@") == NULL){
		//	memset(small_read, '\0', sizeof(small_read));//clear buiffer
		//	charsRead = recv(establishedConnectionFD, small_read, sizeof(small_read)-1, 0); // Read the client's message from the socket
		//	strcat(key, small_read);//add what we have so far
		//	if (charsRead <= 0){ error("ERROR reading from socket\n"); exit(1);}
	
		//}
		//terminalLocation = strstr(key, "@@") - key; // Where is the terminal
		//key[terminalLocation] = '\0';
		
		
		
		
		encode(buffer, key);
		
		// Send a Success message back to the client
		charsRead = send(establishedConnectionFD, buffer, strlen(buffer), 0); // Send success back
		exit(0);
	
	if (charsRead < 0) error("ERROR writing to socket");
	}
	else{//parent
		wait(NULL);
	}
	
	
}

void encode(char* plain, char* key){
	
	long i = 0;
	long length = strlen(plain);
	
	while (i < length){
		//turn space into ] for plaintext
		if(plain[i] == 32){
			plain[i] = 91;	
		}
		//turn space into ] for key
		if(key[i] == 32){
			key[i] = 91;
		}
		//subtract by 65 to go down to 0-27
		plain[i] -= 65;
		key[i] -= 65;
		//add key to plaintext
		plain[i] += key[i];
		//mod by 27 if answer is greater than 27
		plain[i] = plain[i] % 27;	
		//go back to correct characters
		plain[i] += 65;
		//convert ] to space
		if(plain[i] == 91){
			plain[i] = 32;
		}
		
		
		//next letter
		i++;
	}





}


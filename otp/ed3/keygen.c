#include <time.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


int main(int argc, char * argv[]){
	srand(time(NULL));//seed random with time
	int rng;//random
	FILE *fp;//filepath

	if(argc < 2){//if not enough arguments
		printf("error: not enough arguments\n");
		exit(1);//exit with error
	}

	int input = strtol(argv[1],NULL ,10 );//convert string to long
	int i = 0;//counter
	while (i < input){//for size of input
		rng = ((rand()%27))+65;//get all captial ascii letters from 65-91
		if(rng == 91){//if [ change it to a space
			rng = 32;
		}

		fprintf(stdout, "%c", rng);//print letter to stdout
		i++;//increment
	}
	fprintf(stdout,"\n");//add newline




}

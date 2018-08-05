#include <time.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


int main(int argc, char * argv[]){
	srand(time(NULL));
	int rng;
	FILE *fp;

	if(argc < 2){
		printf("error\n");
		exit(1);
	}
	
	int input = strtol(argv[1],NULL ,10 );
	int i = 0;
	//fp = fopen("mykey.txt", "w");
	while (i < input){
		rng = ((rand()%27))+65;
		if(rng == 91){
			rng = 32;
		}

		fprintf(stdout, "%c", rng);
		i++;
	}
	fprintf(stdout,"\n");
	//fclose(fp);




}

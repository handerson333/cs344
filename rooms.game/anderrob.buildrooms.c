#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <pwd.h>
#include <sys/stat.h>
#include <sys/types.h>

//I didn't feel I needed to specify what each function does as I try to name them fairly intuitively
//However, I did explain what each process does inside of the functions

char room_names[10][20] = {//lord of the rings just because 
	"shire", "isengard", "weathertop", "rivendell", "mordor", "moria", "erebor", "lorien", "rohan", "gondor"
};

struct room{	//probably looks the same as literally everybody elses, I tried to do it without a struct... was a bad idea
	char name[30];
	char *connections[7];
	int num_connections;
};

//function definitions
void make_folder(char *buffer);
void make_rooms(char * folder);
void get_room_names(struct room *rooms); 
void make_connections(char *folder, struct room * rooms);
//void both_ways(struct room *rooms);
void zero_out_taken(int *taken);
void get_types(struct room *rooms);



int main(){
	srand(time(NULL)); //seed time for random
	char folder[100]; 
	make_folder(folder);	//make the folder
	make_rooms(folder);		//make the rooms
	return 0;	
}

void make_folder(char *buffer){ 
	sprintf(buffer,"anderrob.rooms.%d",getpid());	//add process id to onid.rooms
	//printf("%s\n", buffer);
	mkdir(buffer, 0777);	//make folder with all permissions

}

void make_rooms(char * folder){
	FILE* fp;
	struct room rooms[7];
	int i = 0;
	int j = 0;
	get_room_names(rooms); //get names for the rooms

	chdir(folder);	//change into that folder
	make_connections(folder, rooms);	//make the connections between the rooms
	i = 0;
	while (i<7){	//for all rooms
		fp = fopen(rooms[i].name, "w");	//create a file with the room name for writing		
		fprintf(fp, "ROOM NAME: %s\n", rooms[i].name);	//while I'm here I'll add the name
		fclose(fp);	//close it from pesky orcses views
		i++;
	}
	//connections
	i = 0;

	while(i<7){	//for all rooms
		j=0;

		while( j < rooms[i].num_connections){	//for all connections of that room
			fp = fopen(rooms[i].name, "a");	//open the room for appending
			fprintf(fp, "CONNECTION %d: %s\n", j+1 ,rooms[i].connections[j]); //add a connection for that room
			fclose(fp);	//close the file just for safe keeping
			j++;
		}

		i++;	
	}
	//types
	i=0;

	while(i<7){	//for all files
		if (i == 0){	//if it's the first file in the array, make it the START_ROOM
			fp = fopen(rooms[i].name, "a");	//open it for appending
			fprintf(fp, "ROOM TYPE: %s\n", "START_ROOM");	//add it's new type
			fclose(fp);	//close it, it's mine, my own, my...precious
		}
		else if (i ==6){	//last file in the array make it the end room
			fp = fopen(rooms[i].name, "a");	//open it for appending
			fprintf(fp, "ROOM TYPE: %s\n", "END_ROOM");	//add its new type
			fclose(fp);	//close it and go to sleep
		}
		else{	//otherwise its a middle room! make it so
			fp = fopen(rooms[i].name, "a");	//open for append
			fprintf(fp, "ROOM TYPE: %s\n", "MID_ROOM");	//I dub thee middle
			fclose(fp);	//now go away
		}
		i++;	//NEXT!
	}




}
/* this whole attempt was fruitless and pointless
   void get_types(struct room *rooms){
   int i = 0;
   while(i<7){//for all rooms
   if(i == 0){
   rooms[i].room_type = 0;//first room gets start
   }
   else if(i == 6){
   rooms[i].room_type = 2;//last room gets end
   }
   else{
   rooms[i].room_type = 1;//all other rooms get mid
   }		
   i++;
   }
   }
   */


void make_connections(char *folder, struct room *rooms){
	int i,j,rng,num_connections;
	int taken[7];
	i=0;
	//set number of connections for each room
	while(i<7){
		num_connections = ((rand()%4)+3); //random number between 3 and 6
		rooms[i].num_connections = num_connections;	//set that random number as amount of connections
		taken[i] =0;	//add it to a copy-ish taken array
		i++;
	}


	//for all rooms
	i = 0;
	while (i < 7){
		j=0;
		zero_out_taken(taken); //zero out taken for each player
		//for all connections of current room
		while(j < rooms[i].num_connections){
			//connect to random room
			//make sure rand did not reference the current room
			rng = (rand() % 7);	//random number between 0 and 6
			if (rng != i){ //if rng does not equal current room number
				if (taken[rng] == 0){//if the rand room wasnt already added to connections
					rooms[i].connections[j] = rooms[rng].name;//add random room to connections
					taken[rng]++;
					j++;
				}
			}
		}
		i++;
	}
	//both_ways(rooms);

}
/* another fruitless venture, should have gone with Bilbo
   void both_ways(struct room *rooms){
   int i;//current room
   int j;//current connection
   int k;//current connections connection
   i =0;
   while ( i < 7 ){//for all rooms
   j = 0;
   while( j < rooms[i].num_connections){//for all connections
   k = 0;
   while(k < rooms[j].connections[k])//for all current connections connections
   if(strcmp(rooms[i].connections[j], rooms[j].connections[k]) != 0){
   rooms[i].connections[j] = rooms[j].connections[k]; //WRONG HERE
   }//if connection has current room as their connection
//if the connection isnt already present
//add connection

j++;
}
i++;
}
}
*/

void get_room_names(struct room *rooms){	//they need names!
	int rng = rand()%10;	//random number between 0 and 9
	int i = 0;
	while (i<7){	//for 7 rooms
		strcpy(rooms[i].name, room_names[rng]);	//copy a random room name as one of our cool room names
		if (rng == 9){	//if were at the end of rng, reset it so we dont seg fault
			rng = 0;
		}
		else{	//otherwise... next!
			rng++;
		}
		i++;
	}
	return;


}
//simple function to zero out an integer array
void zero_out_taken(int *taken){
	int i = 0;
	while(i<7){	//for all 7 numbers of taken
		taken[i] = 0;
		i++;
	}
}

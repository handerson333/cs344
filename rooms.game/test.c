#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>

//I didn't feel I needed to specify what each function does as I try to name them fairly intuitively
//However, I did explain what each process does inside of the functions

enum room_type{
    START_ROOM = 0, MID_ROOM = 1, END_ROOM = 2
};

struct room {
    int num_connections;
    struct room* connections[6];
    enum room_type room_type;
    char name[100];
};
//constants
struct room rooms[7];
char folder[256];
char* time_file = "currentTime.txt";
pthread_mutex_t mutex;


//function declarations
void get_directory();
void make_rooms();
void get_room_names();
void play();
int room_num(char *room);
int time_thread();
void* make_time();
void read_time();


int main(){
    get_directory();//get to the latest made directory
    make_rooms(); 	//make rooms similar to buildrooms to make things easier 
					//and not have to continuously re-read files
    play();			//start game
	return 0;
}


int room_num(char *room){
    int i = 0;
	while(i<7){//for all roomss in the struct
        if( strcmp(rooms[i].name, room) == 0 ){//if current room is the same as input room, return its number
            return i;
        }
		i++;//otherwise go to next room
    }
}

int time_thread(){
    pthread_t write_time; 
    pthread_mutex_lock(&mutex);
	//create thread for time
	//adapted from http://www.cse.psu.edu/~deh25/cmpsc473/programs/thread_sample.c
    if(pthread_create(&write_time, NULL, make_time, NULL) != 0){
        printf("Thread Error");
        return 0;
    }

    pthread_mutex_unlock(&mutex); //set mutex 
    pthread_join(write_time, NULL);//wait for unlock
	
    return 1;
}

void* make_time(){
    char time_holder[256];
    time_t current_time;
    struct tm * time_struct;
    FILE *time_;
	//set buffer for time string
    memset(time_holder, '\0', sizeof(time_holder));
	//seed current time
    time(&current_time); 
	//set local time 
    time_struct = localtime(&current_time);
	//make string into time buffer with specific format
    strftime(time_holder, 256,  "%I:%M%P %A, %B %d, %Y", time_struct); 
    time_ = fopen(time_file, "w"); //open time file
    fprintf(time_, "%s\n", time_holder);//print time
    fclose(time_);//close file

    return NULL; //needed for creating the thread
}

void play(){
    int room;
	int i =0;
    int step = 0;
    int path[1024];
    int match = 0;
    struct room current_room;
    char buffer[1024];
	char *current_room_type;
	
	while(i< 7){ //for all rooms
        if(rooms[i].room_type == START_ROOM){//find start room 
            path[step] = i; //set steps start
        }
		i++;
    }
    while(1){     
        room = path[step];
		
        current_room = rooms[room];
        printf("CURRENT LOCATION: %s\n", current_room.name);//show current location       
		i=0;		
        while(i < current_room.num_connections){//for all connections at this room
            printf("CONNECTION %d: %s\n", i+1, current_room.connections[i]->name);//print current connection
        i++;
		}
		if( current_room.room_type == 0){//set string to room type for start room
			current_room_type = "START_ROOM";
		}
		else if(current_room.room_type == 2){//set string to room type for end room
			current_room_type = "END_ROOM";
		}
		else{
			current_room_type = "MID_ROOM";	//set string to room type for mid room		
		}		
        printf("ROOM TYPE: %s\n", current_room_type);//print type of room to user		
		memset(buffer, '\0', sizeof(buffer));//set buffer for user input
        printf("\nWHERE TO? >");
        scanf("%100s", buffer);//get user input 100 bytes long
        printf("\n"); //newline
        match = 0; //match of end room
        i=0;
        while(i < current_room.num_connections){//for all connections of this room
            if(strcmp(buffer, current_room.connections[i]->name) == 0){ //check entered name with all current connections
                ++step; //pre increment step
                path[step] = room_num(buffer); //add step
                room = path[step]; //record steps
                current_room = rooms[room];
                match = 1;
                if(current_room.room_type == END_ROOM){ //check if they found the end
                    printf("YOU HAVE FOUND THE END ROOM. CONGRATULATIONS!\nYOU TOOK %d STEPS. YOUR PATH TO VICTORY WAS:\n", step);
                    int j=0;
					while(j < step + 1){ //for all steps taken
						printf("%s\n", rooms[path[j]].name);
						j++;
					}
                    return;
                }
            }
			i++;
        }


        if(strcmp(buffer, "time") == 0 && match == 0){//if input was time

            if( time_thread() == 1){//create time thread
                read_time(); //read current time
            }           
        }
        else if(match == 0){//if input didnt match a connection or time
            printf("Please enter an existing room name.\n\n");
        }
    }
}



void read_time(){
    char buffer[256];//large buffer for all of time struct
    FILE *time_;
    memset(buffer, '\0', sizeof(buffer)); //set buffer
    time_ = fopen(time_file, "r");//read the already created time file

    while(fgets(buffer, 256, time_) != NULL){//for all of time until you get null
        printf("%s\n", buffer);
    }
    fclose(time_);
}



//zero out and get all files names into names of struct rooms
void get_room_names(){
    DIR *dir;
    struct dirent *dir_stream;
    int room_num = 0;
    int i=0;
	int j=0;
	while(i<7){//for all rooms
        memset(rooms[i].name, '\0', sizeof(rooms[i].name));//set terminator and buffer; 
        rooms[i].num_connections = 0; //set connections to 0 to start and zero it out
        j = 0; //reset j to 0
		while(j<6){//for all connections
            rooms[i].connections[j] = NULL; //set connection to null
			j++;
		}
		i++;
	}


    if ((dir = opendir (folder)) != NULL) {//open folder if it exists
        while ((dir_stream = readdir (dir)) != NULL) {//read all the files in directory
            if(strlen(dir_stream->d_name) > 2){ //make sure the name of the directory is bigger than 2
                strcpy(rooms[room_num].name, dir_stream->d_name);//copy names into current struct name
                room_num++; //next room file
            }
        }
    }
}


void make_rooms(){
    char line_buff[256];
    char value_buff[256];
    FILE *fp;
    int i =0;
    get_room_names(); //get room names
    chdir(folder); //go into that folder
    while(i < 7){
        fp = fopen(rooms[i].name, "r");//open file at file path for read only
        memset(line_buff, '\0', sizeof(line_buff));//set buffer for all the lines in the file
        memset(value_buff, '\0', sizeof(value_buff));//set buffer for the values I will be extracting
        while(fgets(line_buff, sizeof(line_buff), fp) != NULL){  //read until end of file   
			    int j=0;
				strtok(line_buff, ":"); //get to first semi colon
				strcpy(value_buff, strtok(NULL, ""));//copy in value
				value_buff[strlen(value_buff) - 1] = '\0';//add null terminator
				line_buff[strlen(line_buff) - 1] = '\0';//add null terminator
				while(j < strlen(value_buff)){ //for the whole value lenght
					value_buff[j] = value_buff[j+1];//copy in value to buffer
					j++;
				}			
            if(strcmp(line_buff, "ROOM TYP") == 0){ //find room type
                if(strcmp(value_buff, "START_ROOM") == 0){//set current room type to start room if the value is START_ROOM
                    rooms[i].room_type = START_ROOM;
                }
                else if(strcmp(value_buff, "END_ROOM") == 0){//set current room type to start room if the value is END_ROOM
                    rooms[i].room_type = END_ROOM;
                }
                else{
                    rooms[i].room_type = MID_ROOM;//otherwise set current room type to MID_ROOM
                }
            }
            else if(strcmp(line_buff, "CONNECTION ") == 0){ //get to value connection
                int connected_room = room_num(value_buff);			
                int num_connections1 = rooms[i].num_connections;//count number of connections
				rooms[i].connections[num_connections1] = &rooms[connected_room]; //read in all the connections	
				rooms[i].num_connections++;//add one to number of connections								
            }
        }
        fclose(fp);
		i++;
    }
    chdir(".."); //go back to parent directory
}



//Most of this was very closesly adapted from Spencer Moran's post in Piazza post 234
//credit goes to Spencer Moran, I wouldn't have gotten it to work on time without him
void get_directory(){
    char *fd = "anderrob.rooms.";
    char directory[256];
    DIR *d;
    struct dirent *dp;
    struct stat *buffer;
    time_t lastModified; 
    time_t Newest = 0;
    buffer = malloc(sizeof(struct stat));//set buffer size
    memset(directory, '\0', sizeof(directory));
    getcwd(directory, sizeof(directory));//get current directory path
    d = opendir(directory);

    if (d != NULL) {
        while (dp= readdir(d)) {	//for all files
            if (strstr(dp->d_name, fd) != NULL){//make sure there is a folder
                stat(dp->d_name, buffer);
                lastModified = buffer->st_mtime;//search for the last modified directory with my name
                if(lastModified > Newest){ 
                    Newest = lastModified; //make that newest
                    strcpy(folder, dp->d_name);//last modified directory saved into folder
                }
            }
        }
    }
	free(buffer);
}


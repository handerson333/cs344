#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <dirent.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

int main(){
	//change into created folder SOURCE: Spencer Moran from piazza post 234
	char *fd = "anderrob.rooms.";
	char currentDir[100];
	memset(currentDir, '\0', sizeof(currentDir));
	getcwd(currentDir, sizeof(currentDir));
	//printf("%s\n", currentDir);
	DIR *d;
	struct dirent *dp;
	struct stat *buffer;
	buffer = malloc(sizeof(struct stat));
	
	time_t lastModified;  
	d = opendir(currentDir);
	if (d != NULL) {
		while (dp= readdir(d)) {	
			if (strstr(dp->d_name,fd) != NULL){
				stat(dp->d_name, buffer);
				lastModified = buffer->st_mtime;
				//printf("%s: %s\n", dp->d_name, ctime(&lastModified));
				//printf("%s: %d\n", dp->d_name, lastModified);
				chdir(dp->d_name);
			}
		}
	}
	
	
	//find start room
	char start_room[256];
	FILE *room;
	DIR *current_dir;
	struct dirent *
	start_room = readdir(d);
	printf("directory: %s\n", current_dir);
	
	
	
	//create new thread for time
	
	
	////start game
	
	//print start room
	
	//print "where to?"
	//check if time was entered or if room exists otherwise loop back
	
	//while current room type is not END ROOM
	//print room they asked for
	
	//print "where to?"
	//check if time was entered or if room exists otherwise loop back
	
	
	
	




}



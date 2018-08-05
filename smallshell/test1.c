#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>
#include <errno.h>

//struct for each input entered
struct input{
	char* parsed_input[10];	//holds each argument after tokened
	char input[100];	//general input
	int num_parameters;	//number of arguments
	int num_processes;	//number of processes
};
//global for current pid because of signal trappers
pid_t current_pid;
int foreground_only;	//check for status of ctrlz

//functions
void get_child_status(int options,struct input *current_input);
void convert_to_pid(struct input *current_input);
int parse_parameters(struct input *current_input);
void do_command(char *command, char *args[],struct input *current_input);
void do_command_background(char *command, char *args[],struct input *current_input);
void if_interrupt();
void if_stop();



int main(){
	//create struct
	struct input current_input;
	//set global for foreground only mode to off and rest of globals and struct values
	foreground_only = 0;
	current_input.num_processes = 0;
	current_pid = -1;
	//setup signal handlers
	sigset(SIGTSTP, if_stop);
	sigset(SIGINT, if_interrupt);
	
	//run until run is 0
	int run = 1;
	while(run == 1){
		int i;//process counter
		int j = 0;//process end counter
		for(i = 0; i < current_input.num_processes; i++){
			int status;
			pid_t id = waitpid(-1, &status, WNOHANG );//wait for child processes
			if(id == -1){	//catch fail for any reason
				fprintf(stderr, "failed to wait.\n"); //let user know failed to wait
				fflush(stdout);		//clear standard out
				exit(1);
			}
			else if(id == 0){	//wait until id is 0
				continue;
			}
			if(!WIFCONTINUED(status)){//continue process once done hanging for child processes
				j++; 
				fprintf(stdout, "Background process (PID: %d) terminated with status %d\n", id, status);
				fflush(stdout);		//clear standard out
			}
		}
		current_input.num_processes -= j;
		printf(": ");	//prompt
		fflush(stdout);		//clear standard out
		if(fgets(current_input.input, 100, stdin) == NULL){ //get user input until input isnt nothing
			continue;
		}
		char *new_line = strchr(current_input.input, '\n');	//replace newline character with a string null terminator
		*new_line = '\0';
		if (parse_parameters(&current_input) == 1){	//parse each parameter into their own separate part of array
			continue;
		}
		convert_to_pid(&current_input);	//if any '$$' happens, convert it to the actual pid #
		//if the first argument is exit, then set run to 0 and close loop
		if(strcmp(current_input.parsed_input[0], "exit") == 0){	
			run = 0;
			continue;
		}
		if(strcmp(current_input.parsed_input[0], "cd") == 0){	//if first argument is cd then change directories
			if(current_input.num_parameters < 2){ //if theres only one argument then go to home directory
				chdir(getenv("HOME"));	//get home directory
			}
			else{
				int good_answer = chdir(current_input.parsed_input[1]);	//change directory to inputted directory
				//if the inputted answer doesnt match somewhere in the directory go to the home directory
				if(good_answer != 0){	
					printf("No such directory, changing to home directory.\n");
					fflush(stdout);		//clear standard out
					chdir(getenv("HOME"));
				}
			}
			char buff[255];	//buffer for current working directory length
			if(getcwd(buff, 255) != NULL){	//make sure you can get cwd, if you can write it to buff
				printf("Current Working Directory is %s\n", buff);	//print cwd
				fflush(stdout);		//clear standard out
			}
			continue;
		}
		int background = 0;	//should this command be ran in the background?
		if(current_input.num_parameters > 1){ // only check input with more than 1 parameter
			//check the end parameter for &
			if(strcmp(current_input.parsed_input[current_input.num_parameters - 1], "&") == 0){	
				//explicitly change the & to a null
				current_input.parsed_input[current_input.num_parameters - 1] = (char *) NULL;	
				background = 1;	//background check is on
			}
		}
		if(background == 0 || foreground_only == 1){	//if background is off or foreground only mode is on
			//do command in foreground
			do_command(current_input.parsed_input[0], current_input.parsed_input, &current_input);	
		}
		else{	//otherwise do it in the background
			do_command_background(current_input.parsed_input[0], current_input.parsed_input, &current_input);
		}
	}
	return 0;	//return  without error
}



void convert_to_pid(struct input *current_input){	//converts all $$ to pid 
	int i;	//counter
	for (i = 0; i < current_input->num_parameters; i++){	//for all parameters
		if(strcmp(current_input->parsed_input[i],"$$") == 0){	//if current parameter is $$
			sprintf(current_input->parsed_input[i], "%d", getpid());	//change that parameter to actual pid
		}
	}	
}



int parse_parameters(struct input *current_input){	//tokenizes parameters in input
	char * param = strtok(current_input->input, " ");	//tokenize by space
	if(param == NULL){	//make sure there is infact a parameter
		return 1;	//if not, return with error
	}
	
	int i = 0; //parameter counter
	while(param != NULL){	//until parameter is null
		current_input->parsed_input[i] = param;	//set struct member to current parameter
		i++; // next!
		param = strtok(NULL, " ");	//reset param back to null
	}
	current_input->parsed_input[i] = (char *) NULL;	//change last parameter to null
	current_input->num_parameters = i; //might as well set number of parameters while were here
	return 0;	//return with no error
}



void do_command(char *command, char *args[], struct input *current_input){	//do foreground command
	pid_t pid = fork();	//fork to child to keep parent running
	if(pid == -1){	//if there was a forking error
		fprintf(stderr, "Fork for %s failed. error number: %d\n", command, errno);
		fflush(stdout);		//clear standard out
		exit(1);	//exit with error
	}
	else if(pid == 0){	//child process		
		execvp(command, args);	//execute the entered commands
		//check if command input was an actual command
		fprintf(stderr, "%s isn't a known command. error number: %d\n", command, errno);	
		fflush(stdout);		//clear standard out
		exit(1);	//exit with error
	}
	else{	//parent process
		current_pid = pid;	//set pid
		int status;	
		int temp1 = waitpid(current_pid, &status, 0);	//wait till child process is finished
		if(temp1 == -1){
			if(current_pid == -1){	//execution failed due to keyboard interrupt
			}
			else{	//otherwise the wait failed unexpectedly
				fprintf(stderr, "Wait for %d failed unexpectedly. Sorry dude.\n", current_pid);
				fflush(stdout);		//clear standard out
				exit(1);	//exit with error
			}
		}
		current_pid = -1;	//reset pid to -1
	}
}



void do_command_background(char *command, char *args[], struct input *current_input){
	pid_t pid = fork();	//fork off you child
	if(pid == -1){	//forking failure
		fprintf(stderr, "Fork for %s failed. errno: %d\n", command, errno);
		exit(1);	//exit with error
	}
	else if(pid == 0){	//child process
		execvp(command, args);	//execute command given
		fprintf(stderr, "Execvp (%s) failed. errno: %d\n", command, errno);	//add error to standerard error
		exit(1);	//exit with error
	}
	else{	//parent
		printf("background pid is %d\n", pid);	//show background pid
		fflush(stdout);		//clear standard out
		current_input->num_processes++;    //increase processes
	}
}




void if_interrupt(struct input *current_input){	// if ctrl-c
	signal(SIGINT, SIG_IGN);	//ignore ctrl-c
	char* new_line = "\n";
	write(STDOUT_FILENO, new_line, 1);	//make a new line	
	if(current_pid > 0){	//make sure there is atleast 1 process running
		int temp = kill(current_pid, SIGKILL);	//kill current process save results to temp
		if(temp == -1){	//if the assassination failed
			fprintf(stderr, "process %d failed to be killed", current_pid);//own up to it
		}
		else{	//otherwise wait for it to finish
			waitpid(current_pid, NULL, 0);
			current_pid = -1;	//reset pid
		}
	}
	//clear standard out
	fflush(stdout);	
}


void if_stop(){	// if ctrl-z
	
	if (foreground_only == 0){	//if current foreground only mode is off do this
		char* message = ("\nEntering foreground-only mode (& is now ignored)\n");
        write(STDOUT_FILENO, message, 50);	//let user know they are in foreground only mode
		foreground_only = 1;	//turn foreground only on
	}
	else{	//if current foreground only mode is on do this
		char* message = "\nExiting foreground-only mode\n";
        write(STDOUT_FILENO, message, 31);	//let user know no longer in foreground only mode
		foreground_only = 0;	//turn foreground only off
	}
	fflush(stdout);		//clear standard out
}





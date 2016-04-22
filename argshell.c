#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include <sys/wait.h>
#include <limits.h>
#include <fcntl.h>


#define CD_CWD 0
#define CD_PATH 1

#define WRITE 0
#define APPEND 1
#define S_OUT 2

char **     args;
int 		command_pos[30];
char 		buf[PATH_MAX+1];
int 		args_ptr;

void change_dir(char* path);
void single_command(int filenumber, int mode, char** command);
extern char ** get_args();




void read_file_input(char* filename){
	int i = 1;
	int dup_id = -1;
	int fp = open(filename,O_RDONLY);
	int size = 1;
	if(fp < 0){
	   perror(filename);
	   close(fp);
	}
	else{
		dup_id = dup(0);
		if(dup_id<0){
			close(dup_id);
		}
		char* string;
		while(size>0){
			size = read(fp,string,1);
			write(dup_id,string,size);
		}
		close(fp);
	}
}

void single_command(int filenumber, int mode,char** command){
	if(!strcmp (command[0], "cd")){
		change_dir(command[1]);
		return;
	}
	int result = 0;	
	int inc;
		pid_t pid = fork();
		if(pid == 0){
			if(filenumber < 0 || args[filenumber] == NULL){
				//printf("************************NULL**********************\n\n\n");
				//printf("************************NULL**********************\n");
				execvp(command[0],command);
			} else {
				printf("%s\t%d\n",args[filenumber],pid);
				int fd = open(args[filenumber],O_RDWR);
				close(1);
				dup(fd);
				execvp(command[0],command);
				close(fd);
				printf("Closed FD\n");
			}

		}
		wait(&result);
		if(result < 0) printf("The Error number is %d\n",result);
}

void multiple_commands(void){
	int i = 0;
	int write_flag = S_OUT;
	int args_ptr = 0;
	char* command[50];
	for (; args[args_ptr] != NULL; args_ptr++) {
		if(!strcmp (args[args_ptr], ";")){
			command[i] = NULL;
			single_command(-1,write_flag,command);
			i = 0;
		} else {
			command[i] = args[args_ptr];
			if(!strcmp (args[args_ptr], ">")){
				write_flag = WRITE;
				command[i] = NULL;
				single_command(args_ptr+1,write_flag,command);
			} else if(!strcmp (args[args_ptr], ">>")){
				write_flag = APPEND;
				command[i] = NULL;
				single_command(args_ptr+1,write_flag,command);
			} else {
				write_flag = S_OUT;
			}
			i++;
		}
	}
	command[i] = NULL;
	single_command(args_ptr+1,write_flag,command);
	
}


void change_dir(char* path){
	if(path == NULL){
		chdir(buf);
	}else{
		chdir(path);
	}
	
}

int main()
{
    int         i;
	//Get current directory for record
	getcwd(buf,PATH_MAX+1);
	
	//Enter the never ending while loop
    while (1) 
	{
	//Count of number of commands with ; 	
	//Shell prompt
	printf ("Command ('exit' to quit): ");
	//read from terminal and parse arguments
	args = get_args();
	
	//If there is a valid 

	if (args[0] == NULL) {
	    printf ("No arguments on line!\n");
	} else if(!strcmp(args[0], "|")){
		printf ("Syntax Error: \"|\" unexpected\n");
	} else if(!strcmp(args[0], ">")){
		printf ("Syntax Error: newline unexpected (expecting word)\n");
	} else if(!strcmp(args[0], "<")){
		printf ("Syntax Error: newline unexpected (expecting word)\n");
	} else if(!strcmp(args[0], ";")){
		printf ("Syntax Error: \";\" unexpected");
	} else if ( !strcmp(args[0], "exit")) {
	    printf ("Exiting...\n");
	    break;
	} else {
		multiple_commands();
	}
    }
}


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAXLINE 80
#define SEPARATORS " \n\t"

char * read_input(void){
	printf("ash>");
	fflush(stdout);
	char * input = malloc(sizeof(char) * MAXLINE);
	if(input == NULL){
		perror("ash: Error allocating memory!\n");
		exit(errno);
	}
	fgets(input, MAXLINE, stdin);
	return input;
}

char ** tokenize(char * input){
	int argc = 0;
	char ** tokens = malloc(sizeof(char *) * MAXLINE);
	if(tokens == NULL){
		perror("ash: Error allocating memory!\n");
		exit(errno);
	}
	char * token = strtok(input, SEPARATORS);
	while(token != NULL){
		tokens[argc++] = token;
		token = strtok(NULL, SEPARATORS);
	}
	tokens[argc] = NULL;
	return tokens;
}
int get_argc(char ** tokens){
	int argc;
	for(argc = 0; tokens[argc] != NULL; argc++);
	return argc;
}

int execute(char ** tokens){
	int should_wait = 1;
	int status;
	int argc = get_argc(tokens);
	if(strcmp(tokens[argc-1], "&") == 0){
		should_wait = 0;
		tokens[argc-1] = NULL;
	} else {
		should_wait = 1;
	}
	if(strcmp(tokens[0], "cd") == 0){
		chdir(tokens[1]);
	} else if(strcmp(tokens[0], "clear") == 0){
		printf("\x1b[2J"); //clears the screen
		printf("\x1b[H"); //moves the cursor to home
	} else if(strcmp(tokens[0], "exit") == 0){
		//should_run = 0;
	} else if(fork() == 0){
		execvp(tokens[0], tokens);
		_exit(0);
	} else {
		if(should_wait){
			wait(&status);
		}
	}
	return 0;
}

int main(void){
	char * user_input;
	char ** tokens;
	int should_run = 1;
	int status;

	while(should_run){
		user_input = read_input();	
		tokens = tokenize(user_input);
		status = execute(tokens);

		free(tokens);
		tokens = NULL;
		free(user_input);
		user_input = NULL;
	}
	return 0;
}

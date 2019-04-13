#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAXLINE 80

int main(void){
	char * args[MAXLINE/2 + 1];
	char * token;
	char user_input[MAXLINE];
	int should_run = 1;
	int should_wait;
	int argc = 0;
	int status;

	while(should_run){
		printf("ash>");
		fflush(stdout);
		fgets(user_input, MAXLINE, stdin);
		token = strtok(user_input, " \n\t");
		while(token != NULL){
			args[argc] = (char *) malloc(sizeof(char) * strlen(token));
			strcpy(args[argc++], token);
			token = strtok(NULL, " \n\t");
		}
		args[argc] = NULL;
		if(strcmp(args[argc-1], "&") == 0){
			should_wait = 1;
		} else {
			should_wait = 0;
		}
		if(strcmp(args[0], "cd") == 0){
			chdir(args[1]);
		} else if(strcmp(args[0], "exit") == 0){
			should_run = 0;
		} else if(fork() == 0){
			execvp(args[0], args);
			_exit(0);
		} else {
			if(should_wait){
				wait(&status);
			}
		}
		
		for(int i = 0; i < argc; ++i){
			memset(args[i], 0, sizeof(args[i]));
			free(args[i]);
			args[i] = NULL;
		}
		argc = 0;
	}
	return 0;
}

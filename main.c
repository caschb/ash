#include <stdio.h> //fgets, printf, perror, fflush
#include <stdlib.h> //malloc, free, exit
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAXLINE 80
#define SEPARATORS " \n\t"

int should_run;


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

int exec_cd(char ** tokens){
        int result;
        if(tokens[1] != NULL){
                result = chdir(tokens[1]);
        }
        return chdir(tokens[1]);
}

int exec_clear(char ** tokens){
        printf("\x1b[2J"); //clears the screen
        printf("\x1b[H"); //moves the cursor to home
        return 0;
}

int exec_exit(char ** tokens){
        should_run = 0;
        return 0;
}

int built_ins = 3;
char * built_in_names[] = {"cd", "clear", "\x0c", "exit"};
int (*built_in[])(char ** tokens) = {exec_cd, exec_clear, exec_clear, exec_exit};

int execute(char ** tokens){
        int should_wait = 1;
        int status;
        int argc = get_argc(tokens);
        if(strcmp(tokens[argc-1], "&") == 0){
                should_wait = 0;
                tokens[argc-1] = NULL;
        } 
        int built_in_index = -1;
        for(int i = 0; i < built_ins && built_in_index == -1; ++i){
                if(strcmp(tokens[0], built_in_names[i]) == 0){
                        built_in_index = i;
                }
        }

        if(built_in_index != -1){
                built_in[built_in_index](tokens);
        } else if(fork() == 0){
                execvp(tokens[0], tokens);
                if(errno == ENOEXEC){
                        perror("ash: Not an executable\n");
                }
                else if(errno == ENOENT){
                        perror("ash: Executable doesn't exist\n");
                }
                _exit(0);
        } else {
                if(should_wait){
                        wait(&status);
                }
        }
        return 0;
}

void handle_clear_signal(int signum){
        exec_clear(NULL);
}

int main(void){
        char * user_input;
        char ** tokens;
        int status;
        should_run = 1;

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

#include "LineParser.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <linux/limits.h>
#include <sys/stat.h>
#include <sys/fcntl.h>


#define BUFFER_SIZE 2048
#define HISTLEN 20
char const *pathName;

void execute(cmdLine* pCmdLine, int debug, char **history);

void printPath(){
    char buff[PATH_MAX];
    getcwd(buff, PATH_MAX);
    if (buff == NULL){
        perror("ERANGE err\n");
    }
    printf("Path: %s\n", buff);
}

void addToHistory(char **history, char *cmd){
    if (history[HISTLEN-1] != NULL) {
            free(history[HISTLEN-1]);
    }
    for (int i = HISTLEN-1 ; i > 0 ; i--){
        history[i] = history[i-1];
    }
    history[0] = cmd;
}

void freeHistory(char **history){
    for (int i = 0 ; i < HISTLEN ; i++){
        free(history[i]);
    }
    free(history);
}

/* check for special cmd's*/
int no_process_cmd(cmdLine* command, char **history, int debug){ 
    int nnp = 0;
    if(strcmp(command->arguments[0],"quit")==0){
        freeHistory(history);
        freeCmdLines(command);
        exit(EXIT_SUCCESS);
    }
    if(strcmp(command->arguments[0],"cd")==0){
        nnp = 1;
        if(chdir(command->arguments[1])<0){
            perror("Bad cd command");
        }
    }
    if(strcmp(command->arguments[0],"history")==0){
        nnp = 1;
        for(int i=0; i<HISTLEN; i++){
            if(history[i] != NULL){
                printf("%d: %s", i + 1, history[i]);
            }
        }
    }
    if(strncmp(command->arguments[0],"!",1) == 0){
        nnp = 1;
        const char * ch = &command->arguments[0][1];
        int n;
        if(*ch == '!'){
            n = 0;
        } else {
            n = atoi(ch)-1;
        }
        if (n >= HISTLEN || history[n] == NULL){
            fprintf(stderr,"Number out of bounds");
        } else {
            cmdLine *newCmd = parseCmdLines(history[n]);
            execute(newCmd, debug, history);
            char *cmdcopy = (char*)malloc(strlen(history[0])+1);
            strcpy(cmdcopy, history[0]);
            addToHistory(history, cmdcopy);
            freeCmdLines(newCmd);
        }
    }
    return nnp;
}

void inputRed(cmdLine* pCmdLine, int* input){
    *input = open(pCmdLine->inputRedirect, O_RDONLY);
    if(*input < 0 || dup2(*input, 0) < 0){
        perror("Input file open/dup err");
        free(pCmdLine);
        exit(EXIT_FAILURE);
    }

}

void outputRed(cmdLine* pCmdLine, int* output){
    (*output) = open(pCmdLine->outputRedirect, O_WRONLY | O_CREAT);
    if (*output < 0 || dup2(*output, 1) < 0){
        perror("Dup input redirect error");
        free(pCmdLine);
        exit(EXIT_FAILURE);
    }
}

void exeCmd(cmdLine* pCmdLine){
    if(execvp(pCmdLine->arguments[0], pCmdLine->arguments)<0){
        perror("Cannot execute command");
        freeCmdLines(pCmdLine);
        _exit(EXIT_FAILURE);
    }
}

void exeTwoProc(cmdLine* pCmdLine, int debug, char **history){
    int output, input;
    pid_t cpid1, cpid2;
    int fd[2];
    if (pipe(fd)<0){
        perror("Pipe err");
        exit(EXIT_FAILURE);
    }
    if((cpid1 = fork()) == 0){ // in first child process, same as in task 2
        if(pCmdLine->inputRedirect){ // in writing side of pipe there can be only input redirect
            inputRed(pCmdLine,&input);
        }
        close(STDOUT_FILENO);
        dup(fd[1]);
        close(fd[1]);
        exeCmd(pCmdLine);
        close(input);
        exit(EXIT_SUCCESS);
    } else {   /* in parent process -> creating second child  */
        if (cpid1 > 0){
            close(fd[1]);
            /*  in 2nd child */
            if((cpid2 = fork()) == 0){//second child process, writing side of pipe
                if(pCmdLine->next->outputRedirect){// in writing side of pipe there can be only output redirect
                    outputRed(pCmdLine->next, &output);
                }
                close(STDIN_FILENO);
                dup(fd[0]);
                close(fd[0]);
                if(execvp(pCmdLine->next->arguments[0], pCmdLine->next->arguments)<0){
                    perror("Cannot execute command");
                    freeCmdLines(pCmdLine);
                    _exit(EXIT_FAILURE);
                }
                close(output);
                exit(EXIT_SUCCESS);
            } else { /* in parent process again */
                if(cpid2 > 0) {
                    close(fd[0]);
                    waitpid(cpid1,NULL,0);
                    waitpid(cpid2,NULL,0);
                }
            }
        }
    }
}


void exeOneProc(cmdLine* pCmdLine, int debug, char **history){
    int output, input;
    pid_t cpid1;
    if((cpid1 = fork()) == 0){ // in child process
        if(pCmdLine->inputRedirect){
            inputRed(pCmdLine,&input);
        }
        if(pCmdLine->outputRedirect){
            outputRed(pCmdLine,&output);
        }
        if(debug){
            fprintf(stderr, "PID: %d\nExecuting command: %s\n",getpid(), pCmdLine->arguments[0]);
        }
        exeCmd(pCmdLine);
        close(output);
        close(input);
        exit(EXIT_SUCCESS);
    } else {   // in parent process
        if (cpid1 > 0){
            if(debug){
                fprintf(stderr, "PID: %d\nExecuting command: %s\n",cpid1, pCmdLine->arguments[0]);
            }
            if(pCmdLine->blocking){
                waitpid(cpid1,NULL,0);
            }
        }
    } // end of fork 
}

void execute(cmdLine* pCmdLine, int debug, char **history){
    if(!no_process_cmd(pCmdLine, history, debug)){
        if (pCmdLine->next != NULL){ /* if pipe needed */ 
            exeTwoProc(pCmdLine, debug, history);
        } else { /* no pipe needed */
            exeOneProc(pCmdLine, debug, history);
        }
    }
}


int main(int argc, char **argv){
    char **history = (char**)malloc(HISTLEN*sizeof(char*));
    int debug = 0;
    for (int i = 0 ; i < argc ; i++){
        if (strcmp("-d", argv[i]) == 0){
            debug = 1;
        }
    }
    cmdLine *pCmdLine ;
    char buff[BUFFER_SIZE] ;
    while (1){
        printPath();
        fgets(buff, BUFFER_SIZE, stdin);
        char *cmdcopy = (char*)malloc(strlen(buff)+1);
        strcpy(cmdcopy, buff);
        pCmdLine = parseCmdLines(buff);
        if (pCmdLine == NULL){
            break;
        }
        if (strncmp(pCmdLine->arguments[0],"!",1) != 0){
            addToHistory(history, cmdcopy);
        } else {
            free(cmdcopy);
        }
        if (debug){
            fprintf(stderr, "parent PID: %d\n",getpid());
        }
        execute(pCmdLine, debug, history);
        freeCmdLines(pCmdLine);
    }
    return 0;
}

#include "LineParser.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <linux/limits.h>
#define BUFFER_SIZE 2048

char const *pathName;

void printPath(){
    char buff[PATH_MAX];
    getcwd(buff, PATH_MAX);
    if (buff == NULL){
        perror("ERANGE err\n");
    }
    printf("Path: %s\n", buff);
}

int no_process_cmd(cmdLine* command){   // if nnp - command does not require a process. signals are nnp (dont initiate new process)
    int nnp = 0;
    if(strcmp(command->arguments[0],"quit")==0){
        exit(EXIT_SUCCESS);
    }
    if(strcmp(command->arguments[0],"cd")==0){
        nnp = 1;
        if(chdir(command->arguments[1])<0){
            perror("Bad cd command");
        }
    }
    return nnp;
}

void execute(cmdLine* pCmdLine, int debug){
    if(!no_process_cmd(pCmdLine)){
        pid_t cpid;
        // fork
        if((cpid = fork()) == 0){ // in child process
            // set input \ out put and all that shit !! 
            if(debug){
                fprintf(stderr, "PID: %d\nExecuting command: %s\n",getpid(), pCmdLine->arguments[0]);
            }
            if(execvp(pCmdLine->arguments[0],pCmdLine->arguments)<0){
                perror("Cannot execute command");
                _exit(EXIT_FAILURE);  // dont wait for process to terminate proparly
            }
            exit(EXIT_SUCCESS);
        } else {   // in parent process
            if (cpid > 0){
                if(pCmdLine->blocking){
                    waitpid(cpid,NULL,0);
                }
            }
        } // end of fork 
    }
}


int main(int argc, char **argv){
    int debug = 0;
    for (int i = 0 ; i < argc ; i++){
        if (strcmp("-d", argv[i]) == 0){
            debug = 1;
        }
    }
    cmdLine *pCmdLine ;
    char buff[BUFFER_SIZE] ;
    while (1)
    {
        printPath();
        fgets(buff, BUFFER_SIZE, stdin);
        pCmdLine = parseCmdLines(buff);
        if (pCmdLine == NULL){
            break;
        }
        if (debug){
            fprintf(stderr, "parent PID: %d\n",getpid());
        }
        execute(pCmdLine, debug);
        freeCmdLines(pCmdLine);
    }
    return 0;
}

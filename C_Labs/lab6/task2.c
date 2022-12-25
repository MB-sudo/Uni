#include "LineParser.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <error.h>
#include <sys/signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <linux/limits.h>

#define BUFFER_SIZE 2048

#define RUNNING 1
#define SUSPENDED 0
#define TERMINATED -1

char const *pathName;

typedef struct process{
    cmdLine* cmd;                   /* the parsed command line*/
    pid_t pid; 		                /* the process id that is running the command*/
    int status;                     /* status of the process: RUNNING/SUSPENDED/TERMINATED */
    struct process *next;	        /* next process in chain */
} process;

process* GPL; // head of process list

char* Stat_string(int status){
    switch (status)
    {
    case SUSPENDED:
        return "Suspeneded";
    case TERMINATED:
        return "Terminated";
    case RUNNING:
        return "Runnning";
    default:
        return "Running";
    }
}

void printProcess(process* process){
    char command[BUFFER_SIZE] = "";
    int c_count = process->cmd->argCount;
    if( c_count > 0){
        for(int i=0;i < c_count;i++){
            const char* argument = process->cmd->arguments[i];
            strcat(command,argument);
            strcat(command," ");
        }
    }
    printf("%d\t\t%s\t%s\n",process->pid,command,Stat_string(process->status));
}

void freeProcess(process* proc){
    freeCmdLines(proc->cmd);
    proc->cmd = NULL;
    proc->next = NULL;
    free(proc);
}

/* ----------------- list funtions --------------- */

void freeProcessList(process* process_list){
    process* curr = process_list;
    while(curr != NULL){
        process* next = curr->next;
        freeProcess(curr);
        curr = next;
    }
}

void print_pl(process* process_list){
    process* curr = process_list;
    while(curr != NULL){
        printProcess(curr);
        curr=curr->next;
    }
}

void addProcess(process** process_list, cmdLine* cmd, pid_t pid){
    process* curr = *process_list;
    process* prev = NULL;
    while (curr != NULL) {
        prev = curr;
        curr = curr->next;
    }
    curr = malloc(sizeof(struct process));
    curr->cmd = cmd;
    curr->pid = pid;
    curr->status = RUNNING;
    curr->next = NULL;
    if (prev == NULL){
        (*process_list) = curr;
    } else {
        prev->next = curr;
    }
}

/* changes status of process via its pointer */
void updateProcessStatus(process* process_list, int pid, int status){
    int w_status=RUNNING;
    if(WIFEXITED(status) || WIFSIGNALED(status)) {
        w_status=TERMINATED;
    }
    if(WIFSTOPPED(status)) { 
        w_status=SUSPENDED;
    }
    process_list->status = w_status;
}

void updateProcessList(process **process_list){
    process* curr_process = (*process_list);
    while(curr_process!=NULL){
        int status;
        pid_t pid = curr_process->pid;
        if (waitpid(pid,&status,WNOHANG | WUNTRACED | WCONTINUED)!=0){    // if status is either exited, stopped or cont 
            updateProcessStatus(curr_process,pid,status);
        }
        curr_process=curr_process->next;
    }
}

void deleteTerminatedProcesses(process* process_list){
    process* curr = process_list;
    process* prev = NULL;
    while (curr != NULL){
        if (curr->status == TERMINATED){
            process* next = curr->next;
            freeProcess(curr);
            if(prev == NULL) {  // if we're at the head of the list
                GPL = next;    // overwrite head of list
            } else { 
                prev->next = next;
            }
            curr = next;
        } else { prev = curr; curr = curr->next; }  // if not terminated just move to the next one
    }
}

void printProcessList(process** process_list){
    updateProcessList(process_list);
    printf("PID\t\tCommand\t\tSTATUS\n");
    print_pl(*process_list);
    deleteTerminatedProcesses(*process_list);
}

/* --------------------- task1 func -------------------------- */

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
        freeProcessList(GPL);
        freeCmdLines(command);
        exit(EXIT_SUCCESS);
    }
    if(strcmp(command->arguments[0],"cd")==0){
        nnp = 1;
        if(chdir(command->arguments[1])<0){
            perror("Bad cd command");
        }
    }
    if(strcmp(command->arguments[0],"procs")==0){
        nnp = 1;
        printProcessList(&GPL);
    }
    // --> task c : 
    if(strcmp(command->arguments[0],"kill")==0){
        nnp=1;
        int pid = atoi(command->arguments[1]);
        if(kill(pid,SIGINT)==-1){
            perror("Can't kill process");
        } else{
            printf("%d handling SIGINT\n",pid);
        }
    }
    if(strcmp(command->arguments[0],"suspend")==0){
        nnp = 1;
        int pid = atoi(command->arguments[1]);
        if(kill(pid, SIGTSTP) == -1){
            perror("Can't suspend process");
        }  else {
          printf("%d handling SIGTSTP\n",pid);
        }
    }
    if(strcmp(command->arguments[0],"wake")==0){
        nnp = 1;
        int pid = atoi(command->arguments[1]);
        if(kill(pid, SIGCONT) == -1){
            perror("Can't suspend process");
        }  else {
            printf("%d handling SIGCONT\n",pid);
        }
    }
    if(nnp == 1){
        freeCmdLines(command);
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
                addProcess(&GPL,pCmdLine,cpid);
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
    GPL = NULL;
    cmdLine *pCmdLine ;
    char buff[BUFFER_SIZE] ;
    while (1) {
        printPath();
        fgets(buff, BUFFER_SIZE, stdin);
        pCmdLine = parseCmdLines(buff);
        if (pCmdLine == NULL){
            break;
        }
        execute(pCmdLine, debug);
    }
    return 0;
}

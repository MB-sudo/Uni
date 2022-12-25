#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

int main(int argc, char** argv) {
    int debug=0;
    
    for (int i=0; i< argc; i++){
        if (strncmp(argv[i],"-d",2)==0||strncmp(argv[i],"-D",2) == 0)
            debug =1;
    }

    pid_t cpid1, cpid2;

    /* commands as string */
    char* const ls[3] = {"ls","-l",0};
    char* const tail[4] = {"tail","-n","2",0};
    int fd[2];
    if (pipe(fd)==-1){
      perror(strerror(errno));
      _exit(EXIT_FAILURE);
    }

    if (debug){ printf("parent_process > forking...\n");}
    if ((cpid1 = fork()) == 0){
        if(debug){
            fprintf(stderr,"child1 > redirecting stdout to the write end of the pipe...\n");
        }
        /* instruction for child 1: */
        close(STDOUT_FILENO);
        dup2(fd[1],STDOUT_FILENO);
        close(fd[1]);
        if(debug){
            fprintf(stderr,"child1 > going to execute cmd: ls -l \n");
        }
        execvp(ls[0] , ls); // no error throwing, the command is set 
    } else { /* in parent, initialize child 2 */
        if(debug){
            fprintf(stderr, "parent_process > created process with id: %d\n", cpid1);
            fprintf(stderr,"parent_process > closing the write end of the pipe...\n");
        }
        /* instruction for parent: */
        close(fd[1]); /* commenting step 4 will leave us waiting - process reading will wait for '\0' */
        if(debug){
            fprintf(stderr, "parent_process > forking...\n");
        }
        /* second child */
        if((cpid2 = fork()) == 0){
            if (debug){
                fprintf(stderr,"child2 > redirecting stdin to the read end of the pipe...\n");
            }
            /* instruction for child 2: */
            close(STDIN_FILENO);
            dup2(fd[0],STDIN_FILENO);
            close(fd[0]);
            if(debug){
                fprintf(stderr,"child2 > going to execute cmd: tail -n 2 \n");
            }
            execvp(tail[0],tail);
        } else { /* again in parent process */
            if(debug) {
                fprintf(stderr, "parent_process > created process with id: %d\n", cpid2);
                fprintf(stderr,"parent_process > closing the read end of the pipe...\n");
            }
            close(fd[0]); /* commenting this out doesnt change much (child already read, no more reading)*/
            if(debug) {
                fprintf(stderr,"parent_process > waiting for child processes to terminate...\n");
            }
            waitpid(cpid1,NULL,0);
            if(debug) {fprintf(stderr,"parent_process > waiting for child processes to terminate...\n");
            }
            waitpid(cpid2,NULL,0);
            /* if wait is commented out program will finish before execution (child 2 still waiting for input (\0))*/
        }
      }
      if(debug){
        fprintf(stderr,"parent_process > closing the read end of the pipe\n");
        fprintf(stderr,"parent_process > exiting...\n");
      }
    return 0;
}

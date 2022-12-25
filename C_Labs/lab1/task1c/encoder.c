#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void echo(int debug){
    int c;
    while ((c=fgetc(stdin))!= EOF) {  
        if (c == '\n') {
            fprintf(stdout, "\n");
            continue;
        }
        if(debug) {fprintf(stderr,"%x\t%x\n", c, c);}
        fprintf(stdout , "%c", c);
        } 
    if (c == EOF){
        return;
    }

}

void upper_case(int debug){
    int c;
    while ((c=fgetc(stdin))!= EOF) {  
    if(c >= 97 && c <= 122){
        if(debug) {fprintf(stderr,"%x\t%x\n", c, c-32);}
        fprintf(stdout, "%c", (c-32));
        } else {
            /* to continue without exit signal and drop line*/
            if (c == '\n') {
                fprintf(stdout, "\n");
                continue;
            }
            /* ----- */
            if(debug) {fprintf(stderr,"%x\t%x\n", c, c);}
            fprintf(stdout , "%c", c);
        } 
    }
    if (c == EOF){
        return;
    }
}

void encrypt(int spin, char key[] , int size, int debug){
    int c;
    int lap = 0;

    /*check if there are no arguments to encryption*/
    if (strcmp(key, "") == 0){
        echo(debug);
        return;
    }
    while ((c=fgetc(stdin))!= EOF) { 
        if(lap == size){
            lap = 0;
        }
        if (c == '\n'){
            fprintf(stdout, "\n");
            lap = 0;
            continue;
        }
        int act = (spin)*(key[lap]) + (-spin)*48;
        /*circular - basic ascii only delete doesnt count it doesnt have a char*/
        if (c == 32 && spin == -1){
            act+=94;
        }
        if (c == 126 && spin == 1){
            fprintf(stderr, "%c", c);
            act-=94;
        }
        /*-----*/
        if(debug) {fprintf(stderr,"%x\t%x\n", c, c + act);}
            fprintf(stdout, "%c", (char)(c + act));
        lap++;
    }
    if (c == EOF){
        return;
    }

}

int main(int argc, char *argv[])
{   
    int keySize;
    int debug = 0;
    int flag = 0;
    char *key;
    int i;
    for( i =1; i<argc; i++){
        if(strcmp("-D", argv[i]) == 0){
            printf("%s\n" ,argv[i]);
            debug = 1;
        }
        if (strncmp("-e", argv[i], 2) == 0 || strncmp("+e", argv[i], 2) == 0) {
            if (argv[i][0] == '-'){
                flag = 1;
            } else {
                flag = 2;
            }
            keySize = strlen(argv[i]) - 2;
            key = argv[i]+2;
        }
    }

    switch (flag) {
        case 0 :
            upper_case(debug);
            break;
        case 1 :
            encrypt(-1, key , keySize, debug);
            break;
        case 2 :
            encrypt(1, key , keySize, debug);
            break;
        default :
            break;
    }
    return 0;
}

    
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void echo(int debug, FILE *in){
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

void upper_case(int debug, FILE *in){
    int c;
    while ((c=fgetc(in))!= EOF) {  
    if(c >= 97 && c <= 122){
        if(debug) {fprintf(stderr,"%x\t%x\n", c, c-32);}
        fprintf(stdout, "%c", (c-32));
        } else {
            if (c == '\n') {
                fprintf(stdout, "\n");
                continue;
            }
            if(debug) {fprintf(stderr,"%x\t%x\n", c, c);}
            fprintf(stdout , "%c", c);
        } 
    }
    if (c == EOF){
        return;
    }
}

void encrypt(int spin, char key[] , int size, int debug, FILE *in){
    int c;
    int lap = 0;
    if (strcmp(key, "") == 0){
        echo(debug, in);
        return;
    }
    while ((c=fgetc(in))!= EOF) { 
        if(lap == size){
            lap = 0;
        }
        if (c == '\n'){
            fprintf(stdout, "\n");
            lap = 0;
            continue;
        }
        int act = (spin)*(key[lap]) + (-spin)*48;
        /*circular - basic ascii only*/
        if (c == 32 && spin == -1){
            act+= 94;
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
        fprintf(stdout, "\n");
        return;
    }

}

int main(int argc, char *argv[])
{   
    FILE * out = stdout;
    FILE * in = stdin;
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
        if(strncmp("-i", argv[i], 2) == 0){
            in = fopen(argv[i] + 2, "r");
            if(in == NULL){
                fprintf(stderr, "can't open file - %s", argv[i] + 2);
                return 1;
            }
        }
    }

    switch (flag) {
        case 0 :
            upper_case(debug, in);
            break;
        case 1 :
            encrypt(-1, key , keySize, debug, in);
            break;
        case 2 :
            encrypt(1, key , keySize, debug, in);
            break;
        default :
            break;
    }
    return 0;
}
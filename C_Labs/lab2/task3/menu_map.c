#include <stdlib.h>
#include <stdio.h>
#include <string.h>
 
char censor(char c) {
  if(c == '!')
    return '.';
  else
    return c;
}
 
char* map(char *array, int array_length, char (*f) (char)){
  char* mapped_array = (char*)(malloc(array_length*sizeof(char)));
    for (int i = 0; i < array_length ; i++){
      mapped_array[i] = (char)f(array[i]);
    }
    /* TODO: Complete during task 2.a */
  return mapped_array;
}

// what about end of line ? menu ?
char my_get(char c){
  return fgetc(stdin);
}

char cprt(char c){
  if( c >= 0X20 && c <= 0X7E ){
    printf("%c\n", c);
  } else {
    printf(".\n");
  }
  return c;
}

char encrypt(char c){
  if( c >= 0X20 && c <= 0X7E){
    return c+3;
  } else {
    return c;
  }
}

char decrypt(char c){
  if( c >= 0X20 && c <= 0X7E ){
    return c-3;
  } else {
    return c;
  }
}

char xprt(char c){
  if( c >= 0X20 && c <= 0X7E ){
    printf("%x\n", c);
  } else {
    printf(".\n");
  }
  return c;
}

char quit(char c){
  if (c == 'q'){
    exit(0);
  } else {
    return c;
  }
}

typedef struct 
{
  char *name;
  char (*fun)(char);
} fun_desc;


int main(int argc, char **argv){

  char *carray = (char*)malloc(5*sizeof(char));
  fun_desc menu[] = { { "0) Get string", &my_get }, { "1) Print string", &cprt }, { "2) Print Hex" , &xprt }, { "3) Censor" , &censor }, { "4) Encrypt" , &encrypt}, { "5) Decrypt" , &decrypt} , { "6) Quit" , quit} , { NULL, NULL} };
  int option = -1;

  do{
    /* print menu */
    printf("Please choose a function:\n");
    int i = 0 ;
    while (i < 7){
      char *p = menu[i].name;
      while (*p != '\0'){ fprintf(stdout, "%c", (char)*p++ ); }
      fprintf(stdout, "\n");
      i++;
    }
    /* user input*/
    char end;
    printf("Option : ");
    scanf("%d", &option);
    end = getc(stdin);
    if ( end == '\n' && option >= 0 && option < 7){
      printf("Whithin bounds\n");
    } else {
      printf("Out of bounds\n\n");
      continue;
    }

    /* send to fun of choice*/
    carray = map(carray, 5, menu[option].fun);
    printf("DONE!\n\n");

  } while ( option != -1);

  // when should free be called?
  free(carray);
  exit(1);
  
}


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
/* Gets a char c, and if the char is 'q' , ends the program with exit code 0. Otherwise returns c. */



 
int main(int argc, char **argv){
  int base_len = 5;
  char arr1[base_len];
  char* arr2 = map(arr1, base_len, my_get);
  char* arr3 = map(arr2, base_len, cprt);
  char* arr4 = map(arr3, base_len, xprt);
  char* arr5 = map(arr4, base_len, encrypt);
  char* arr6 = map(arr5, base_len, decrypt);
  free(arr2);
  free(arr3);
  free(arr4);
  free(arr5);
  free(arr6);

  /* TODO: Test your code */
}


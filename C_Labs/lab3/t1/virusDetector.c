#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct virus {
   unsigned short SigSize;
   char virusName[16];
   unsigned char* sig;
} virus ;


typedef struct virusDetector{
  char *name;
  void (*fun)(void);
} virusDetector ;

struct link {
   struct link *nextVirus;
   struct virus *vir;
} ;

static FILE *f;

static struct link* list_head;



/* ------------------------- virus -------------------------- */

virus* readVirus(FILE* input){
   virus *mal = malloc(sizeof(virus));
   if (mal == NULL){
      printf("allocation error - readVirus\n");
      exit(-1);
   }
   fread(mal, 1, 18, input);
   int size = mal->SigSize;
   mal->sig = malloc(size);
   fread(mal->sig, 1, size, input);
   return mal;
   /* this allocates a virus and a signatue that needs to be freed.*/
}

void printVirus(virus* virus, FILE* output){
   int size = virus->SigSize;
   fprintf(output, "Name: %s\nSignature length: %d\nSig: ", virus->virusName,size);
   unsigned char *signature = virus->sig;
   for (int i = 0 ; i < size ; i++){
      fprintf(output, "%02X ",signature[i]);
   }
   fprintf(output, "\n\n");
}

/* -------------------- linked list ---------------------------*/


void list_print(struct link *virus_list, FILE* out){
   struct link *curr = virus_list;
   while (curr != NULL){ 
      printVirus(curr->vir, out);
      curr = curr->nextVirus;
   }
   /* Print the data of every link in list to the given stream. Each item followed by a newline character. */
}

struct link* list_append(struct link* virus_list, virus* data){
   
   if (virus_list == NULL){
      virus_list = (struct link*)malloc(sizeof(struct link));
      virus_list->vir = data; virus_list->nextVirus = NULL;
   } else {
      struct link *curr = virus_list;
      struct link *prev;
      while (curr != NULL) {
         prev = curr;
         curr = curr->nextVirus;
      }
      curr = (struct link*)malloc(sizeof(struct link));
      curr->vir = data; curr->nextVirus =NULL;
      prev->nextVirus = curr;
   }
   return virus_list;
}

void list_free(struct link *virus_list){
   struct link * curr = virus_list;
   while(curr != NULL){
      struct link *next = curr->nextVirus;
      free(curr->vir->sig); free(curr->vir); free(curr);
      curr = next;
   }
   /* Free the memory allocated by the list. */
}

/* --------------------  menu function -------------------------*/

void quit(){
   list_free(list_head);
}

/* -------------------- helper functiond -----------------------*/

FILE* getFilePointer(char *mode){
   printf("Please enter file name: ");
   char *buffer = (char*)malloc(sizeof(char)*50);
   fgets(buffer, 50, stdin);
   char src[strlen(buffer)-1];
   sscanf(buffer, "%s", src);
   free(buffer);
   FILE *input = fopen(src,mode);
   if( input == NULL ){
      printf("Error opening file\n\n");
      return NULL;
   }
   return input;
}

int getFileSize(FILE* input){
   fseek(input, 0, SEEK_END);
   int len = ftell(input);
   fseek(input, 0, SEEK_SET);
   return len;
}

int getIntVal(){
   int val;
   int result = fscanf(stdin, "%d", &val);
   if (result == EOF){
      quit();
   }
   if (result == 1) {
      getc(stdin);
      return val;
   } else {
      int nl;
      do {
         nl = fgetc(stdin);
      } while(nl != '\n');
      printf("invalid input, please try again: ");
      return getIntVal();
   }
}

/* -------------------- menu functions --------------------- */

void load(){

   FILE *input = getFilePointer("r");
   if(input == NULL){
      return;
   }
   /* handling */
   int len = getFileSize(input);
   char dump[4];
   fread(dump, 1, 4, input);
   int indx = ftell(input);
   while(indx < len){
      list_head = list_append(list_head, readVirus(input));
      indx = ftell(input);
   }
   fclose(input);
   printf("Loaded.\n\n");
}

void print(){
   FILE *out = stdout;
   int action = 0;
   do{
      printf("Choose option:\n1) Print to screen\n2) Print to file\n");
      printf("Option : ");
      action = getIntVal();

   } while (action != 1 && action != 2);
   if(action == 2){
      out = getFilePointer("w");
      if(out == NULL){
         return;
      }
   } 
   list_print(list_head, out);
   if(action == 2){
      fclose(out);
   }
}


void detect_virus(char *buffer, unsigned int size, struct link *virus_list){
   struct link* curr = virus_list;
   char *loc = buffer;
   while(curr != NULL){
      for(int i = 0 ; i < size; i++){
         if (memcmp((char*)(loc+i), curr->vir->sig, curr->vir->SigSize)==0){
            printf("\nVirus detected!\nLocation: %d's byte\nName: %s\nSignature size: %d\n", i, curr->vir->virusName, curr->vir->SigSize);
         }
      }
      curr = curr->nextVirus;
   }
   printf("\n");

}

void detect(){
   int len = getFileSize(f);
   char bytes[10000];
   fread(bytes, 1, len, f);
   detect_virus(bytes, len , list_head);
}


void fix(){
   printf("Not yes implemented\n\n");
}





/* -------------------- main ----------------------------*/


int main(int argc, char **argv){

   if(argc > 1){
      f = fopen(argv[1], "r+");
      if (f == NULL){
         printf("Failed to open file\n");
         exit(1);
      }
   }


   virusDetector command[] = { { "1) Load Signatures", &load }, { "2) Print signatures", &print }, { "3) Detect viruses" , &detect }, { "4) Fix file" , &fix } , { "5) Quit" , &quit} , { NULL, NULL} };
   int action = 6;
   list_head = NULL;


   do{
      printf("Please choose a function:\n");
      int i = 0 ;
      while (i < 5){
         char *p = command[i].name;
         while (*p != '\0' && *p != '\n'){ fprintf(stdout, "%c", (char)*p++); }
         fprintf(stdout, "\n");
         i++;
      }
      /* user input*/
      printf("Option : ");
      action = getIntVal();
      while(action < 0 || action > 5){
         printf("invalid input, please try again: ");
         action = getIntVal();
      } 
      command[action-1].fun();

   } while (action != 5);
   
   if(argc == 2){ 
      fclose(f);
   }   
}
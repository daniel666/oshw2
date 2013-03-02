#define _GNU_SOURCE
#include<unistd.h>
#include<sys/syscall.h>
#include "hashtable.h"
#include<sys/types.h>
#include<sys/stat.h>
#include<dirent.h>
#include<stdio.h>
#include<stdbool.h>
#include<string.h>

#define SET_COLORS 223
void get_pids_cmd(char* cmd[], pid_t pids[], int size);
bool isDir(struct dirent* dent);
char* contain_name(char* line);
char* contain_pid(char* line);

static int DEBUG = 1;
int main(int argc, char* argv[])
{
   int size = --argc/2;

   char* cmd[size];
   pid_t pids[size];
   u_int16_t colors[size];
   int retval[size];
   int i;
   /*
    *this is only for testing
    */
   //cmd[0] = "bash"; cmd[1] = "vim";
   //cmd[3] = "chromium-browse"; cmd[2] = "dhclient";

   if(!size%2){
      printf("Please input in pair cmd and color\n");
      exit(1);
   }


   for(i=0;i<size;i++){
         cmd[i] = argv[2*i+1];
         colors[i] = *argv[2*i+2];
   }

   get_pids_cmd(cmd, pids, size);
   syscall(SET_COLORS, size, cmd, colors, retval);
   for(i = 0; i<size; i++)
   printf("%d ",retval[i]);

   return 0;
}


void get_pids_cmd(char* cmd[], pid_t pids[], int size)
{
      HASHTBL *hashtbl;
      hash_size hashsize = 1000;
      DIR *dir;
      char* proc = "/proc";
      struct dirent* dent;

      if(!(hashtbl = hashtbl_create(hashsize,NULL))){
            fprintf(stderr, "ERROR: hashtbl_create() failed\n");
            exit(EXIT_FAILURE);
      }

      dir = opendir(proc);
      if(!dir){
         printf("%s doesn't exist\n", proc);
         exit(-1);
      }

      while(dent=readdir(dir)){
            if(isDir(dent)){
                  //char status_file[100];
                  char cmdline_file[100];
                  FILE* fh;
                  bool cflag=0;
                  bool pflag=0;
                  if(!strcmp(dent->d_name,".") || !strcmp(dent->d_name, "..")||!strcmp(dent->d_name,"self")) continue;
 //                 sprintf(status_file,"/proc/%s/status",dent->d_name);

                  pid_t pid;
                  char cmd_name[100];
                  sprintf(cmdline_file,"/proc/%s/cmdline",dent->d_name);
                  if(fh = fopen(cmdline_file,"r")){
                        sscanf(dent->d_name, "%hu", &pid);
                        if(fgets(cmd_name, 100, fh)){
                              if(DEBUG)
                                    printf("Found pair (%s %hu)\n", cmd_name, pid);
                              hashtbl_insert(hashtbl, cmd_name, pid);
                        }else
                              printf("Cannot get cmd_name from %s\n", cmdline_file);
                  }
//                  if(fh = fopen(status_file,"r")){
//                        char *line;
//                        line = malloc(1000*sizeof(char));
//                        char *pos;
//                        size_t len=0;
//
//                          if(DEBUG)
//                                 printf("------In File <%s>-------\n", status_file);
//                         while(fgets(line, 1000, fh)){
//                           char cmd_name[50];
//                           pid_t pid;
//                           if(DEBUG)
//                                 printf("Reading line: %s\n", line);
//                           if(pos=contain_name(line)){
//                                 strcpy(cmd_name, pos);
//                                 cflag = true;
//                           }
//                           if(pos=contain_pid(line)){
//                                 sscanf(pos, "%hu", &pid);
//                                 pflag = true;
//                           }
//                           if(cflag&&pflag){
//                                 if(DEBUG)
//                                       printf("Found pair (%s %hu)", cmd_name, pid);
//                                 hashtbl_insert(hashtbl, cmd_name, pid);
//                                 break;
//                           }
//                        }
//                        fclose(fh);
//                  }
            }
      }

      if(DEBUG){
            printf("------TRAVERSING THE HASH\n");
            hashtbl_traverse(hashtbl, hashsize);
      }
      int i;
      pid_t* tmp;

      for(i=0;i<size;i++){
            pid_t tmp;
            if(tmp = hashtbl_get(hashtbl,cmd[i]))
                  pids[i] = tmp;
            else
                  pids[i] = -1;
            printf("FROM HASHTBL:(%s %hu)\n", cmd[i], pids[i]);
      }
}

char* contain_pid(char* line)
{
      char* tgt = "Pid:";
      while(*tgt){
            if(*tgt++!=*line++)
               return NULL;
      }
      while(*line==' '||*line=='\t'){
            line++;
      }

      char* end=line;

      while(*end!=' '&& *end!='\t' && *end!='\n')
            end++;
      *end='\0';
      return line;
}

char* contain_name(char* line)
{
      char *tgt= "Name:";
      while(*tgt){
            if(*tgt++!=*line++)
               return NULL;
      }
      while(*line==' '||*line=='\t'){
            line++;
      }

      char* end=line;

      while(*end!=' '&& *end!='\t' && *end!='\n')
            end++;
      *end='\0';
      return line;
}
bool isDir(struct dirent* dent){
      char name[100];
      struct stat s;

      sprintf(name, "/proc/%s", dent->d_name);//need to see if it is full path
      if(stat(name, &s)){
         printf("Error in stat\n");
         exit(-1);
      }
      if(s.st_mode&S_IFDIR)
            return true;
      return false;
}

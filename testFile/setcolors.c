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
#include<errno.h>

#define SET_COLORS 223
void get_pids_cmd(char* cmd[], pid_t pids[], int size);
bool isDir(struct dirent* dent);
char* contain_name(char* line);
char* contain_pid(char* line);

static int DEBUG = 1;

void disp_setcolors(int size, char* cmd[], pid_t pids[] , u_int16_t colors[], int retval[]){
      int i;
      printf("----------Display results from setcolors----------\n");
      for(i = 0; i<size; i++){
            printf("\nProcess:%s\n", cmd[i]);
            (pids[i]==-1)?printf("Found no pid\n"):printf("Pid:%d\n",pids[i]);
            if(retval[i]){
                  perror("Error invoking system call setcolors");
                  continue;
            }
            printf("Retval%d\n",retval[i]);
            printf("Assigned color%hu\n",colors[i]);
      }
}

int main(int argc, char* argv[])
{
   int size = argc-1;

   if(size%2){
      printf("Please input in pair cmd and color\n");
      exit(1);
   }

   size /=2;
   char* cmd[size];
   pid_t pids[size];
   u_int16_t colors[size];
   int retval[size];
   int i;
   int err;



   for(i=0;i<size;i++){
         cmd[i] = argv[2*i+1];
         sscanf(argv[2*i+2], "%hu", &colors[i]);
   }

   get_pids_cmd(cmd, pids, size);
   err = syscall(SET_COLORS, size, pids, colors, retval);
   if(err){
         perror("Mistakes in invoking setcolors");
         exit (1);
   }
   if(DEBUG){
      disp_setcolors(size, cmd, pids, colors, retval);
   }

   return 0;
}


void get_pids_cmd(char* cmd[], pid_t pids[], int size)
{
      HASHTBL *hashtbl;
      hash_size hashsize = 1000;

      if(!(hashtbl = hashtbl_create(hashsize,NULL))){
            fprintf(stderr, "ERROR: hashtbl_create() failed\n");
            exit(EXIT_FAILURE);
      }

      int i;


      for(i=0;i<size;i++){
            pid_t tmp;
            if(tmp = hashtbl_get(hashtbl,cmd[i]))
                  pids[i] = tmp;
            else
                  pids[i] = -1;
            if(DEBUG)
               printf(">>cmd to pid:(%s %d)\n", cmd[i], pids[i]);
      }
}


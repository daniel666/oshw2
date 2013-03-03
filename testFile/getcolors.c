#include<stdio.h>
#include<sys/types.h>
#include<sys/syscall.h>
#include<unistd.h>
#include "hashtable.h"
#include<errno.h>
static int debug = 1;
#define GET_COLORS 251

void disp_result(int size, char* cmd[], pid_t* pids, u_int16_t* colors, int* retval)
{
      int i;
      printf("--------Display getcolors result(s)------------\n");
      for(i=0; i<size; i++){
            printf("\nProcess:%s\n", cmd[i]);
            printf("Rerval:%d\n", retval[i]);
            if(retval[i]){
                  perror("Error");
                  printf("\n");
                  continue;
            }
            printf("Pid:%d\nColor:%hu\n", pids[i], colors[i]);
      }
}


int main(int argc, char* argv[])
{
      int size = argc - 1;
      argv++;

      pid_t pids[size];
      u_int16_t colors[size];
      int retval[size];
      int i;
      int err;
      HASHTBL *hashtbl;
      hash_size hashsize = 1000;

      if(!(hashtbl = hashtbl_create(hashsize,NULL))){
            fprintf(stderr, "ERROR: hashtbl_create() failed\n");
            exit(EXIT_FAILURE);
       }

      for(i=0;i<size;i++){
            pids[i] = hashtbl_get(hashtbl, argv[i]);
      }

      err = syscall(GET_COLORS, size , pids, colors, retval);
      if(err){
            perror("In invoking system call 'getcolors'");
            exit(1);
      }
      if(debug)
            disp_result(size, argv, pids, colors, retval);

      return 0;
}







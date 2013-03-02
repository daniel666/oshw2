#include<stdio.h>
#include<sys/types.h>
#include<sys/syscall.h>
#include<unistd.h>
#include "hashtable.h"

static int debug = 1;
#define GET_COLORS 251

void disp_result(int size, pid_t* pids, u_int16_t* colors, int* retval)
{
      int i;

      for(i=0; i<size; i++){
            if(retval[i]){
                  printf("Failed to get color of pid %d, error code %d\n", pids[i], retval[i]);
                  continue;
            }
            printf("PID:%d---COLOR:%hu\n", pids[i], colors[i]);
      }
}
int main(int argc, char* argv[])
{
      int size = argc - 1;
      pid_t pids[size];
      u_int16_t colors[size];
      int retval[size];
      int i;
      HASHTBL *hashtbl;
      hash_size hashsize = 1000;

      if(!(hashtbl = hashtbl_create(hashsize,NULL))){
            fprintf(stderr, "ERROR: hashtbl_create() failed\n");
            exit(EXIT_FAILURE);
       }

      for(i=0;i<size;i++){
            pids[i] = hashtbl_get(hashtbl, argv[i+1]);
      }

      syscall(GET_COLORS, size , pids, colors, retval);
      if(debug)
            disp_result(size, pids, colors, retval);

      return 0;
}







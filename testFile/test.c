#include<unistd.h>
#include<stdio.h>
#include<sys/syscall.h>
#include<linux/types.h>
#define set_colors 223
#define get_colors 251
int main()
{
   #define nr_pids  5
   pid_t    pids[nr_pids] = {1 ,2 , 3, 4, 5};
   u_int16_t   colors[nr_pids] = {1, 2, 3, 4, 5};
   int retval[nr_pids];

   printf("User space call start: ready to call sys %d\n",set_colors);
   syscall(set_colors, nr_pids, pids, colors, retval);
   printf("User space call end\n");


   u_int16_t   ret_colors[nr_pids];
   int   retval2[nr_pids];
   printf("User space call start: ready to call sys %d\n",get_colors);
   syscall(get_colors, nr_pids, pids, ret_colors, retval2);
   printf("User space call end\n");
}

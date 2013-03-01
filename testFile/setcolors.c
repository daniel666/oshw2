#include<unistd.h>
#include<stdio.h>
#include<sys/syscall.h>
#include<types.h>

#define MAX 50

void parse(char* start, char** cmd, u_int16_t* colors)
{
      char* end = start;
      int i=0;
      int pair=0;
      int flag=0;
loop:
      while(end!=' '&&end!='\t'&&end!='\0'){
            end++;
      }
      if(end!='\0'){
         end++='\0';
         if(!flag){
            colors[i++] = (u_int16_t) start; //need to check whether it's digit.
            flag = 1;
            pair++;
         }
         else{
            cmd[i++]=start;
            flag=0;
         }
         start=end;
         goto loop;
      }
      if(flag)
         return -1;

      return pair;
}

void find_pids(char** cmd, pid_t *pids, int nr_pids)
{
   int i;

   for(i=0;i<nr_pids;i++){

   }
}
int main(int argc, char** argv)
{
   char *cmd[MAX];
   pid_t pids[MAX];
   int nr_pids;
   u_int16_t colors[MAX];
   char* start=*argv;
   while(start!=' '&&start!='\t')
         start++ ;
   while(start==' '||start=='\t')
         start++ ;
   if(nr_pids = parse(start, cmd, colors)){
         find_pids(cmd, pids, nr_pids);
         if(syscall(SETCOLORS,nr_pids, pids, colors, retval))
//           print_color(colors);
//           print_retval(ret_val);
   }
   return -1;
}

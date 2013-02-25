#include<unistd.h>
#include<sys/syscall.h>
#define set_colors 223
#define get_colors 251
int main()
{
   printf("User space call start: ready to call sys %d\n",set_colors);
   syscall(set_colors);
   printf("User space call end\n");
   printf("User space call start: ready to call sys %d\n",get_colors);
   syscall(get_colors);
   printf("User space call end\n");
}

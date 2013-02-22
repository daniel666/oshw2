#include <stdio.h>
#include <stdlib.h>
#include <linux/unistd.h>
#include <sys/syscall.h>

int main (){
	syscall(251);
}


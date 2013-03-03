#include<unistd.h>
#include<stdio.h>
#include<sys/syscall.h>
#include<linux/types.h>
#define set_colors 223
#define get_colors 251

void testfork(int delay, char* f, char* cmdline[], int size_cmdline){
    sleep(delay);
    pid_t pid;
    fflush(stdout);
    if(strcmp(f, "fork") == 0){
        pid = fork();
    }
    else if(strcmp(f, "vfork") == 0){
        pid = vfork();
    }
    else if(strcmp(f, "clone") == 0){
     //   pid=clone();
    }
    else{
        printf("Wrong argument usage");
        return;
    }
    if(pid == 0){
	    int execcode;
	    if(strcmp(cmdline[0],"forktest")==0){
		    execcode = execvp("/data/tmp/forktest",cmdline);
		}
	
	    else {
	            execcode = execvp(cmdline[0], cmdline);
		}
	    if(execcode==-1) {
		printf("EXEC failed\n");
  	        return;
        }
    }
    else if(pid < 0){
        printf("\n Fork failed.\n");
        return;
    }
    else{
	wait(NULL);
    }
}



int main(int argc, char **argv){
    
    int size_cmdline = argc - 3;
    char* cmdline[size_cmdline];
    int i;
    for(i=0; i<size_cmdline; i++){
        cmdline[i] = (char*) malloc(strlen(argv[i+3])+1);
        strcpy(cmdline[i], argv[i+3]);
	printf("NewCMDLINE[%d]: %s\n",i,cmdline[i]);
    }
    cmdline[size_cmdline] = NULL;
    if(size_cmdline > 0)
	    testfork(atoi(argv[1]), argv[2], cmdline, size_cmdline);

    
    return 0;
}

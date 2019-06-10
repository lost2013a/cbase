#include<stdio.h>
#include<string.h>
#include<stdlib.h>
//#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

char *execv_str[]={"echo","shell test echo",NULL};
char *execv_str1[]={"iftp","ftpget -u myftp -p 1234 192.168.251.170 /tmp/iftp iftp",NULL};
char *execv_str2[]={"chmod","chmod 755 /tmp/iftp",NULL};



int main(int argc, char *argv[])
{
	int childpid;
	if(fork()==0){
		if(execv("/bin/echo",execv_str)<0){
			perror("error on exec");
			exit(0);
		}
		
		if(execv("/bin/iftp",execv_str1)<0){
			perror("error on exec");
			exit(0);
		}
		
		if(execv("/bin/chmod",execv_str2)<0){
			perror("error on exec");
			exit(0);
		}
		
	}
	else{
		//parent process
		wait(&childpid);
		printf("execv done\n\n");
	}
	return 0;
}




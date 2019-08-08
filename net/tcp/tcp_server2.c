#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <strings.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>

#define SERV_PORT 5678
#define WAIT_COUNT 5

 
void send_mp3(void);
void sig_child(int signo)
{
	int stat;
	pid_t pid = wait(&stat);
	printf("close connnet: %d\n",pid);
	return;
}
static int real_fd;
int main(int argc, char** argv)
{
	int listen_fd;
	struct sockaddr_in listen_addr, client_addr;
	socklen_t len = sizeof(struct sockaddr_in);
	listen_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(listen_fd == -1)
	{
		perror("socket failed   ");
		return -1;
	}
	signal(SIGCHLD, sig_child);
	bzero(&listen_addr,sizeof(listen_addr));
	listen_addr.sin_family = AF_INET;
	listen_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	listen_addr.sin_port = htons(SERV_PORT);
	bind(listen_fd,(struct sockaddr *)&listen_addr, len);
	listen(listen_fd, WAIT_COUNT);
    while(1)
	{
		real_fd = accept(listen_fd, (struct sockaddr*)&client_addr, &len);
		if(real_fd == -1)
		{
			perror("accpet fail  ");
			return -1;
		}
		if(fork() == 0)
		{
			close(listen_fd);
			send_mp3();
			close(real_fd);
			exit(0);			
		}
		close(real_fd);
	}	
	return 0;
}

void send_mp3(void)
{
	while(1)
	{
		if(write(real_fd ,"123",4) == 0)
		{
			perror("write client failed ");
			return ;
		}
		sleep(1);
	}
}

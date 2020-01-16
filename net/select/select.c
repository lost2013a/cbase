#include<stdio.h>
#include<pthread.h>
#include<netinet/in.h>
#include<stdlib.h>
#include<string.h>
#include<strings.h>
#include<sys/types.h>
#include<sys/socket.h>
int main(int argc,char **argv)
{
    int socketfd,ret;
    fd_set fd;
    FD_ZERO(&fd);
    FD_SET(0,&fd);
        
    
    socketfd = socket(PF_INET,SOCK_STREAM,0);
    FD_SET(socketfd,&fd);
    ret = select(socketfd+1,&fd,NULL,NULL,NULL);
    struct sockaddr_in saddr;
    memset(&saddr,0,sizeof(saddr));
    saddr.sin_family = PF_INET;
    saddr.sin_port = htons(50003);
    saddr.sin_addr.s_addr = inet_addr("192.168.251.170");
    int res= connect(socketfd,(struct sockaddr *)&saddr,sizeof(struct sockaddr));
	while(res < 0){

		printf("wait connct\n");
		sleep(2);
	}

//只要与服务器连上立马将自己的昵称发送过去
   
    char buf[50],buf1[50];

	volatile struct	timeval timeover={0};
	int	nsel;
	timeover.tv_sec = 1;
	timeover.tv_usec = 500000;
	
    while(1)
    {
        bzero(buf,50);
        bzero(buf1,50);
        timeover.tv_sec = 1;
	timeover.tv_usec = 500000;
    FD_SET(0,&fd);
    FD_SET(socketfd,&fd);
    ret = select(socketfd+1,&fd,NULL,NULL,&timeover);
	printf("ret=%d\n",ret);
            if(FD_ISSET(socketfd,&fd))
            {
            if(recv(socketfd,buf1,sizeof(buf1),0)<0)
                perror("recv() error!\n");
                printf("%s\n",buf1);
                continue;
            }
            if(FD_ISSET(0,&fd))
            {
            scanf("%s",buf);
            strcat(buf,":");
            strcat(buf,argv[0]);    
            if(send(socketfd,buf,strlen(buf),0)<0)
            perror("send() error!\n");
            continue;
            }    

        }
}
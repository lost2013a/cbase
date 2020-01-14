#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <signal.h>

#define SERV_PORT 		50003
#define TRANSMIT_PORT	50003
#define TRANSMIT_IP	"192.168.251.181"

#define BUFF_SIZE	(1024 * 4)


#define WAIT_COUNT 5

int creat_connect(void);

static int real_fd;

void serve_func(void)
{
	unsigned char rbuf[1460];
	int rlen, wlen;
	int tran_fd;
	
	tran_fd= creat_connect();
	if(tran_fd < 1)
		return;
	while(1)
	{
		//len= recv(real_fd, rbuf, 1460, MSG_DONTWAIT);
		rlen= recv(real_fd, rbuf, 1460, MSG_DONTWAIT);
		if(rlen > 1){
			printf("client to server %d byte\n", rlen);
			wlen= write(tran_fd, rbuf, rlen);
			if(wlen<0){
				printf("write fd err\n");
				return ;
			}
		}
#if 1

		rlen= recv(tran_fd, rbuf, 1460, MSG_DONTWAIT);
		if(rlen > 1){
			printf("server to client %d byte\n", rlen);
			wlen= write(real_fd, rbuf, rlen);
			if(wlen<0){
				printf("write fd err\n");
				return ;
			}
		}
		
#endif
		
		usleep(1000);
	}
}

void sig_child(int signo)
{
	int stat;
	pid_t pid = wait(&stat);
	printf("close connnet: %d\n",pid);
	return;
}

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
	printf("list to port %d\n", SERV_PORT);
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
			serve_func();
			close(real_fd);
			exit(0);			
		}
		close(real_fd);
	}	
	return 0;
}


int creat_connect(void)
{
    int conn_sock, rlen;
    struct sockaddr_in	server_addr;

    conn_sock	= socket(AF_INET, SOCK_STREAM, 0);
    if (conn_sock < 0) {
        perror("socket(2) error");
        return  -1;
    }

    (void)memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family		= AF_INET;
    server_addr.sin_port		= htons(TRANSMIT_PORT);
    server_addr.sin_addr.s_addr	= inet_addr(TRANSMIT_IP);
    
    printf("wait connect to transmit addr......\n");
	while(1){

		if (connect(conn_sock,
                (struct sockaddr *)&server_addr,
                sizeof(server_addr)) >= 0) {
	        perror("connect(2) ok");
	        break;
   		 }
		else
			sleep(2);		
	}
	printf("connect ok\n");
	return conn_sock;
}


#if 0
int main(int argc, char *argv[])
{
    int conn_sock, rlen;
    char test_str[BUFF_SIZE]	= "tcp echo test";
    struct sockaddr_in	server_addr;

    conn_sock	= socket(AF_INET, SOCK_STREAM, 0);
    if (conn_sock < 0) {
        perror("socket(2) error");
        goto create_err;
    }

    (void)memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family		= AF_INET;
    server_addr.sin_port		= htons(TRANSMIT_PORT);
   
    server_addr.sin_addr.s_addr	= inet_addr(TRANSMIT_IP);
    


	while(1){

		if (connect(conn_sock,
                (struct sockaddr *)&server_addr,
                sizeof(server_addr)) >= 0) {
	        perror("connect(2) ok");
	        break;
   		 }
		else
			sleep(2);
				

	}
	printf("connect ok\n");
	if (write(conn_sock, "ok", strlen("ok")) < 0) {
        perror("send data error");
        goto err;
    }
	 
	while(1){

		rlen= recv(conn_sock, test_str, BUFF_SIZE, MSG_DONTWAIT);
		if(rlen > 0){
			printf("%s\n", test_str);
		}
	   	
	    printf("rlen =%d , %d\n", rlen, errno);
		if (write(conn_sock, "ok", strlen("ok")) < 0) {
        perror("send data error");
        goto err;
    }

		
		sleep(1);
		

	}
	
   


    return EXIT_SUCCESS;

 err:
    close(conn_sock);
 create_err:
    fprintf(stderr, "client error");
    return EXIT_FAILURE;
}

#endif

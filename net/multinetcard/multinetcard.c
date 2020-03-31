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

#include <net/if.h>


#define SERVER_IP	"192.168.251.170"
#define SERVER_PORT	((uint16_t)2010)
#define BUFF_SIZE	(1024 * 4)

#define NET_IF_1 "lo"


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
    server_addr.sin_port		= htons(SERVER_PORT);
   
    server_addr.sin_addr.s_addr	= inet_addr(SERVER_IP);
    
	struct ifreq interface;
	printf("1\n");
    strncpy(interface.ifr_ifrn.ifrn_name, NET_IF_1, sizeof(NET_IF_1));
    if (setsockopt(conn_sock, SOL_SOCKET, SO_BINDTODEVICE, (char *)&interface, sizeof(interface))  < 0) {
           perror("SO_BINDTODEVICE failed");
    }

	printf("2\n");
	while(1){
		int ret= connect(conn_sock, (struct sockaddr *)&server_addr,sizeof(server_addr));
		printf("ret= %d\n", ret);
		if (ret >= 0) {
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



#if 0
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>
#include <netinet/in.h>
#include <net/if.h>
#include <linux/sockios.h>
#include <sys/ioctl.h>
 
#define BUF_SIZE    1024
#define INTERFAXENAME "eth1"
 
int sock;
 
void * network_recv_thread(void * arg)
{
    int n = 0;
    char buff[BUF_SIZE];
    struct sockaddr_in peer;
    int len = sizeof(peer);
 
    while(1)
    {
        n = recvfrom(sock, buff, BUF_SIZE, 0, (struct sockaddr *)&peer, &len);
        if (n>0)
        {
            buff[n] = 0;
            printf("received:");
            puts(buff);
        }
        else if (n==0)
        {
            printf("server closed\n");
            close(sock);
            break;
        }
        else if (n == -1)
        {
            perror("recvfrom");
            close(sock);
            break;
        }
    }
 
    return NULL;
}
 
int main(int argc, char *argv[])
{
    int ret = 0;
    
    if (argc != 3)
    {
        printf("Usage: %s ip port\n", argv[0]);
        exit(1);
    }
    
    printf("This is a UDP client\n");
 
    if ( (sock=socket(AF_INET, SOCK_DGRAM, 0)) <0)
    {
        perror("socket");
        exit(1);
    }
    
    struct ifreq interface;
    strncpy(interface.ifr_ifrn.ifrn_name, INTERFAXENAME, sizeof(INTERFAXENAME));
    if (setsockopt(sock, SOL_SOCKET, SO_BINDTODEVICE, (char *)&interface, sizeof(interface))  < 0) {
           perror("SO_BINDTODEVICE failed");
    }
    
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(atoi(argv[2]));
    addr.sin_addr.s_addr = inet_addr(argv[1]);
    if (addr.sin_addr.s_addr == INADDR_NONE)
    {
        printf("Incorrect ip address!");
        close(sock);
        exit(1);
    }
 
    pthread_t pid;
    ret = pthread_create(&pid, NULL,network_recv_thread, NULL);
    if (ret)
    {
        printf("Create pthread error!\n");
        return 0;
    }
    
    char buff[BUF_SIZE];
    int n = 0;
 
    while (1)
    {
        gets(buff);
        n = sendto(sock, buff, strlen(buff), 0, (struct sockaddr *)&addr, sizeof(addr));
        if (n < 0)
        {
            perror("sendto");
            close(sock);
            break;
        }
    }
 
    pthread_join(pid, NULL);
 
    return 0;
}

#endif

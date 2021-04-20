#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdbool.h>
#include <stdint.h>

#define SERVER_IP	"192.168.251.17"
#define SERVER_PORT	((uint16_t)6666)
#define BUFF_SIZE	(1024 * 4)
const char *str = "hello world\n";
int main(int argc, char *argv[])
{
    int conn_sock;
    char test_str[BUFF_SIZE]	= {0xbb,1,2,3,5};
    struct sockaddr_in	server_addr;
    socklen_t	addr_len	= sizeof(server_addr);
    fd_set	sockset;
    struct timeval timeout	= {
        .tv_sec	= 3,
    };

    conn_sock	= socket(AF_INET, SOCK_DGRAM, 0);
    if (conn_sock < 0) {
        perror("socket(2) error");
        goto create_err;
    }
	unsigned short port= SERVER_PORT;
	 if (argc != 2) {
		
       
    } else {
		port= atoi(argv[1]);
  
    }
	
    (void)memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family		= AF_INET;
    server_addr.sin_port		= htons(port);

	 if (bind(conn_sock, (struct sockaddr *)&server_addr, sizeof(server_addr))) {
        perror("bind(2) error");
        goto err;
    }
	 server_addr.sin_addr.s_addr	= inet_addr(SERVER_IP); 
   
	 printf("port =%d\n", port);
#if 1
	while(1)
	{
		
		if (sendto(conn_sock, str, strlen(str), 0,
				   (struct sockaddr *)&server_addr, addr_len) < 0) {
			perror("send data error");
			goto err;
		}
		//test_str[5]++;
		usleep(2000*1000);
	}
#endif
    if (sendto(conn_sock, test_str, strlen(test_str), 0,
               (struct sockaddr *)&server_addr, addr_len) < 0) {
        perror("send data error");
        goto err;
    }

    while (true) {
        int num	= 0;

        FD_ZERO(&sockset);
        FD_SET(conn_sock, &sockset);
        num	= select(conn_sock + 1, &sockset, NULL, NULL, &timeout);
        if (num < 0) {
            if (errno == EINTR) {
                continue;
            } else {
                perror("select(2) error");
                goto err;
            }
        } else if (num == 1) {
            if (FD_ISSET(conn_sock, &sockset)) {
                break;
            }
        } else if (num == 0) {
            fprintf(stderr, "%s\n", "Waiting for echo time out!");
            goto err;
        } else {
            fprintf(stderr, "%s\n", "Code should NOT reach here");
            goto err;
        }
    }

    (void)memset(test_str, 0, BUFF_SIZE);
    if (recvfrom(conn_sock, test_str, BUFF_SIZE, 0,
                 (struct sockaddr *)&server_addr, &addr_len) < 0) {
        perror("receive data error");
        goto err;
    }
    printf("get echo from %s:%hu\n",
           inet_ntoa(server_addr.sin_addr),
           ntohs(server_addr.sin_port));
    printf("%s\n", test_str);
    fflush(stdout);

    return EXIT_SUCCESS;

 err:
    close(conn_sock);
 create_err:
    fprintf(stderr, "client error");
    return EXIT_FAILURE;
}

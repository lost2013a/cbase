#include <sys/types.h>  
#include <sys/stat.h> 
#include <fcntl.h>
#include <errno.h>  
#include <sys/wait.h>
#include <netinet/tcp.h> // for TCP_NODELAY


#include "nstar_adt_test.h"
#include "nstar_adt_httputil.h"

#include <netinet/tcp.h> // for TCP_NODELAY

#define myprintf printf
#define myerro   perror
#define LISTEN_PORT ((unsigned short)80)

char *nstar_adt_web_tbuf; 
char *nstar_adt_web_rbuf;



int server_sock, conn_sock;

static int web_socket_init(void)
{
	 int on = 1;  
	 struct sockaddr_in server_addr, client_addr;
	 socklen_t	 sock_len	 = sizeof(client_addr);

	 server_sock = socket(AF_INET, SOCK_STREAM, 0);
	 if (server_sock < 0) {
		 myerro("socket(2) error");
		 return -1;
	 }


    setsockopt(server_sock, SOL_SOCKET, SO_KEEPALIVE, (void *)&on, sizeof(on));  
    setsockopt(server_sock, IPPROTO_TCP, TCP_NODELAY, (void *)&on, sizeof(on));  
	
	 (void)memset(&server_addr, 0, sock_len);
	 server_addr.sin_family 	 = AF_INET;
	 server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	 server_addr.sin_port		 = htons(LISTEN_PORT);

	
	 
	 if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr))) {
		 myerro("bind(2) error");
		 goto err;
	 }
 
	 if (listen(server_sock, 5)) {
		 myerro("listen(2) error");
		 goto err;
	 }
	 return 0;
err:
    close(server_sock);
	return -2;
}

int main(int argc, char *argv[])
{
	int ret;
	struct sockaddr_in client_addr;
	socklen_t sock_len= sizeof(client_addr);
	ret= web_socket_init();
	if(ret < 0)
		return -1;
#if 1
	unsigned char *p_webram= malloc(2*2000);/*2*MAX_URI_SIZE*/
	if(p_webram == NULL){
		myerro("web malloc failed\n");
		return -1;
	}
	nstar_adt_web_tbuf= (char*)p_webram;
	nstar_adt_web_rbuf= (char*)(p_webram+ 2000);
		
#endif	
	#include <sys/prctl.h>
	prctl(PR_SET_NAME, (unsigned long)"p_web_test");
	while (1) 
	{
		conn_sock= accept(server_sock, (struct sockaddr *)&client_addr, &sock_len);
		if (conn_sock < 0) {
			if (errno == EINTR) {
				continue;
			}
			myerro("web err accept\n");
			continue;
		}
		do_https(conn_sock);
		usleep(20*1000);
		close(conn_sock);
		
	}
	free(p_webram);
	return 0;
}







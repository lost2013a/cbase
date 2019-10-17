#include <netinet/in.h>
#include <netinet/tcp.h> 
#include <sys/socket.h>
#include "nstar_adt_httputil.h"
#include "nstar_adt_http_server.h"

#define WEB_PORT 80
#define FIN_WCNT 5

static unsigned char *p_webmem;
static int server_sock;
int conn_sock;

char *nstar_web_tx_buf; 
char *nstar_web_rx_buf;

static void* aligned_malloc(size_t required_bytes, size_t alignment)
{
    int offset = alignment - 1 + sizeof(void*);
    void* p1 = (void*)malloc(required_bytes + offset);
    if (p1 == NULL)
        return NULL;
    void** p2 = (void**)( ( (size_t)p1 + offset ) & ~(alignment - 1) );
    p2[-1] = p1;
    return p2;
}

static void aligned_free(void *p2)
{
    void* p1 = ((void**)p2)[-1];
    free(p1);
}

static int web_mem_init(void)
{
	unsigned int mem_len= 2*MAX_URI_SIZE+32;
	
	unsigned char *p_webmem= aligned_malloc(mem_len,8);
	if(p_webmem == NULL){
		perror("web malloc failed\n");
		return -1;
	}
	nstar_web_tx_buf= (char*)p_webmem;
	nstar_web_rx_buf= (char*)(p_webmem+ MAX_URI_SIZE+8);
	return 0;
}

static int web_socket_init(void)
{
	int on = 1; 
	int ret;
	struct sockaddr_in server_addr, client_addr;
	socklen_t	sock_len	= sizeof(client_addr);
	server_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (server_sock < 0) {
		perror("web socket error\n");
		return 0;
	}
	
	if(setsockopt(server_sock, IPPROTO_TCP, TCP_NODELAY, (void *)&on, sizeof(on)) < 0) 
	{
		perror("web setsockopet error\n");
		return -1;
	}
	
	(void)memset(&server_addr, 0, sock_len);
	server_addr.sin_family		= AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port		= htons(WEB_PORT);
	
	ret= bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr));
	if (ret<0) {
		perror("web bind error\n");
		goto err;
	}

	if (listen(server_sock, 5)) {
		perror("web listen error\n");
		goto err;
	}
	return server_sock;
err:
   return -1;

}


void nstar_web_servo(void* arg)
{
	int fin_waiting;
	struct sockaddr_in client_addr;
	socklen_t sock_len= sizeof(client_addr);
	if(web_mem_init() < 0)
		return;
	server_sock = web_socket_init();
	if(server_sock < 1)
		goto release;
#if 1	
#include "write_log.h"
	log_file_open();
#endif

	cookie_init();
	while (1) 
	{
		conn_sock= accept(server_sock, (struct sockaddr *)&client_addr, &sock_len);
		if (conn_sock < 0) {
			printf("web shutdown\n");
			goto shutdown;
		}
		fin_waiting=FIN_WCNT;
		do{
			if(do_https()){
				fin_waiting=FIN_WCNT;
			}
			usleep(10*1000);
		}while(fin_waiting--);
		close(conn_sock);
		usleep(10*1000);
	}

shutdown:
	close(server_sock);
release:	
	aligned_free(p_webmem);
	return ;
}

int main(void)
{
	nstar_web_servo(0);
	close(conn_sock);
	close(server_sock);
	return 0;
}







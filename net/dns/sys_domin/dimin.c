#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
void error_handling(char *message);

int hostname_to_ip(const char* hostname)
{
        int ret = 0;
 
        if(!hostname)
        {
                printf("invalid params\n");
                return -1;
        }
 
        struct addrinfo hints;
        struct addrinfo *res, *res_p;
        memset(&hints, 0, sizeof(struct addrinfo));
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_flags = AI_CANONNAME;
        hints.ai_protocol = 0;
 
        ret = getaddrinfo(hostname, NULL, &hints, &res);
        if(ret != 0)
        {
                printf("getaddrinfo: %s\n", gai_strerror(ret));
                return -1;
        }
 
        for(res_p = res; res_p != NULL; res_p = res_p->ai_next)
        {
                char host[1024] = {0};
                ret = getnameinfo(res_p->ai_addr, res_p->ai_addrlen, host, sizeof(host), NULL, 0, NI_NUMERICHOST);
                if(ret != 0)
                {
                        printf("getnameinfo: %s\n", gai_strerror(ret));
                }
                else
                {
                        printf("ip1: %s\n", host);
                }
        }
 
        freeaddrinfo(res);
        return ret;
}

int main(int argc, char *argv[])
{
    int i;
    struct hostent *host;
    if(argc!=2) {
        printf("Usage : %s <addr>\n", argv[0]);
        exit(1);
    }
#if 0
    host=getnameinfo(argv[1]);
    if(!host)
        error_handling("gethost... error");

    printf("Official name: %s \n", host->h_name);

    for(i=0; host->h_aliases[i]; i++)
        printf("Aliases %d: %s \n", i+1, host->h_aliases[i]);

    printf("Address type: %s \n", 
        (host->h_addrtype==AF_INET)?"AF_INET":"AF_INET6");

    for(i=0; host->h_addr_list[i]; i++)
        printf("IP addr %d: %s \n", i+1,
                    inet_ntoa(*(struct in_addr*)host->h_addr_list[i]));//！！！
    return 0;
#else
	hostname_to_ip(argv[1]);	

	return 0;
#endif	
}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}
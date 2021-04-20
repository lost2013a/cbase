#include <sys/socket.h>
#include <netinet/tcp.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <sys/time.h>
#include <errno.h>
#include <netdb.h>
#include <string.h>
#define LOGD printf
#define LOGI printf
#define LOGE printf

#define JUST_SOCK_DEFAULT_TIMEOUT 5

static int socket_wait_writable(int sockfd, int timeout_ms);

int just_sock_connect_url(char *url, int port)
{
    if(url == NULL) return -EINVAL;
    
    LOGI("just_sock_connect_url(%s:%d)\n", url, port);
    
    struct hostent *hptr = gethostbyname(url);
    if(hptr == NULL) return -EFAULT;
    
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd <= 0){
        LOGE("socket error");
        return -EFAULT;
    }
 
    struct sockaddr_in servaddr;
    memset((void*)&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    //inet_pton(AF_INET, ipBuf, &servaddr.sin_addr.s_addr);
    servaddr.sin_addr.s_addr = *((unsigned long*)hptr->h_addr_list[0]);
    
    int reval = connect(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr));
    if (reval == 0) return sockfd;
    
    switch(errno)
    {
        case EISCONN:
            reval = 0;
            break;
        case EINPROGRESS:
        case EALREADY:
            reval = socket_wait_writable(sockfd, 30000);
            break;
        default:
            reval = -EFAULT;
            break;
    }
    if (reval < 0)
    {
        LOGE("connect [%s:%d] failed, reval=%d", url, port, reval);
        close(sockfd);
        return reval;
    }
    LOGI("connect [%s:%d] Success!", url, port);
 
    return sockfd;
}

static int socket_wait_writable(int sockfd, int timeout_ms)
{
    struct timeval tpstart,tpend,timeout_v;
    fd_set wset, eset;
    long time_used = 0;
    int reval = 0;
 
    LOGI("socket_wait(sockfd=%d,timeout_ms=%d)", sockfd, timeout_ms);
 
    gettimeofday(&tpstart,NULL);
 
    while(1)
    {
        gettimeofday(&tpend,NULL);
        time_used=1000*(tpend.tv_sec-tpstart.tv_sec);
        time_used += (tpend.tv_usec-tpstart.tv_usec)/1000;
        if (time_used > timeout_ms)
        {
            LOGI("write timeout1 [%d ms]", timeout_ms);
            return -ETIME;
        }
        long time_seft = timeout_ms - time_used;
        timeout_v.tv_sec = time_seft/1000;
        timeout_v.tv_usec = time_seft%1000 * 1000;
 
        FD_ZERO(&wset);
        FD_ZERO(&eset);
        FD_SET(sockfd, &wset);
        FD_SET(sockfd, &eset);
        reval = select(sockfd + 1, NULL, &wset, &eset , &timeout_v);
        if(reval == -1 && errno != EINTR)
        {
            LOGE("error in socket_wait 1:%s", strerror(errno));
            return -EFAULT;
        }
        else if( reval == 0)
        {
            LOGI("write timeout2 [%d ms]", 0);
            return -ETIME;
        }
        else if(reval > 0 && FD_ISSET(sockfd, &eset))
        {
            LOGE("error in socket_wait 2:%s", strerror(errno));
            return -EFAULT;
        }
        else if ( reval > 0 && FD_ISSET(sockfd, &wset))
        {
            //write event:
            LOGI("socket[%d] writable ...", sockfd);
            break;
        }
        else
        {
            LOGE("unexpected err in socket_wait");
        }
    }
    return 0;
}

void just_sock_connect_test(void)
{
    int reval = 0;
    int fd = -1;
 
    char *this_url = "www.baidu.com";
    fd = just_sock_connect_url(this_url, 80);
    if (fd < 0)
    {
        LOGE("just_sock_connect_url failed\n");
        return;
    }
    LOGE("just_sock_connect_url[%s] success\n", this_url);
}


int main(void *parm)
{
	just_sock_connect_test();
	return 0;
	
}
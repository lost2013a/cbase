/* unpudpsendmsg.c */
#include <stdio.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>

#define SERV_PORT 51002
#define MAXLINE   256
#define SA struct sockaddr

void
err_quit(const char *sstr)
{
    printf("%s\n", sstr);
    exit(0);
}

int 
main(int argc, char **argv)
{
    int sockfd, n;
    struct sockaddr_in servaddr, dstaddr;
    char buff[MAXLINE], buff2[MAXLINE];
    struct msghdr msgsent, msgrecvd;
    struct cmsghdr cmsg, *cmsgtmp;
    struct iovec iov, iov2;
    const int on = 1;
    char control[CMSG_SPACE(64)];  // 使用CMSG_DATA分配cmsg_control内存,实际应该根据已知的结构分配。

    if ( argc < 2 ) {
        err_quit("usage: unpudpsendmsg <IPaddress>");
    }

    sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    inet_pton(AF_INET, argv[1], &servaddr.sin_addr);
    servaddr.sin_port = htons(SERV_PORT);

    // 处理sendmsg的msghdr结构
    msgsent.msg_name = NULL;
    msgsent.msg_namelen = 0;
    msgsent.msg_iovlen = 1;
    iov.iov_base = buff;         // 为iov[0]分配内存
    iov.iov_len = MAXLINE;
    msgsent.msg_iov = &iov;
    msgsent.msg_control = 0;     // 对sendmsg，msg_control要设置为0。
    msgsent.msg_controllen = 0;

    // 处理recvmsg的msghdr结构
    msgrecvd.msg_name = &dstaddr;
    msgrecvd.msg_control = control;
    msgrecvd.msg_controllen = sizeof(control);
    iov2.iov_base = (void *)buff2;
    iov2.iov_len = MAXLINE;
    msgrecvd.msg_iov = &iov2;
    msgrecvd.msg_iovlen = 1;
    msgrecvd.msg_flags = 0;

    connect(sockfd, (SA *)&servaddr, sizeof(servaddr));
#if defined(IP_PKTINFO)
    setsockopt(sockfd, IPPROTO_IP, IP_PKTINFO, &on, sizeof(on));
#elif defined(IP_RECVDSTADDR)
    setsockopt(sockfd, IPPROTO_IP, IP_RECVORIGDSTADDR, &on, sizeof(on));
#endif

    while ( 1 ) {
        fgets(buff, MAXLINE, stdin);
        n = sendmsg(sockfd, &msgsent, 0);
        if ( n <= 0 ) { 
            continue;
        }
        n = recvmsg(sockfd, &msgrecvd, 0);
        printf("recvmsg: %s", (char *)msgrecvd.msg_iov[0].iov_base);
            // 通过缓冲数据组获取服务器端返回的数据。
        printf("msg_controllen: %d\n", (int)msgrecvd.msg_controllen);
        for ( cmsgtmp = CMSG_FIRSTHDR(&msgrecvd); cmsgtmp != NULL; cmsgtmp = CMSG_NXTHDR(&msgrecvd, cmsgtmp) ) {
#if defined(IP_RECVDSTADDR)
            if ( cmsgtmp->cmsg_level == IPPROTO_IP && cmsgtmp->cmsg_type == IP_RECVDSTADDR ) {
            // 判断msg_level和msg_type再进行相应的处理。
                struct sockaddr_in *addrtmp;
                char ip[14];
                addrtmp = (struct sockaddr_in *)CMSG_DATA(cmsgtmp);
                inet_ntop(AF_INET, addrtmp, ip, sizeof(ip));
                printf("recv ip: %s, port: %d\n", ip,  ntohs(addrtmp->sin_port));
            }
#elif defined(IP_PKTINFO)
            if ( cmsgtmp->cmsg_level == IPPROTO_IP && cmsgtmp->cmsg_type == IP_PKTINFO ) {
                struct in_pktinfo *pktinfo;
                pktinfo = (struct in_pktinfo*)CMSG_DATA(cmsgtmp);
                printf("recv ip: %s, ifindex: %d\n", inet_ntoa(pktinfo->ipi_addr), pktinfo->ipi_ifindex);
            }
#endif
        }
    }
}

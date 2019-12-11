#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "unistd.h"
#include "sys/wait.h"
#include "sys/select.h"
#include "sys/poll.h"

#define MAXSIZE 100
#define PORT 5000


int main(int argc, char ** argv) {
        int sockfd;
        struct sockaddr_in serv_socket;
        struct sockaddr_in  * client_socket = (struct sockaddr_in *) malloc (sizeof(struct sockaddr_in));
        char buf[MAXSIZE + 1];

        sockfd = socket(AF_INET, SOCK_DGRAM, 0);
        bzero(&serv_socket, sizeof(serv_socket));
        serv_socket.sin_family = AF_INET;
        serv_socket.sin_addr.s_addr = htonl(INADDR_ANY);
        serv_socket.sin_port = htons(atoi(PORT));
        bind(sockfd, (struct sockaddr *)&serv_socket, sizeof(serv_socket));

        struct msghdr msg;
        msg.msg_name = client_socket;

        //如果想得到对方的地址和端口，一定要把初始化完毕的内存头指针放入msg之中
        msg.msg_namelen = sizeof(struct sockaddr_in);//长度也要指定
        struct iovec io;
        io.iov_base = buf;
        io.iov_len = MAXSIZE;
        msg.msg_iov = &io;
        msg.msg_iovlen = 1;

        ssize_t len = recvmsg(sockfd, &msg, 0);
        client_socket = (struct sockaddr_in *)msg.msg_name;
        char ip[16];
        inet_ntop(AF_INET, &(client_socket->sin_addr), ip, sizeof(ip));
        int port = ntohs(client_socket->sin_port);
        char * temp = msg.msg_iov[0].iov_base;
        temp[len] = '\0';
        printf("get message from %s[%d]: %s\n", ip, port, temp);
        close(sockfd);
}

 
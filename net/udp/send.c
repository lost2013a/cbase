#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
 
#define PORT 6789

int main(int argc, char* argv[])
{
	SOCKET socket1;
	int iReturn=0;
	
	AfxSocketInit();
	struct sockaddr_in local;
	struct sockaddr_in from;
	int fromlen =sizeof(from);
	int locallen=sizeof(local);
	local.sin_family=AF_INET;
	local.sin_port=htons(PORT);            ///监听端口
	local.sin_addr.s_addr=INADDR_ANY;       ///本机
 
 	struct sockaddr_in sockSend;
 	int sendlen=sizeof(sockSend);
	sockSend.sin_family=AF_INET;
 	sockSend.sin_port=htons(PORT);
	
	socket1=socket(AF_INET,SOCK_DGRAM,0);
	bind(socket1,(struct sockaddr*)&local,sizeof local);
	while (1)
	{
		char buffer[1024]="\0";
		char bufferSend[30]="UDPServer answer Successful\0";
		printf("waiting for message from others-------------\n");
		if (recvfrom(socket1,buffer,sizeof buffer,0,(struct sockaddr*)&from,&fromlen)!=SOCKET_ERROR)
		{
			printf("Received datagram from %s--%s\n",inet_ntoa(from.sin_addr),buffer);
			////给cilent发信息
			sockSend.sin_addr.s_addr=from.sin_addr.s_addr;
			iReturn=sendto(socket1,bufferSend,sizeof bufferSend,0,(struct sockaddr*)&sockSend,fromlen);	
		}
		Sleep(500);
	}
	closesocket(socket1);
	
	return 0;

}
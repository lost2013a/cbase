#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <strings.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>

#define SERV_PORT 50003
#define WAIT_COUNT 5

 
void send_mp3(void);
void sig_child(int signo)
{
	int stat;
	pid_t pid = wait(&stat);
	printf("close connnet: %d\n",pid);
	return;
}
static int real_fd;
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
	printf("listen to port %d\n", SERV_PORT);
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
			send_mp3();
			close(real_fd);
			exit(0);			
		}
		close(real_fd);
	}	
	return 0;
}


#define __PACKED__    __attribute__ ((__packed__)) 

struct _MSG_FIXED_HEADER{
unsigned short head_flag;
unsigned short version;
unsigned int session_id;
unsigned char type;
unsigned char sign_flag;
unsigned short data_len;
unsigned char resource_code[12];
unsigned short items;
unsigned char items_arry[12];
unsigned char business_type;
unsigned short business_len;
unsigned char business_data;
}__PACKED__;

typedef struct _MSG_FIXED_HEADER MSG_FIXED_HEADER;


#define swap32(value) \
( (value & 0x000000FFU) << 24 | (value & 0x0000FF00U) << 8 | \
(value & 0x00FF0000U) >> 8 | (value & 0xFF000000U) >> 24 )

#define swap16(value) \
( (value & 0x00FFU) << 8 | (value & 0xFF00U) >> 8 )

const char* type_str[10]={
"NULL",
"开始播发",
"停止播发",
"系统心跳",
"终端查询",
"终端参数设置",
};

unsigned char visit_type_sq(unsigned char type)
{
	unsigned char ret=0;
	switch(type){
		case 0x01:
			ret= 1;
			break;
		case 0x02:
			ret= 2;
			break;
		case 0x10:
			ret= 3;
			break;
		case 0x11:
			ret= 4;
			break;
		case 0x12:
			ret= 5;
			break;
	}
	return ret;
}


struct _CMD_PLAY{
	unsigned char source_id[18];
	unsigned char radio_type;
	unsigned char rank;
	char event[5];
	unsigned char vol;
	unsigned int start_time;
	unsigned int stop_time;
	unsigned char assist_items;
	unsigned char assist_type;
	unsigned short assist_len;
	char assist_url;
}__PACKED__;

typedef struct _CMD_PLAY CMD_PLAY_HEADER;

struct _CMD_ASSIST{
	unsigned char assist_type;
	unsigned short assist_len;
	char assist_url;
}__PACKED__;

typedef struct _CMD_PLAY CMD_PLAY_HEADER;


static void cmd_play_printf(CMD_PLAY_HEADER *cmd)
{
	printf("radio_type=%x\n", cmd->radio_type);
	printf("cmd->rank=%x\n", cmd->rank);
	//printf("event=%s\n", cmd->event);
	printf("vol=%d\n", cmd->vol);
	printf("start_time=%d\n", cmd->start_time);
	printf("stop_time=%d\n", cmd->stop_time);

	printf("assist_type=%d\n", cmd->assist_type);
	printf("assist_len=%d\n", swap16(cmd->assist_len));
	printf("assist_url=%s\n", &cmd->assist_url);

}


void parse_cmd_play(unsigned char *cmd)
{

	CMD_PLAY_HEADER *head= (CMD_PLAY_HEADER*)cmd;
	cmd_play_printf(head);
	
}



void msg_pritf(MSG_FIXED_HEADER *msg)
{
#define dbg printf
	unsigned char type;
	dbg("session_id= %d\n", swap32(msg->session_id));
	dbg("type= %x\n", msg->type);
	dbg("data_len= %d\n", swap16(msg->data_len));
	//dbg("business_type= %x\n", msg->business_type);
	type= visit_type_sq(msg->business_type);
	dbg("business_type= %s\n", type_str[type]);
	dbg("business_len= %d\n\n", swap16(msg->business_len));

	if(type == 1){
		printf("parse_cmd_play\n");
		parse_cmd_play(&msg->business_data);
	}


}


void send_mp3(void)
{
	unsigned char rbuf[1460];
	int len;
	printf("connect ok\n");
	MSG_FIXED_HEADER *msg;
	while(1)
	{
		len= recv(real_fd, rbuf, 1460, 0);
		msg= (MSG_FIXED_HEADER *)rbuf;
		printf("read len =%d\n", len);
		msg_pritf(msg);
		
		usleep(1000*1000);
	}
}

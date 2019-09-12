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
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>  
#include <sys/time.h>

#include "write_log.h"
#include "ts_monitor.h"
#include "nstar_msg.h"



#define LISTEN_PORT ((uint16_t)2010)
#define SERVER_IP	"192.168.251.127"
#define SERVER_PORT	((uint16_t)2010)
#define WAIT_COUNT 5

#define RDS_DATA_BUF_SIZE     (1024 * 40)

#define mylog(fmt,...) do{\
printf("[%s] ", gettime_str());\
printf(""fmt"", ##__VA_ARGS__);\
}while(0)



const char* str_operate_lv[]={
	"无效",
	"国家",
	"省级",
	"市级",
	"县级",
	"镇级",
	"村级",
	"无效",
};

const char* str_operate_type[]={
	"开关指令",
	"控制指令",
	"文字指令",
	"其他指令",
};


struct rds_buf_t user_rds[1]={0};
static int real_fd;
//static int server;
unsigned char rbuf[1024*10];

void pthred_func(void);

void sig_child(int signo)
{
	int stat;
	pid_t pid = wait(&stat);
	printf("close connnet: %d\n",pid);
	return;
}



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
	listen_addr.sin_port = htons(LISTEN_PORT);
	bind(listen_fd,(struct sockaddr *)&listen_addr, len);
	listen(listen_fd, WAIT_COUNT);
    while(1)
	{
		real_fd = accept(listen_fd, (struct sockaddr*)&client_addr, &len);
		if(real_fd == -1)
		{
			perror("accpet fail  ");
			return -1;
		}
		printf("accpet ok\n");
		if(fork() == 0)
		{
			close(listen_fd);
			pthred_func();
			close(real_fd);
			exit(0);			
		}
		close(real_fd);
	}	
	return 0;
}


int  connect_init(void)
{

    int conn_sock;
    struct sockaddr_in	server_addr;
    conn_sock	= socket(AF_INET, SOCK_STREAM, 0);
    if (conn_sock < 0) {
        perror("socket(2) error");
        return -1;
    }
    (void)memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family		= AF_INET;
    server_addr.sin_port		= htons(SERVER_PORT);
    server_addr.sin_addr.s_addr	= inet_addr(SERVER_IP);

    if (connect(conn_sock,(struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("connect(2) error");
        return -1;
    }
	printf("connect_init ok\n");
	return conn_sock;
}

char* gettime_str(void)
{
	struct tm nowtime;
	struct timeval tv;
	static char time_now[64];
	gettimeofday(&tv, NULL);
	localtime_r(&tv.tv_sec,&nowtime);
	sprintf(time_now,"%02d:%02d:%02d",
		nowtime.tm_hour,
		nowtime.tm_min,
		nowtime.tm_sec
	);
	return time_now;
}

static void searchRdsSync(struct rds_buf_t *rds)
{
	
	while(rds->size >= 376)
	{
		rds->ptr++;
		rds->size--;				
		if(rds->ptr >= rds->tail)
		{
			rds->ptr = rds->head;								
			rds->size = 0;
			break;
		}			 
		if(*rds->ptr == 0x47 && *(rds->ptr + 188) == 0x47) 
		{
			memcpy(rds->head, rds->ptr, rds->size);
			rds->ptr = rds->head;	
			break;
		}		 
	}									
}


static void log_ts_data(unsigned char *src_ts)
{
	static unsigned int err_cnt=0;
	unsigned char *l_id , cnt;
	int date_len;
	COMM_TS_HEAD_TYPE *p_ts = (COMM_TS_HEAD_TYPE*)src_ts;
	date_len= sw16(p_ts->section_len);
	if(date_len < NSTAR_MSG_TITILE_LEN+ 4){
		return ;
	}
#if 0
{
	int i, len;
	unsigned char *log_data;
	len= date_len;
	log_data= src_ts + sizeof(COMM_TS_HEAD_TYPE);
	for(i=0; i< len; i++){
		printf("%02x ", log_data[i]);
		if(len > 0 && (len%32) == 0)
			printf("\r\n");
	}
	printf("\r\n");
}
#endif
	date_len-= NSTAR_MSG_TITILE_LEN+4 ;
	if((p_ts->check_byte&0xf) +(p_ts->check_byte>>0x4) != 24){
		/*验证错误*/
		if((err_cnt++)%100 == 0 ){
			mylog("指令验证错误，计数:%d\r\n", err_cnt);
		}
		return ;
	}
	cnt= p_ts->cnt;
	l_id= p_ts->log_addr;
	switch(p_ts->type){
		case 1:
			while(date_len >=  NSTAR_PARM_LEN && cnt >0){
				mylog("[%s(%s)]%04d, %02x%02x%02x%02x%02x%02x, %02d, %s\n", str_operate_type[p_ts->type],str_operate_lv[p_ts->lv],
					p_ts->pid, l_id[0], l_id[1], l_id[2], l_id[3], l_id[4], l_id[5],
					 date_len, webmsg_nstar_msg_parm((unsigned char*)p_ts));	
				date_len-= NSTAR_PARM_LEN;
				cnt--;
			}
			break;
		case 2:
			if(date_len >=  NSTAR_TEXT_LEN && cnt >0){
				mylog("[%s(%s)]%04d, %02x%02x%02x%02x%02x%02x, %02d, %s\n", str_operate_type[p_ts->type],str_operate_lv[p_ts->lv],
					 p_ts->pid, l_id[0], l_id[1], l_id[2], l_id[3], l_id[4], l_id[5],
					 date_len, webmsg_nstar_msg_text((unsigned char*)p_ts));
				
			}	
			break;
		case 3:
			while(date_len >=  NSTAR_OTHER_CTL_LEN && cnt >0){
				const char *pstr= webmsg_nstar_msg_other_ctl((unsigned char*)p_ts);
				if(pstr != NULL){
					mylog("[%s(%s)]%04d, %02x%02x%02x%02x%02x%02x, %02d, %s\n", str_operate_type[p_ts->type],str_operate_lv[p_ts->lv],
						 p_ts->pid, l_id[0], l_id[1], l_id[2], l_id[3], l_id[4], l_id[5],
						 date_len, pstr);
				}
				date_len-= NSTAR_OTHER_CTL_LEN;
				cnt--;
			}
			break;	
		default:
			break;	

	}

	
}

static void sendRDS(struct rds_buf_t *rds)
{	
	unsigned char *data;
	while(rds->size >= 188)
	{		
		data= rds->ptr;
		rds->ptr++; 
		rds->pid = *rds->ptr++;
		rds->pid <<= 8;
		rds->pid |= *rds->ptr++;
		rds->pid &= 0x1fff;
		rds->ptr += 185;
		log_ts_data(data);
		if(rds->ptr >= rds->tail) 
			rds->size = 0;
    	else 
			rds->size -= 188;		
	}
	if(rds->size > 0) 
	{
		memcpy(rds->head, rds->ptr, rds->size);		
	}	
	rds->ptr = rds->head;	
}

void init_user_rds(void)
{
	static unsigned char buf[RDS_DATA_BUF_SIZE+1];
	int i=0;
	user_rds[i].timer = 0;
	user_rds[i].head = buf;	
	user_rds[i].tail = user_rds[i].head + RDS_DATA_BUF_SIZE;
	user_rds[i].ptr = user_rds[i].head;	
	user_rds[i].size = 0;
	user_rds[i].pid = 0x1fff;
	user_rds[i].send = 1;
	user_rds[i].packCntPerSec = 0;
	user_rds[i].bitRate = 0;
}

int func_parse_ts(int fd)
{
	unsigned char i=0;
	unsigned char *end_ptr = user_rds[i].ptr + user_rds[i].size;
#if 1
	static unsigned int tcnt=0;
	int depth= user_rds[i].tail - end_ptr;
	if((tcnt++)%10==0 && depth!= 40960)
		printf("depth:%04d\n", depth);
#endif
	
	int len= read(fd, end_ptr, user_rds[i].tail - end_ptr);	
	if(len > 0){
		user_rds[i].size += len;																														
		if(user_rds[i].size >= 376 && *user_rds[i].ptr != 0x47) 
			searchRdsSync(&user_rds[i]);
		if(user_rds[i].size >= 188 && *user_rds[i].ptr == 0x47) 
			sendRDS(&user_rds[i]);
		
	}
	return len;
}

void pthred_func(void)
{
	int len;
#if 0	
	server= connect_init();
	if(server < 0)
		return;
#endif
	init_user_rds();
	while(1)
	{
		len= func_parse_ts(real_fd);
		if(len < 0){
			perror("read client failed");
			return ;
		}
	}
}







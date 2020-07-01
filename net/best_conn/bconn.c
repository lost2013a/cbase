#include "bconn.h"
#include "app_delay.h"


#define TEST_IP (192 << 24 | 168 <<16 | 251 <<8 | 170)
#define TEST_PORT 50003

typedef int nstar_handle;



struct bsockt{
	int fd;
	unsigned int lastTimeSec;
	unsigned int sndDtaCnt;
	unsigned int speedCnt;
	unsigned int speed;
	volatile unsigned int ipaddr;
	volatile unsigned short port;
	unsigned char keepSec;
	volatile unsigned char isNeedCreat;
};
#define FD_INVALID 0




static unsigned char rdata[2048];
static struct _net_pg g_netForward = {FD_INVALID,0,0};


static volatile unsigned int command_heart_timer;
#define HEART_TICK_SLEEP(n) app_sleep(&command_heart_timer, n*1000)
#define HEART_TICK_PEND(n) app_pend_wake(command_heart_timer)

static volatile unsigned int command_cnt_timer;
#define CNT_TICK_SLEEP(n) app_sleep(&command_cnt_timer, n*1000)
#define CNT_TICK_PEND(n) app_pend_wake(command_cnt_timer)


static struct bsockt command_net_handle = {FD_INVALID,0,0};

static unsigned int ebm_net_keep_wcnt=0;

#define EBM_NET_KEEP_WCNT_VAL 10


static unsigned char command_heart_ticks_get(unsigned short *ticks)
{
	unsigned ret=0;
	unsigned short n_sec;
	n_sec= ebm_env_get_heart_tp();
	if(n_sec != 0)
		ret =1;
	
	if(n_sec < 10 ){
		*ticks= 10;
	}
	else{
		*ticks= n_sec;
	}
	return ret;
}

void command_hearttick_wcnt_clear(void)
{
	ebm_net_keep_wcnt= EBM_NET_KEEP_WCNT_VAL;
}

static unsigned char command_hearttick_wcnt_tick(void)
{
	unsigned char ret=0;
	if(ebm_net_keep_wcnt == 1)
		ret= 1;
	if(ebm_net_keep_wcnt > 0)
		ebm_net_keep_wcnt--;
	return ret;
}


static void _socket_close(int *fd)
{
	if(*fd != FD_INVALID){
		close(*fd);
		*fd = FD_INVALID;
	}
}

#if 1
//add for protect network
#define MAX_SOCKET_WCNT 20
static volatile unsigned int socket_wcnt=0;
void socket_wcnt_add(void)
{
extern void nstar_adt_restart(void);

	if(socket_wcnt< MAX_SOCKET_WCNT){
		socket_wcnt++;
		printf("sys socket wcnt :%d / %d\n", socket_wcnt, MAX_SOCKET_WCNT);
	}
	else{
		printf("sys socket wcnt too much, reboot now\n");
		socket_wcnt=0;
		nstar_adt_restart();
	}
}

void socket_wcnt_clear(void)
{
	socket_wcnt=0;
}

#endif


static int _socket_open(unsigned int ip, unsigned short port)
{
    int conn_sock;
    struct sockaddr_in	server_addr;
    conn_sock	= socket(AF_INET, SOCK_STREAM , 0);
    if (conn_sock < 0) {
	    printf("command socket creat error\n");
		socket_wcnt_add();
	    return  FD_INVALID;
    }
	socket_wcnt_clear();
    (void)memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family		= AF_INET;
    server_addr.sin_port		= htons(port);
   	server_addr.sin_addr.s_addr = htonl(ip);
	if (connect(conn_sock,
            (struct sockaddr *)&server_addr,
            sizeof(server_addr)) >= 0) {
        printf("command connect ok\n");
		sys_log("指令网络连接成功");
  
	}
	else{
		ebmdbg_command("wait connect to command addr failed %d.%d.%d.%d:%d\n",
				ip>>24, (ip&0xff0000)>>16, (ip&0xff00)>>8, ip&0xff, port);
		close(conn_sock);
		nstar_sys_net_restore();
		conn_sock= FD_INVALID;
	}	
	return conn_sock;
}


static unsigned char command_creat_connect_socket(struct bsockt *h_net)
{
	int fd=0;
	unsigned int ip;
	unsigned short port;
	unsigned char ret = 0;
	_socket_close(&h_net->fd);

	ip = h_net->ipaddr;
	port = h_net->port;	
	if(ip > 0 && port > 0 ){
		fd = _socket_open(ip, port);
		if(fd  != FD_INVALID ){
			command_hearttick_wcnt_clear();
			h_net->fd = fd;
			h_net->lastTimeSec = 0;
			h_net->keepSec = 0;
			ret = 1;	
		}
	}	
	return ret;
}


unsigned char command_net_send(unsigned char *data, unsigned int len)
{
	int ret= write(command_net_handle.fd, data, len);
	if(ret <= 0){
		printf("command send err=%d\n", ret);
		command_net_restart();
	}
	return 0;
}


static void command_block_rec(unsigned int fd)
{
	static unsigned char rbuf[1460];
	int	nsel;
	struct	timeval timeover={0};
	fd_set rfd_set; 
	timeover.tv_sec = 0;
	timeover.tv_usec = 10*1000;
	FD_ZERO( &rfd_set );
	FD_SET(fd, &rfd_set );
	nsel = select( fd+1, &rfd_set, NULL, NULL, &timeover );
	if ( nsel > 0 )
	{
		if(FD_ISSET( fd, &rfd_set )){
			int rlen = read(fd, rbuf, 1460);
			if(rlen <= 0){
				printf("command socket unvaild\n");
				command_net_restart();
			}
			else{
				handle_ebm_msg(rbuf, rlen);
			}
				
		}
	}			
}

static void command_data_process(void)
{
	command_block_rec(command_net_handle.fd);
}



void command_send_hearttick(void)
{
static unsigned int last_session_id= 1;

	unsigned char data[200];
	unsigned char *pdata;
	MSG_FIXED_HEADER *msg;
	HEART_TICK_CONTENT* business;
	msg= (MSG_FIXED_HEADER*)data;

	msg->head_flag= 0xFDFE;
	msg->version= 0x0001;
	msg->session_id= swap32(last_session_id);
	last_session_id++;
	msg->type= MSG_TYPE_REQ;
	msg->sign_flag= 0;
	msg->data_len= swap16(0x82);
	
	ebm_env_get_sourceid(msg->src_socde, 12);
	
	msg->items=swap16(1);
	memset(msg->items_arry, 0, 12);
	
	msg->business_type= PASSBACK_HEARTTICK;/*需要更新*/
	msg->business_len= swap16(0x09);
	business= (HEART_TICK_CONTENT*)&msg->business_data;

	business->sta= ebm_env_get_playsta();
	business->register_1st_time= ebm_env_get_register_1st_time();
	business->phyid_len=6;
	
	ebm_env_get_phyid(business->phyid, business->phyid_len);
	
	pdata= business->phyid;
	pdata+=6;
	pdata[0]= 0x00;
	pdata[1]= 0x4a;

	memset(pdata+2, 0, 0x4a);

	unsigned int *p_crc= (unsigned int*) (data+ 126);

	unsigned int crc_val= nstar_crc32_ts(data, 126);
	*p_crc= swap32(crc_val);

	command_net_send(data, 130);
	if(1)
	{
		int i;
		ebmdbg_command("send heart msg:\n");
		for(i=0; i< 130;i++){
			ebmdbg_command("%02X ", data[i]);
		}
		ebmdbg_command("\n");
	}
}



static void command_net_machine(void *parm)
{
	struct bsockt *h_net= (struct bsockt *)parm;
	if(h_net->isNeedCreat == 3){
		h_net->isNeedCreat = 0;
		_socket_close(&h_net->fd);
	}
	else if(h_net->isNeedCreat == 1){
		if(1 == command_creat_connect_socket(h_net)){
			h_net->isNeedCreat = 0;
		}
		else{
			h_net->isNeedCreat = 2;
			h_net->lastTimeSec = app_get_systime();
		}
	}
	else if(h_net->isNeedCreat == 2){
		if(h_net->lastTimeSec + 10*1000 < app_get_systime()){
			if(1 == command_creat_connect_socket(h_net)){
				h_net->isNeedCreat = 0;
			}
			else{
				h_net->lastTimeSec = app_get_systime();
			}
		}
	}
}

void command_net_start(unsigned int remote_ip, unsigned short remote_port)
{
	struct bsockt *h_net= &command_net_handle;
	h_net->isNeedCreat = 1;
	h_net->ipaddr= remote_ip;
	h_net->port= remote_port;
}
void command_net_stop(void)
{
	struct bsockt *h_net= &command_net_handle;
	h_net->isNeedCreat = 3;
}

void command_net_restart(void)
{
	struct bsockt *h_net= &command_net_handle;
	if(h_net->fd != FD_INVALID){
		_socket_close(&h_net->fd);
		h_net->isNeedCreat = 2;
		h_net->lastTimeSec = app_get_systime() + 2*1000;
	}
}

extern unsigned char led_visit_rtp_sta(void);
static void led_visit_worksta(void)
{
	if(command_net_handle.fd != FD_INVALID ){
		if(1 == led_visit_rtp_sta()){
			led_ip_work_on();
		}
		else{
			led_ip_work_blink();
		}
	}
	else
		led_ip_work_idle();
}

void command_net_loop(void)
{

	if(CNT_TICK_PEND()){
		command_net_machine(&command_net_handle);
		led_visit_worksta();
		CNT_TICK_SLEEP(2);
	}
	
	if(command_net_handle.fd != FD_INVALID)
	{
		command_data_process();
		if(HEART_TICK_PEND())
		{
			unsigned char is_vaild; unsigned short ticks;
			command_send_hearttick();
			is_vaild= command_heart_ticks_get(&ticks);
			if(is_vaild && command_hearttick_wcnt_tick()){
				printf("command net wcnt time out\n");
				sys_log("指令网络心跳超时");
				command_net_restart();
				passback_net_restart();
			}
			HEART_TICK_SLEEP(ticks);
		}
		
	}
	usleep(10*1000);
}


const char* visit_command_net_sta(void)
{
	const char *sta_str[]={
		"未连接",
		"心跳超时",
		"正常",
	};
	unsigned char ret=0 ;
	if(command_net_handle.fd != FD_INVALID ){
		if(ebm_net_keep_wcnt >= EBM_NET_KEEP_WCNT_VAL/2 )
			ret=2;
		else
			ret=1;
	}
	return sta_str[ret];
		
}








int main(int argc, char *argv[])
{
	nstar_net_forward_need_creat();
	while(1){
		nstar_net_forward_loop();
		usleep(10*1000);
	}
	return 0;
}





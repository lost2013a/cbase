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



typedef struct nstar_ts_text{
	unsigned short type;
	unsigned char content_len;
	unsigned char content[144];
}__attribute__((packed))NSTAR_TEXT_TYPE;


typedef struct comm_ts_head{
	unsigned char sync_byte;
	unsigned short pid;	/*mask :0x1FFF*/
	unsigned char order:4;	
	unsigned char unused1:4;
	unsigned char unused2;
	unsigned char table_id;
	unsigned short section_len;/*数据长度 包含CRC， mask :0x0FFF*/
	unsigned char check_byte;  /*h+l= 24*/
	unsigned char cnt; 		/*指令次数*/
	unsigned char lv:3;		/*指令等级*/
	unsigned char bit:3;	/*指令位移*/
	unsigned char type:2;	/*指令类型*/
	unsigned char log_addr[6];
}__attribute__((packed))COMM_TS_HEAD_TYPE;


typedef struct nstar_ts_parm{
	COMM_TS_HEAD_TYPE head;
	unsigned int ser_id;
	unsigned char phyid[6];
	unsigned char type;
	unsigned char program_search:1;
	unsigned char vol:7;
	unsigned char work_freq[3];
	unsigned char logid[6];
	unsigned char remain_cycle;
	unsigned char sys_ctl_update:1;
	unsigned char sys_ctl_restore:1;
	unsigned char sys_ctl_reboot:1;
	unsigned char sys_ctl_pri:1;
	unsigned char sys_ctl_reserve:4;
	unsigned short update_road;
	unsigned short sign_rate:14;
	unsigned short work_mode:2;
	unsigned char t_fft:2;
	unsigned char t_workmode:3;
	unsigned char t_fmkmode:3;
	unsigned char t_parm[3];
}__attribute__((packed))NSTAR_PARM_TYPE;


typedef struct nstar_ts_text{
	COMM_TS_HEAD_TYPE head;
	unsigned int ser_id;
	unsigned char phyid[6];
	unsigned char type;
	unsigned char program_search:1;
	unsigned char vol:7;
	unsigned char work_freq[3];
	unsigned char logid[6];
	unsigned char remain_cycle;
	unsigned char sys_ctl_update:1;
	unsigned char sys_ctl_restore:1;
	unsigned char sys_ctl_reboot:1;
	unsigned char sys_ctl_pri:1;
	unsigned char sys_ctl_reserve:4;
	unsigned short update_road;
	unsigned short sign_rate:14;
	unsigned short work_mode:2;
	unsigned char t_fft:2;
	unsigned char t_workmode:3;
	unsigned char t_fmkmode:3;
	unsigned char t_parm[3];
}__attribute__((packed))NSTAR_PARM_TYPE;



typedef struct nstar_ts_other{
	COMM_TS_HEAD_TYPE head;
	unsigned short type;
	unsigned char content_len;
	unsigned char content[144];
}__attribute__((packed))NSTAR_OTHER_CTL_TYPE;


#define NSTAR_MSG_TITILE_LEN (9)
#define NSTAR_PARM_LEN  (sizeof(NSTAR_PARM_TYPE)-sizeof(COMM_TS_HEAD_TYPE))
#define NSTAR_OTHER_CTL_LEN  (sizeof(NSTAR_OTHER_CTL_TYPE)-sizeof(COMM_TS_HEAD_TYPE))







const char* webmsg_nstar_msg_parm(unsigned char* data);
const char* webmsg_nstar_msg_other_ctl(unsigned char* data);




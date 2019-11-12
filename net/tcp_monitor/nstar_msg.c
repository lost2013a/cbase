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

#define OTHER_CTL_MAX_TYPE 41
const char* str_nstar_other_ctl[]={
	"无效",
	"GPRS开关",
	"LED开关控制",
	"LED长宽设置",
	"GPRS注册IP设置",
	"GPRS注册URL设置",
	"LED字体设置",
	"LED透传内容",
	"LED分区设置",
	"LED时间/亮度调节",
	"LED边框设置",
	"LED校时",
	"LED日历设置",
	"LED显示/关屏",
	"LED播放属性",
	"TTS参数设置",
	"操作有效设置",	
	"时间同步",		
	"第三通道开关", 
	"STB文字参数",
	"接收优先级",
	"会议系统参数",
	"对讲参数",
	"对讲验证码",
	"PID设置",
	"模式设置",
	"闪灯设置",
	"锁频地址",
	"主控连接开关",
	"IP地址设置",
	"注册地址设置",
	"终端打印开关",
	"蜂鸣器开",
	"版本开关",
	"音频节目信息",
	"IP节目信息",
	"放音持续时间",
	"MP3节目",	
	"机顶盒提示文字",
	"提示文字类型",
	"锁址信息",
	"升级流IP地址",
	"未知指令",
};

#define PARM_MAX_TYPE 19
const char* str_nstar_parm[]={
	"音量",
	"节目收搜",
	"工作主频",
	"逻辑地址",
	"维持周期",
	"优先级",
	"重启指令",
	"恢复出厂配置",
	"升级指令",
	"升级通道",
	"工作模式",
	"符号率",
	"调试方式",
	"运行模式",
	"载波数",
	"帧头模式",
	"调制方式",
	"FEC码率",
	"时域交织",
	"未知指令",
};

#define TEXT_EM_MAX_TYPE 4
const char* str_nstar_text_em[]={
	"普通",
	"应急",
	"插播",
	"只播音频",
	"未知",
};


#define SW_EM_MAX_TYPE 2
const char* str_nstar_sw_em[]={
	"普通",
	"应急",
};

#define SW_STA_MAX_TYPE 4
const char* str_nstar_sw_sta[]={
	"维持",
	"开启",
	"关闭",
	"演练",
};


const char* webmsg_nstar_msg_parm(unsigned char* data)
{
#define MAX_LEN 60
	static char buf[MAX_LEN];
	unsigned char type;
	NSTAR_PARM_TYPE *pmsg;
	pmsg= (NSTAR_PARM_TYPE*)data;
	type= pmsg->type;
	if(type > PARM_MAX_TYPE)
		type= PARM_MAX_TYPE+1;
	switch(type){
		default:
			snprintf(buf, MAX_LEN, "参数：%s",  str_nstar_parm[type]);
			break;
	}
	return buf;
}


const char* webmsg_nstar_msg_other_ctl(unsigned char* data)
{
#define MAX_LEN 60
	static char buf[MAX_LEN];
	unsigned char type;
	NSTAR_OTHER_CTL_TYPE *pmsg;
	pmsg= (NSTAR_OTHER_CTL_TYPE*)data;
	type= sw16(pmsg->type);
	if(type > OTHER_CTL_MAX_TYPE)
		type= OTHER_CTL_MAX_TYPE+1;
	switch(type){
		case 13:
			if(pmsg->content[0] == 0){
				snprintf(buf, MAX_LEN, "指令：LED关屏", pmsg->content_len);
			}			
			else if(pmsg->content[0] == 2){
				snprintf(buf, MAX_LEN, "指令：LED清除节目", pmsg->content_len);
			}
			else {
				#if 0				
					snprintf(buf, MAX_LEN, "指令：LED节目列表发送%s, 指令长度：%02d",  
						pmsg->content[1] == 0?"开始":"结束", pmsg->content_len);
				#else
					return NULL;
				#endif
			}
			break;
		default:
			snprintf(buf, MAX_LEN, "指令：%s, 指令长度：%02d",  
				str_nstar_other_ctl[type], pmsg->content_len);
			break;
	}
	return buf;
}


const char* webmsg_nstar_msg_text(unsigned char* data)
{
#define MAX_TEXT_LEN 100
	static char buf[MAX_TEXT_LEN];
	char tmp[20];
	unsigned char type;
	NSTAR_TEXT_TYPE *pmsg;
	pmsg= (NSTAR_TEXT_TYPE*)data;
	type= pmsg->act_em;
	if(type > TEXT_EM_MAX_TYPE)
		type= TEXT_EM_MAX_TYPE+1;
	switch(type){
		default:
			if(pmsg->repet_cnt == 0)
				snprintf(tmp, 20, "定时播放");
			else
				snprintf(tmp, 20, "播放%d次", pmsg->repet_cnt);
			
			snprintf(buf, MAX_TEXT_LEN, "节目%d[%s], 帧序%d/%d, 帧长%02d, 时间%02d:%02d:%02d-%02d:%02d:%02d, %s",    
				sw16(pmsg->act_id), str_nstar_text_em[type], pmsg->pack_id+1, pmsg->pack_cnt, pmsg->pack_len,
				pmsg->start_h, pmsg->start_m, pmsg->start_s, pmsg->end_h, pmsg->end_m, pmsg->end_s, tmp);
			break;
	}
	return buf;
}



#include "app_delay.h"


unsigned char app_capture_interval(volatile unsigned int *ptcnt, unsigned int u_ms);
void app_update_interval(volatile unsigned int *ptcnt);

static volatile unsigned int hear_tcnt=0;
#define REC_HEAR_TICK 1
#define HEART_FLOW_TIME 3000
#define TIMER_UPDATE_HEART_TCIK() app_update_interval(&hear_tcnt)
#define TIMER_GET_FLOW_HEART_TCIK() app_capture_interval(&hear_tcnt, HEART_FLOW_TIME)


static unsigned char s_nb_program_vaild=0,s_last_nb=0,s_refresh_flag=0;
unsigned char get_ts_program(void)
{
	return s_nb_program_vaild;
}
void reduce_ts_refresh_flag(void)
{
	if(s_refresh_flag > 0)
		s_refresh_flag-=1;
}
unsigned char get_ts_refresh_flag(void)
{
	return s_refresh_flag;
}
static void set_ts_refresh_flag(void)
{
	s_refresh_flag= 5;
}



const char* webmsg_nstar_msg_onoff(unsigned char* data, unsigned short len, unsigned char cnt)
{
#define MAX_STA_LEN 1000
	static char buf[MAX_STA_LEN];	
	unsigned char*p_aid, nb=0;
	int idx=0, reman_len;
	NSTAR_SWITCH_BODY_TYPE *pmsg;
	/*cut the head*/
	data+= 10;
	len+=7;
	while(cnt>0  && len>= sizeof(NSTAR_SWITCH_BODY_TYPE))
	{
		pmsg= (NSTAR_SWITCH_BODY_TYPE*)data;
		p_aid= pmsg->log_addr;
		reman_len= MAX_STA_LEN-idx;
		if(reman_len > 1){
			if(nb > 12)
				break;
			idx+= snprintf(buf+idx, reman_len, "%02d.[%s]节目地址%02x%02x-%02x%02x-%02x%02x,节目号%03d%s",
				nb+1, str_nstar_sw_em[pmsg->em_flag],p_aid[0], p_aid[1], p_aid[2], p_aid[3], p_aid[4], p_aid[5], 
				sw16(pmsg->server_id),  ((nb%2)==1)? "\r\n": "\t\t");
			nb++;
		}
		cnt--;
		len-= sizeof(NSTAR_SWITCH_BODY_TYPE);
		data+= sizeof(NSTAR_SWITCH_BODY_TYPE);
	}
	if(nb < s_last_nb)
		s_nb_program_vaild= s_last_nb;
	else
		s_nb_program_vaild=nb;
	s_last_nb= nb;
	if(nb==0)
		return "暂无节目\r\n";
	reman_len= MAX_STA_LEN-idx;
	if(reman_len > 1){
		idx+= snprintf(buf+idx, reman_len, "\r\n" );
	}
	set_ts_refresh_flag();
	return buf;
}



void *nstar_loop_check_sta(void *parm)
{
	while(1)
	{
		if(TIMER_GET_FLOW_HEART_TCIK()){
			
			printf("time out\r\n");
		}
		usleep(500*1000);
	}
}


const char* webmsg_nstar_msg_heartick(unsigned char* data, unsigned short len)
{
#define MAX_TEXT_LEN 100
	static char buf[MAX_TEXT_LEN];	
	if(len == 60){
		unsigned char*p_aid;
		NSTAR_HEART_TICK_TYPE *pmsg;
		pmsg= (NSTAR_HEART_TICK_TYPE*)data;
		p_aid= pmsg->area_info;
		snprintf(buf, MAX_TEXT_LEN, "锁频地址：%02x%02x-%02x%02x-%02x%02x， 心跳版本:%d",
			p_aid[0], p_aid[1], p_aid[2], p_aid[3], p_aid[4], p_aid[5], pmsg->version);

	}
	else{

		snprintf(buf, MAX_TEXT_LEN, "心跳无");
			
	}
	return buf;
}






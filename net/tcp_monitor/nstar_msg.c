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

#define OTHER_CTL_MAX_TYPE 15
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
	"保留指令",
};



const char* webmsg_nstar_msg_parm(unsigned char* data)
{
#define MAX_LEN 60
	static char buf[MAX_LEN];
	unsigned char type;
	NSTAR_PARM_TYPE *pmsg;
	pmsg= (NSTAR_PARM_TYPE*)data;
	type= pmsg->type%PARM_MAX_TYPE;
	switch(type){
		default:
			snprintf(buf, 60, "参数：%s",  str_nstar_parm[type]);
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
	type= pmsg->type%OTHER_CTL_MAX_TYPE;
	switch(type){
		case 13:
			if(pmsg->content[0] == 0){
				snprintf(buf, 60, "指令：LED关屏, 指令长度：%02d", pmsg->content_len);
			}
			else if(pmsg->content[0] == 1){
				snprintf(buf, 60, "指令：LED节目列表发送%s, 指令长度：%02d",  
					pmsg->content[1] == 0?"开始":"结束", pmsg->content_len);	
			}
			break;
		default:
			snprintf(buf, 60, "指令：%s, 指令长度：%02d",  
				str_nstar_other_ctl[type], pmsg->content_len);
			break;
	}
	return buf;
}

const char* webmsg_nstar_msg_text(unsigned char* data, unsigned int len)
{
#define MAX_LEN 60
	static char buf[MAX_LEN];
	unsigned char type;
	NSTAR_OTHER_CTL_TYPE *pmsg;
	pmsg= (NSTAR_OTHER_CTL_TYPE*)data;
	type= pmsg->type%OTHER_CTL_MAX_TYPE;
	snprintf(buf, MAX_LEN, "%s, %02d字节",  str_nstar_other_ctl[type], pmsg->content_len);
	return buf;
}







#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "zcx_msg.h"
#include "app_delay.h"

extern void fm_uart_send(unsigned char *data, unsigned int len);
extern int fm_uart_read(unsigned char *data, unsigned int len);
extern void arrry_print(unsigned char *data, unsigned int len);

#define zcx_dbg printf
#define _fm_cmm_write 

static struct usart_machine *gp_zcxCmd;
static void zcx_msg_parse(unsigned char*p_data, unsigned short data_len);

static struct zxc_env h_env;
static struct zxc_env *p_env= &h_env;
static struct zxc_sta h_sta;
static struct zxc_sta *p_sta= &h_sta;



#define _STEP_SET_0()	do{gp_zcxCmd->step = gp_zcxCmd->recvLen = 0;}while(0)
#define _STEP_ADD_1()	do{gp_zcxCmd->step++;}while(0)

static unsigned short zcx_crc16( unsigned char *data, unsigned int size)
{
    unsigned short i,j;
    unsigned short crc = 0;
	if(size > 1024){
		printf("data too long\n");
		return 0;
	}
    for( i = 0; i < size; i++ )
    {
        crc = crc ^ (unsigned short)data[i] << 8;
        for( j = 0; j < 8; j++ )
            if( crc & 0x8000)
                crc = crc << 1 ^ 0x1021;
            else
                crc = crc << 1;
    }
    return crc;
}

void _zcx_data_parse(unsigned char data)
{
	gp_zcxCmd->p_dta[gp_zcxCmd->recvLen++]=data;
	switch(gp_zcxCmd->step)
	{
		case 0:
			if(data == 0xa5)
				gp_zcxCmd->step = 1;	
			else 
				gp_zcxCmd->recvLen = 0;
			break;
		case 1:
			gp_zcxCmd->allLen= data<<8;
			_STEP_ADD_1();
			break;
		case 2:
			gp_zcxCmd->allLen |= data;
			gp_zcxCmd->allLen += ZXC_MSG_PACK_LEN;
			if(gp_zcxCmd->allLen <= _ZCX_DTA_MAX_LEN || gp_zcxCmd->allLen > 5){			
				_STEP_ADD_1();				
			}
			else{
				_STEP_SET_0();
				printf("zxc data len err: %d byte\n", gp_zcxCmd->allLen);
			}
			break;
		case 3:
			if(gp_zcxCmd->recvLen  >= gp_zcxCmd->allLen)
			{
				unsigned short len= gp_zcxCmd->recvLen;
				unsigned short cal_crc= zcx_crc16(gp_zcxCmd->p_dta, len-2);
				unsigned short rec_crc= gp_zcxCmd->p_dta[len-2] << 8 | gp_zcxCmd->p_dta[len-1];
				gp_zcxCmd->p_dta[gp_zcxCmd->recvLen] = 0;
				if(cal_crc != rec_crc)
				{
					printf("zxc data crc wrong %04x / %04x\n", cal_crc, rec_crc);
				}
				//else
				{
					if(DBG_FM_UART){
						printf("zxc rec %02d/%02d: ", len, gp_zcxCmd->allLen);
						arrry_print(gp_zcxCmd->p_dta, gp_zcxCmd->recvLen);
					}
					zcx_msg_parse(gp_zcxCmd->p_dta+3, gp_zcxCmd->recvLen-5);
				}	
				_STEP_SET_0();
			}	
			break;		
		default:
			break;
	}
}


static unsigned short parm_parse(unsigned char*p_data, unsigned short data_len)
{
	
	struct parm_head *parm= (struct parm_head*)p_data;
	unsigned short parm_len= swap16(parm->parm_len);
	if(parm_len+3 > data_len){
		printf("parm len too long %d / %d\n", parm_len+3, data_len);
		return 0;
	}
	else if(parm_len == 0){
		printf("parm len is 0\n");
		//return 0;
	}

	switch(parm->type){
		case ANALOG_CMD_STATUS:
			if(parm_len == 1){
				p_sta->lock_cnt= LOCK_CNT_NUMB;
				if(parm->data == 0x82){
					p_sta->sta= ZXC_STA_DAILY;
				}
				else if(parm->data == 0x80){
					p_sta->sta= ZXC_STA_EMERGENCY;
				}
			}
			
			break;
		case ANALOG_CMD_STOP:
			if(parm_len == 1){
				p_sta->lock_cnt= LOCK_CNT_NUMB;
				p_sta->sta= ZXC_STA_IDLE;
			}		
			break;
		case ANALOG_CMD_VOLUME:
			if(parm_len == 1){
				printf("vol: %d\n", parm->data);
			}
			break;				
		case ANALOG_CMD_FRE:
			if(parm_len == 3){
				unsigned char *id= &parm->data;
				printf("fq: ");
				for(int i=0; i< 3; i++){
					printf("%02x", id[i]);	
				}
				printf("\n");
			}
			break;
		case ANALOG_CMD_LOGIC_ADDR:
            if(parm_len == 12){
				unsigned char *id= &parm->data;
				printf("logic id: ");
				for(int i=0; i< 12; i++){
					printf("%02x", id[i]);	
				}
				printf("\n");
			}
			break;
		case ANALOG_CMD_MAC_ADDR:
			if(parm_len == 6){
				unsigned char *id= &parm->data;
				printf("phy id: ");
				for(int i=0; i< 6; i++){
					printf("%02x", id[i]);	
				}
				printf("\n");
			}
			break;
		case ANALOG_CMD_SIGNAL_STRENGTH:
			if(parm_len == 1){
				printf("sq: %d\n", parm->data);
			}
			break;	
		case ANALOG_CMD_DATA_TRANS:
		case ANALOG_CMD_GET_PARAM:
		default:
			printf("undefined type %02x %d\n", parm->type, parm_len);
			break;
	}
	return parm_len+3;
}



static void zcx_msg_parse(unsigned char*p_data, unsigned short data_len)
{
	unsigned short parm_len;
	while(data_len >= ZXC_PARM_HEAD_LEN){
		parm_len= parm_parse(p_data, data_len);
		//printf("parm len %d\n", parm_len);
		if(0 != parm_len){
			p_data+= parm_len;
			data_len-= parm_len;
		}
		else{
			break;
		}
	}
	if(data_len!=0){
		printf("parm len is't 0 after parse %d\n", data_len);
	}
}

static void send_fram(unsigned char *data, unsigned short len)
{
	unsigned short send_crc;
	data[0]= ZXC_MSG_HEAD;
	len+= ZXC_MSG_PACK_LEN; //add type len 1 byte
	send_crc= zcx_crc16(data, len-2);
	data[len-2]= send_crc >> 8;
	data[len-1]= send_crc&0xff;
	printf("send msg #%02x [%d]: \n", data[3], len);
	arrry_print(data, len);
	fm_uart_send(data, len);
}

static void zxc_send_parm_phyid(void)
{
	unsigned char *phyid= p_env->phy_id;
	
	unsigned char buf[30];
	struct zcx_parm_one *msg= (struct zcx_parm_one*)buf;
	
	unsigned short parm_len= 6;
	unsigned short msg_len= ZXC_PARM_PACK_LEN+parm_len;

	msg->type= ZCX_SET_PARM_PHYID;
	msg->len= swap16(msg_len);
	msg->parm_len= swap16(parm_len);
	memcpy(&msg->data, phyid, parm_len);
	send_fram(buf, msg_len);
}

static void zxc_send_parm_fre(void)
{
	unsigned int fre;	
	unsigned char buf[30], *p_fre;
	struct zcx_parm_one *msg= (struct zcx_parm_one*)buf;
	
	unsigned short parm_len= 3;
	unsigned short msg_len= ZXC_PARM_PACK_LEN+parm_len;

	msg->type= ZCX_SET_PARM_FRI;
	msg->len= swap16(msg_len);
	msg->parm_len= swap16(parm_len);
	fre= swap32(p_env->main_fq);	
	p_fre= (unsigned char*)&fre;
	p_fre+=1;
	memcpy(&msg->data, p_fre, parm_len);
	send_fram(buf, msg_len);
}


static void zxc_send_parm_logid(void)
{
	unsigned char *logid= p_env->logic_id;
	unsigned char buf[30];
	struct zcx_parm_one *msg= (struct zcx_parm_one*)buf;
	
	unsigned short parm_len= 12;
	unsigned short msg_len= ZXC_PARM_PACK_LEN+parm_len;

	msg->type= ZCX_SET_PARM_LOGID;
	msg->len= swap16(msg_len);
	msg->parm_len= swap16(parm_len);
	memcpy(&msg->data, logid, parm_len);
	send_fram(buf, msg_len);
}

static void zxc_send_parm_vol(void)
{
	unsigned char vol=p_env->vol;
	unsigned char buf[30];
	struct zcx_parm_one *msg= (struct zcx_parm_one*)buf;
	
	unsigned short parm_len= 1;
	unsigned short msg_len= ZXC_PARM_PACK_LEN+parm_len;

	msg->type= ZCX_SET_PARM_VOL;
	msg->len= swap16(msg_len);
	msg->parm_len= swap16(parm_len);
	memcpy(&msg->data, &vol, parm_len);
	send_fram(buf, msg_len);
}

static void zxc_send_get_parm(void)
{
	const unsigned char data=0xff;
	unsigned char buf[30];
	struct zcx_parm_one *msg= (struct zcx_parm_one*)buf;
	
	unsigned short parm_len= 1;
	unsigned short msg_len= ZXC_PARM_PACK_LEN+parm_len;

	msg->type= ZCX_SET_GET_PARM;
	msg->len= swap16(msg_len);
	msg->parm_len= swap16(parm_len);
	memcpy(&msg->data, &data, parm_len);
	send_fram(buf, msg_len);
}


static unsigned int flag_send_parm=0;
void flag_set_bit(unsigned char bit)
{
	if(bit < 32){
		flag_send_parm|= 0x1 << bit;
	}	
}
void flag_clear_bit(unsigned char bit)
{
	if(bit < 32){
		flag_send_parm&= ~(0x1 << bit);
	}	
}

void _fm_zcx_init()
{
	gp_zcxCmd = (struct usart_machine*)malloc(sizeof(struct usart_machine));
	gp_zcxCmd->p_dta = malloc(_ZCX_DTA_MAX_LEN+1);
	
	zxc_send_get_parm();

	zcx_dbg(" _fm_zcx_init ok\n");
}


void fm_send_handle(void)
{
	void(*func_arry[])(void)={
		zxc_send_parm_fre,
		zxc_send_parm_vol,
		zxc_send_parm_logid, 	
		zxc_send_parm_phyid,
		NULL,
	};	
#define MSG_GAPS 20 //单位10ms+
	static unsigned char cnt=0;
	if(cnt++ > MSG_GAPS){
		cnt=MSG_GAPS/5;
		if(flag_send_parm != 0){
			unsigned char i;
			for(i=0; i< FLAG_BIT_MAX; i++){
				if(flag_send_parm& (0x1 << i)){
					
					break;
				}
			}

			if(i < FLAG_BIT_MAX){
				printf("flag set %d\n", i);
				if(func_arry[i] != NULL){
					func_arry[i]();
					cnt=0;
				}
				flag_clear_bit(i);	
			}
		}
	}
}


#define ZXC_STA_INIT 0
#define ZXC_STA_UNLOCK 1
#define ZXC_STA_IDLE 2
#define ZXC_STA_DAILY 3
#define ZXC_STA_EMERGENCY 4

static void STA_STEP(unsigned char step)
{
	p_sta->step= step;
	printf("zxc step to %d\n", step);
}

static volatile unsigned int zxc_sta_timer;
#define STA_TICK_SLEEP(n) app_sleep(&zxc_sta_timer, n*1000)
#define STA_TICK_PEND() app_pend_wake(zxc_sta_timer)



static void zxc_sta_machine(void)
{
	if(STA_TICK_PEND()){
		if(p_sta->lock_cnt > 0){
			p_sta->locked=1;
			p_sta->lock_cnt--;
		}
		else{
			p_sta->locked=0;
		}
		STA_TICK_SLEEP(1);
	}
}


unsigned char visit_zxc_sta(void)
{
	static volatile unsigned int zxc_printf_timer;
#define PRINTF_TICK_SLEEP(n) app_sleep(&zxc_printf_timer, n*1000)
#define PRINTF_TICK_PEND() app_pend_wake(zxc_printf_timer)

	const char *str_str[]={
		"#0 init",
		"#1 unlock",
		"#2 idle",
		"#3 daily",
		"#4 emergency",
	};
	static unsigned char last_sta=0;
	unsigned char currrent_sta;
	
	if(p_sta->locked == 0){
		currrent_sta=ZXC_STA_UNLOCK;
	}
	else{
		currrent_sta= p_sta->sta;
	}
	
	if(currrent_sta != last_sta){
		printf("zxc sta: %s\n", str_str[currrent_sta]);
	}
	else if(PRINTF_TICK_PEND()){
		printf("zxc sta: %s\n", str_str[currrent_sta]);
		//zxc_send_get_parm();
		PRINTF_TICK_SLEEP(10);
	}
	last_sta= currrent_sta;


	

}

void zxc_fm_run_sevo(void)
{
	unsigned char buf[128];
	int len, i;
	while(1)
	{
		len= fm_uart_read(buf, 128);
		if(len > 0){
			for(i = 0; i<len; i++){
				_zcx_data_parse(buf[i]);
			}
		}
		fm_send_handle();
		zxc_sta_machine();
		visit_zxc_sta();
		usleep(10*1000);
	}
}


void zxc_env_set_phyid(unsigned char *phyid, unsigned int len)
{
	if(len == 6 && 0!=memcmp(p_env->phy_id, phyid, 6)){
		memcpy(p_env->phy_id, phyid, len);
		flag_set_bit(FLAG_BIT_PHYID);
	}
}

void zxc_env_set_fre(unsigned int fre)
{
	if(fre != p_env->main_fq){
		p_env->main_fq= fre;
		flag_set_bit(FLAG_BIT_FRE);
	}
}


void zxc_env_set_logid(unsigned char *logid, unsigned int len)
{
	if(len == 12 && 0!=memcmp(p_env->logic_id, logid, 12)){
		memcpy(p_env->logic_id, logid, len);
		flag_set_bit(FLAG_BIT_LOGID);
	}
}

void zxc_env_set_vol(unsigned char vol)
{
	if(vol > 99)
		vol =99;
	if(vol != p_env->vol){
		p_env->vol= vol;
		flag_set_bit(FLAG_BIT_VOL);
	}
}




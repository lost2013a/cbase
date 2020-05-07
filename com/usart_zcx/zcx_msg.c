#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "zcx_msg.h"

extern void fm_uart_send(unsigned char *data, unsigned int len);

#define zcx_dbg printf
#define _fm_cmm_write 

static struct _zcx_cmd *gp_zcxCmd;

#define _STEP_SET_0()	do{gp_zcxCmd->step = gp_zcxCmd->recvLen = 0;}while(0)
#define _STEP_ADD_1()	do{gp_zcxCmd->step++;}while(0)

static unsigned int ticktime_get(void)
{
#define TICKS_PER_SEC 10
#include<sys/times.h>
	unsigned int ticks = (unsigned int)times( NULL);
	return (unsigned int)ticks*TICKS_PER_SEC;
}

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

static unsigned int ticktime_getsec(void)
{
	return ticktime_get()/1000;
}




static void _zcx_gprs_ip_parse(unsigned char *pBuf,unsigned short length)
{
	
}
static void _zcx_gprs_url_parse(unsigned char *pBuf,unsigned short length)
{
	
}


static void _zcx_bc_control_others(unsigned char*p_data,unsigned short len)
{
	switch(p_data[0]<<8 | p_data[1])
	{
		case BROADCAST_CONTRIL_OTHER_IP:
			_zcx_gprs_ip_parse(p_data+3,p_data[2]);
			break;
		case BROADCAST_CONTRIL_OTHER_URL:
			_zcx_gprs_url_parse(p_data+3,p_data[2]);
			break;
		case BROADCAST_CONTRIL_OTHER_AISLE_SELECT:
			//control_aisle_select(p_data+3,p_data[2]);
			break;
		default:
			//printf(" others can not be identified!\n");
			break;
	}
}
static void _zcx_play_close(void)
{
	gp_zcxCmd->openStatus = _ZCX_OPEN_STATUS_CLOSE;
	//NSTAR_HCF4051_RADIO_SET_CLOSE();			
	zcx_dbg("zcx close!\n");
}
static void _zcx_cmd_get_param_buf_get(unsigned char cmd)
{
	unsigned char buf[7] = {0xa5,0x00,0x04,0xb0,0x00,0x01,0xff};
	_fm_cmm_write(buf, 7);
}
static void _zcx_startup_get_param(void)
{
	gp_zcxCmd->openStatus = _ZCX_OPEN_STATUS_WAITING;
	gp_zcxCmd->recvLastOpenSec = ticktime_get();

	_zcx_cmd_get_param_buf_get(ANALOG_CMD_LOGIC_ADDR);

}
static void _zcx_bc_open_status_check(void)
{

	switch(gp_zcxCmd->openStatus){
		case _ZCX_OPEN_STATUS_OPEN:
			if( gp_zcxCmd->recvLastOpenSec + 16 < ticktime_getsec()){
				zcx_dbg("zcx timeout close!\n");
				_zcx_play_close();
			}
			break;
		case _ZCX_OPEN_STATUS_GET_PARAM:
			_zcx_startup_get_param();
			break;
		case _ZCX_OPEN_STATUS_WAITING:
			if( gp_zcxCmd->recvLastOpenSec + 8 < ticktime_getsec()){
				gp_zcxCmd->openStatus = _ZCX_OPEN_STATUS_GET_PARAM;
			}
			break;
	}
}


static unsigned short _zcx_data_control(unsigned char*p_data)
{
	unsigned char cmd = p_data[0];
	unsigned short len = (p_data[1]<< 8) | p_data [2];
	p_data += 3;

	printf("cmd type:%x\n", cmd);
	switch(cmd){
		case ANALOG_CMD_STATUS:
			if( len ==1 && gp_zcxCmd->openStatus < _ZCX_OPEN_STATUS_GET_PARAM){
				if (p_data[0] == 0x82 || p_data[0] == 0x80){
					gp_zcxCmd->recvLastOpenSec = ticktime_getsec();
					if(gp_zcxCmd->openStatus == _ZCX_OPEN_STATUS_CLOSE )
					{
						gp_zcxCmd->openStatus = _ZCX_OPEN_STATUS_OPEN;
						zcx_dbg(" FM_CMD_STATUS OPEN!\n");
					}
				}					
			}
			break;
		case ANALOG_CMD_STOP:
			if( len ==1){
				if(gp_zcxCmd->openStatus == _ZCX_OPEN_STATUS_OPEN){
					_zcx_play_close();
				}
			}				
			break;
		case ANALOG_CMD_VOLUME:
			if(  len ==1 &&p_data[0] < 65){
				if(gp_zcxCmd->volume!=  p_data[0]){
					gp_zcxCmd->volume = p_data[0];
				}
			}
			break;				
		case ANALOG_CMD_FRE:
			if( len == 2){
			 unsigned short fre;
			  fre = (p_data[0]<<8)|p_data[1];
			  if(gp_zcxCmd->fre !=  fre){
				  gp_zcxCmd->fre = fre; 
			  }
            }
			break;
		case ANALOG_CMD_LOGIC_ADDR:
            if (len ==  6) {
            	if(memcmp( gp_zcxCmd->logic,p_data,6) != 0){
					memcpy( gp_zcxCmd->logic,p_data,6);
				}
				if(gp_zcxCmd->openStatus > _ZCX_OPEN_STATUS_OPEN){
					gp_zcxCmd->openStatus = _ZCX_OPEN_STATUS_CLOSE;
				}
            }else{
				zcx_dbg("ANALOG_CMD_LOGIC_ADDR err %d\n",len);
			}
			break;
		case ANALOG_CMD_MAC_ADDR:
			if( len == 6){
				if(memcmp(gp_zcxCmd->phyaddr,p_data,6)){
					memcpy(gp_zcxCmd->phyaddr,p_data,6);
				}
			}
			break;
		case ANALOG_CMD_SIGNAL_STRENGTH:
			if(  len ==1 ){
				if(gp_zcxCmd->strength !=  p_data[0]){
					gp_zcxCmd->strength = p_data[0];
					printf("sq:%d\n", gp_zcxCmd->strength);
				}
			}
			break;	
		case ANALOG_CMD_DATA_TRANS:
			_zcx_bc_control_others(p_data,len);
			break;
		case ANALOG_CMD_GET_PARAM:
			break;
		default:
			break;
	}
	return len+3;
}
static void    ___zcx_data_parse(unsigned char*p_data,unsigned short len)
{
	unsigned char i,cnt;
	unsigned char packLen;
	//ANALOG_DEBUG_STR("zcx dta",p_data,len);
	for(i=0,cnt= 0;i<len && cnt < 20 ; i += packLen,cnt++){//加CNT避免参数长度出错进入4死循环
		 packLen = _zcx_data_control(p_data+i);
	}
}


static void zcx_msg_parse(unsigned char*p_data, unsigned short data_len)
{
	unsigned short len;
	struct zcx_msg_head *msg= (struct zcx_msg_head*)p_data;
	len= swap16(msg->len);
//	if(data_len != len + ZXC_MSG_PACK_LEN){
//		printf("zxc msg err: msg len can't match %d /%d \n", data_len, len + ZXC_MSG_PACK_LEN);
//		return;
//	}
//	printf("msg type:0x%02x, len:%02d\n", msg->type, len);
	switch(msg->type){
		case ANALOG_CMD_STATUS:
			
			break;
		case ANALOG_CMD_STOP:
						
			break;
		case ANALOG_CMD_VOLUME:
			
			break;				
		case ANALOG_CMD_FRE:
			
			break;
		case ANALOG_CMD_LOGIC_ADDR:
            
			break;
		case ANALOG_CMD_MAC_ADDR:
		
			break;
		case ANALOG_CMD_SIGNAL_STRENGTH:
			
			break;	
		case ANALOG_CMD_DATA_TRANS:
			
			break;
		case ANALOG_CMD_GET_PARAM:
			break;
		default:
			break;
	}
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
			if(gp_zcxCmd->allLen <= _ZCX_DTA_MAX_LEN || gp_zcxCmd->allLen < 5){			
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
				else{
					if(1){
						printf("zxc rec %02d/%02d: ", len, gp_zcxCmd->allLen);
						arrry_print(gp_zcxCmd->p_dta, gp_zcxCmd->recvLen);
					}
					zcx_msg_parse(gp_zcxCmd->p_dta, gp_zcxCmd->recvLen);
				}	
				_STEP_SET_0();
			}	
			break;		
		default:
			break;
	}
}



#if 1

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
const unsigned char phyid[6]={0x1,0x2,3,4,5,6};
	unsigned char buf[30];
	struct zcx_msg_head *msg= (struct zcx_msg_head*)buf;
	msg->len= swap16(3+6);
	msg->type= ZCX_SET_PARM_PHYID;
	memcpy(&msg->data, phyid, 6);
	send_fram(buf, 6);

}

static void zxc_send_parm_fre(void)
{
	const unsigned char fre[3]={0,9,5};
	unsigned char buf[30];
	struct zcx_msg_head *msg= (struct zcx_msg_head*)buf;
	msg->len= swap16(3+3);
	msg->type= ZCX_SET_PARM_FRI;
	memcpy(&msg->data, fre, 3);
	send_fram(buf, 3);
}

static void zxc_send_parm_logid(void)
{
	const unsigned char logid[12]={0,9,5};
	unsigned char buf[30];
	struct zcx_msg_head *msg= (struct zcx_msg_head*)buf;
	msg->len= swap16(3+12);
	msg->type= ZCX_SET_PARM_LOGID;
	memcpy(&msg->data, logid, 12);
	send_fram(buf, 12);
}

static void zxc_send_parm_vol(void)
{
	const unsigned char vol=88;
	unsigned char buf[30];
	struct zcx_msg_head *msg= (struct zcx_msg_head*)buf;
	msg->len= swap16(3+1);;
	msg->type= ZCX_SET_PARM_VOL;
	memcpy(&msg->data, &vol, 1);
	send_fram(buf, 1);
}

static void zxc_send_get_parm(void)
{
	const unsigned char data=0xff;
	unsigned char buf[30];
	unsigned short parm_len= 1;
	struct zcx_parm_head *msg= (struct zcx_parm_head*)buf;
	msg->len= swap16(3+parm_len);
	msg->type= ZCX_SET_GET_PARM;
	msg->parm_len= swap16(parm_len);
	memcpy(&msg->data, &data, 1);
	send_fram(buf, 3+parm_len);
}


#endif

void _fm_zcx_init()
{
	gp_zcxCmd = (struct _zcx_cmd*)malloc(sizeof(struct _zcx_cmd));
	gp_zcxCmd->p_dta = malloc(_ZCX_DTA_MAX_LEN+1);
	gp_zcxCmd->openStatus = _ZCX_OPEN_STATUS_GET_PARAM;
	zcx_dbg(" _fm_zcx_init ok\n");
	zxc_send_get_parm();
	//zxc_set_parm_phyid();
}

#if 0
static void _printk(void)
{
	zcx_dbg("****  FM  *****\n");
	zcx_dbg("status:%d step:%d fre:%d strength:%d volume:%d\n",
		gp_zcxCmd->openStatus,
		gp_zcxCmd->step,
		gp_zcxCmd->fre,
		gp_zcxCmd->strength,
		gp_zcxCmd->volume);
}
static NSTAR_BOOL __fm_zcx_param_set(unsigned short cmd,unsigned short len,unsigned char*p_dta)
{
	NSTAR_BOOL ret = NSTAR_FALSE;

	switch(cmd){
		case NSTAR_FM_PRINTK:
			_printk();
			ret = NSTAR_TRUE;
			break;
	}
	return ret;
}
static unsigned short _fm_zcx_param_set_get(unsigned short cmd,unsigned short len,unsigned char*p_dta)
{
	unsigned short ret = 0;
	switch(cmd){
		case NSTAR_FM_SET_GET_FRE:
			if(len > 2){
				unsigned int fre = gp_zcxCmd->fre*100;
				p_dta[0] = fre>>16;
				p_dta[1] = (fre>>8)&0xff;
				p_dta[2] = fre&0xff;
				ret = 3;
			}
			break;
		case NSTAR_FM_SET_GET_LOGIC:
			if(len > 5){
				memcpy(p_dta,gp_zcxCmd->logic,6);
				ret  = 6;
			}
			break;
		case NSTAR_FM_SET_GET_PHY:
			if(len > 5){
				memcpy(p_dta,gp_zcxCmd->phyaddr,6);
				ret  = 6;
			}
			break;
		case NSTAR_FM_SET_GET_VOLUME:
			if(len > 0){
				p_dta[0] = gp_zcxCmd->volume;
				ret  = 1;
			}
			break;
		case NSTAR_FM_SET_GET_STRENGTH:
			if(len > 0){
				p_dta[0] = gp_zcxCmd->strength;
				ret  = 1;
			}
			break;
	}
	return ret;
}
unsigned short _fm_zcx_param_set(unsigned short cmd,unsigned short len,unsigned char*p_dta)
{
	unsigned short ret = 0;
	if(cmd < NSTAR_FM_SET_MAX){
		ret = __fm_zcx_param_set(cmd, len,p_dta);
	}else if(cmd < NSTAR_FM_SET_GET_MAX ){
		ret = _fm_zcx_param_set_get(cmd, len,p_dta);
	}
	return ret;
}

#endif



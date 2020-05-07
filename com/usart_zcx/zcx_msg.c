#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "zcx_msg.h"

extern void fm_uart_send(unsigned char *data, unsigned int len);
extern void arrry_print(unsigned char *data, unsigned int len);

#define zcx_dbg printf
#define _fm_cmm_write 

static struct _zcx_cmd *gp_zcxCmd;
static void zcx_msg_parse(unsigned char*p_data, unsigned short data_len);

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
					if(1){
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
				printf("sta: %x\n", parm->data);
			}
			break;
		case ANALOG_CMD_STOP:
			if(parm_len == 1){
				printf("stop: %x\n", parm->data);
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
	const unsigned char phyid[6]={0x90,1,2,3,4,5};
	
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
	const unsigned char fre[3]={0,9,0x30};
	unsigned char buf[30];
	struct zcx_parm_one *msg= (struct zcx_parm_one*)buf;
	
	unsigned short parm_len= 3;
	unsigned short msg_len= ZXC_PARM_PACK_LEN+parm_len;

	msg->type= ZCX_SET_PARM_FRI;
	msg->len= swap16(msg_len);
	msg->parm_len= swap16(parm_len);
	memcpy(&msg->data, fre, parm_len);
	send_fram(buf, msg_len);
}


static void zxc_send_parm_logid(void)
{
	const unsigned char logid[12]={0xf6,0x52,0x04,0x25,\
		0x10,0x22,0x12,\
		0x03,0x14,0x01,0x04,0x07};
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
	const unsigned char vol=88;
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


void _fm_zcx_init()
{
	gp_zcxCmd = (struct _zcx_cmd*)malloc(sizeof(struct _zcx_cmd));
	gp_zcxCmd->p_dta = malloc(_ZCX_DTA_MAX_LEN+1);
	
	zxc_send_parm_fre();
	usleep(100*1000);
	zxc_send_parm_logid();
	usleep(100*1000);
	zxc_send_parm_phyid();
	usleep(100*1000);
	zxc_send_get_parm();
	
	zcx_dbg(" _fm_zcx_init ok\n");
}



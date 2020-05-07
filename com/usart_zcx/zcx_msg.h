#ifndef _ZCX_FM_MSG
#define _ZCX_FM_MSG 

#define __PACKED__    __attribute__ ((__packed__)) 
#define swap32(value) \
( (value & 0x000000FFU) << 24 | (value & 0x0000FF00U) << 8 | \
(value & 0x00FF0000U) >> 8 | (value & 0xFF000000U) >> 24 )

#define swap16(value) \
( (value & 0x00FFU) << 8 | (value & 0xFF00U) >> 8 )


#define  _BUF_LEN 120


//NSTAR_FM_SET_GET_FRE
//NSTAR_FM_SET_GET_LOGIC
//NSTAR_FM_SET_GET_PHY
//NSTAR_FM_SET_GET_VOLUME
//NSTAR_FM_SET_GET_STRENGTH



enum{
	ANALOG_CMD_VOLUME = 0X03,
	ANALOG_CMD_FRE=0X04,
	ANALOG_CMD_LOGIC_ADDR = 0x05,
	ANALOG_CMD_MAC_ADDR = 0x06,
	ANALOG_CMD_STATUS=0X08,
	ANALOG_CMD_STOP = 0x09,
	ANALOG_CMD_DATA_TRANS = 0XD1,
	ANALOG_CMD_GET_PARAM = 0XB0,
	ANALOG_CMD_SIGNAL_STRENGTH = 0XB3,
};
	
enum{
	BROADCAST_CONTRIL_OTHER_IP = 4,
	BROADCAST_CONTRIL_OTHER_URL = 5,
	BROADCAST_CONTRIL_OTHER_AISLE_SELECT = 18,
};

enum{
	ZCX_SET_GET_PARM = 0xb0,
	ZCX_SET_PARM_PHYID = 0xb7,
	ZCX_SET_PARM_FRI = 0xb5,
	ZCX_SET_PARM_LOGID = 0xb6,
	ZCX_SET_PARM_VOL = 0xb8,
};

struct zcx_msg_head{
	unsigned char head;
	unsigned short len;
	unsigned char type;
	unsigned char data;
}__PACKED__;

struct zcx_parm_one{
	unsigned char head;
	unsigned short len;
	unsigned char type;
	unsigned short parm_len;
	unsigned char data;
}__PACKED__;



#define ZXC_MSG_HEAD 0xA5
#define ZXC_MSG_CRC_LEN 2
#define ZXC_MSG_PACK_LEN 5

#define PHYID_LEN 6
#define FRE_LEN 3
#define LOGIC_LEN 12
#define VOL_LEN 1


struct parm_head{
	unsigned char type;
	unsigned short parm_len;
	unsigned char data;
}__PACKED__;


#define ZXC_PARM_HEAD_LEN 4
#define ZXC_PARM_PACK_LEN 3


//串口协议 
struct _zcx_cmd{
#define _ZCX_DTA_MAX_LEN	(255)
#define _ZCX_OPEN_STATUS_CLOSE 	0
#define _ZCX_OPEN_STATUS_OPEN  	1
#define _ZCX_OPEN_STATUS_GET_PARAM	(2)
#define _ZCX_OPEN_STATUS_WAITING	(3)

	unsigned char openStatus;
	unsigned char step;
	unsigned char strength;
	unsigned char volume;
	unsigned char logic[6];
	unsigned char phyaddr[6];
	unsigned short allLen;
	unsigned short recvLen;
	unsigned short fre;
	unsigned int recvLastOpenSec;
	unsigned char*p_dta;

};

extern void arrry_print(unsigned char *data, unsigned int len);

void _fm_zcx_init();

void _zcx_data_parse(unsigned char data);







#endif 


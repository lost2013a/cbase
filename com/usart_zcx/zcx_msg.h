#ifndef _ZCX_FM_MSG
#define _ZCX_FM_MSG 

#define __PACKED__    __attribute__ ((__packed__)) 
#define swap32(value) \
( (value & 0x000000FFU) << 24 | (value & 0x0000FF00U) << 8 | \
(value & 0x00FF0000U) >> 8 | (value & 0xFF000000U) >> 24 )

#define swap16(value) \
( (value & 0x00FFU) << 8 | (value & 0xFF00U) >> 8 )

#define DBG_FM_UART 0

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
	
enum flag_bit_enum{
	FLAG_BIT_FRE=0x0,
	FLAG_BIT_VOL,	
	FLAG_BIT_LOGID,
	FLAG_BIT_PHYID,
	FLAG_BIT_GETPARM,
	FLAG_BIT_MAX,
};

enum{
	ZCX_SET_GET_PARM = 0xb0,
	ZCX_SET_PARM_FRI = 0xb5,
	ZCX_SET_PARM_LOGID = 0xb6,
	ZCX_SET_PARM_PHYID = 0xb7,
	ZCX_SET_PARM_VOL = 0xb8,
};

struct zcx_parm_one{
	unsigned char head;
	unsigned short len;
	unsigned char type;
	unsigned short parm_len;
	unsigned char data;
}__PACKED__;

struct parm_head{
	unsigned char type;
	unsigned short parm_len;
	unsigned char data;
}__PACKED__;

#define ZXC_MSG_HEAD 0xA5
#define ZXC_MSG_PACK_LEN 5
#define ZXC_PARM_HEAD_LEN 4
#define ZXC_PARM_PACK_LEN 3


//串口协议 
struct usart_machine{
#define _ZCX_DTA_MAX_LEN	(255)

	unsigned char step;;
	unsigned short allLen;
	unsigned short recvLen;
	unsigned char*p_dta;

};

struct zxc_env{
	unsigned char phy_id[6];
	unsigned char logic_id[12];
	unsigned char vol;
	unsigned int main_fq;
	unsigned int scan_fq[3];
};
struct zxc_sta{
#define LOCK_CNT_NUMB 30

#define ZXC_STA_INIT 0
#define ZXC_STA_UNLOCK 1
#define ZXC_STA_IDLE 2
#define ZXC_STA_DAILY 3
#define ZXC_STA_EMERGENCY 4

	unsigned char step;
	unsigned char sta;
	volatile unsigned char locked;
	volatile unsigned char lock_cnt;
	unsigned char sq;
	unsigned int current_fq;
	unsigned int last_lockfq;
};


void _fm_zcx_init();
void _zcx_data_parse(unsigned char data);
void zxc_fm_run_sevo(void);

void zxc_env_set_phyid(unsigned char *phyid, unsigned int len);
void zxc_env_set_fre(unsigned int fre);
void zxc_env_set_logid(unsigned char *logid, unsigned int len);
void zxc_env_set_vol(unsigned char vol);





#endif 


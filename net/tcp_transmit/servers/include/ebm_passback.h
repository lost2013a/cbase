#ifndef	_EBM_PASSBACK_H
#define _EBM_PASSBACK_H


#define __PACKED__    __attribute__ ((__packed__)) 


#define swap32(value) \
( (value & 0x000000FFU) << 24 | (value & 0x0000FF00U) << 8 | \
(value & 0x00FF0000U) >> 8 | (value & 0xFF000000U) >> 24 )

#define swap16(value) \
( (value & 0x00FFU) << 8 | (value & 0xFF00U) >> 8 )




struct _PASSBACK_FIXED_HEADER{
unsigned short head_flag;
unsigned short version;
unsigned int session_id;
unsigned char type;
//unsigned char sign_flag;
unsigned short data_len;
unsigned char src_socde[12];
unsigned short items;
unsigned char items_arry[12];
unsigned char business_type;
unsigned short business_len;
unsigned char business_data;
}__PACKED__;

typedef struct _PASSBACK_FIXED_HEADER PASSBACK_FIXED_HEADER;


enum {
PASSBACK_HEARTTICK= 0x10,
PASSBACK_QUERY= 0x11,
PASSBACK_FAILURE= 0x13,
PASSBACK_PLAY_SWITCH= 0x14,
PASSBACK_PLAY_STA= 0x15,
PASSBACK_PLAY_RECORD= 0x16,
}ENUM_PASSBAKC;

/*
心跳
FE FD 01 00 00 00 03 8F 01 00 35 
F6 52 04 25 00 00 00 03 14 01 02 01 
00 01
00 00 00 00 00 00 00 00 00 00 00 00
10 
00 09 
01 02 
06 
12 34 56 78 90 02 
F0 70 A9 7E 
*/

struct _HEART_TICK_HEADER{
unsigned char sta;
unsigned char register_1st_time;
unsigned char phyid_len;
unsigned char phyid[6];
}__PACKED__;

typedef struct _HEART_TICK_HEADER HEART_TICK_CONTENT;


void func_send_hearttick(void);

unsigned int nstar_crc32_ts(unsigned char *buffer,unsigned int size);


/*
【Receive from 192.168.251.188 :6029】：
FE FD 01 00 00 00 02 91 01 00 82 F6 52 04 25 00 00 00 03 14 01 02 01 00 01 00 00 00 00 00 00 00 00 00 00 00 00 10 00 09 01 02 06 12 34 56 78 90 02 00 4A 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 D7 0F B5 D5 
【Receive from 192.168.251.188 :6006】：
FE FD 01 00 00 00 02 92 01 00 35 F6 52 04 25 00 00 00 03 14 01 02 01 00 01 00 00 00 00 00 00 00 00 00 00 00 00 10 00 09 01 02 06 12 34 56 78 90 02 B0 10 DE D1 
【Receive from 192.168.251.188 :6007】：
FE FD 01 00 00 00 02 93 01 00 35 F6 52 04 25 00 00 00 03 14 01 02 01 00 01 00 00 00 00 00 00 00 00 00 00 00 00 10 00 09 01 02 06 12 34 56 78 90 02 BE C2 13 48
*/




#endif




#ifndef	_EBM_MSG_H
#define _EBM_MSG_H


#define __PACKED__    __attribute__ ((__packed__)) 


#define swap32(value) \
( (value & 0x000000FFU) << 24 | (value & 0x0000FF00U) << 8 | \
(value & 0x00FF0000U) >> 8 | (value & 0xFF000000U) >> 24 )

#define swap16(value) \
( (value & 0x00FFU) << 8 | (value & 0xFF00U) >> 8 )

#define VAILD_HEAD_FLAG 	0xFDFE /* 0xFEFD大端*/
#define VAILD_VERSION_FLAG 	0x0001 /* 0x0100大端*/
#define VAILD_SOURCECODE_TYPE 	0x1403 /* 0x0314大端*/
#define VAILD_ASSIST_TYPE 	61


#if 1
typedef struct source_code{
	unsigned char reserve:4;
	unsigned char lv:4;
	unsigned char province;	/*省*/
	unsigned char city;		/*市*/
	unsigned char county;	/*县*/
	unsigned short town;	/*镇*/
	unsigned short village;	/*村*/
	unsigned short type;
	unsigned char type_idx;
	unsigned char sub_type;
	unsigned char sub_type_idx;
}__PACKED__ TYPE_SCODE;
#endif

struct _MSG_FIXED_HEADER{
unsigned short head_flag;
unsigned short version;
unsigned int session_id;
unsigned char type;
unsigned char sign_flag;
unsigned short data_len;
unsigned char src_socde[12];
unsigned short items;
unsigned char items_arry[12];
unsigned char business_type;
unsigned short business_len;
unsigned char business_data;
}__PACKED__;

typedef struct _MSG_FIXED_HEADER MSG_FIXED_HEADER;



struct _SOURCE_ID{
	unsigned char lv:4;
	unsigned char reseve:4;
	unsigned char area[6];
	unsigned short type;	/*2字节大端*/
	unsigned char type_idx;
	unsigned char subtype;
	unsigned char subtype_idx;
	/*日期 8位*/
	unsigned char date[4];
	/*顺序码 4位*/
	unsigned char idx[2];
}__PACKED__;

typedef struct _SOURCE_ID SOURCE_ID_TYPE;


struct _CMD_PLAY{
	unsigned char source_id[18];
	unsigned char radio_type;
	unsigned char rank;
	char event[5];
	unsigned char vol;
	unsigned int start_time;
	unsigned int stop_time;
	unsigned char assist_items;
	unsigned char assist_type;
	unsigned short assist_len;
	char assist_url;
}__PACKED__;

typedef struct _CMD_PLAY CMD_PLAY_HEADER;

struct _CMD_ASSIST{
	unsigned char assist_type;
	unsigned short assist_len;
	char assist_url;
}__PACKED__;

struct cmd_param{
unsigned char type;
unsigned char len;
unsigned char *data;
}__PACKED__;

struct _CMD_TERMINAL_SETUP{
	unsigned char terminal_nb;
	struct cmd_param data;
}__PACKED__;

typedef struct _CMD_TERMINAL_SETUP CMD_PARM_HEADER;



void handle_ebm_msg(unsigned char *buf ,unsigned int len);


#endif




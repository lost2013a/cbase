#include "ebm_env.h"
#include "ebm_msg.h"



#define __PACKED__    __attribute__ ((__packed__)) 

struct _MSG_FIXED_HEADER{
unsigned short head_flag;
unsigned short version;
unsigned int session_id;
unsigned char type;
unsigned char sign_flag;
unsigned short data_len;
unsigned char resource_code[12];
unsigned short items;
unsigned char items_arry[12];
unsigned char business_type;
unsigned short business_len;
unsigned char business_data;
}__PACKED__;

typedef struct _MSG_FIXED_HEADER MSG_FIXED_HEADER;


#define swap32(value) \
( (value & 0x000000FFU) << 24 | (value & 0x0000FF00U) << 8 | \
(value & 0x00FF0000U) >> 8 | (value & 0xFF000000U) >> 24 )

#define swap16(value) \
( (value & 0x00FFU) << 8 | (value & 0xFF00U) >> 8 )

const char* type_str[]={
"NULL",
"开始播发",
"停止播发",
"系统心跳",
"终端查询",
"终端参数设置",
};

unsigned char visit_type_sq(unsigned char type)
{
	unsigned char ret=0;
	switch(type){
		case 0x01:
			ret= 1;
			break;
		case 0x02:
			ret= 2;
			break;
		case 0x10:
			ret= 3;
			break;
		case 0x11:
			ret= 4;
			break;
		case 0x12:
			ret= 5;
			break;
	}
	return ret;
}


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



static void vist_buf(unsigned char *buf ,unsigned int len)
{
	unsigned int JUMP_LEN= 16;
	int i;
	for(i=0; i<len; i++){
		printf("%02x ",buf[i]);
		if(i%JUMP_LEN == JUMP_LEN-1)
			printf("\n");
	}
	printf("\n");

}


static void visit_source_id(unsigned char *id)
{
#define SID_EMBID_TYPE 0x1403 /*大端：0x0314*/

	int i;
	printf("sid= ");
	for(i=0; i<18; i++){
		printf("%02x ", id[i]);
	}
	printf("\n");

	SOURCE_ID_TYPE *p_id= (SOURCE_ID_TYPE*)id;
	printf("lv= %x\n", p_id->lv);
	printf("area= %02x%02x%02x%02x%02x%02x\n", p_id->area[0], p_id->area[1], p_id->area[2], p_id->area[3], p_id->area[4], p_id->area[5] );
	printf("type= %02x\n", p_id->type );

	printf("type_idx= %02x\n", p_id->type_idx);

	printf("subtype= %02x\n", p_id->subtype);

	
	printf("subtype_idx= %02x\n", p_id->subtype_idx);

	
	printf("date= %02x%02x%02x%02x\n", p_id->date[0],  p_id->date[1],  p_id->date[2],  p_id->date[3] );
	printf("idx= %02x%02x\n", p_id->idx[0],  p_id->idx[1]);


}
static void cmd_play_printf(CMD_PLAY_HEADER *cmd)
{
	visit_source_id(cmd->source_id);
	printf("radio_type=%x\n", cmd->radio_type);
	printf("cmd->rank=%x\n", cmd->rank);
	//printf("event=%s\n", cmd->event);
	printf("vol=%d\n", cmd->vol);
	printf("start_time=%x\n", cmd->start_time);
	printf("stop_time=%x\n", cmd->stop_time);

	printf("assist_type=%d\n", cmd->assist_type);
	printf("assist_len=%d\n", swap16(cmd->assist_len));
	printf("assist_url=%s\n", &cmd->assist_url);

}


void parse_cmd_play(unsigned char *cmd)
{

	CMD_PLAY_HEADER *head= (CMD_PLAY_HEADER*)cmd;
	cmd_play_printf(head);
	
}


struct cmd_param{
unsigned char type;
unsigned char len;
unsigned char data;
}__PACKED__;

struct _CMD_TERMINAL_SETUP{
	unsigned char terminal_nb;
	struct cmd_param data;
}__PACKED__;

typedef struct _CMD_TERMINAL_SETUP CMD_PARM_HEADER;

static char* PARA_CMD_STR[]={
"无效",	
"音量",		
"本地IP地址信息",
"回传地址信息",
"地址信息",
"功放开关",
"校时",
"回传周期",
};




static void cmd_para_printf(CMD_PARM_HEADER *cmd)
{
	unsigned char *data, total_nb;
	total_nb= cmd->terminal_nb;
	printf("total_nb=%x\n", total_nb);

	data= &cmd->terminal_nb+1;
	struct cmd_param *p_param =  (struct cmd_param *)data;
	printf("type= #%d %s, len=%d\n", p_param->type, PARA_CMD_STR[p_param->type], p_param->len);

}

void parse_cmd_parm(unsigned char *cmd)
{

	CMD_PARM_HEADER *head= (CMD_PARM_HEADER*)cmd;
	//cmd_play_printf(head);
	cmd_para_printf(head);
	
}



void msg_pritf(MSG_FIXED_HEADER *msg)
{
#define dbg printf
	unsigned char type= visit_type_sq(msg->business_type);
	//dbg("\n[%04d] #%d%s\n", swap32(msg->session_id), msg->type, type_str[type]);
	//dbg("data_len= %d\n", swap16(msg->data_len));
	//dbg("business_len= %d\n", swap16(msg->business_len));
	
	dbg("\n[%04d] #%d[%02d]%s\n", swap32(msg->session_id), msg->type, swap16(msg->business_len), type_str[type]);
	vist_buf(msg->resource_code, 12);
	dbg("items=%d\n", msg->items);
	vist_buf(msg->items_arry, 12);


	switch(type){
		case 0x1:
			if(0)
				break;
			parse_cmd_play(&msg->business_data);
			break;
		case 0x12:
			parse_cmd_parm(&msg->business_data);
			break;
		default:
			break;

	}
}




void handle_ebm_msg(unsigned char *buf ,unsigned int len)
{
	MSG_FIXED_HEADER *msg;
	msg= (MSG_FIXED_HEADER *)buf;
	msg_pritf(msg);
}





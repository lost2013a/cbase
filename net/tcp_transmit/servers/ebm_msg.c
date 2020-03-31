#include "ebm_env.h"
#include "ebm_msg.h"

#define dbg printf

const char* type_str[]={
"NULL",
"开始播发",
"停止播发",
"系统心跳",
"终端查询",
"终端参数设置",
};

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



#if 0
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

#endif

static void vist_buf(const char* head, unsigned char *buf ,unsigned int len)
{
	unsigned int JUMP_LEN= 16;
	int i;
	printf("[%s]:", head);
	for(i=0; i<len; i++){
		printf("%02x ",buf[i]);
		if(i%JUMP_LEN == JUMP_LEN-1)
			printf("\n");
	}
	printf("\n");

}


static void visit_source_id(unsigned char *id)
{

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

#if 1



static unsigned char parse_url(const char* url, unsigned int *ip, unsigned short *port)
{
	unsigned char ret=0, ipstr_len;
	unsigned int tmp_ip; int tmp_port;
	char *p1,*p2;
	char tmpbuf[50], ipbuf[20];
	strncpy(tmpbuf, url, 50);
	tmpbuf[49]=0;
//	printf("url buf=%s\n", tmpbuf);
/*url类型1 udp://@24002*/
	p1= strstr(tmpbuf, "udp://@");
	if(p1 != NULL){
		*port= atoi(p1+ strlen("udp://@"));
		*ip= h_env->plat_ip;
		//*ip= swap32(h_env->plat_ip);
		printf("url ip =%x\n, port= %d\n", *ip, *port);
		return 1;
	}
	p1= strstr(tmpbuf, "rtp://");
	if(p1 == NULL){
		p1= strstr(tmpbuf, "udp://");
	}
	if(p1 == NULL){
		/*暂不支持rtsp*/
		return 0;
	}
	p1+= strlen("rtp://");
	printf("p1=%s\n", p1);
	p2= strstr(p1, ":");
	ipstr_len= p2-p1;
	if(p2 == NULL || ipstr_len > 15)
		return 0;
	
	strncpy(ipbuf, p1, ipstr_len);
	ipbuf[ipstr_len]=0;
	printf("ipbuf=%s\n", ipbuf);
	tmp_ip= inet_addr(ipbuf);
	tmp_ip= swap32(tmp_ip);
	printf("tmp_ip=%x\n", tmp_ip);
	tmp_port= atoi(p2+1);
	if(tmp_port < 0 || tmp_port > 0xffff)
		return 0;
	printf("tmp_port=%x\n", tmp_port);

	*ip= tmp_ip;
	*port= tmp_port;
	return 2;

}
#else
static unsigned char parse_url(const char* url, unsigned int *ip, unsigned short *port)
{
	unsigned char ret=0, ipstr_len;
	unsigned int tmp_ip; int tmp_port;
	char *p1,*p2;
	char tmpbuf[50], ipbuf[20];
	strncpy(tmpbuf, url, 50);
	tmpbuf[49]=0;
	p1= strstr(tmpbuf, "rtp://");
	if(p1 == NULL){
		p1= strstr(tmpbuf, "udp://");
	}
	if(p1 == NULL){
		/*暂不支持rtsp*/
		return 0;
	}
	p1+= strlen("rtp://");
	printf("p1=%s\n", p1);
	p2= strstr(p1, ":");
	ipstr_len= p2-p1;
	if(p2 == NULL || ipstr_len > 15)
		return 0;
	
	strncpy(ipbuf, p1, ipstr_len);
	printf("ipbuf=%s\n", ipbuf);
	tmp_ip= inet_addr(ipbuf);
	tmp_ip= swap32(tmp_ip);
	printf("tmp_ip=%x\n", tmp_ip);
	tmp_port= atoi(p2+1);
	if(tmp_port < 0 || tmp_port > 0xffff)
		return 0;
	printf("tmp_port=%x\n", tmp_port);

	*ip= tmp_ip;
	*port= tmp_port;

}

#endif




void parse_cmd_play(unsigned char *data)
{
	static unsigned int last_ip=0;
	static unsigned short last_port=0;
	unsigned int ip; unsigned short port;
	CMD_PLAY_HEADER *palyer= (CMD_PLAY_HEADER*)data;
	//cmd_play_printf(palyer);
	if(palyer->assist_type == VAILD_ASSIST_TYPE){
		if(0 != parse_url(&palyer->assist_url, &ip,&port)){
#if 1			
			if(last_ip != ip || last_port != port){
				printf("new mp3 play start\n");
				rtp_net_start(ip, port);
			}
#endif			
			last_ip= ip;
			last_port= port;
		}
	}

	
	
}


void parse_cmd_stop(unsigned char *data)
{
//	vist_buf("stop:", data, 18);
	rtp_net_stop();
}



static void cmd_para_printf(CMD_PARM_HEADER *cmd)
{
	unsigned char *data, total_nb;
	total_nb= cmd->terminal_nb;
	printf("total_nb=%x\n", total_nb);

	data= &cmd->terminal_nb+1;
	struct cmd_param *p_param =  (struct cmd_param *)data;
	printf("type= #%d %s, len=%d\n", p_param->type, PARA_CMD_STR[p_param->type], p_param->len);

}

static void _parse_parm(struct cmd_param* p_data)
{
	unsigned char type= p_data->type;
	printf("type= #%d %s, len=%d\n", type, PARA_CMD_STR[type], p_data->len);
	switch(type)
	{
		case 1:
			if(p_data->len == 1)
				ebm_env_set_vol(p_data->data);
			break;
		case 2:
			if(p_data->len == 12)
				ebm_env_set_ip(p_data->data);	
			break;	
		case 3:
			
			if(p_data->data == 0x1 && p_data->len == 7){

				unsigned short *port= (unsigned short*) (&p_data->data+5);
				ebm_env_set_backip(&p_data->data +1, *port);
			}
			else if(p_data->data == 0x2){
				printf("can't parse domain type ip addr\n");
			}
			break;
		case 4:
#define PHY_ID_LEN 12
			if(p_data->data== PHY_ID_LEN )
				ebm_env_set_sourceid(p_data->data+1+ PHY_ID_LEN);
			break;	
		case 5:
			if(p_data->data== 0x1 || p_data->data == 0x2)
				ebm_env_set_trumpet(p_data->data);
			break;
		case 6:
			if(p_data->len == 4){
				ebm_env_set_systime(p_data->data);
			}
			break;	
		case 7:
			ebm_env_set_back_period(p_data->data);
			break;
		default:
			break;
	}
	

}


static void do_parse_parm(CMD_PARM_HEADER *cmd, unsigned int date_len)
{
#define FILL_LEN (1+2+4)
	struct cmd_param *parm;
	unsigned char *p_data, total_nb;
	unsigned int idx=0;
	total_nb= cmd->terminal_nb;
	
	p_data= &cmd->terminal_nb +1;
	//printf("total_nb=%x pdata=%x\n", total_nb, *p_data);
	if(date_len > FILL_LEN)
		date_len-=FILL_LEN;
	else{
		printf("%s parm len not match\n", __func__);
		return;
	}

	while(total_nb && date_len >= 3)
	{
		parm= (struct cmd_param*)p_data;

		//printf("date_len=%d, parm-len=%d\n", date_len, parm->len);
		if(date_len-2 >= parm->len){
			_parse_parm(parm);
			total_nb--;
			date_len-= (2+ parm->len);
			p_data+= 2+ parm->len;
		}
		else{
			printf("%s parm len not match\n", __func__);
			break;
		}

	
	}

}

void parse_cmd_parm(unsigned char *cmd, unsigned int len)
{

	CMD_PARM_HEADER *head= (CMD_PARM_HEADER*)cmd;
	//cmd_play_printf(head);
	do_parse_parm(head, len);
}

void parse_req_sta(unsigned char *cmd, unsigned int len)
{

	CMD_PARM_HEADER *head= (CMD_PARM_HEADER*)cmd;
	//cmd_play_printf(head);
	do_parse_parm(head, len);
}


void msg_pritf(MSG_FIXED_HEADER *msg)
{

//	unsigned char type= visit_type_sq(msg->business_type);
//	
//	dbg("\n[%04d] #%d[%02d]%s\n", swap32(msg->session_id), msg->type, swap16(msg->business_len), type_str[type]);
//	//vist_buf(msg->resource_code, 12);
//	dbg("items=%d\n", msg->items);
//	//vist_buf(msg->items_arry, 12);
//	switch(type){
//		case 0x1:
//			if(0)
//				break;
//			parse_cmd_play(&msg->business_data);
//			break;
//		case 0x12:
//			//parse_cmd_parm(&msg->business_data);
//			break;
//		default:
//			break;

//	}
}


void convert_source_code(unsigned char *id, TYPE_SCODE *scode)
{
	memcpy(scode, id, 4);
	scode->town= id[4]<<4 | id[5]>>4;
	scode->village= (id[5]&0xf)<<8 | id[6];
	memcpy(&scode->type, &id[7], 5);
}

void parse_msg(MSG_FIXED_HEADER *msg, unsigned int len)
{
	unsigned char business_type;
	//dbg("\n[%04d] #%d%s\n", swap32(msg->session_id), msg->type, type_str[type]);
	//dbg("data_len= %d\n", swap16(msg->data_len));
	//dbg("business_len= %d\n", swap16(msg->business_len));
	unsigned int dlen;
	if(msg->head_flag != VAILD_HEAD_FLAG || msg->version != VAILD_VERSION_FLAG){
		dbg("bad farme\n");
		return;
	}

	if(msg->type != MSG_TYPE_REQ){
		dbg("isn't a request frame\n");
		//return;
	}
		
	TYPE_SCODE scode;
	TYPE_SCODE *src= &scode;
	convert_source_code(msg->src_socde, src);

if(0){
	dbg("province= %x\n", src->province);
	dbg("city= %x\n", src->city);
	
	dbg("county= %x\n", src->county);
	
	dbg("town= %x\n", src->town);
	dbg("village= %x\n", src->village);
	
	dbg("type= %x\n", src->type);
	
	dbg("type_idx= %x\n", src->type_idx);
	dbg("sub_type= %x\n", src->sub_type);
	dbg("sub_type_idx= %x\n", src->sub_type_idx);
}
	//vist_buf("items_arry", msg->items_arry, 12);
	//business_type= visit_type_sq(msg->business_type); 
	business_type= msg->business_type; 
	dbg("business type =%x\n", business_type);

if(0)
{
	int i;
	unsigned char *data= (unsigned char*)msg;
	for(i=0; i< len;i++){
		printf("%02X ", data[i]);
		if(i%16 == 15)
			printf("\n");
	}
	printf("\n");
}

/*
0x01：开始播发
0x02：停止播发
0x10：终端心跳
0x11：终端状态查询
0x12：终端参数设置
0x17：应急广播证书授权协议
*/

	switch(business_type){
		case 0x1:
			parse_cmd_play(&msg->business_data);
			break;
		case 0x2:
			parse_cmd_stop(&msg->business_data);
			break;	
		case 0x11:
			break;	
		case 0x12:
			
			dlen= len- (unsigned int)&(((MSG_FIXED_HEADER*)0)->business_data);
			parse_cmd_parm(&msg->business_data, dlen);
			break;
		default:
			break;

	}
}


void handle_ebm_msg(unsigned char *buf , int len)
{
	MSG_FIXED_HEADER *msg;
	msg= (MSG_FIXED_HEADER *)buf;
	//msg_pritf(msg);

	//data_len
	unsigned short msg_len;
	while(len > sizeof(MSG_FIXED_HEADER))
	{
		msg= (MSG_FIXED_HEADER *)buf;
		msg_len= swap16 (msg->data_len);
		printf("msg len =%d %d\n", msg_len , len);
		
		if( len >= msg_len){
			parse_msg(msg, msg_len);
			len-= msg_len;
			buf+= msg_len;
		}
		else
			break;
	}
}

/*
FE FD 01 00 00 00 03 8F 01 00 35 F6 52 04 25 00 00 00 03 14 01 02 01 00 01 00 00 00 00 00 00 00 00 00 00 00 00 10 00 09 01 02 06 12 34 56 78 90 02 F0 70 A9 7E 


*/



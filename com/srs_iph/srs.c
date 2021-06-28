#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>

#include "serial.h"

#define NSTAR_HCF4051_INTERPH_SET_OPEN() HCF4051_OPEN_INTERPHONE()
#define NSTAR_HCF4051_INTERPH_SET_CLOSE() HCF4051_CLOSE_INTERPHONE()

#define USART_NAME "/dev/ttyS1"

#define _interph_cmm_write(p_dta,writleLen)    	nstar_adt_device_write(USART_NAME,p_dta,writleLen)
#define _interph_cmm_read(p_dta,readLen)    	nstar_adt_device_read(USART_NAME,p_dta,readLen)
#define _interph_cmm_read_status()				!nstar_adt_device_read_status(USART_NAME,NSTAR_NULL,0)


#define iph_set_frqr(n)     sys_interphone_set_RFV(n)
#define iph_get_frqr()      sys_interphone_get_RFV()

#define iph_set_vol(n)      sys_interphone_set_volume(n)
#define iph_get_vol()       sys_interphone_get_volume()

#define iph_set_colorCode(n)      cfgset_ebm_inph_ColorCode(n)
#define iph_get_colorCode()       cfgget_ebm_inph_ColorCode()

#define iph_set_gid(p, len) cfgset_ebm_inph_slaveCallnum(p, len)
#define iph_get_gid()       cfgget_ebm_inph_slaveCallnum()

#define _NICE_SERIAL_SET_NETT_STEP()	do{gp_srsSerial->step++; }while(0)
#define _NICE_SERIAL_SET_HEAD_STEP()	_srs_head_set()

enum{
SRS_SET_VERSION = 0,
SRS_SET_GROUP,  
SRS_SET_VOL,
};

const char *SRS_CMD[] = {
"AT+DMOVERQ",
"AT+DMOSETGROUP=0,%d.%04d,%d.%04d,%d,%d,%d,1",
"AT+DMOSETVOLUME=%d", 
};

const char *SRS_ACK[] = {
"+DMOVERQ",
"+DMOSETGROUP",
"+DMOSETVOLUME:0",

};


#if 0
#else
#define INTERPH_DEBUG(...) 	  do{}while(0)
#endif


static struct  _interph_cmd_dta g_srsCmdDta;
static struct _nice_serial *gp_srsSerial;
static struct _nice_oprate  *gp_srsOprate;
static nstar_u32 (*gp_interph_loop)(void) = NSTAR_NULL;
static unsigned char (*gp_interph_param_set)(nstar_u8 cmd,nstar_u16 len,nstar_u8*p_dta) = NSTAR_NULL;


#define _INTERPH_CMD_DTA_SET_MODE_DTA(num, p_backCmd, backCmdLen)		do{g_srsCmdDta.cmdDtaLen = 0;\
															g_srsCmdDta.formatNum = (num);\
															g_srsCmdDta.p_backCmd = (nstar_u8*)(p_backCmd);\
															g_srsCmdDta.backCmdLen = (backCmdLen);\
														}while(0)

extern void iph_dbg_hex(unsigned char *data, unsigned int len, const char* title);

extern void pack_gid(unsigned int id, unsigned char *data);

static void _srs_param_printk(void)
{
    //myarg(iph_get_frqr());
    //myarg(iph_get_vol());
}

static unsigned int nstar_interph_srs_machine(void);
static unsigned char nstar_interph_srs_param_set(nstar_u8 cmd,nstar_u16 len,nstar_u8*p_dta);

static void srs_init(void)
{
	gp_srsOprate = (struct  _nice_oprate*)nstar_adt_malloc(sizeof( struct _nice_oprate));
	gp_srsSerial = &gp_srsOprate->niceSerial;
	gp_srsOprate->step = _NICE_STEP_START_UP;
	gp_interph_loop = nstar_interph_srs_machine;
	gp_interph_param_set = nstar_interph_srs_param_set;	
}


typedef struct {
    int fd;
    unsigned char step;
}T_SRS;

static T_SRS srs;

enum {
SRS_STEP_STARTUP,
SRS_STEP_CHECK_VERSION,
SRS_STEP_CFG,
SRS_STEP_LISTEN,
SRS_STEP_OPEN_CONFIRM,
SRS_STEP_OPEN_CHECK,
SRS_STEP_CLOSE
};

unsigned char _hadware_init(void)
{
    int fd = usart_init(USART_NAME, 9600);
    if(fd) {
        srs.fd = fd;
        return 1;
    }
    return 0;
}

static unsigned char _srs_rst(void) {
    
}

static void _srs_step(unsigned char s) 
{
    srs.step = s;
}

static unsigned char _srs_step_version(void)
{
    unsigned char i = SRS_SET_VERSION;
	return srs_nice_serial_device_read((char*)SRS_CMD[i], \
                (char*)SRS_ACK[i], 10);
}


static unsigned char _srs_step_setgroup(void)
{
    unsigned char i = SRS_SET_GROUP;
    char cmd[64];
    snprintf(cmd, 64, "AT+DMOSETGROUP=1,%d.%04d,%d.%04d,20,8,20,1",\
                425,6000,425,6000);
    
	return srs_nice_serial_device_read(cmd, \
                (char*)SRS_ACK[i], 10);
}


static unsigned char _srs_step_setvol(void)
{
    unsigned char i = SRS_SET_VOL;
    char cmd[64];
    snprintf(cmd, 64, "AT+DMOSETVOLUME=%d",\
                iph_get_vol());
    myarg(iph_get_vol());
	return srs_nice_serial_device_read(cmd, \
                (char*)SRS_ACK[i], 10);
}


void _srs_loop(void)
{
    switch(srs.step) {
    case SRS_STEP_STARTUP:
        _srs_rst();
        _srs_step(SRS_STEP_CHECK_VERSION);
        break;
    case SRS_STEP_CHECK_VERSION:
        _srs_step_version();
        _srs_step(SRS_STEP_CFG);
        break;

    }
}


static char* check_ack(char *rbuf, char *str)
{
	char *strx= NULL;
	if(str != NULL) {
		strx= strstr((char*)rbuf, str);
	}
	return strx;
}

/*blocking rec*/
static unsigned int blocking_rec(char *rbuf, unsigned short maxLen)
{
    
    int len, rlen = 0; 
	do{
		len= _interph_cmm_read((unsigned char*)&rbuf[rlen], maxLen- rlen);
		if(len>0) {
			rlen+=len;
            usleep(10*1000);       
		}
	}while(len > 0 && rlen < maxLen);
        
    if(rlen > 0) {
        rbuf[rlen] = 0;
        printf("rec %d : %s\n", rlen, rbuf);
    }
	return rlen;
}
static unsigned char _interph_nice_serial_device_read(nstar_s32 timeOutMs,nstar_u8 cmd)
{
    return 1;
}

static unsigned char srs_nice_serial_device_read(char *cmd, char *ack, unsigned short waittime)
{
    int len;
	unsigned char ret=0;
    char tbuf[128];
    char rbuf[128];
	len = snprintf(tbuf, 128, "%s\r\n", cmd);
    printf("tbuf %s len = %d\n", tbuf, len);
    _interph_cmm_write((unsigned char*)tbuf, len);
	if(ack&&waittime){
		while(--waittime)		//等待倒计时
		{ 
            memset(rbuf, 0 , 128);
			if(blocking_rec(rbuf, 128)){
				if(check_ack(rbuf, ack)){
					ret= 1;
					break;    	//得到正确响应，退出 
				}
			} 
			nstar_adt_task_sleep(10);
            
		}
	}
	return ret;
}

c unsigned char _interh_srs_step_startup(void)
{
	unsigned char ret = 1;
	NICE_DEBUG(" _interh_srs_step_startup startup\n");
	nstar_adt_gpio_set(NSTAR_GPIO_RESET_INTERPHONE,0);
	nstar_adt_task_sleep(2000);
	nstar_adt_gpio_set(NSTAR_GPIO_RESET_INTERPHONE,NSTAR_GPIO_RESET_INTERPHONE);
	if(gp_srsOprate->restartCnt < 255){
		gp_srsOprate->restartCnt++;
	}
	return ret;
}
static unsigned char _srs_step_listen(void)
{	
	unsigned char ret = 0;
	if( _interph_cmm_read_status() == 1){
		NICE_DEBUG(" nice open\n");
		ret = 1;
	}
	return ret;
}






static unsigned char _srs_step_open_confirm(void)
{	
	unsigned char ret = 0;
	if(_interph_cmm_read_status() == 1){//uhf SQ低有�?
		NICE_DEBUG(" nice _step_open_confirm\n");
		NSTAR_HCF4051_INTERPH_SET_OPEN();
		ret = 1;
	}
	return ret;
}
static unsigned char _srs_step_open_check(void)
{	
	unsigned char ret = 0;
	if(_interph_cmm_read_status() != 1){
		NICE_DEBUG(" nice _step_open_check close\n");
		NSTAR_HCF4051_INTERPH_SET_CLOSE();
		ret = 1;
	}
	return ret;
}


static void _step_oprate_faile_check_set_startup(nstar_u8 times)
{
	if(gp_srsOprate->continuCnt++ > 30 ){
		_nice_oprate_step_set(_NICE_STEP_START_UP);
	}
}

static nstar_u32 nstar_interph_srs_machine(void)
{
	nstar_u32 retMs = 40;
	if(gp_srsOprate->step > _NICE_STEP_START_UP){
		_interph_nice_serial_device_read(0,_NICE_CMD_INVALID);	
	}
	switch(gp_srsOprate->step){
		case _NICE_STEP_START_UP:
			nstar_adt_device_close(USART_NAME);
			if(nstar_adt_device_init(USART_NAME,9600) == 1){
				_interh_srs_step_startup();
				_nice_oprate_step_set(_NICE_STEP_READ_VERSION);
				retMs = 3000;
			}else{
				retMs = 6000;
			}
		case _NICE_STEP_READ_VERSION:
			if(_srs_step_version() == 1 ){
				_nice_oprate_step_set(_NICE_STEP_SCAN_FRE_STATUS);
			}else{
				retMs = 1000;
				_step_oprate_faile_check_set_startup(30);
			}
			break;
		case _NICE_STEP_SCAN_FRE_STATUS:
			if(_srs_step_setgroup() == 1  && _srs_step_setvol() == 1){
                
				_nice_oprate_step_set(_NICE_STEP_LISTENING);
			}else{
				retMs = 1000;
				_step_oprate_faile_check_set_startup(30);
			}
			break;

//		case _NICE_STEP_SET_VOLUME:
//			if(_srs_step_set_volume() == 1 ){
//				_nice_oprate_step_set(_NICE_STEP_LISTENING);
//			}else{
//				retMs = 1000;
//				_step_oprate_faile_check_set_startup(30);
//			} 
//			break;	

		case _NICE_STEP_LISTENING:

  
			if(_srs_step_listen() == 1){
				_nice_oprate_step_set(_NICE_STEP_OPEN_CONFIRM);
			}

			break;

		case _NICE_STEP_OPEN_CONFIRM:
			if(1 ==_srs_step_open_confirm()){
				_nice_oprate_step_set(_NICE_STEP_OPEN_CHECK);
			}
			else{
				_nice_oprate_step_set(_NICE_STEP_LISTENING);
			}
			break;
		case _NICE_STEP_OPEN_CHECK:
			if(1 ==_srs_step_open_check()){
				_nice_oprate_step_set(_NICE_STEP_LISTENING);
			}
			break;
	
	}
 	return retMs;
}




static unsigned char nstar_interph_srs_param_set(nstar_u8 cmd,nstar_u16 len,nstar_u8*p_dta)
{
	unsigned char ret = 1;
	switch(cmd){
		case NSTAR_INTERPH_PARAM_SET_MODIFY:

			break;
		case NSTAR_INTERPH_PARAM_SET_CLOSE:		
			
			break;
		case NSTAR_INTERPH_PARAM_SET_PRINTK:
			_srs_param_printk();
			break;
		case NSTAR_INTERPH_PARAM_SET_GET_STATUS:
			ret = 0;
			break;
		case NSTAR_INTERPH_PARAM_SET_RESET:
			break;
		default:
			ret = 0;
			break;
	}
	return ret;
}










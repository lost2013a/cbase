#include <netinet/in.h>
#include <netinet/tcp.h> 
#include <sys/socket.h>
#include "nstar_adt_httputil.h"
#include "nstar_adt_http_server.h"
#include "ntart_cookie.h"


#include "app_delay.h"

#define SERIAL_NUMB 0x13980642

#define COOKIE_LEN	32
#define COOKIE_HEAD "NSTAR_COOKIE"
#define COOKIE_PERIOD_TIME (20*60*1000)	/*20分钟有效期限*/

#define USER_PASS  "admin"
#define SUPPER_PASS "newstar2919"
#define PASS_LEN (strlen(USER_PASS))
#define SUPPER_PASS_LEN (strlen(SUPPER_PASS))

struct http_cookie{
	char cookie[COOKIE_LEN+1];
	unsigned int  vaild_len;
	unsigned int  last_time;
};

static struct http_cookie h_cookie= {0};

static void _set_cookie(unsigned int last_time)
{
	unsigned int number1= (last_time << 1)+ SERIAL_NUMB;
	unsigned int number2= (last_time >> 1)- SERIAL_NUMB;
	number1%=100000000;
	number2%=100000000;
	h_cookie.vaild_len= snprintf(h_cookie.cookie, COOKIE_LEN, "%s%08d%08d", COOKIE_HEAD, number1, number2);
	//mydbg("h_cookie[%d]:%s\n", h_cookie.vaild_len, h_cookie.cookie);
}

static void _update_cookie(void)
{
	unsigned int *p_last= &h_cookie.last_time;
	unsigned int current_time= app_get_systime();
	int off = current_time- *p_last;
	if(off > COOKIE_PERIOD_TIME){
		//mydbg("update cookie: period\n");
		_set_cookie(current_time);
		*p_last= current_time;
	}
}

void cookie_init(void)
{
	unsigned int current_time= app_get_systime();
	_set_cookie(current_time);
	h_cookie.last_time= current_time;
	//mydbg("update cookie: init\n");
}


int cookie_verify(unsigned char *http)
{
	char *p_cooki = strstr((const char *)http, (const char *)COOKIE_HEAD);
	if(p_cooki == NULL)
		return -1;

	_update_cookie();
	if(memcmp(p_cooki, h_cookie.cookie, h_cookie.vaild_len)==0){
		return 1;
	}
	else
		return 0;	
}

const char* cookie_getstr(void)
{
	return (const char*)h_cookie.cookie;
}


unsigned char login_pass_check(const char* dst)
{
	unsigned char ret=0;
	if(dst == NULL)
		return ret;
	if(memcmp(SUPPER_PASS, dst, SUPPER_PASS_LEN) == 0)
		ret=1;
	else if(memcmp(USER_PASS, dst, PASS_LEN) == 0)
		ret=1;
	return ret;
}



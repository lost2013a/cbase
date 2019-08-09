#include <netinet/in.h>
#include <netinet/tcp.h> 
#include <sys/socket.h>
#include "nstar_adt_httputil.h"
#include "nstar_adt_http_server.h"


#include "app_delay.h"

#define COOKIE_LEN	16
struct http_cookie{
	unsigned char cookie[COOKIE_LEN+1];
	unsigned int  last_time;
};

static struct http_cookie h_cookie= {0};


 void set_cookie(unsigned int last_time, unsigned int vaild_period)
{
	//unsigned int current_time= app_get_systime();
	//int off = current_time- last_time;
	unsigned int number= 1999;
	snprintf(h_cookie.cookie, COOKIE_LEN, "newstar%08d", number);
}


static unsigned char* get_cookie(void)
{
	printf("cookie=%s\n", h_cookie.cookie);
	return  h_cookie.cookie;
}





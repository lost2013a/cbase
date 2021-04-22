#include <pthread.h>
#include "bconn.h"
#include "app_delay.h"

static int command_rec(unsigned char *data, unsigned int len) {
    printf("command rec %d\n", len);
    return 0;
}

static void command_send_func(struct bsockt *b) {
    const char* str = "hello!\n";
    if(PEND_T_S()){
        bconn_send(b, (unsigned char*)str, sizeof(str));
        SLEPP_T_S(1);  
    }
}
static struct bsockt command_net_handle = \
BCONN_INIT_STRUCTURE("command", 0, 0, command_rec, DEFAULT_TR_OUT);

static void *command_tast(void *parm)
{
#define TEST_IP (192 << 24 | 168 <<16 | 251 <<8 | 170)
#define TEST_PORT 50003  
    struct bsockt *b = &command_net_handle;
	bconn_start(b, TEST_IP, TEST_PORT);
	while(1)
	{
		bconn_machine(b);
        bconn_block_rec(b);
        command_send_func(b);
        usleep(10*1000);
	}
	return 0;
}

static void creat_command_pthread(void)
{
	pthread_t threads;
	long t=0;
	if (pthread_create(&threads, NULL, command_tast, (void *)t)){
		puts(__func__);
		exit(-1);
	}
}

int main(int argc, char *argv[])
{
    app_delay_test();
    creat_command_pthread();
	while(1){
		sleep(1);
	}
	return 0;
}




#include <pthread.h>
#include "ebm_env.h"
#include "ebm_msg.h"
#include "ebm_passback.h"

static void *rtp_player(void *parm)
{
	//rtp_net_start(h_env->rtp_ip, h_env->rtp_port);
	while(1)
	{
		rtp_net_loop();
	}
	rtp_net_stop();
	return 0;
}

void creat_rtp_pthread(void)
{
	pthread_t threads;
	int rc;
	long t=0;
	rc = pthread_create(&threads, NULL, rtp_player, (void *)t);
	if (rc){
		printf("ERROR; return code from pthread_create() is %d\n", rc);
		exit(-1);
	}
}




int main(int argc, char** argv)
{
	ebm_env_init(0);
	command_net_start(h_env->plat_ip, h_env->plat_port);
	passback_net_start(h_env->passback_ip, h_env->passback_port);
	creat_rtp_pthread();
	while(1)
	{
		command_net_loop();
		passback_net_loop();
	}
	command_net_stop();
	return 0;
}


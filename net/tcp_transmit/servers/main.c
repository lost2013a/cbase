#include "ebm_env.h"
#include "ebm_msg.h"






int main(int argc, char** argv)
{
	ebm_env_init(0);
	command_net_start(h_env->plat_ip, h_env->plat_port);
	while(1)
	{
		command_net_loop();
	}
	return 0;
}


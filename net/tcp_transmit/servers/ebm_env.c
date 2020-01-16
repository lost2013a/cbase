#include <stdio.h>

#include "ebm_env.h"


static struct ebm_env s_env;
struct ebm_env *h_env;

unsigned char ebm_env_init(unsigned char *input)
{
static const unsigned int test_ip= 192 << 24 | 168 << 16 | 251 << 8 | 181;
#define TRANSMIT_PORT	50003


	h_env= &s_env;
	h_env->plat_ip= test_ip;
	h_env->plat_port= TRANSMIT_PORT;
	return 1;

}








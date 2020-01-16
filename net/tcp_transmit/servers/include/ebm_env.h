#ifndef	_EBM_ENV_H
#define _EBM_ENV_H
#include "ebm_sys.h"
#include "ebm_msg.h"
#include "ebm_net.h"


struct ebm_env{
	unsigned int plat_ip;
	unsigned short plat_port;
};

unsigned char ebm_env_init(unsigned char *input);


extern struct ebm_env *h_env;


#endif

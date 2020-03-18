#ifndef	_EBM_ENV_H
#define _EBM_ENV_H
#include "ebm_sys.h"
#include "ebm_msg.h"
#include "ebm_net.h"
#include "ebm_passback.h"

#include "app_delay.h"


struct ebm_env{
	unsigned int plat_ip;
	unsigned short plat_port;
	unsigned int rtp_ip;
	unsigned short rtp_port;
	unsigned int passback_ip;
	unsigned short passback_port;
	unsigned char phy_id[6];
	unsigned char source_id[12];
	unsigned char register_1st_time;
};

extern struct ebm_env *h_env;


unsigned char ebm_env_init(unsigned char *input);
unsigned char ebm_env_set_vol(unsigned char vol);
unsigned char ebm_env_set_ip(unsigned char *ip);

unsigned char ebm_env_set_backip(unsigned char *ip, unsigned short port);
unsigned char ebm_env_set_sourceid(unsigned char *sourceid);
unsigned char ebm_env_set_trumpet(unsigned char val);
unsigned char ebm_env_set_systime(unsigned char val);
unsigned char ebm_env_set_back_period(unsigned char val);



unsigned char *ebm_env_get_sourceid(void);
unsigned char *ebm_env_get_phyid(void);

unsigned char ebm_env_get_register_1st_time(void);




#endif

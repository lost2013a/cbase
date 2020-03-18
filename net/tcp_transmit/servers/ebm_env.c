#include <stdio.h>

#include "ebm_env.h"


static struct ebm_env s_env;
struct ebm_env *h_env;

static unsigned char test_soucrceid[12]= {0xf6, 0x52, 0x04, 0x25, 0x10, 0x22,\
	0x01, 0x03, 0x14, 0x01 , 0x02 ,0x03};

static unsigned char test_phyid[6]= {0x12, 0x34, 0x56, 0x78, 0x90, 0x02};

unsigned char ebm_env_init(unsigned char *input)
{
static const unsigned int test_ip= 192 << 24 | 168 << 16 | 251 << 8 | 7;
#define TRANSMIT_PORT	4443

static const unsigned int test_mrtp_ip= 192 << 24 | 168 << 16 | 251 << 8 | 175;
#define TRANSMIT_MRTP_PORT	24002

static const unsigned int passback_ip= 192 << 24 | 168 << 16 | 251 << 8 | 7;
#define PASSBACK_PORT	8889


	h_env= &s_env;
	h_env->plat_ip= test_ip;
	h_env->plat_port= TRANSMIT_PORT;
	
	h_env->rtp_ip= test_mrtp_ip;
	h_env->rtp_port= TRANSMIT_MRTP_PORT;

	h_env->passback_ip= passback_ip;
	h_env->passback_port= PASSBACK_PORT;

	h_env->register_1st_time= 0;

	memcpy(h_env->source_id, test_soucrceid, 12);
	memcpy(h_env->phy_id, test_phyid, 6);
	

	
	return 1;

}


unsigned char ebm_env_set_vol(unsigned char vol)
{
	return 1;
}

unsigned char ebm_env_set_ip(unsigned char *ip)
{
#define TEST_SET_IP(a)
#define TEST_SET_MASK(a)
#define TEST_SET_GATWY(a)

	TEST_SET_IP(ip);
	TEST_SET_MASK(ip+4);
	TEST_SET_GATWY(ip+8);
	return 1;
}

unsigned char ebm_env_set_backip(unsigned char *ip, unsigned short port)
{
#define TEST_SET_IP(a)
#define TEST_SET_PORT(a)

	unsigned short tmp= swap16(port);
	TEST_SET_IP(ip);
	TEST_SET_PORT(tmp);
	return 1;
}

unsigned char ebm_env_set_sourceid(unsigned char *sourceid)
{
	return 1;
}

unsigned char ebm_env_set_trumpet(unsigned char val)
{

	return 1;
}

unsigned char ebm_env_set_systime(unsigned char val)
{

	return 1;
}


unsigned char ebm_env_set_back_period(unsigned char val)
{
	return 1;
}



unsigned char *ebm_env_get_sourceid(void)
{
	return h_env->source_id;
}

unsigned char ebm_env_get_register_1st_time(void)
{
	//debug
		return 2;

	if(h_env->register_1st_time == 0)
		h_env->register_1st_time= 1;
	else
		h_env->register_1st_time= 2;
	return h_env->register_1st_time;
}

unsigned char *ebm_env_get_phyid(void)
{
	return h_env->phy_id;

}




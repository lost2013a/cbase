#include <stdio.h>

#include "ebm_env.h"


static struct ebm_env s_env;
struct ebm_env *h_env;

unsigned char ebm_env_init(unsigned char *input)
{
static const unsigned int test_ip= 192 << 24 | 168 << 16 | 251 << 8 | 181;
#define TRANSMIT_PORT	50003
#if 1
static const unsigned int test_mrtp_ip= 192 << 24 | 168 << 16 | 251 << 8 | 170;
#define TRANSMIT_MRTP_PORT	5678
#else
static const unsigned int test_mrtp_ip= 237 << 24 | 12 << 16 | 12 << 8 | 12;
#define TRANSMIT_MRTP_PORT	1212
#endif

	h_env= &s_env;
	h_env->plat_ip= test_ip;
	h_env->plat_port= TRANSMIT_PORT;
	
	h_env->rtp_ip= test_mrtp_ip;
	h_env->rtp_port= TRANSMIT_MRTP_PORT;
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




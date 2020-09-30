#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "main.h"

#define SYSCFG_LEN sizeof(struct s_sys)

const unsigned char default_logic[NSTAR_LOGIC_ADDR_LENGTH]={
	0x51, 0x01, 0x08, 0x24, 0xff, 0xff	
};


static struct s_sys gsys={0};
const struct s_sys *rcfg = &gsys;
struct s_sys *wcfg = &gsys;

static volatile unsigned int saveflag = 0;
static void do_setflag(void)
{
    saveflag = 1;
}

static void do_clearflag(void)
{
    saveflag = 0;
}

static unsigned int flash_crc(unsigned char *data, unsigned int len)
{
    return nstar_crc32(data, len);
}

#define DO_SAVE_INTEGER() do{\
    int ret = -1;\
    if(*p != val) {\
        *p = val;\
        do_setflag();\
        ret = 0;\
    }\
    return ret;\
}while(0)

int setcfg_u32(unsigned int *p, unsigned int val) {
    DO_SAVE_INTEGER();
}

int setcfg_u16(unsigned short *p, unsigned short val) {
    DO_SAVE_INTEGER();
}

int setcfg_u8(unsigned char *p, unsigned char val) {
    DO_SAVE_INTEGER();
}

int setcfg_bytes(unsigned char *member, const unsigned char *data, int len) {
    int ret = -1;
    //if(sizeof(member) == len ) {
        if(0 != memcmp(member, data, len)) {
            memcpy(member, data, len);
            do_setflag();
            ret = 0;
        }
    //}
    return ret;
}

static void save_cfg_flash(void) {
    
    printf("plat write flash\n");
}

void handle_savecfg(void) {
    if(saveflag != 0) {
        gsys.crc32 = 0;
        gsys.crc32 = flash_crc((unsigned char*)&gsys, SYSCFG_LEN);
        printf("save crc = %08x\n", gsys.crc32);
        save_cfg_flash();
        do_clearflag();
    }
}




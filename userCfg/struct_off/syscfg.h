#ifndef SYS_CFG_H
#define SYS_CFG_H

#define NSTAR_LOGIC_ADDR_LENGTH 6
#define NSTAR_PASSWD_LENGTH		6

struct s_sys{
    unsigned char logic_addr[NSTAR_LOGIC_ADDR_LENGTH];
    unsigned char passwd[NSTAR_PASSWD_LENGTH];
    unsigned int shutdown_wcnt;
    unsigned int crc32;
    unsigned int reserve[32];
};


extern const struct s_sys *rcfg;
extern struct s_sys *wcfg;



extern int setcfg_u32(unsigned int *p, unsigned int val);
extern int setcfg_u16(unsigned short *p, unsigned short val);
extern int setcfg_u8(unsigned char *p, unsigned char val);
extern int setcfg_bytes(unsigned char *member, const unsigned char *data, int len);
extern void handle_savecfg(void);





#define CFGSET_SHUTDOWN_WCNT(val) setcfg_u32(&wcfg->shutdown_wcnt, val)
#define CFGSET_LOGIC_ADDR(data, len) setcfg_bytes(wcfg->logic_addr, data, len)



#endif





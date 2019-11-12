#include <ctype.h>
#include <dirent.h>
#include <grp.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
 
//未完成
#define container_of(ptr, type, member) ({const typeof( ((type *)0)-> member ) *__mptr = (ptr);(type *)( (char *)__mptr - offsetof(type,member) );})

 
struct tcfg{
unsigned char a;
unsigned short b;
unsigned int c;	
};

struct fcfg{
unsigned char e;
unsigned short f;
unsigned int g;	
struct tcfg *old_cfg;
};

 
int main(int argc, char *argv[]) {
	struct fcfg my_cfg;
	struct tcfg *p_cfg= &(my_cfg->old_cfg);
	
	struct fcfg *p_get_cfg= container_of(p_cfg, struct fcfg, old_cfg);
	
	printf("addr1=0x%x, addr2=0x%x\n", &my_cfg, p_get_cfg);
	return 0;
}
 


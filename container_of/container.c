#include <ctype.h>
#include <dirent.h>
#include <grp.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
 
/*通过member的地址计算container的首地址*/
#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)
#define container_of(ptr, type, member) ({const typeof( ((type *)0)-> member ) *__mptr = (ptr);(type *)( (char *)__mptr - offsetof(type,member) );})


//
 
struct child{
	unsigned char a;
	unsigned short b;
	unsigned int c;	
};

struct father{
	unsigned char e;
	unsigned short f;
	unsigned int g;	
	struct child *child_pstu;
};

 
int main(int argc, char *argv[]) {
	struct father father_stu;
	struct child *p= &(father_stu.child_pstu);
	struct father *p_father= container_of(p, struct father, child_pstu);
	printf("container addr1=0x%x, addr2=0x%x\n", &father_stu, p_father);
	return 0;
}
 

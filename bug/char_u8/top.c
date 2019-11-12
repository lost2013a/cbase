#include <ctype.h>
#include <dirent.h>
#include <grp.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
 
 

 
int main(int argc, char *argv[]) {
	char ip1[4]={192,168,251,5};
	char ip2[4]={192,168,251,5};
	int addr1,addr2;
	//addr1= (ip1[0]<<24)|(ip1[1]<<16)|(ip1[2]<<8)|(ip1[3]<<0);
	//addr1= (ip1[1]<<16)|(ip1[2]<<8)|(ip1[3]<<0);
	addr2= (ip2[0]<<24)|(ip2[1]<<16)|(ip2[2]<<8)|(ip2[3]<<0);
	addr1= (ip1[2]<<8);
	//addr2= (ip2[0]<<24);
	printf("addr1=0x%x, addr2=0x%x\n", addr1, addr2);
	return 0;
}
 


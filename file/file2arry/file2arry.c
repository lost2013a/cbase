
/*
将二进制转化成数组头文件 
*/
//Version:2016.12.16
//author:Y.X.YANG
#include <stdio.h>  
#include <stdlib.h>  
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
typedef unsigned char u8;
typedef unsigned int  u32;
unsigned int read_bin(char *path, u8 *buf, u32 size)  
{  
    FILE *infile;  
      
    if((infile=fopen(path,"rb"))==NULL)  
    {  
        printf( "\nCan not open the path: %s \n", path);  
        exit(-1);  
    }  
	unsigned int i;
	int idx=0,addlen;
	unsigned char tmp;
	for(i=0; i< size; i++){
		 fread(&tmp, sizeof(u8), 1, infile);
		 if(i%10 ==9){
			addlen= sprintf((char*)(buf+ idx), "0x%02X,\r\n", tmp);
		 }
		 else{
			 addlen= sprintf((char*)(buf+ idx), "0x%02X, ", tmp);
		 }
		 idx+=addlen;
	}
	//printf("%s\r\n", buf);
    fclose(infile);  
	return idx;
	
} 

void write_bin(char *path, u8 *buf, u32 size)  
{  
    FILE *outfile;  
      
    if((outfile=fopen(path,"wr"))==NULL)  
    {  
        printf( "\nCan not open the path: %s \n", path);  
        exit(-1);  
    }  
	
	fwrite(buf, size, 1, outfile);
	
    fclose(outfile);  
	
} 

u32 GetBinSize(char *filename)  
{     
    u32  siz = 0;     
    FILE  *fp = fopen(filename, "rb");     
    if (fp)   
    {        
        fseek(fp, 0, SEEK_END);        
        siz = ftell(fp);        
        fclose(fp);     
    }     
    return siz;  
} 
 
 
int main()  
{ 
	u8 *buf = NULL;  
	u32 size,wlen;  
	char *srcbin="test.png";  
	char *dstfile="out.h";  
	//读取目标.bin文件 
	//printf("please input src file path\n");  
	//scanf("%s",srcbin);
	
	//获取文件的大小 
	size = GetBinSize(srcbin);  
	//申请用于存放该文件的数组 
	buf = (unsigned char *)malloc(size*10);    
	//读取文件 
	wlen= read_bin(srcbin, buf, size);  	  
	//制作头文件，该头文件下含有两个数组，一个是有数据的，另外一个是全0数组
	//全0主要备用，以后要清空可以调用这个数组 
	write_bin(dstfile, buf, wlen);  
    return 0;  
}  

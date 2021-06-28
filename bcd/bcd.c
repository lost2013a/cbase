unsigned char BCDtoStr(char *str,unsigned char *BCD,int BCD_length)
{
	if(BCD==0 || BCD_length==0)
	return 0;
	int i,j;
	for(i=0,j=0;i<BCD_length;i++,j+=2)
	{
	str[j]= (BCD[i]>>4) > 9 ? (BCD[i]>>4)-10+'A' : (BCD[i]>>4)+'0';
	str[j+1]=(BCD[i] & 0x0F) >9 ? (BCD[i] & 0x0F)-10+'A' : (BCD[i] & 0x0F)+'0';
	}
	str[j]='\0';
	return 1;
}

unsigned char StrtoBCD(char *str,unsigned char *BCD,int *BCD_length)
{
	if(str==0) return 0;
	int tmp=strlen(str);
	tmp-=tmp%2;
	if(tmp==0) return 0;
	int i,j;
	for(i=0;i<tmp;i++)
	{
	if(str==0 ||
	!(str[i]>='0' && str[i]<='9' || str[i]>='a' && str[i]<='f' || str[i]>='A' && str[i]<='F')
	)
	return 0;
	}

	for(i=0,j=0;i<tmp/2;i++,j+=2)
	{
	str[j]>'9' ? 
	(str[j]>'F' ? BCD[i]=str[j]-'a'+10 : BCD[i]=str[j]-'A'+10)
	: BCD[i]=str[j]-'0';
	str[j+1]>'9' ? 
	(str[j+1]>'F' ? BCD[i]=(BCD[i]<<4)+str[j+1]-'a'+10 : BCD[i]=(BCD[i]<<4)+str[j+1]-'A'+10)
	: BCD[i]=(BCD[i]<<4)+str[j+1]-'0';        
	}
	if(BCD_length)
	*BCD_length=tmp/2;
	return 1;
}	 
	 
int main(void)
{
	char *str1 = "12345";
	unsigned char bcd1[10];
	
	char str2[20];
	unsigned char bcd2[4]={0x12,0x23,0x34,0x56};
	
	printf("StrtoBCD\n");
	if(StrtoBCD(str1, bcd1, 10)) {
		for(int i = 0; i < 10; i++) {
			printf("%02x ", bcd1[i]);	
		}
		printf("\n");
	}
	int len;
	printf("BCDtoStr\n");
	if(BCDtoStr(str2, bcd2, &len)) {
		printf("len %d %s\n",len, str2);
	}
	
	
	
	
}
/******************************************************************   
File   :   mp3info.c   
 *******************************************************************************/


#include<stdio.h>    
#include<string.h>   
#include<stdlib.h>  


#define SEEK_END 2    
#define ID3V1_size  128   

void getBin(int, char*);
int GetBitRate(int, int, int); //函数宣告  

int SamplingrateTable [3][3] = {
	{44100, 22050, 11025},
	{48000, 24000, 120000},
	{32000, 16000, 8000}
};


void main(int argc, char *argv[])
{
	FILE *fp;
	int bHasID3V2;
	char cVer;
	char cRevision;
	int bHasExtHeader = 0;
	int ID3V2_size;
	char cID3V1[128];
	char cID3V2_head[10];
	char cID3v2Frame_head[10];
	char *cID3v2Frame;
	long iID3v2Frame_length;
	if (argc < 2) {
		exit(0);
	}
	printf("open %s\n", argv[1]);
	/*##########################Symbol   In   Frame###################################*/

	long iFrameStart, iFrameMove = 0, iFrameCount = 0;
	unsigned long iFrameSize = 0;
	char cFrameHead_bin[33];
	char cFrameHead_bin_temp[9];

	/*##########################Symbol   In   ID3V1###################################*/

	/*##########################Symbol   Above######################################*/
	unsigned char* cBuffer;

	char* ChannelDescrip[4] = {"立体声", "混合立体声", "双声", "单声"};


	int cBuffer_size = 1024, Bitrate, FrameSize, FrameCount, mp3Duration;
	char LayerDescript, bRateIndex, bSampleRate, Version, bPadding, bChannelMode;
	long int flength;


	int i = 0, j = 0;


	/*##########################Symbol   Above######################################*/

	if ((fp = fopen(argv[1], "rb")) == NULL) {
		printf("ERROR:can't   open   the   file!");
		getchar();
		exit(1);
	}


	/*##########################ID3V2   Reading#####################################*/


	fread(cID3V2_head, 10, 1, fp);
	if ((cID3V2_head[0] == 'I' || cID3V2_head[1] == 'i')
		&&(cID3V2_head[1] == 'D' || cID3V2_head[2] == 'd')
		&& cID3V2_head[2] == '3') {
		bHasID3V2 = 1;
		cVer = cID3V2_head[3];
		cRevision = cID3V2_head[4];

		if (cID3V2_head[5]&0x64 == 0x64) bHasExtHeader = 1;
		if (cID3V2_head[5]&0x10 == 0x10) bHasExtHeader = 1;

		//ID3V2_size   =   (cID3V2_head[6]&0x7F)*0x200000   
		//+   (cID3V2_head[7]&0x7F)*0x400   
		//+   (cID3V2_head[8]&0x7F)*0x80   
		//+   (cID3V2_head[9]&0x7F);   
		ID3V2_size = (cID3V2_head[6]&0x7F) << 21
		| (cID3V2_head[7]&0x7F) << 14
		| (cID3V2_head[8]&0x7F) << 7
		| (cID3V2_head[9]&0x7F) + 10;

		printf("ID3V2_size: %d bytes\n", ID3V2_size);
		iFrameStart = --ID3V2_size;
	} 
	else {
		bHasID3V2 = 0;
		printf("There is no ID3V2\n");
	}
	
	rewind(fp);
	fseek(fp, 10L, 1); /*改文件指针指向文件头*/
	printf("\0=%d\n", '\0');
	while ((ftell(fp) + 10) <= ID3V2_size)
	{
		memset(cID3v2Frame_head, 0, 10); /*内存清空*/
		fread(cID3v2Frame_head, 10, 1, fp); /*ID3V2帧信息头读取*/
		iID3v2Frame_length = (long) (cID3v2Frame_head[4]*0x100000000
		+ cID3v2Frame_head[5]*0x10000
		+ cID3v2Frame_head[6]*0x100
		+ cID3v2Frame_head[7]); /*计算帧的大小.*/
		if (cID3v2Frame_head[0] == '\0'
		&& cID3v2Frame_head[1] == '\0'
		&& cID3v2Frame_head[2] == '\0'
		&& cID3v2Frame_head[3] == '\0')
		break;


		for (i = 0; i < 4; i++)
			putchar(cID3v2Frame_head[i]); /*FrameSign(4)*/
		putchar('-');
		putchar('>'); /*   Print   "->"*/


		cID3v2Frame = malloc(iID3v2Frame_length); /*   为   cID3v2Frame   指针分配内存,   功能类似于VB中的动   

		态数组*/
		if (cID3v2Frame != NULL) //   检测动态内存分配是否已经成功   
		{
			fread(cID3v2Frame, iID3v2Frame_length, 1, fp);
			i = 0;
			while (i < iID3v2Frame_length) {
				putchar(cID3v2Frame[i]);
				i++;
			}
			putchar('\n');
			free(cID3v2Frame);
		} 
		else {
			if (fp != NULL) fclose(fp);
			printf("ERROR:Memory   Exhausted!");
			exit(1);
		}
	}


	/*(mp3_FrameHeader)   Reading*/
	fseek(fp, 0L, SEEK_END); //移文件指针到文件尾   
	flength = ftell(fp); //文件长   


	rewind(fp);
	fseek(fp, 10 + ID3V2_size, 1);
	while ((ftell(fp) + 4) < flength) 
	{
		if (ftell(fp) + 1024 <= flength)
			cBuffer_size = 1024;
		else
			cBuffer_size = flength - ftell(fp);


		cBuffer = malloc(cBuffer_size);
		if (cBuffer == NULL) //   检测动态内存分配是否已经成功   
		{
			if (fp != NULL) fclose(fp);
			printf("ERROR:Memory   Exhausted!");
			exit(1);
		}
		memset(cBuffer, 0, cBuffer_size);
		fread(cBuffer, cBuffer_size, 1, fp);
		for (i = 0; i < (cBuffer_size - 4); i++) 
		{
			//Mp3帧头(FRAMEHEADER)格式如下：AAAAAAAA   AAABBCCD   EEEEFFGH   IIJJKLMM   
			LayerDescript = (cBuffer[i + 1] & 0x6) >> 1; //Part   CC   IN   'AAABBCCD'   ,Layer   description   '00'   -  reserved    

			//Part     EEEE   in   'EEEEFFGH',   But   '1111'   is   Bad   Bitrate   index,   "bad"   means   that   this   is   not   an   allowed   value   
			bRateIndex = cBuffer[i + 2] / 0x10; //   EEEEXXXX!='1111'   
			bSampleRate = (cBuffer[i + 2] & 0xA) >> 2; //Part   FF   IN   'EEEEFFGH'   ,'11'   reserved   

			if (cBuffer[i] == 0xFF
				&& cBuffer[i + 1] > 0xE0
				&& bRateIndex != 0xF
				&& LayerDescript != 0x0
				&& bSampleRate < 0x3) 
			{
				printf("Mp3_FrameHeader->");
				memset(cFrameHead_bin, 0, 32);
				j = cBuffer[i];
				getBin(j, cFrameHead_bin_temp);
				strcat(cFrameHead_bin, cFrameHead_bin_temp);
				putchar('-');

				j = cBuffer[i + 1];
				getBin(j, cFrameHead_bin_temp);
				strcat(cFrameHead_bin, cFrameHead_bin_temp); //strcat的两个传入参数都必需以   '\0'   代表结尾  
				putchar('-');

				j = cBuffer[i + 2];
				getBin(j, cFrameHead_bin_temp);
				strcat(cFrameHead_bin, cFrameHead_bin_temp);
				putchar('-');

				j = cBuffer[i + 3];
				getBin(j, cFrameHead_bin_temp);
				strcat(cFrameHead_bin, cFrameHead_bin_temp);

				Version = (cBuffer[i + 1] & 0x18) >> 3;
				bPadding = (cBuffer[i + 1] & 0x2) >> 1;
				Bitrate = GetBitRate(bRateIndex, LayerDescript, Version);
				bChannelMode = (cBuffer[i + 3] & 0xC0) >> 6;
				putchar('\n');

				if (bRateIndex != 0) {
					switch (Version) 
					{
						case 0:
							printf("MPEG   2.5   层   %d,   %dHz   %s\n", 4 - LayerDescript,
							SamplingrateTable[bSampleRate][2], ChannelDescrip[bChannelMode]);

							FrameSize = ((72 * Bitrate * 1000) / SamplingrateTable[bSampleRate][2]) + bPadding;
							FrameCount = (int) ((flength - (ftell(fp) - cBuffer_size + i)) / FrameSize);
							mp3Duration = (int) (FrameCount * 0.026); //每帧的播放时间：无论帧长是多少，每帧的播放时间都是26ms；   
							printf("CBR:->   %dKbit     %d帧\n", Bitrate, FrameCount);
							printf("长度:%d   秒", mp3Duration);
							break;
						case 2:
							printf("MPEG   2.0   层   %d,   %dHz   %s\n", 4 - LayerDescript,
							SamplingrateTable[bSampleRate][1], ChannelDescrip[bChannelMode]);

							FrameSize = ((72 * Bitrate * 1000) / SamplingrateTable[bSampleRate][1]) + bPadding;
							FrameCount = (int) ((flength - (ftell(fp) - cBuffer_size + i)) / FrameSize);
							mp3Duration = (int) (FrameCount * 0.026); //每帧的播放时间：无论帧长是多少，每帧的播放时间都是26ms；   
							printf("CBR:->   %dKbit     %d帧\n", Bitrate, FrameCount);
							printf("长度:%d   秒", mp3Duration);
							break;
						case 3:
							printf("MPEG   1.0   层   %d,   %dHz   %s\n", 4 - LayerDescript,
							SamplingrateTable[bSampleRate][0], ChannelDescrip[bChannelMode]);

							FrameSize = ((144 * Bitrate * 1000) / SamplingrateTable[bSampleRate][0]) + bPadding;
							FrameCount = (int) ((flength - (ftell(fp) - cBuffer_size + i)) / FrameSize);
							mp3Duration = (int) (FrameCount * 0.026); //每帧的播放时间：无论帧长是多少，每帧的播放时间都是26ms；   

							printf("CBR:->   %dKbit     %d帧\n", Bitrate, FrameCount);
							printf("长度:%d   秒", mp3Duration);
							break;
					}
				} else
				printf("\nThis   a   Free   Rate   MP3   File!\n");


				cBuffer_size = -1;
				break;
			}


		}
		free(cBuffer);
		if (cBuffer_size == (-1)) 
			break;
	}


	/*ID3V1   Reading*/


	fseek(fp, -128L, SEEK_END);
	fread(cID3V1, ID3V1_size, 1, fp);


	if ((cID3V1[0] == 'T' || cID3V1[0] == 't')
	&&(cID3V1[1] == 'A' || cID3V1[1] == 'a')
	&&(cID3V1[2] == 'G' || cID3V1[2] == 'g')){
		printf("\n*\nID3V1_size:%d   bytes\n", ID3V1_size);
		for (i = 0; i < 128; i++) 
		putchar(cID3V1[i]);
		putchar('|');
		putchar('\n');
	} 
	else {
		printf("\nThere   is   no   ID3V1*");
	}

	fclose(fp);
	}


char *itoa(int value, char *string, int radix)
{
	int rt = 0;
	if (string == NULL)
		return NULL;
	if (radix <= 0 || radix > 30)
		return NULL;
	rt = snprintf(string, radix, "%d", value);
	if (rt > radix)
		return NULL;
	string[rt] = '\0';
	return string;
}


void getBin(int dec, char* bin)
{
	int i, j, len;
	char temp[8];
	memset(temp, 0, 8);
	itoa(dec, temp, 2); ///把整数i转换成字符串将i转换为字符串放入temp中,最后一个数字表示2进制     
	len = strlen(temp);
	memset(bin, 0, strlen(bin));
	for (i = 0, j = 0; i < 8; i++, j++) {
		if (temp[j] != '\0')
			bin[i] = temp[j];
		else
			bin[i] = '0';
	}
	bin[i] = '\0';
	for (i = 0, j = 0; i < 8; i++, j++)
		putchar(bin[i]);
}


int GetBitRate(int bRateIndex, int LayerDescript, int Version)
{
	int BitrateTable[6][15] = {
		{-1, 32, 64, 96, 128, 160, 192, 224, 256, 288, 320, 352, 384, 416, 448},
		{-1, 32, 48, 56, 64, 80, 96, 112, 128, 160, 192, 224, 256, 320, 384},
		{-1, 32, 40, 48, 56, 64, 80, 96, 112, 128, 160, 192, 224, 256, 320},
		{-1, 32, 64, 96, 128, 160, 192, 2324, 256, 288, 320, 352, 384, 416, 448},
		{-1, 32, 48, 56, 64, 80, 96, 112, 128, 160, 192, 224, 256, 320, 384},
		{-1, 8, 16, 24, 32, 64, 80, 56, 64, 128, 160, 112, 128, 256, 320}
	}; //kbps       (-1)   means   :free   
	int i, j;
	if (Version == 3 && LayerDescript == 3)j = 0;
	else if (Version == 3 && LayerDescript == 2)j = 1;
	else if (Version == 3 && LayerDescript == 1)j = 2;
	else if (Version == 2 && LayerDescript == 1)j = 3;
	else if (Version == 2 && LayerDescript == 2)j = 4;
	else if (Version == 2 && LayerDescript == 3)j = 5;
	i = BitrateTable[j][bRateIndex];
	return i;
} 
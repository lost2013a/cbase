#include <stdio.h>;
#include <sys/types.h>;
#include <regex.h>;



/*
int regexec (regex_t *compiled, char *string, size_t nmatch, regmatch_t matchptr [], int eflags)

@compiled 已经由regcomp函数编译好的规则表达式
@string 是目标文本串
@nmatch 是regmatch_t结构体数组的长度
@matchptr 包含的成员
	rm_so 存放匹配文本串在目标串中的开始位置
	rm_eo 存放结束位置
*/

/* 取子串的函数 */
static char* substr(const char*str, unsigned start, unsigned end)
{
    unsigned n = end - start;
    static char stbuf[256];
    strncpy(stbuf, str + start, n);
    stbuf[n] = 0;
    return stbuf;
}
/* 主程序 */

//regex23

int main(int argc, char** argv)
{
	char * pattern;
	int x, z, lno = 0, cflags = 0;
	char ebuf[128], lbuf[256];
	regex_t reg;
	regmatch_t pm[1];
	const size_t nmatch = 1;
	

	if(argc < 2){
		printf("please input the match pattern\n");
		return;
	}
	pattern = argv[1];
	printf("pattern is %s\n", pattern);
	
	/* 编译正则表达式，放入reg*/
	z = regcomp(&reg, pattern, cflags);
	if (z != 0){
		regerror(z, &reg, ebuf, sizeof(ebuf));
		fprintf(stderr, "%s: pattern '%s' \n", ebuf, pattern);
		return 1;
	}
	while(fgets(lbuf, sizeof(lbuf), stdin))
	{
		++lno;
		if ((z = strlen(lbuf)) >0 && lbuf[z-1] == '\n')
			lbuf[z - 1] = 0;

		
		char *idx= lbuf;
		/* 对每一行应用正则表达式进行匹配 */
		z = regexec(&reg, idx, nmatch, pm, 0);
		
		if (z == REG_NOMATCH){
			continue;
		}
		else if (z != 0){
			regerror(z, &reg, ebuf, sizeof(ebuf));
			fprintf(stderr, "%s: regcom('%s')\n", ebuf, idx);
			return 2;
		}
		for (x = 0; x < nmatch && pm[x].rm_so != -1; ++ x) {
			if (!x){
				printf("line %04d: %s\n", lno, idx);
			}
			printf("    $%d='%s'\n", x, substr(idx, pm[x].rm_so, pm[x].rm_eo));
		}
	}
	/* 释放正则表达式    */
	regfree(&reg);
	return 0;
}

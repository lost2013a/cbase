#include <stdio.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>

#define MAX_LIST_LEN 1024
#define MAX_PATH_LEN 1024
#define MAX_NAME_LEN 256

struct media_node {
    char path[MAX_PATH_LEN];
    char name[MAX_NAME_LEN];
};

struct media_list {
    unsigned short n;
    unsigned short pos;
    unsigned char flag;
    struct media_node list[MAX_LIST_LEN];
}g_media;

/*优先NSTARMP3目录->根目录*/
/*
stat 结构的数据结构成员st_mode
S_ISLNK(st_mode)：是否是一个连接.
S_ISREG(st_mode)：是否是一个常规文件.
S_ISDIR(st_mode)：是否是一个目录
S_ISCHR(st_mode)：是否是一个字符设备.
S_ISBLK(st_mode)：是否是一个块设备
S_ISFIFO(st_mode)：是否 是一个FIFO文件.
S_ISSOCK(st_mode)：是否是一个SOCKET文件 
*/
//优先级（U盘目录） NSTARMP3 -> MP3 -> 根目录
//mp3搜索模式    : 0全目录搜索 1单目录搜索


static void list_reinit(void) {
    memset(&g_media, 0, sizeof(g_media));
}

const char* strstri(const char* str, const char* subStr)
   {
       int len = strlen(subStr);
       if(len == 0)
       {
           return NULL;
       }

       while(*str)
       {
           if(strncasecmp(str, subStr, len) == 0)
           {
               return str;
           }
           ++str;
       }
       return NULL;
   }

static int cmp_file_postfix(const char *d_name, const char *postfix) 
{
    int len = strlen(d_name) - strlen(postfix);
    if(len) {
        if(strncasecmp(d_name+len, postfix, len) == 0)
        {
            return 1;
        }
    }
    return 0;
}    

static int list_add(const char *path, const char *name) 
{
    unsigned short i = g_media.n;
    if(i < MAX_LIST_LEN ) {
        strncpy(g_media.list[i].path, path, MAX_PATH_LEN - 1);
        strncpy(g_media.list[i].name, name, MAX_NAME_LEN - 1);
        g_media.n++;
        return 0;
    }
    printf("media list full\n");
    return -1;
}   

static void display_list(const struct media_list *m) {
    printf("media list: all %d tracks / play position is %d\n", \
            m->n, m->pos);
    for(int i=0; i< m->n; i++) {
        printf("%d: [%s] (%s)\n", i, m->list[i].path, m->list[i].name);
    }
}


static int dir_run(const char *path, int mode)
{   
    DIR *dir;
    struct stat st;
    struct dirent *entry;
    char fp[MAX_PATH_LEN];
    if( NULL == (dir = opendir(path)) ) {
        return -1;
    }
    while((entry = readdir(dir)) != NULL)
    {
       
        if((strcmp(entry->d_name, ".") == 0) || (strcmp(entry->d_name, "..") == 0))
        {
            continue;
        }

        sprintf(fp,"%s/%s",path, entry->d_name);
        stat(fp,&st);
        if(S_ISREG(st.st_mode))
        {    
            if(cmp_file_postfix(entry->d_name, ".mp3"))
            {
                if(list_add(path, entry->d_name) != 0) {
                    return 0;
                }
                g_media.flag = 1;
            }
        }
        if(S_ISDIR(st.st_mode)){
            if(mode == 0 && g_media.flag == 0) {
                dir_run(fp, mode);
            }      
        }
    }
    return 0;
}


int usb_media_reinit(const char *path) 
{
    list_reinit();
    return dir_run(path, 0);
}

void ubs_media_relase(void) 
{
    list_reinit();
}


int main(void) 
{
    if( 0 == usb_media_reinit(".")) {
        display_list(&g_media);
    }
    return 0;
}



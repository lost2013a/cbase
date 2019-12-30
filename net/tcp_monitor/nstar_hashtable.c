#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUCKETCOUNT 20

#define LOGIDLEN 6
#define TCNT_INITVAL 5
struct hashEntry
{
    unsigned short key;
    unsigned char* value;
	unsigned char tcnt;
    struct hashEntry* next;
};

typedef struct hashEntry entry;

struct hashTable
{
    entry bucket[BUCKETCOUNT];  
};

typedef struct hashTable table;


unsigned char* strDup(const unsigned char* str)
{
    int len= LOGIDLEN;
    unsigned char* ret;
    if (str == NULL)
		return NULL;
    ret = (unsigned char*)malloc(len);
    if (ret != NULL) {
        memcpy(ret , str , len);
    }
    return ret;
}

//哈希散列方法函数
int keyToIndex(unsigned short key)
{
    return key%BUCKETCOUNT;
}


//初始化哈希表
void initHashTable(table* t)
{
    int i;
    if (t == NULL)return;

    for (i = 0; i < BUCKETCOUNT; ++i) {
        t->bucket[i].key = 0;
        t->bucket[i].value = NULL;
	 	t->bucket[i].tcnt = 0;
        t->bucket[i].next = NULL;
    }
}

//释放哈希表
void freeHashTable(table* t)
{
    int i;
    entry* e,*ep;
    if (t == NULL)return;
    for (i = 0; i<BUCKETCOUNT; ++i) {
        e = &(t->bucket[i]);
        while (e->next != NULL) {
            ep = e->next;
            e->next = ep->next;
            free(ep->value);
            free(ep);
        }
    }
}


//向哈希表中插入数据
int insertEntry(table* t , unsigned short key , const unsigned char* value)
{
    int index , vlen1 , vlen2;
    entry* e , *ep;

    if (t == NULL || key == 0 || value == NULL) {
        return -1;
    }

    index = keyToIndex(key);
    if (t->bucket[index].key == 0) {
        t->bucket[index].key = key;
        t->bucket[index].value = strDup(value);
		t->bucket[index].tcnt = TCNT_INITVAL;
    }
    else {
        e = ep = &(t->bucket[index]);
        while (e != NULL) { 
            if (memcmp(e->value , value, LOGIDLEN) == 0) {
				e->tcnt= TCNT_INITVAL;
                return index;   //已存在
            }
            ep = e;
            e = e->next;
        } 
        //没有在当前桶中找到
        //创建条目加入
        e = (entry*)malloc(sizeof (entry));
        e->key = key;
        e->value = strDup(value);
        e->next = NULL;
		e->tcnt = TCNT_INITVAL;
        ep->next = e;
    }
    return index;
}

//在哈希表中查找key对应的value
//找到了返回value的地址，没找到返回NULL
const unsigned char* findValueByKey(const table* t , unsigned short key)
{
    int index;
    const entry* e;
    if (t == NULL || key == 0) {
        return NULL;
    }
    index = keyToIndex(key);
    e = &(t->bucket[index]);
    if (e->key == 0) return NULL;//这个桶还没有元素
    while (e != NULL) {
        if (key == e->key) {
            return e->value;    //找到了，返回值
        }
        e = e->next;
    }
    return NULL;
}


//在哈希表中查找key对应的entry
//找到了返回entry，并将其从哈希表中移除
//没找到返回NULL
entry* removeEntry(table* t , unsigned short key)
{
    int index;
    entry* e,*ep;   //查找的时候，把ep作为返回值
    if (t == NULL || key == 0) {
        return NULL;
    }
    index = keyToIndex(key);
    e = &(t->bucket[index]);
    while (e != NULL) {
        if (key == e->key) {	/*找到元素*/
            if (e == &(t->bucket[index])) { //如果是桶的第一个
                ep = e->next;
                if (ep != NULL) {
					//如果这个桶有两个或以上元素 
               		//交换第一个和第二个，然后移除第二个
                    entry tmp = *e; //做浅拷贝交换
                    *e = *ep;//相当于链表的头节点已经移除
                    *ep = tmp;  //这就是移除下来的链表头节点
                    ep->next = NULL;
                }
                else {//这个桶只有第一个元素
                    ep = (entry*)malloc(sizeof(entry));
                    *ep = *e;
                    e->key =0;
					e->value = NULL;
                    e->next = NULL;
                }
            }
            else {
                //如果不是桶的第一个元素
                //找到它的前一个(这是前面设计不佳导致的多余操作)
                ep = &(t->bucket[index]);
                while (ep->next != e)ep = ep->next;
                //将e从中拿出来
                ep->next = e->next;
                e->next = NULL;
                ep = e;
            }
            return ep;
        }// end if(strcmp...
        e = e->next;
    }
    return NULL;
}


//在哈希表中查找key对应的entry
//找到了返回entry，并将其从哈希表中移除
//没找到返回NULL
entry* reduce_entry(table* t , unsigned short key)
{
    int index;
    entry* e,*ep;   //查找的时候，把ep作为返回值
    if (t == NULL || key == 0) {
        return NULL;
    }
    index = keyToIndex(key);
    e = &(t->bucket[index]);
    while (e != NULL) {
        if (key == e->key) {	/*找到元素*/
            if (e == &(t->bucket[index])) { //如果是桶的第一个
                ep = e->next;
                if (ep != NULL) {
					//如果这个桶有两个或以上元素 
               		//交换第一个和第二个，然后移除第二个
                    entry tmp = *e; //做浅拷贝交换
                    *e = *ep;//相当于链表的头节点已经移除
                    *ep = tmp;  //这就是移除下来的链表头节点
                    ep->next = NULL;
                }
                else {//这个桶只有第一个元素
                    ep = (entry*)malloc(sizeof(entry));
                    *ep = *e;
                    e->key =0;
					e->value = NULL;
                    e->next = NULL;
                }
            }
            else {
                //如果不是桶的第一个元素
                //找到它的前一个(这是前面设计不佳导致的多余操作)
                ep = &(t->bucket[index]);
                while (ep->next != e)ep = ep->next;
                //将e从中拿出来
                ep->next = e->next;
                e->next = NULL;
                ep = e;
            }
            return ep;
        }// end if(strcmp...
        e = e->next;
    }
    return NULL;
}


void printTable(table* t)
{
    int i;
    entry* e;
	unsigned short old;
    if (t == NULL)return;
    for (i = 0; i<BUCKETCOUNT; ++i) {
        printf("bucket[%d]:\n" , i);
        e = &(t->bucket[i]);
		old=0;
        while (e->key != 0) {
			if(old != e->key)
			{
            	printf("\t%d\t=\t%02x%02x%02x%02x%02x%02x\n" , e->key , e->value[0],
					e->value[1],e->value[2],e->value[3],e->value[4],e->value[5]);
				old= e->key;
			}
            if (e->next == NULL)break;
            e = e->next;
        }
    }
}


table t;
void id_table_init(void)
{
	 initHashTable(&t);
}

void id_table_add(unsigned short id, unsigned char *buf)
{
	 insertEntry(&t , id , buf);
}


void id_table_handle(void)
{
    int i;
    entry* e;
    if (t == NULL)
		return;
    for (i = 0; i<BUCKETCOUNT; ++i) {
        e = &(t->bucket[i]);
        while (e->key != 0) {

			if(e->tcnt--){

			}
			else{
				removeEntry
			}
				
		
            if (e->next == NULL)
				break;
            e = e->next;
        }
    }
}


void id_table_loop(unsigned short id, unsigned char *buf)
{
	 insertEntry(&t , id , buf);
}



void id_table_serve(void *parm)
{
    table t;entry* e ;
    initHashTable(&t);

    insertEntry(&t , 6502 , buf);
	insertEntry(&t , 6503 , buf);

	buf[5]++;
	insertEntry(&t , 6504 , buf);

	buf[5]++;
	insertEntry(&t , 6504 , buf);

	buf[5]++;
	insertEntry(&t , 6504 , buf);

    e = removeEntry(&t , 6504);
    if (e != NULL) {
        printf("找到后要释放\n");
		e->key=0;
        free(e->value);
        free(e);
        e = NULL;
    }
	
    printTable(&t);
    unsigned short keys[4] = { 6501 , 6502, 6509 , 0 };
    for (int i = 0; i < 4; ++i) {
        const unsigned char* value = findValueByKey(&t , keys[i]);
        if (value != NULL) {
            printf("find %d\t=\t%02x%02x%02x%02x%02x%02x\n\n" , keys[i] , value[0],
					value[1], value[2], value[3], value[4], value[5]);
        }
        else {
            printf("not found %d\n",keys[i]);
        }
    }

    freeHashTable(&t);
    return 0;
}


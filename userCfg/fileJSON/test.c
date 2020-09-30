/*
2020-09-17 总结：
反序列化太麻烦，还是不太适用。
如果只做序列化到还可以一用。
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cJSON.h"

#define FileName "test.json"



/* Used by some code below as an example datatype. */
struct record
{
    const char *precision;
    int lat;
    int lon;
    const char *address;
    const char *city;
    const char *state;
    const char *zip;
    const char *country;
};

struct Cfg {
   unsigned int magic;
   unsigned int version;
   unsigned int crcval;
   unsigned int crcval;
   struct record fields[2];
};

/* Our array of "records": */
struct Cfg userCfg ={
    .enable = 1,
    .fields = {
        {
            "zip",
            377668,
            223959,
            "",
            "SAN FRANCISCO",
            "CA",
            "94107",
            "US"
        },
        {
            "zip",
            -231,
            1999,
            "",
            "SUNNYVALE",
            "CA",
            "94085",
            "US"
        }
    }
};

static size_t size = 0, maxsize = 0;
static void *cjson_malloc(size_t sz)
{
    void *ptr = malloc(sz);
    size+= sz;
    if(maxsize < size)
        maxsize = size;
    //printf("cjson_malloc %ld byte %p, %ld\n", sz, ptr, maxsize);
	return ptr;
}

static void cjson_free(void *ptr)
{
	//printf("cjson_free %p\n", ptr);
    size = 0;
	free(ptr);
}

/* Create a bunch of objects as demonstration. */
static int save(cJSON *root)
{ 
#if 0    
    char *out = NULL;
    out = cJSON_Print(root); /*注意必须用了之后free*/
    printf("%ld :\n %s\n",strlen(out), out);
	FILE* fp = fopen(FileName ,"w+"); 	
	fprintf(fp, "%s", out);
    free(out);
	fclose(fp);
    return 0;
#else
    char *out = malloc(FileName);
    if (!cJSON_PrintPreallocated(root, out, MaxRam, 1)) {
        printf("cJSON Print failed!\n");
        return -1;
    }
    FILE* fp = fopen(FileName ,"w+"); 	
	fprintf(fp, "%s", out);
	fclose(fp);
    free(out);
#endif
    return 0;
}





/* Create a bunch of objects as demonstration. */
static void serialize(void)
{
    cJSON *root = NULL;
    cJSON *fields = NULL;
    cJSON *fld = NULL;
    int i = 0;
    root = cJSON_CreateObject();
    cJSON_AddNumberToObject(root, "Enable", userCfg.enable);
    cJSON_AddItemToObject(root, "fields", fields = cJSON_CreateArray());
    for (i = 0; i < 2; i++)
    {
        cJSON_AddItemToArray(fields, fld = cJSON_CreateObject());
        cJSON_AddStringToObject(fld, "precision", userCfg.fields[i].precision);
        cJSON_AddNumberToObject(fld, "Latitude", userCfg.fields[i].lat);
        cJSON_AddNumberToObject(fld, "Longitude", userCfg.fields[i].lon);
        cJSON_AddStringToObject(fld, "Address", userCfg.fields[i].address);
        cJSON_AddStringToObject(fld, "City", userCfg.fields[i].city);
        cJSON_AddStringToObject(fld, "State", userCfg.fields[i].state);
        cJSON_AddStringToObject(fld, "Zip", userCfg.fields[i].zip);
        cJSON_AddStringToObject(fld, "Country", userCfg.fields[i].country);
    }
    save(root); 
}


//static 
//cJSON* getItem(const cJSON * const object, const char * const string) {
//    
//}

int unserialize(char *text)
{
    int ret = -1;
    cJSON *root = cJSON_ParseWithLength(text, MaxRam);
    cJSON *fields = NULL;
    cJSON *fld = NULL;
	if (!root) {
        printf("cJSON unserialize failed\n");
    }else {
        fields = cJSON_GetObjectItem(root, "fields");
        if(fields != NULL) {
            fld = cJSON_GetArrayItem(fields, 0);
            if(fld  == NULL) {
                printf("fld not find\n");
            }else {
                cJSON *a = cJSON_GetObjectItem(fld, "precision");
                if(a != NULL) {
                    printf("a = %d", cJSON_GetNumberValue(a));
                    //printf("a = %s\n", cJSON_GetStringValue(a));
                }
            }
        }else {
            printf("not find\n");
        }
        
       
	}
}

void dofile(void)
{
	FILE *f;long len;char *data;
	f= fopen(FileName ,"rb");
	fseek(f, 0, SEEK_END);
	len= ftell(f);
	fseek(f, 0, SEEK_SET);
	data=(char*)malloc(len + 1);
	fread(data, 1, len, f);
	fclose(f);
	unserialize(data);
	free(data);
}

int CJSON_CDECL main(void)
{
    printf("Version: %s\n", cJSON_Version());
    cJSON_Hooks hooks= {cjson_malloc, cjson_free};
	cJSON_InitHooks(&hooks);
    serialize();
    dofile();
    return 0;
}

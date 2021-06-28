#include "stdio.h"
#include "string.h"

#include "libini.h"

#if 1
#define GXOTA_DBG(...)                printf( __VA_ARGS__)
#else
#define GXOTA_DBG(...)
#endif

#define OEM_UPDATE_SECTION              ("update")
#define OEM_UPDATE_FLAG                 ("flag")
#define OEM_UPDATE_VARIABLE             ("yes")

#define OTA_FIND_START_ADDR          		0x10000
#define OTA_FIND_STEP                   	0x10000
#define OTA_USED_SIZE                   	0x30000

static const char *inifile = "1.ini";



static unsigned int GxOem_INILoad(char *buf, int size)
{
	return ini_loadmemory(buf, size);
}

static char* GxOem_INIGetValue(unsigned int handle, const char* section, const char* key)
{
	char* value;

	if (!handle) {
		printf("handle error in %s\n", __FUNCTION__);
		return NULL;
	}

	value = ini_get(handle, section, key);

	return value;
}

static int GxOem_INISetValue(unsigned int handle, const char* section,
		const char* key, const char* value)
{
	int ret = 0;

	if (!handle) {
		printf("handle error in %s\n", __FUNCTION__);
		return -1;
	}

	ret = ini_set(handle, section, key, value);

	return ret;
}

static int GxOem_INISave(unsigned int handle, char* buf, int size)
{
	if (!handle) {
		printf("handle error in %s\n", __FUNCTION__);
		return -1;
	}
	memset(buf, 0, size);
	size = ini_savememory(handle, buf, size);

	return size;
}


void read_inifile(char *buf, unsigned int len) {
    int fd = open(inifile), len;
    if(fd) {
        len = read(fd, buf, len);
        if(len > 0) {
            printf("read ini ok %d bytes\n", len);
        }  
    }
}

int main(void)
{
	char   *pflag;
	struct partition_info* partition_ota;
	unsigned int oem_handle;
	char *pv_oem = NULL;
    unsigned int v_oem_size = 1024*10;
	pv_oem = malloc(v_oem_size);
	memset(pv_oem, 0, v_oem_size);
    read_inifile(pv_oem, v_oem_size);
	oem_handle = GxOem_INILoad(pv_oem, v_oem_size);
	pflag = GxOem_INIGetValue(oem_handle, OEM_UPDATE_SECTION, OEM_UPDATE_FLAG);
	if (pflag){
		GXOTA_DBG("pflag = %s \r\n",  pflag);
	} else{
		printf("pflag=%d, will not enter ota.\n", pflag);
	}
    return 0;
}


/*
  Copyright (c) 2009-2017 Dave Gamble and cJSON contributors

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cJSON.h"

/* Used by some code below as an example datatype. */
struct record
{
    const char *precision;
    double lat;
    double lon;
    const char *address;
    const char *city;
    const char *state;
    const char *zip;
    const char *country;
};

/* Our array of "records": */
struct record fields[2] =
{
    {
        "zip",
        37.7668,
        -1.223959e+2,
        "",
        "SAN FRANCISCO",
        "CA",
        "94107",
        "US"
    },
    {
        "zip",
        37.371991,
        -1.22026e+2,
        "",
        "SUNNYVALE",
        "CA",
        "94085",
        "US"
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
static int creat(cJSON *root)
{
    char *out = NULL;
    out = cJSON_Print(root);
    printf("%s\n", out);
	FILE* fp = fopen("test.json", "w+"); 	
	fprintf(fp, "%s", out);   
	fclose(fp);
    return 0;
}

/* Create a bunch of objects as demonstration. */
static void create_objects(void)
{
    cJSON *root = NULL;
    cJSON *fmt = NULL;
    cJSON *fld = NULL;
    int i = 0;
    root = cJSON_CreateObject();
    cJSON_AddNumberToObject(root, "Enable", 1);
    cJSON_AddItemToObject(root, "Image", fmt = cJSON_CreateArray());
    for (i = 0; i < 2; i++)
    {
        cJSON_AddItemToArray(fmt, fld = cJSON_CreateObject());
        cJSON_AddStringToObject(fld, "precision", fields[i].precision);
        cJSON_AddNumberToObject(fld, "Latitude", fields[i].lat);
        cJSON_AddNumberToObject(fld, "Longitude", fields[i].lon);
        cJSON_AddStringToObject(fld, "Address", fields[i].address);
        cJSON_AddStringToObject(fld, "City", fields[i].city);
        cJSON_AddStringToObject(fld, "State", fields[i].state);
        cJSON_AddStringToObject(fld, "Zip", fields[i].zip);
        cJSON_AddStringToObject(fld, "Country", fields[i].country);
    }
    creat(root); 
}


int CJSON_CDECL main(void)
{
    printf("Version: %s\n", cJSON_Version());
    cJSON_Hooks hooks= {cjson_malloc, cjson_free};
	cJSON_InitHooks(&hooks);
    create_objects();
    return 0;
}

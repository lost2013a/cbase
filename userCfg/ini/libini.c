#include "libini.h"
#include "list.h"
#include "string.h"
#include "stdio.h"

typedef struct {
	struct list_head  entry;
	char*             param;
	char*             value;
}INI_PARAM;

typedef struct {
	struct list_head  entry;
	char*             section;
	struct list_head  param_list;
}INI_SECTION;

typedef struct {
	char*             fname;
	struct list_head  section_list;
}INI;

static char* skip_head_blank(char* str)
{
	char* tmp;

	////assert(str != NULL);

	for(tmp = str; *tmp==' '||*tmp=='\t'; tmp++) ;

	return tmp;
}

static char* skip_tail_blank(char* str)
{
	int     len;

	//assert(str != NULL);

	for(len = strlen(str) - 1; str[len] == ' ' || str[len] == '\t'; len--) {
		;
	}

	str[++len] = '\0';

	return str;
}

static char* skip_comment(char* str)
{
	char*   tmp = str;

	//assert(str != NULL);

	for(tmp = str; *tmp != '\0' && *tmp != ';'; tmp++) {
		;
	}

	*tmp = '\0';

	return str;
}

static INI_SECTION* ini_append_section(INI*         pini,
		const char*  section)
{
	INI_SECTION* sec;

	//assert(pini != NULL);
	//assert(section != NULL);

	sec = malloc(sizeof(INI_SECTION));
	if (sec == NULL) {
		return NULL;
	}
	sec->section = malloc(strlen(section) + 1);
	//sec->section = strdup(section);
	if (sec->section == NULL) {
		return NULL;
	}
	strcpy(sec->section, section);


	INIT_LIST_HEAD(&sec->param_list);
	list_add_tail( &sec->entry, &pini->section_list);

	return sec;
}

static INI_PARAM* ini_append_param(INI_SECTION*     section,
		const char*      param,
		const char*      value)
{
	INI_PARAM* pa;

	//assert(section != NULL);
	//assert(param != NULL);
	//assert(value != NULL);

	pa = malloc(sizeof(INI_PARAM));
	if (pa == NULL) {
		return NULL;
	}
	pa->param = malloc(strlen(param) + 1);
	pa->value = malloc(strlen(value) + 1);

	//pa->param = strdup(param);
	//pa->value = strdup(value);

	if(pa->param == NULL) {
		return NULL;
	}

	if (pa->value == NULL) {
		free(pa->param);
		return NULL;
	}

	strcpy(pa->param, param);
	strcpy(pa->value, value);
	list_add_tail(&pa->entry, &section->param_list);

	return pa;
}

INI_SECTION *ini_addline(handle_t ini, INI_SECTION* sec, char *line)
{
	INI*            pini = (INI*) ini;
	char*           p;
	char*           s;
	char*           param = NULL;
	char*           value = NULL;

	p = skip_head_blank(line);
	if ( *p == '#' || *p == '\r' || *p == '\n') {
		return NULL;
	}

	if (*p == '[') {
		p = skip_head_blank(++p);
		for (s = p; *p != ']' && *p != '\r' && *p != '\n'&&*p !='\0'; p++) {
			;
		}
		*p = '\0';
		skip_tail_blank(s);
		sec = ini_append_section(pini, s);
	} else if (sec != NULL) {
		for (s = p; *p != '='&&*p != '\r' && *p != '\n'&&*p !='\0'; p++) {
			;
		}
		*p++ = '\0';

		param = s;
		p = skip_head_blank(p);
		for (s = p; *p!='\0'&&*p!='='&&*p!='\r'&&*p!='\n'; p++){
			;
		}
		*p = '\0';
		value = s;

		param = skip_comment(skip_tail_blank(param));
		value = skip_comment(skip_tail_blank(value));
		ini_append_param(sec, param , value);
	}

	return sec;
}

handle_t ini_loadmemory(const char *memory, int size)
{
	INI*    pini;
	int     DataLen = 0, cur_index = 0;
	char    line[INI_LINE_LEN];
	char    *pStr;
	INI_SECTION*    sec = NULL;

	pini = (INI *)malloc(sizeof(INI));
	memset(pini, 0, sizeof(INI));
	//assert(pini!=NULL);
	if (pini == NULL) {
		return EINVALID_HANDLE;
	}

	memset(pini, 0, sizeof(INI));
	INIT_LIST_HEAD(&pini->section_list);

	while (DataLen < size) {
		memset(line, 0, INI_LINE_LEN);
		cur_index = 0;

		while (cur_index < INI_LINE_LEN - 1) {
			if (DataLen >= size)
				break;
			if (memory[DataLen] == 0xFF) {
				DataLen++;
				continue;
			}
			if (memory[DataLen] == 0xA) {
				line[cur_index++] = memory[DataLen++];
				break;
			}
			line[cur_index++] = memory[DataLen++];
		}
		line[cur_index] = '\0';
		if (strlen(line) == 0) {
			continue;
		}

		pStr = strchr(line, '\n');

		if (pStr != NULL) *pStr = 0;

		sec = ini_addline((handle_t)pini, sec, line);
	}

	return (handle_t) pini;
}

unsigned int ini_savememory(handle_t ini, char* mem, unsigned int size)
{
	char line[INI_LINE_LEN];
	struct list_head*     pos1;
	struct list_head*     pos2;
	struct list_head*     n1;
	struct list_head*     n2;
	INI_SECTION*            sec;
	INI_PARAM*              pa;
	INI*                    pini = (INI*)ini;
	unsigned int                  w_size = 0;
	unsigned int                  len = 0;

	if (EINVALID_HANDLE == ini || pini == NULL) {
		return 0;
	}

	list_for_each_safe(pos1, n1, &pini->section_list) {
		sec = list_entry(pos1, INI_SECTION, entry);
		memset(line, 0, INI_LINE_LEN);
		strcpy(line, "[");
		strcat(line, sec->section);
		strcat(line, "]");
		len = strlen(line);
		w_size += len;
		if (w_size > size) {
			char* ptr;

			ptr = malloc(size + 1024);
			//mem = realloc(mem, size);
			if (ptr == NULL) {
				return 0;
			}
			memcpy(ptr, mem, size);
			free(mem);
			mem = ptr;
			size += 1024;
		}
		strcat(mem, line);
		strcat(mem, "\n");
		w_size +=1;//算上新加的'\n'

		list_for_each_safe(pos2, n2, &sec->param_list) {
			pa = list_entry(pos2, INI_PARAM, entry);
			memset(line, 0, INI_LINE_LEN);
			strncpy(line, pa->param, INI_LINE_LEN);
			strcat(line, "=");
			strcat(line, pa->value);
			len = strlen(line);
			w_size += len;
			if (w_size > size) {
				char* ptr;
				ptr = malloc(size + 1024);
				//mem = realloc(mem, size);
				if (ptr == NULL) {
					return 0;
				}
				memcpy(ptr, mem, size);
				free(mem);
				mem = ptr;
				size += 1024;
			}
			strcat(mem, line);
			strcat(mem, "\n");
			w_size +=1;//算上新加的'\n'
		}
	}
	return w_size;
}








char *ini_get(handle_t          ini,
		const char*       section,
		const char*       param)
{
	struct list_head*     pos;
	struct list_head*     n1;
	struct list_head*     n2;
	INI_SECTION*            sec;
	INI_PARAM*              pa;
	INI*                    pini = (INI*)ini;

	if (EINVALID_HANDLE == ini
			|| pini == NULL
			|| section == NULL
			|| param == NULL) {
		return NULL;
	}

	list_for_each_safe(pos, n1, &pini->section_list) {
		sec = list_entry(pos, INI_SECTION, entry);
		if (strcmp(sec->section, section) == 0) {
			list_for_each_safe(pos, n2, &sec->param_list) {
				pa = list_entry(pos, INI_PARAM, entry);
				if (strcmp(pa->param, param) == 0) {
					return pa->value;
				}
			}
		}
	}

	return NULL;
}


int ini_set(handle_t            ini,
		const char*         section,
		const char*         param,
		const char*         value)
{
	struct list_head*     pos;
	struct list_head*     n1;
	struct list_head*     n2;
	INI_SECTION*            sec;
	INI_PARAM*              pa;
	INI*                    pini = (INI*)ini;

	if (EINVALID_HANDLE == ini
			||pini == NULL
			|| section == NULL
			|| param == NULL
			|| value == NULL) {
		return -1;
	}

	list_for_each_safe(pos, n1, &pini->section_list) {
		sec = list_entry(pos, INI_SECTION, entry);
		if (strcmp(sec->section, section) == 0) {
			list_for_each_safe(pos, n2, &sec->param_list) {
				pa = list_entry(pos, INI_PARAM, entry);
				if (strcmp(pa->param, param) == 0) {
					if (strlen(pa->value) >= strlen(value)) {
						strcpy(pa->value, value);
					} else {
						free(pa->value);
						pa->value = malloc(strlen(value) + 1);
						strcpy(pa->value, value);
						//pa->value = strdup(value);
					}
					return 0;
				}
			}
		}
	}

	return -1;
}

int ini_append(handle_t         ini,
		const char*      section,
		const char*      param,
		const char*      value)
{
	struct list_head*     pos;
	struct list_head*     n1;
	INI_SECTION*            sec;
	INI*                    pini = (INI*)ini;

	if (EINVALID_HANDLE == ini
			||pini == NULL
			|| param == NULL
			|| value == NULL) {
		return -1;
	}

	list_for_each_safe(pos, n1, &pini->section_list) {
		sec = list_entry(pos, INI_SECTION, entry);
		if (strcmp(sec->section, section) == 0) {
			if( ini_append_param(sec, param ,value) == NULL) {
				return -1;
			}
			return 0;
		}
	}

	sec = ini_append_section(pini, section);
	if( ini_append_param(sec, param, value) == NULL) {
		return -1;
	}

	return 0;
}


int ini_remove(handle_t         ini,
		const char*      section,
		const char*      param)
{
	struct list_head*     pos;
	struct list_head*     n1;
	struct list_head*     n2;
	INI_SECTION*            sec;
	INI_PARAM*              pa;
	INI*                    pini = (INI*)ini;

	if (EINVALID_HANDLE == ini
			|| pini == NULL
			|| section == NULL) {
		return -1;
	}

	list_for_each_safe(pos, n1, &pini->section_list) {
		sec = list_entry(pos, INI_SECTION, entry);
		if (strcmp(sec->section, section) == 0) {
			list_for_each_safe(pos, n2, &sec->param_list) {
				pa = list_entry(pos, INI_PARAM, entry);
				if (param != NULL) {
					if (strcmp(pa->param, param) != 0 ) {
						continue;
					}
				}
				list_del(&pa->entry);
				free(pa->param);
				free(pa->value);
				free(pa);

			}
			list_del(&sec->entry);
			free(sec->section);
			free(sec);
			return 0;
		}
	}

	return -1;
}

handle_t ini_merge(handle_t  old_ini, handle_t new_ini)
{
	struct list_head*     pos;
	struct list_head*     n1;
	struct list_head*     n2;
	INI_SECTION*            sec;
	INI_PARAM*              pa;
	INI*                    src_old = (INI*)old_ini;
	INI*                    src_new = (INI*)new_ini;

	if (EINVALID_HANDLE == old_ini
			|| EINVALID_HANDLE == new_ini
			|| src_old == NULL
			|| src_new == NULL) {
		return EINVALID_HANDLE;
	}

	list_for_each_safe(pos, n1, &src_new->section_list) {
		sec = list_entry(pos, INI_SECTION, entry);
		list_for_each_safe(pos, n2, &sec->param_list) {
			pa = list_entry(pos, INI_PARAM, entry);
			if (ini_set(old_ini, sec->section, pa->param, pa->value) != 0) {
				ini_append(old_ini, sec->section, pa->param, pa->value);
			}
		}

	}

	return old_ini;
}

#if 0
int main(int argc, char **argv)
{
#if 0
	handle_t ini = ini_open("./a.ini");

	printf("bbb=%s\n", ini_get(ini, "aaa", "bbb"));
	ini_append(ini, "book", "a", "1");
	ini_append(ini, "book", "b", "2");
	ini_append(ini, "book", "c", "3");
	ini_append(ini, "book", "d", "4");
	ini_save(ini);

	printf("a=%s\n", ini_get(ini, "book", "a"));
	printf("b=%s\n", ini_get(ini, "book", "b"));
	printf("c=%s\n", ini_get(ini, "book", "c"));
	printf("d=%s\n", ini_get(ini, "book", "d"));
#endif

#if 1
	char buf[1024];
	FILE *fp = fopen("./a.ini", "r");
	int size= fread(buf, 1, 1024, fp);
	handle_t ini = ini_loadmemory(buf, size);
	printf("a=%s\n", ini_get(ini, "book", "a"));
	printf("b=%s\n", ini_get(ini, "book", "b"));
	printf("c=%s\n", ini_get(ini, "book", "c"));
	printf("d=%s\n", ini_get(ini, "book", "d"));
	fclose(fp);
#endif
	return 0;
}
#endif


#include <stdio.h>
#include "attribute.h"


#define __AT_APP __attribute__((unused, section("nstarcmdlist")))


int a[1000];

void __AT_APP test1(int *data)
{
    *data = 0;
}
    
void __AT_APP test2(int *data)
{
    *data = 0;
}


struct cmd_tbl {
    char *name;
    void (*func)(void);
};

void do_showvar(void) {
    printf("version v1.00\n");
}

void do_help(void) {
    printf("help info :\nhello, bye\n");
}

#define U_BOOT_CMD(_name, pfunc)\
	ll_entry_declare(struct cmd_tbl, _name, cmd) = { #_name, pfunc}

U_BOOT_CMD(
	showvar, do_showvar
);
    
U_BOOT_CMD(
	help, do_showvar
);

void print(void);

int main(void) 
{	
	//a[0] = 0;
	test1(a);
    test2(a);
    print();
    //printf("%s\n", showvar.name);
	return 0;
}

//struct cmd_tbl *find_cmd(const char *cmd)
//{
//    struct cmd_tbl *start = ll_entry_start(struct cmd_tbl, cmd);
//    const int len = ll_entry_count(struct cmd_tbl, cmd);
//    return find_cmd_tbl(cmd, start, len);
//}

/*
ll_entry_declare(_type, _name, _list)	向_list列表添加元素
ll_entry_get(_type, _name, _list)	    获取_list中名字为_name 元素的值


ll_entry_start(_type, _list)	获取_list起始地址
ll_entry_end(_type, _list)	    获取_list结束位置
ll_entry_count(_type, _list)	获取_list的元素个数

*/

/* find command table entry for a command */
struct cmd_tbl *find_cmd_tbl(const char *cmd, struct cmd_tbl *table,
			     int table_len)
{
#ifdef CONFIG_CMDLINE
	struct cmd_tbl *cmdtp;
	struct cmd_tbl *cmdtp_temp = table;	/* Init value */
	const char *p;
	int len;
	int n_found = 0;

	if (!cmd)
		return NULL;
	/*
	 * Some commands allow length modifiers (like "cp.b");
	 * compare command name only until first dot.
	 */
	len = ((p = strchr(cmd, '.')) == NULL) ? strlen (cmd) : (p - cmd);

	for (cmdtp = table; cmdtp != table + table_len; cmdtp++) {
		if (strncmp(cmd, cmdtp->name, len) == 0) {
			if (len == strlen(cmdtp->name))
				return cmdtp;	/* full match */

			cmdtp_temp = cmdtp;	/* abbreviated command ? */
			n_found++;
		}
	}
	if (n_found == 1) {			/* exactly one match */
		return cmdtp_temp;
	}
#endif /* CONFIG_CMDLINE */

	return NULL;	/* not found or ambiguous command */
}


struct cmd_tbl *find_cmd(const char *cmd)
{
	struct cmd_tbl *start = ll_entry_start(struct cmd_tbl, cmd);
	const int len = ll_entry_count(struct cmd_tbl, cmd);
	return find_cmd_tbl(cmd, start, len);
}

void print(void)
{
    struct cmd_tbl *cmdtp;
	struct cmd_tbl *start = ll_entry_start(struct cmd_tbl, 0);
    struct cmd_tbl *end = ll_entry_end(struct cmd_tbl, 0);
	const int len = ll_entry_count(struct cmd_tbl, 0);
    printf("len = %ld %ld\n", start, end);

    for (cmdtp = start; cmdtp != start + len; cmdtp++) {
        printf("cmd name =%s\n", cmdtp->name);
    }
	
}




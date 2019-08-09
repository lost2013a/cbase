#ifndef   _WRITE_LOG_H_
#define   _WRITE_LOG_H_

int dbg_write_open(void);
void write_printf(char* fmt,...);  
void write_dbg(unsigned char *p_dta, unsigned int dtalen , char* head);


#endif



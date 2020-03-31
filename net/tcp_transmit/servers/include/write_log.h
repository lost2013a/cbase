#ifndef   _WRITE_LOG_H_
#define   _WRITE_LOG_H_

int log_file_open(void);
void log_file_close(void);

void log_printf(char* fmt,...);
void log_write(unsigned char *p_dta, unsigned int dtalen , char* head);



#endif



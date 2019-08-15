#ifndef __NSTAR_HTML_PAGE_H
#define __NSTAR_HTML_PAGE_H



#define LOCAL_IP "192.168.251.175"







#define HTML_PAGE0_NAME "login"
void parm0_pos_htm(unsigned char mode);
void parm0_rpos_cgi(char *url);
void verify_time_out(void);


#define HTML_PAGE1_NAME "sta"
void parm1_pos_htm(unsigned char mode);
void parm1_rpos_cgi(char *url);


#define HTML_PAGE2_NAME "ipset"
void parm2_pos_htm(unsigned char mode);
void parm2_rpos_cgi(char *url);



#define HTML_PAGE3_NAME "devset"
void parm3_pos_htm(unsigned char mode);
void parm3_rpos_cgi(char *url);


#define HTML_PAGE4_NAME "info"
void parm4_pos_htm(unsigned char mode);
void parm4_rpos_cgi(char *url);




#endif





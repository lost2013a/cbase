#ifndef __NSTAR_HTML_PAGE_H
#define __NSTAR_HTML_PAGE_H


#define HTML_PAGE1_NAME "ipset"
void parm1_pos_htm(unsigned char mode);
void parm1_rpos_cgi(st_http_request *http_request, unsigned char mode , char* jump_to, unsigned char jumplen);



#define HTML_PAGE2_NAME "devset"
void parm2_pos_htm(unsigned char mode);


#endif





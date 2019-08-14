#ifndef  _HTML_LOGIN_H_
#define  _HTML_LOGIN_H_

#include "nstar_adt_httputil.h"

void cgi_log_in(st_http_request *http_request, unsigned char mode , char* jump_to, unsigned char jumplen);


#endif	

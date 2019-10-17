#ifndef __NSTAR_COOKIE_H
#define __NSTAR_COOKIE_H

void cookie_init(void);
int cookie_verify(unsigned char *html);
const char* cookie_getstr(void);



unsigned char login_pass_check(const char* dst);


#endif

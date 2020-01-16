#ifndef	_EBM_NET_H
#define _EBM_NET_H




void command_net_loop(void);
void command_net_start(unsigned int remote_ip, unsigned short remote_port);
void command_net_stop(void);
void command_net_restart(void);


#endif



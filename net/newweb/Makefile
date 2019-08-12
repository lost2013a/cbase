.PHONY:clean
CC = gcc
OBJ = test
CFLAGS=-Wall -O2
LIBS=-lpthread
OBJS = nstar_adt_test.c	nstar_adt_http_server.c nstar_adt_httputil.c write_log.c app_delay.c ntart_cookie.c

$(OBJ):$(OBJS)
	$(CC) -o $@ $(CFLAGS) $^ $(LIBS)

clean:
	rm  $(OBJ) 

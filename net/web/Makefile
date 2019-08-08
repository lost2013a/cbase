.PHONY:clean
#CC =powerpc-fsl-linux-gnuspe-gcc
#CFLAGS=-Wall  -O2
OBJ = web
#OBJ = test
CFLAGS=-Wall -O2
LIBS=-lpthread
OBJS = nstar_adt_test.c nstar_adt_httputil.c nstar_adt_http_server.c
$(OBJ):$(OBJS)
	$(CC) -o $@ $(CFLAGS) $^ $(LIBS)

clean:
	rm  $(OBJ) 

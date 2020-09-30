.PHONY:clean
#CC =powerpc-fsl-linux-gnuspe-gcc
#CFLAGS=-Wall  -O2
OBJ = test
#OBJ = test
#CFLAGS=-Wall -O2 -Wl,-Map,obj.map
CFLAGS= -Wall -O2
LIBS= -lpthread
OBJS += main.c
OBJS += share.c
OBJS += share_crc.c
OBJS += syscfg.c
$(OBJ):$(OBJS)
	$(CC) -o $@ $(CFLAGS) $^ $(LIBS)

clean:
	rm  $(OBJ) 

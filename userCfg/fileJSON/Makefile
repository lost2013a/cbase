.PHONY:clean
#CC =powerpc-fsl-linux-gnuspe-gcc
#CFLAGS=-Wall  -O2
OBJ = test
#OBJ = test
#CFLAGS=-Wall -O2
CFLAGS= -Wall  -Wl,-Map,obj.map
LIBS=
OBJS = test.c cJSON.c
$(OBJ):$(OBJS)
	$(CC) -o $@ $(CFLAGS) $^ $(LIBS)

clean:
	rm  $(OBJ) 

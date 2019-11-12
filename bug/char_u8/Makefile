.PHONY:clean
#CC =powerpc-fsl-linux-gnuspe-gcc
#CFLAGS=-Wall  -O2
OBJ = test
#OBJ = test
CFLAGS=-Wall -O2
#CFLAGS= -Wall
LIBS=-lpthread -lm
OBJS = top.c
$(OBJ):$(OBJS)
	$(CC) -o $@ $(CFLAGS) $^ $(LIBS)

clean:
	rm  $(OBJ) 

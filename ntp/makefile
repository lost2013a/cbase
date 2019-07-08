.PHONY:clean
#CC =csky-linux-gcc
CC =gcc
#CFLAGS=-Wall  -O2
OBJ = test
#OBJ = test
#CFLAGS=-Wall -O2
CFLAGS= -Wall -static 
LIBS=-lpthread
OBJS = sntp.c
all:$(OBJ) 
$(OBJ):$(OBJS)
	$(CC) -o $@ $(CFLAGS) $^ $(LIBS)

clean:
	rm  $(OBJ) 
	rm  $(OBJ2) 


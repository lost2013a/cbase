.PHONY:clean
CC =gcc
#CFLAGS=-Wall  -O2
OBJ = test
#OBJ = test
#CFLAGS=-Wall -O2
CFLAGS= -Wall
LIBS=-lpthread
OBJS = rec.c
$(OBJ):$(OBJS)
	$(CC) -o $@ $(CFLAGS) $^ $(LIBS)

clean:
	rm  $(OBJ) 

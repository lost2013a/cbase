.PHONY:clean
CC =gcc
#CFLAGS=-Wall  -O2
OBJ = test
#OBJ = test
#CFLAGS=-Wall -O2
#CFLAGS= -Wall
CFLAGS=
LIBS=-lpthread
OBJS = main.c nstar_queue.c
all:$(OBJ) $(OBJ2)
$(OBJ):$(OBJS)
	$(CC) -o $@ $(CFLAGS) $^ $(LIBS)


clean:
	rm  $(OBJ) 



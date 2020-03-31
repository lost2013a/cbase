.PHONY:clean
CC =gcc
#CFLAGS=-Wall  -O2
OBJ = test
OBJ2 = rec
#OBJ = test
#CFLAGS=-Wall -O2
CFLAGS= -Wall
LIBS=-lpthread
OBJS = multinetcard.c
OBJ2S = rec.c
all:$(OBJ) $(OBJ2)
$(OBJ):$(OBJS)
	$(CC) -o $@ $(CFLAGS) $^ $(LIBS)
$(OBJ2):$(OBJ2S)
	$(CC) -o $@ $(CFLAGS) $^ $(LIBS)	

clean:
	rm  $(OBJ) 
	rm  $(OBJ2) 


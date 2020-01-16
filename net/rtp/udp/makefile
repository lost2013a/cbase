.PHONY:clean
CC =gcc
#CFLAGS=-Wall  -O2
OBJ = server
OBJ2 = client
#OBJ = test
#CFLAGS=-Wall -O2
CFLAGS= -Wall
LIBS=-lpthread
OBJS = udp-server.c
OBJ2S = udp-client.c
all:$(OBJ) $(OBJ2)
$(OBJ):$(OBJS)
	$(CC) -o $@ $(CFLAGS) $^ $(LIBS)
$(OBJ2):$(OBJ2S)
	$(CC) -o $@ $(CFLAGS) $^ $(LIBS)	

clean:
	rm  $(OBJ) 
	rm  $(OBJ2) 


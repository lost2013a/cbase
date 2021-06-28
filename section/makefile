.PHONY:clean
#CC =csky-linux-gcc
CC =gcc
#CFLAGS=-Wall  -O2
OBJ = test
#OBJ = test
#CFLAGS=-Wall -O2
CFLAGS= -Wall -O2
LIBS=
OBJS = section.c
all:$(OBJ) 
$(OBJ):$(OBJS)
	$(CC) -o $@ $(CFLAGS) $^ $(LIBS)

clean:
	rm  $(OBJ) 
	rm  $(OBJ2) 


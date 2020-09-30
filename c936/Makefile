.PHONY:clean
#CC = arm-linux-gnueabihf-gcc
OBJ = test
CFLAGS=-Wall -O2
LIBS=
SRC =  main.c 
$(OBJ):$(SRC)
	$(CC) -o $@ $(CFLAGS) $^ $(LIBS)
clean:
	rm  $(OBJ) 

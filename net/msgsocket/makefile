.PHONY:clean

OBJ = test
CFLAGS= -Wall
LIBS=-lpthread
OBJS =  client.c
$(OBJ):$(OBJS)
	$(CC) -o $@ $(CFLAGS) $^ $(LIBS)

clean:
	rm  $(OBJ) 

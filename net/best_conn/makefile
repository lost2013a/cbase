.PHONY:clean
OBJ = test
CFLAGS= -Wall
LIBS=-lpthread
OBJS =  bconn.c app_delay.c
$(OBJ):$(OBJS)
	$(CC) -o $@ $(CFLAGS) $^ $(LIBS)

clean:
	rm  $(OBJ) 

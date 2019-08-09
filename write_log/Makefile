.PHONY:clean
OBJ = test
CFLAGS= -Wall
LIBS=-lpthread
OBJS = write_log.c
$(OBJ):$(OBJS)
	$(CC) -o $@ $(CFLAGS) $^ $(LIBS)

clean:
	rm  $(OBJ) 

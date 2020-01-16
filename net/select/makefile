.PHONY:clean
OBJ = test
CFLAGS= -Wall
LIBS=-lpthread
OBJS =  select.c
$(OBJ):$(OBJS)
	$(CC) -o $@ $(CFLAGS) $^ $(LIBS)

clean:
	rm  $(OBJ) 

.PHONY:clean

OBJ = test
CFLAGS= -Wall
CFLAGS+= -I ./include
LIBS=-lpthread
SRC =  main.c ebm_net.c ebm_env.c ebm_msg.c emb_rtp.c
$(OBJ):$(SRC)
	$(CC) -o $@ $(CFLAGS) $^ $(LIBS)

clean:
	rm  $(OBJ) 

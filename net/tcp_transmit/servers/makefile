.PHONY:clean

#CC=csky-linux-gcc
OBJ = test
CFLAGS= -Wall -static
CFLAGS+= -I ./include
LIBS=-lpthread
SRC =  main.c ebm_net.c ebm_env.c ebm_msg.c emb_rtp.c ebm_passback.c app_delay.c
$(OBJ):$(SRC)
	$(CC) -o $@ $(CFLAGS) $^ $(LIBS)

clean:
	rm  $(OBJ) 

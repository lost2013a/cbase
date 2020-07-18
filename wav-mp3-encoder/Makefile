.PHONY:clean
CC =gcc
#CFLAGS=-Wall  -O2
OBJ = test
#OBJ = test
#CFLAGS=-Wall -O2
CFLAGS= -fPIC -static
LIBS=-L lame/ -lmp3lame -lpthread -lm
//LDLIBS    = -lpthread ./libiconv-1.8/lib/libiconv.a -L./xml/lib -lxml2
OBJS = Encode_WAV_To_MP3.c
$(OBJ):$(OBJS)
	$(CC) -o $@ $(CFLAGS) $^ $(LIBS)

clean:
	rm  $(OBJ) 

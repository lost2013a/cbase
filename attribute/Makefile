.PHONY:clean
#CC =powerpc-fsl-linux-gnuspe-gcc
#CFLAGS=-Wall  -O2
STRIP = strip
OBJ = test
CFLAGS= -Wall -O2
LIBS=
OBJS = attribute.c a.lds
$(OBJ):$(OBJS)
	$(CC) -o $@ $(CFLAGS) $^ $(LIBS); $(STRIP) $@
#	$(STRIP) $@

clean:
	rm  $(OBJ) 

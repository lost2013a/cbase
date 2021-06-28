#!/bin/bash
CROSS_PATH = arm-linux-gnueabihf-
#export CROSS_PATH=
CC = $(CROSS_PATH)gcc
AR = $(CROSS_PATH)ar

OUTPUT_PATH = /home/qlqcetc/nfs
CFLAGS = -Wall -O2
OUTPUT = out.elf
#LIBS = -lplat
LDFLAGS = -L lib -lplat

##################################################################################

#add src
SRCS += $(wildcard src/*.c)

#add inc
CFLAGS += -I inc/

##################################################################################

OBJS=$(addprefix obj/, $(addsuffix .o, $(basename $(notdir $(SRCS)))))


all : deps $(OUTPUT) run

.PHONY+= deps
deps: 
	@for f in $(SRCS); do \
		OBJ=obj/`basename $$f|sed -e 's/\.cpp/\.o/' -e 's/\.cxx/\.o/' -e 's/\.cc/\.o/' -e 's/\.c/\.o/'`; \
		echo $$OBJ: $$f>> deps; \
		echo '	@echo -e "compiling \033[032m[$(CC)]\033[0m": ' $$f >> deps; \
		echo '	@$(CC) $$(CFLAGS) -c -o $$@ $$^'>> deps; \
	done
	@echo "deps creat"
-include ./deps

.PHONY += $(OUTPUT)
$(OUTPUT): $(OBJS)
	$(CC) $(CFLAGS) $^ $(LDFLAGS) -o $@ 

.PHONY += run
run :
	cp $(OUTPUT) $(OUTPUT_PATH)/.
	
.PHONY += clean	
clean: 
	@rm -rf obj/*.o deps $(OUTPUT)
	


#!/bin/bash
CROSS_PATH = arm-linux-gnueabihf-
#export CROSS_PATH=
CC = $(CROSS_PATH)gcc
AR = $(CROSS_PATH)ar

OBJ_PATH = obj
SRC_PATH = src
INC_PATH = inc
LIBNAME = libplat.a
OUTPUT_PATH = ../v3sapp/lib

#add src
SRCS += $(shell ls $(SRC_PATH)/*.c)

#add inc
CFLAGS += -I$(INC_PATH)/

##################################################################################


CFLAGS += -Wall -O2

APP = test.elf
#LDFLAGS += -static
OBJS=$(addprefix obj/, $(addsuffix .o, $(basename $(notdir $(SRCS)))))

all : deps $(LIBNAME) copyfile

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

.PHONY += $(LIBNAME)
$(LIBNAME): $(OBJS)
	@$(AR) r $@ $^

.PHONY += copyfile
copyfile :
	@mkdir -pv $(OUTPUT_PATH)
	cp $(LIBNAME) $(OUTPUT_PATH)/.
	
	
.PHONY += app
app : $(OBJS)
	@$(CC) -o $(APP) $^

.PHONY += clean	
clean: 
	@rm -rf obj/*.o deps $(OUTPUT)
	


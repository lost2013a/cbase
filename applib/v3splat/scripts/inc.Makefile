-include $(GXSRC_PATH)/scripts/$(ARCH)-$(OS)-compiler.mak
CFLAGS += -pipe -O2 -g
MAKE    += -s
MAKEFLAGS += -s
ifeq ($(OS), ecos)
WARNING +=-Werror
#WARNING += -Wundef -Wall
CFLAGS  +=  $(TARGET_DEFS) -DARCH=$(ARCH) $(WARNING) 
endif

ifeq ($(OS), linux)
WARNING += -Werror
WARNING += -Wundef -Wall
CFLAGS  += -g -O2   $(TARGET_DEFS) -DARCH=$(ARCH) $(WARNING) 
LIBS +=  -lz -lrt -Wl --whole-archive -lpthread -Wl --no-whole-archive
#LDFLAGS += -static
endif

ifeq ($(NSTAR_CUSTOM), )
else
CFLAGS  += -D$(NSTAR_CUSTOM)
endif

#LDFLAGS += -Wl,--warn-common
#LDFLAGS += -L$(GXLIB_PATH)/lib 
OBJS=$(addprefix objects/, $(addsuffix .o, $(basename $(notdir $(SRC)))))
all:   env  $(BEFORE)   deps objects $(OBJS) $(LIB) 	
env:
	@echo "env start ... "
	@echo "OBJS=${OBJS}"
#ifndef GXLIB_PATH
#	$(error Error: you must set the GXLIB_PATH environment variable to point to your gxsoft Path.)
#endif
	@-rm -f *.a
	@echo "env start end... "
deps: 
	@echo "Generating new dependency file...";
	@echo "CFLAGS=${CFLAGS}"
	@-rm -f deps;
	@for f in $(SRC); do \
		OBJ=objects/`basename $$f|sed -e 's/\.cpp/\.o/' -e 's/\.cxx/\.o/' -e 's/\.cc/\.o/' -e 's/\.c/\.o/'`; \
		echo $$OBJ: $$f>> deps; \
		echo '	@echo -e "compiling \033[032m[$(CC)]\033[0m": ' $$f >> deps; \
		echo '	@$(CC) $$(CFLAGS) -c -o $$@ $$^'>> deps; \
	done
	@echo "deps OBJ=${OBJ}.."
-include ./deps
objects:
	@echo "objects start ...."
	@mkdir objects
	@echo "$(LIB).."
	@echo "objects end ...."	
.PHONY: $(LIB)
$(LIB): $(OBJS)
	@$(AR) r $(GXSRC_PATH)/$(LIB) $(GXSRC_PATH)/objects/*.o;
clean: 
	@rm -rf $(OBJS) *.o .*swp objects deps $(CLEANFILE) $(BIN) *.log $(LIB) cscope.* tags *.img *.gz
	@find -name "*~" -exec rm {} \;


format:
	@echo "Makeing format...";
	@find -name "*.[chCH]" -print |xargs dos2unix -U

str = this!is!a!test!str!
para = a.c b.c
empty:= 
space:=$(empty) #end
new_str= $(subst !,$(space),$(str))

new_para= $(patsubst %.c, %.o, $(para))
all:
	@echo $(str)
	@echo $(new_str)
	@echo $(para)
	@echo $(new_para)
	
#分别测试定向到标准输入输出错误
	@echo >&0 "The present kernel configuration has modules disabled."
	@echo >&1 "The present kernel configuration has modules disabled."
	@echo >&2 "The present kernel configuration has modules disabled."
.PHONY: all

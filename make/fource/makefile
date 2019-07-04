file = test.txt
 
all: generate-a-file
 
generate-a-file: $(file) 
 
$(file):FORCE
	@echo "Force to generate a test file for every make ..."
	rm -rf $(file) && echo `date "+%Y-%m-%d %H:%M:%S"` > $(file)
 
FORCE:
.PHONY: FORCE
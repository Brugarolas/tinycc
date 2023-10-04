file_curr := main
dir_examples := examples
dir_out := $(dir_examples)/out/
dir_bin := $(dir_examples)/out/bin
dir_interp := $(dir_examples)/out/interp
dir_dump := $(dir_examples)/out/dump
bin_extension = .bin

CXX = g++
TCC = ./tcc
DUMP = ./poxim-dump
INTERP = ./poxim-interp
BIN2HEX = ./bin2hex

# Not necessary to provide these options anymore since they're default in poxim arch
# Although it is necessary to run with tcc-i386
# TCC_RUN = $(TCC) -nostdlib -static $(dir_examples)/$(file_curr).c -o./$(dir_bin)/$(file_curr)$(bin_extension) -I./ -I./include -L./ -Wl,--oformat=binary 
TCC_RUN = $(TCC)  $(dir_examples)/$(file_curr).c -o./$(dir_bin)/$(file_curr)$(bin_extension) 

dirs:
	@mkdir -p ./$(dir_bin)
	@mkdir -p ./$(dir_interp)
	@mkdir -p ./$(dir_examples)
	@mkdir -p ./$(dir_dump)

debug: run
	gdb --args 	$(TCC_RUN)

debug-i386: TCC = ./tcc-i386
debug-i386: bin_extension = .i386.bin
debug-i386: run-i386
	gdb --args $(TCC_RUN)

interp: poxim-interp.cpp
	$(CXX) -Wall -std=c++14 -ggdb ./poxim-interp.cpp -o poxim-interp

dump: poxim-dump.c
	$(CC) -Wall -std=c99 ./poxim-dump.c -ggdb -o poxim-dump -lm

bin2hex: bin2hex.c
	$(CC) -std=c99 bin2hex.c -o bin2hex

utils: bin2hex interp dump

# XXX: The necessary flags to compile directly to binary as we need: -nostdlib -static -Wl,--oformat=binary 
run: TCC = ./tcc 
run: bin_extension = .bin
run: dump all utils interp dirs examples
	$(TCC_RUN)
	$(DUMP) --bin $(dir_bin)/$(file_curr)$(bin_extension) > $(dir_dump)/$(file_curr).poxim.dump
	$(INTERP) --bin $(dir_bin)/$(file_curr)$(bin_extension)  $(dir_interp)/$(file_curr).interp

display: run
	cat $(dir_interp)/$(file_curr).term

#XXX: Dependes on the existence of tcc-i386
run-i386: TCC = ./tcc-i386
run-i386: TCC_RUN = $(TCC) -nostdlib -static $(dir_examples)/$(file_curr).c -o./$(dir_bin)/$(file_curr)$(bin_extension) -I./ -I./include -L./ -Wl,--oformat=binary
run-i386: bin_extension = .i386.bin
run-i386: dump utils interp dirs
	@echo $(TCC_RUN)
	$(TCC_RUN)
	objdump -b binary --adjust-vma=0x0 -Mintel,i386 -m i386 -D $(dir_bin)/$(file_curr)$(bin_extension) > $(dir_dump)/$(file_curr).i386.dump
	# objcopy -O binary --only-section=.text  <rest>

EXAMPLES := $(patsubst ./examples/%.c, %, $(wildcard ./examples/*.c))
examples: ./$(TCC) dirs interp dump # If the binary havent changed, so we don't need to run examples
	for var in $(EXAMPLES) ; do \
		echo "[INFO] : running example" $$var; \
		echo  $(TCC) -nostdlib -static $(dir_examples)/$$var.c -o $(dir_bin)/$$var.bin -I./ -I./include -L./ -Wl,--oformat=binary && \
		$(TCC) -nostdlib -static $(dir_examples)/$$var.c -o $(dir_bin)/$$var.bin -I./ -I./include -L./ -Wl,--oformat=binary && \
		$(DUMP) --bin $(dir_bin)/$$var.bin > $(dir_dump)/$$var.poxim.dump && \
		$(INTERP) --bin $(dir_bin)/$$var.bin  $(dir_interp)/$$var.interp ; \
		$(BIN2HEX) $(dir_bin)/$$var.bin  $(dir_bin)/$$var.hex; \
	done


clean:

.PHONY: run run-i386 display

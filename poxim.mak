file_curr := curr
dir_examples := examples
dir_bin := $(dir_examples)/out/bin
dir_interp := $(dir_examples)/out/interp
dir_dump := $(dir_examples)/out/dump
bin_extension := .bin
TCC = tcc
DUMP = poxim-dump
INTERP = poxim-interp
TCC_RUN = $(TCC) -nostdlib -static $(dir_examples)/$(file_curr).c -o./$(dir_bin)/$(file_curr)$(bin_extension) -I./ -I./include -L./ -Wl,--oformat=binary 

dirs:
	@mkdir -p ./$(dir_bin)
	@mkdir -p ./$(dir_interp)
	@mkdir -p ./$(dir_examples)
	@mkdir -p ./$(dir_dump)

debug: run
	gdb --args 	$(TCC_RUN)

debug-i386: run-i386
	gdb --args $(TCC_RUN)

interp: poxim-interp.cpp
	g++ -Wall -ggdb ./poxim-interp.cpp -o poxim-interp

dump: poxim-dump.c
	gcc ./poxim-dump.c -ggdb -o poxim-dump -lm

utils: bin2strhex.c
	gcc bin2strhex.c -o bin2strhex

# XXX: The necessary flags to compile directly to binary as we need: -nostdlib -static -Wl,--oformat=binary 
run: TCC = tcc 
run: bin_extension = .bin
run: dump all run-i386 utils interp dirs
	./$(TCC_RUN)
	./$(DUMP) --bin $(dir_bin)/$(file_curr)$(bin_extension) > $(dir_dump)/$(file_curr).poxim.dump
	./$(INTERP) --bin $(dir_bin)/$(file_curr)$(bin_extension)  $(dir_interp)/$(file_curr).interp

#XXX: Dependes on the existence of tcc-i386
run-i386: TCC = tcc-i386
run-i386: bin_extension = .i386.bin
run-i386: dump all utils interp dirs
	./$(TCC_RUN)
	objdump -b binary --adjust-vma=0x0 -Mintel,i386 -m i386 -D $(dir_bin)/$(file_curr)$(bin_extension) > $(dir_dump)/$(file_curr).i386.dump
	# objcopy -O binary --only-section=.text  <rest>

EXAMPLES := $(patsubst ./examples/%.c, %, $(wildcard ./examples/*.c))
examples: run
	for var in $(EXAMPLES) ; do \
		./tcc -nostdlib -static $(dir_examples)/$$var.c -o $(dir_bin)/$$var.bin -I./ -I./include -L./ -Wl,--oformat=binary && \
		./poxim-dump --bin $(dir_bin)/$$var.bin > $(dir_dump)/$$var.poxim.dump && \
		./poxim-interp --bin $(dir_bin)/$$var.bin  $(dir_interp)/$$var.interp ; \
	done


.PHONY: run run-i386

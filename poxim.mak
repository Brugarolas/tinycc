# test_file := 1_erro
# test_file := 1_exemplo
# test_file := 1_fatorial
test_file := 2_fpu
test_cfile := ex1
test_cfile := ex1
out_dir := out
poxim_dir := "tests/poxim"
TCC = tcc

debug: run
	gdb --args tcc -c ./examples/$(test_cfile).c -o./$(out_dir)/$(test_cfile).bin -I. -Iinclude -L.

debug-i386: 
	gdb --args tcc-i386 -c ./examples/$(test_cfile).c -o./$(out_dir)/$(test_cfile).bin -I. -Iinclude -L.

poxim-interp: poxim-interp.cpp
	g++ -Wall -O3 ./poxim-interp.cpp -o poxim-interp

poxim-dump: ./poxim-dump.c
	gcc ./poxim-dump.c -ggdb -o poxim-dump

utils: poxim-dump bin2strhex.c
	gcc bin2strhex.c -o bin2strhex

dump: poxim-dump
	./poxim-dump ./$(poxim_dir)/$(test_file).hex ./$(poxim_dir)/$(test_file).dump
	objdump -b binary -Mintel,x86-64 --adjust-vma=0x0 -D ./$(out_dir)/$(test_cfile).text.dump -m i386 > ./$(out_dir)/$(test_cfile).x86.dump
	xxd -r -p ./$(poxim_dir)/1_fatorial.hex ./$(poxim_dir)/$(test_file).bin
	./poxim-dump --bin ./$(poxim_dir)/$(test_file).bin

interp: poxim-interp
	./poxim-interp ./$(poxim_dir)/$(test_file).hex ./$(poxim_dir)/$(test_file).interp.out


# ./tcc -nostdlib -static examples/ex1.c -o./out/ex1.bin -I./ -I./include -L./ -Wl,--oformat=binary
# DONE: necessary flags to compile directly to binary as we need: -nostdlib -static -Wl,--oformat=binary 
run: poxim-dump all examples/ex1.c run-i386 utils
	./tcc -nostdlib -static examples/$(test_cfile).c -o./$(out_dir)/$(test_cfile).bin -I./ -I./include -L./ -Wl,--oformat=binary
	./poxim-dump --bin $(out_dir)/$(test_cfile).bin > $(out_dir)/$(test_cfile).poximdump
	# objcopy -O binary --only-section=.text ./$(out_dir)/$(test_cfile).bin > ./$(out_dir)/$(test_cfile).objcopy
	# objdump -M intel -m i386 -d ./$(out_dir)/$(test_cfile).objcopy > ./$(out_dir)/$(test_cfile).objdump

run-i386: examples/ex1.c
	# ./tcc-i386 -static -nostdlib examples/$(test_cfile).c -o./$(out_dir)/$(test_cfile).i386.bin -I./ -I./include -L./
	./tcc-i386 -static -nostdlib examples/$(test_cfile).c -o./$(out_dir)/$(test_cfile).i386.bin -I./ -I./include -L./ -Wl,--oformat=binary
	objdump -b binary --adjust-vma=0x0 -Mintel,i386 -m i386 -D ./$(out_dir)/$(test_cfile).i386.bin > ./$(out_dir)/$(test_cfile).i386.objdump
	# objdump	-b binary -Mintel,i386  -D ./x64code.bin -m i386 > x64code.s
	# objcopy -O binary --only-section=.text ./$(out_dir)/$(test_cfile).i386.bin ./$(out_dir)/$(test_cfile).text.i386.dump


.PHONY: dump interp run utils

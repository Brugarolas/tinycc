# test_file := 1_erro
# test_file := 1_exemplo
# test_file := 1_fatorial
test_file := 2_fpu
test_cfile := ex1
out_dir := out
poxim_dir := "tests/poxim"

debug: 
	gdb --args tcc -c ./examples/$(test_cfile).c -o./$(out_dir)/$(test_cfile).bin -I. -Iinclude -L.

poxim-interp: poxim-interp.cpp
	g++ -Wall -O3 ./poxim-interp.cpp -o poxim-interp

poxim-dump: ./poxim-dump.c
	gcc ./poxim-dump.c -ggdb -o poxim-dump

dump: poxim-dump
	./poxim-dump ./$(poxim_dir)/$(test_file).hex ./$(poxim_dir)/$(test_file).dump

interp: poxim-interp
	./poxim-interp ./$(poxim_dir)/$(test_file).hex ./$(poxim_dir)/$(test_file).interp.out

run: poxim-dump all dump
	./tcc -c -nostdlib examples/$(test_cfile).c -o./$(out_dir)/$(test_cfile).bin -I./ -I./include -L./ -Wl,--oformat=binary
	objcopy -O binary --only-section=.text ./$(out_dir)/$(test_cfile).bin ./$(out_dir)/$(test_cfile).text.dump
	objdump -M intel -d ./$(out_dir)/$(test_cfile).bin > ./$(out_dir)/$(test_cfile).dump
	./poxim-dump --bin ./$(out_dir)/$(test_cfile).text.dump > ./$(out_dir)/$(test_cfile).poxim.dump
	objdump -b binary -Mintel,x86-64 --adjust-vma=0x0 -D ./$(out_dir)/$(test_cfile).text.dump -m i386 > ./$(out_dir)/x86.dump
	xxd -r -p ./$(poxim_dir)/1_fatorial.hex ./$(poxim_dir)/$(test_file).bin
	./poxim-dump --bin ./$(poxim_dir)/$(test_file).bin

.PHONY: dump interp run

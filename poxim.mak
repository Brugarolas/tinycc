# test_file := 1_erro
# test_file := 1_exemplo
test_file := 1_fatorial
test_cfile := ex1

debug: 
	gdb --args tcc -c ./examples/$(test_cfile).c -o./tests/out/$(test_cfile).bin -I. -Iinclude -L.

poxim-interp: poxim-interp.cpp
		g++ -Wall -O3 ./poxim-interp.cpp -o poxim-interp

poxim-dump: ./poxim-dump.c
	gcc ./poxim-dump.c -ggdb -o poxim-dump

dump: poxim-dump
	./poxim-dump ./tests/poxim/res/$(test_file).hex ./tests/poxim/res/$(test_file).dump

interp: poxim-interp
	./poxim-interp ./tests/poxim/res/$(test_file).hex ./tests/poxim/res/$(test_file).interp.out

run: poxim-dump poxim-gen.c
	./tcc -c -nostdlib examples/$(test_cfile).c -o./out/$(test_cfile).bin -I./ -I./include -L./ -Wl,--oformat=binary
	objdump -M intel -d ./out/$(test_cfile).bin > ./out/$(test_cfile).dump
	objcopy -O binary --only-section=.text ./out/$(test_cfile).bin ./out/$(test_cfile).text.dump
	./poxim-dump --bin ./out/$(test_cfile).text.dump > ./out/$(test_cfile).poxim.dump
	objdump -b binary -Mintel,x86-64 --adjust-vma=0x0 -D ./out/$(test_cfile).text.dump -m i386:x86-64 > ./out/x64code.s
	# $ xxd -r -p tests/poxim/res/1_fatorial.hex lol.bin

.PHONY: dump interp run

# test_file := 1_erro
# test_file := 1_exemplo
test_file := 1_fatorial

debug: 
	gdb --args tcc -c ./examples/ex1.c -o./tests/out/ex1.bin -I. -Iinclude -L.

poxim-interp: poxim-interp.cpp
		g++ -Wall -O3 ./poxim-interp.cpp -o poxim-interp

dump: poxim-dump.c
	gcc ./poxim-dump.c -o poxim-dump && ./poxim-dump ./tests/poxim/res/$(test_file).hex ./tests/poxim/res/$(test_file).dump

interp: poxim-interp
	./poxim-interp ./tests/poxim/res/$(test_file).hex ./tests/poxim/res/$(test_file).interp.out

run:
	./tcc -c -nostdlib examples/ex1.c -o./tests/out/ex1.bin -I./ -I./include -L./ -Wl,--oformat=binary
	objdump -M intel -d ./tests/out/ex1.bin > ./tests/out/ex1.dump

.PHONY: dump interp run

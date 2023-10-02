# Tiny C Compiler

This project tries to extend tiny c compiler to support `Poxim Architecture`
(docs will soon be added)

## Recommended Setup for Development

# Do this first 

$ ./configure --extra-cflags=-ggdb --debug --cpu=poxim

$ make run

Now inspect ``examples/out/`` folder .

## Compiling

$ ./tcc input.c -o output.bin

## Disasembly 

$ ./poxim-dump --bin input.bin output.dump

## Execution

Execute the bytes, the machine code.

$ ./poxim-interp --bin input.bin output.interp

This will create a files called output.interp and another called output.term
.term means has the output of poxim terminal. The other one has everything else.


## More Info

Search for tcc-doc\* after running:

$ make doc

## Resources

- Avr port of TCC https://github.com/thethumbler/avr-tcc
- Another option https://github.com/chaoslawful/tcc
- Kernel with tcc https://wiki.osdev.org/TCC
- SDCC

## Tips

If you're cross compiling for i386 ``pacman -S lib32-glibc`` is necessary

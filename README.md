# Tiny C Compiler

This project tries to extend tiny c compiler to support `Poxim Architecture`
(docs will soon be added)

## Recommended Setup for Development

If you're cross compiling for i386 ``pacman -S lib32-glibc`` is necessary


$ ./configure --extra-cflags="-ggdb" --debug --cpu="poxim"

$ compiledb make # Works better with clangd ls

$ gdb ./tcc

$ make run

Now inspect ``examples/out/`` folder .

## Debuging
Disasembly binary .text section with 

$ ./poxim-dump --bin <inputfile>

## More Info

Search for tcc-doc\* after running:

$ make doc

## Resources

- Avr port of TCC https://github.com/thethumbler/avr-tcc
- Another option https://github.com/chaoslawful/tcc
- Kernel with tcc https://wiki.osdev.org/TCC
- SDCC

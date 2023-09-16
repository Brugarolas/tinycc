# Tiny C Compiler

This project tries to extend tiny c compiler to support `Poxim Architecture`
(docs will soon be added)

## Recommended Setup for Development

$ ./configure --cc=clang --ar=ar --extra-cflags="-ggdb" --debug # --cpu="poxim"

$ compiledb make # Works better with clangd ls

$ ggdb ./tcc

## More Info

Search for tcc-doc\* after running:

$ make doc

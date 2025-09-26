#!/bin/bash
if command -v clang &> /dev/null ; then
    clang -std=c11 -x c main.c parse.c -o silver8
elif command -v gcc &> /dev/null ; then
    gcc -o silver8 main.c parse.c -static
else
    echo 'Neither clang nor gcc could be found, please install LLVM or gcc'
fi

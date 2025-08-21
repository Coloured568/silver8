if (Get-Command "clang" -ErrorAction SilentlyContinue) {
    clang --target=x86_64-pc-windows-gnu -std=c11 -x c main.c parse.c -o silver8
} else if (Get-Command "gcc") {
    gcc -o silver8 main.c parse.c -lkernel32 -static -static-libgcc -static-libstdc++ -lmsvcrt -O2 -march=native
} else {
    Write-Host "Could not find clang or gcc, please install LLVM or GCC"
}

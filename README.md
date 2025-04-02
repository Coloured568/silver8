# silver8
My own CPU

# Linux Instructions
1. Build it from source
2. Download program.s8 (to ensure the emulator works)
3. Run ./silver8 (while program.s8 is in the same directory)
4. Boom!!!!!!!!!!!!

# Windows Instructions
1. Either build it from the source (preferred) or [download it](https://github.com/Coloured568/silver8/releases) from the latest release.
   2a. To build it from source, first install [MSYS2](https://msys2.org) from the website or in powershell using the command `winget install --id MSYS2.MSYS2 --scope user` and follow the instructions of the installer
   2b. Then, once installed, open `MSYS2 UCRT64` and type the following commands to install the compilation tools (the first command will close the window, just reopen it and continue):
       ```sh
       pacman -Syu
       pacman -S ucrt64/mingw-w64-ucrt-x86_64-gcc
       ```
   2c. Then, type
3. Download program.s8 (to just make sure the emulator works)
4. MAKE SURE "silver8.exe" AND "program.s8" are in the same directory!
5. Open up a terminal in the same directory
6. Run `.\silver8.exe`
7. It should output whatever program you ran!

P.S the program file *has* to be named "program.s8" it will not work otherwise. i havent really implemented a "file-loader" or anything like that yet.
# yes i am totally interested and i totally want to write programs for this emulator!!!!!!!!!!!!!!!!!!!!!!1
documentation will come soon (eventually or even never)

Hello and welcome to our little Game Boy Emulator project!

We've developed our emulator accompanying our Operating System lecture.

The emulator was developed in C Programming Language for an Linux environment.

Do run the emulator you first have to install a few things:
- gcc compiler: sudo apt install build-essential -> compile files
- cmake: sudo apt install cmake -> build executable
- SDL2: sudo apt-get install libsdl2-2.0 -> game window
        sudo apt install libsdl2-ttf-dev

To run the emulator:
1) go to folder cmake
2) open terminal
3) write "make all" -> this creates executable GB-Emulator
4) to start emulator with ROM write "./GB-Emulator ../../roms/Tetris.gb"

Warning: This emulator isn't fully implemented yet. You won't be able to play the attached ROM file.  
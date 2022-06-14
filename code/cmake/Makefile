# to run, enter: make all

INCL = ../include/
EMU = ../lib/emulator/
REND = ../lib/rendering/
PROC = ../lib/processor/
MAIN = ../main/
LIB = ../lib/

#/usr/lib/x86_64-linux-gnu/cmake/SDL2
SDL_CFLAGS := $(shell sdl2-config --cflags)
SDL_LDFLAGS := $(shell sdl2-config --libs) -lSDL2_ttf

all: gb_emulator

gb_emulator: main.o emulator.o instruction_set.o interrupts.o lcd.o cpu.o joypad.o io.o dma.o game_window.o pixel_fetcher.o  ppu_modes.o ppu.o bus.o ram.o rom.o stack.o timer.o
	gcc -o GB-Emulator $(MAIN)main.o $(EMU)emulator.o $(PROC)instruction_set.o $(PROC)interrupts.o $(REND)lcd.o $(PROC)cpu.o $(LIB)joypad.o $(LIB)io.o $(REND)dma.o $(REND)game_window.o $(SDL_CFLAGS) $(SDL_LDFLAGS) $(REND)pixel_fetcher.o $(REND)ppu_modes.o $(REND)ppu.o $(LIB)bus.o $(LIB)ram.o $(LIB)rom.o $(LIB)stack.o $(LIB)timer.o 

main.o: $(MAIN)main.c $(INCL)emulator.h $(INCL)cpu.h
	gcc -o $(MAIN)main.o -c $(MAIN)main.c -I$(INCL)

emulator.o: $(INCL)cpu.h $(INCL)rom.h $(INCL)emulator.h $(INCL)timer.h $(INCL)dma.h $(INCL)game_window.h $(INCL)ppu.h
	gcc -o $(EMU)emulator.o -c $(EMU)emulator.c -I$(INCL)

instruction_set.o: $(INCL)instruction_set.h $(INCL)cpu.h $(INCL)bus.h $(INCL)define.h
	gcc -o $(PROC)instruction_set.o -c $(PROC)instruction_set.c -I$(INCL)	

interrupts.o: $(INCL)interrupts.h $(INCL)cpu.h $(INCL)stack.h
	gcc -o $(PROC)interrupts.o -c $(PROC)interrupts.c -I$(INCL)

lcd.o: $(INCL)lcd.h $(INCL)define.h $(INCL)dma.h
	gcc -o $(REND)lcd.o -c $(REND)lcd.c -I$(INCL)

cpu.o: $(INCL)cpu.h $(INCL)emulator.h $(INCL)interrupts.h $(INCL)instruction_set.h $(INCL)bus.h $(INCL)stack.h
	gcc -o $(PROC)cpu.o -c $(PROC)cpu.c -I$(INCL)

joypad.o: $(INCL)joypad.h $(INCL)define.h 
	gcc -o $(LIB)joypad.o -c $(LIB)joypad.c -I$(INCL)

io.o: $(INCL)define.h 
	gcc -o $(LIB)io.o -c $(LIB)io.c -I$(INCL)

dma.o: $(INCL)define.h 
	gcc -o $(REND)dma.o -c $(REND)dma.c -I$(INCL)	

game_window.o: $(INCL)game_window.h $(INCL)joypad.h $(INCL)emulator.h $(INCL)bus.h $(INCL)ppu.h  $(INCL)define.h 
	gcc -o $(REND)game_window.o $(SDL_CFLAGS) $(SDL_LDFLAGS) -c $(REND)game_window.c -I$(INCL) 
	
pixel_fetcher.o: $(INCL)pixel_fetcher.h $(INCL)define.h $(INCL)ppu.h
	gcc -o $(REND)pixel_fetcher.o -c $(REND)pixel_fetcher.c -I$(INCL)

ppu_modes.o: $(INCL)ppu_modes.h $(INCL)pixel_fetcher.h $(INCL)ppu.h $(INCL)lcd.h $(INCL)cpu.h $(INCL)interrupts.h $(INCL)define.h
	gcc -o $(REND)ppu_modes.o -c $(REND)ppu_modes.c -I$(INCL)
	
ppu.o: $(INCL)ppu.h $(INCL)ppu_modes.h $(INCL)pixel_fetcher.h $(INCL)lcd.h $(INCL)define.h
	gcc -o $(REND)ppu.o -c $(REND)ppu.c -I$(INCL)

bus.o: $(INCL)bus.h $(INCL)define.h $(INCL)ram.h $(INCL)cpu.h $(INCL)ppu.h $(INCL)dma.h
	gcc -o $(LIB)bus.o -c $(LIB)bus.c -I$(INCL)

ram.o: $(INCL)ram.h $(INCL)define.h
	gcc -o $(LIB)ram.o -c $(LIB)ram.c -I$(INCL)

rom.o: $(INCL)rom.h $(INCL)define.h
	gcc -o $(LIB)rom.o -c $(LIB)rom.c -I$(INCL)
	
stack.o: $(INCL)stack.h $(INCL)define.h $(INCL)cpu.h $(INCL)bus.h
	gcc -o $(LIB)stack.o -c $(LIB)stack.c -I$(INCL)

timer.o: $(INCL)timer.h $(INCL)interrupts.h $(INCL)define.h
	gcc -o $(LIB)timer.o -c $(LIB)timer.c -I$(INCL)

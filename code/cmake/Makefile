# to run, enter: make all

INCL = ../include/
EMU = ../lib/emulator/
PROC = ../lib/processor/
MAIN = ../main/

all: gb_emulator

gb_emulator: main.o emulator.o cpu.o
	gcc -o GB-Emulator $(MAIN)main.o $(EMU)emulator.o $(PROC)cpu.o

main.o: $(MAIN)main.c $(INCL)emulator.h $(INCL)cpu.h
	gcc -o $(MAIN)main.o -c $(MAIN)main.c -I$(INCL)

emulator.o: $(INCL)cpu.h
	gcc -o $(EMU)emulator.o -c $(EMU)emulator.c -I$(INCL)

cpu.o: $(INCL)cpu.h
	gcc -o $(PROC)cpu.o -c $(PROC)cpu.c -I$(INCL)

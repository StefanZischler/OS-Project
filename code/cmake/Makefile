# to run, enter: make all

INCL = ../include/
EMU = ../lib/emulator/
PROC = ../lib/processor/
MAIN = ../main/
LIB = ../lib/

all: gb_emulator

gb_emulator: main.o emulator.o instruction_set.o cpu.o bus.o rom.o
	gcc -o GB-Emulator $(MAIN)main.o $(EMU)emulator.o $(PROC)instruction_set.o $(PROC)cpu.o $(LIB)bus.o $(LIB)rom.o

main.o: $(MAIN)main.c $(INCL)emulator.h $(INCL)cpu.h
	gcc -o $(MAIN)main.o -c $(MAIN)main.c -I$(INCL)

emulator.o: $(INCL)cpu.h $(INCL)rom.h $(INCL)emulator.h
	gcc -o $(EMU)emulator.o -c $(EMU)emulator.c -I$(INCL)

instruction_handling.o: $(INCL)emulator.h $(INCL)cpu.h $(INCL)bus.h
	gcc -o $(PROC)instruction_handling.o -c $(PROC)instruction_handling.c -I$(INCL)

instruction_set.o: $(INCL)instruction_set.h $(INCL)cpu.h $(INCL)bus.h $(INCL)define.h
	gcc -o $(PROC)instruction_set.o -c $(PROC)instruction_set.c -I$(INCL)	

cpu.o: $(INCL)cpu.h $(INCL)emulator.h $(INCL)instruction_set.h $(INCL)bus.h
	gcc -o $(PROC)cpu.o -c $(PROC)cpu.c -I$(INCL)

bus.o: $(INCL)bus.h $(INCL)define.h
	gcc -o $(LIB)bus.o -c $(LIB)bus.c -I$(INCL)

rom.o: $(INCL)rom.h $(INCL)define.h
	gcc -o $(LIB)rom.o -c $(LIB)rom.c -I$(INCL)	

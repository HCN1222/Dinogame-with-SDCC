#
# makefile for checkpoint 5-2
#
# This assumes you have SDCC installed and this targets EdSim51.
# The generated .hex file should be one that can be loaded and run
# directly.
#
# Author: HUANG CHENG-NING
# 01/03/2024
#

CC = sdcc
CFLAGS = -c --model-small
LDFLAGS = 
OBJ = preemptive.rel lcdlib.rel keylib.rel game.rel dino.rel

all: dino.hex

dino.hex: $(OBJ)
	$(CC) $(LDFLAGS) -o $@ $(OBJ)

preemptive.rel: preemptive.c preemptive.h memory.h
	$(CC) $(CFLAGS) preemptive.c

lcdlib.rel: lcdlib.c lcdlib.h memory.h
	$(CC) $(CFLAGS) lcdlib.c

keylib.rel: keylib.c keylib.h memory.h
	$(CC) $(CFLAGS) keylib.c

game.rel: game.c game.h memory.h
	$(CC) $(CFLAGS) game.c

dino.rel: dino.c preemptive.h lcdlib.h keylib.h game.h memory.h
	$(CC) $(CFLAGS) dino.c

clean:
	rm -f *.hex *.ihx *.lnk *.lst *.map *.mem *.rel *.rst *.sym
# serial port of BSL programmer
PROG_PORT = /dev/ttyUSB0   

# mspdebug
MSPDEBUG = ~/mspdebug/mspdebug

# cross compiler installation location
MSPDIR = /opt/msp430-gcc

CC      = $(MSPDIR)/bin/msp430-elf-gcc
OBJCOPY = $(MSPDIR)/bin/msp430-elf-objcopy
SIZE    = $(MSPDIR)/bin/msp430-elf-size

CFLAGS  = -mmcu=msp430g2755 -I$(MSPDIR)/include -Iinclude
LDFLAGS = -mmcu=msp430g2755 -L$(MSPDIR)/include -Linclude

all: main upload

main: main.o uart.o led.o spi.o epd.o

hex: main
	$(SIZE) $<
	$(OBJCOPY) -O ihex $< $<.hex

upload: hex
	$(MSPDEBUG) -d $(PROG_PORT) rom-bsl --bsl-entry-sequence Dr,R,r,R,r,d "prog main"
	./reset_board.py -d $(PROG_PORT)

debug: 
	$(MSPDEBUG) -d $(PROG_PORT) rom-bsl --bsl-entry-sequence Dr,R,r,R,r,d 
	./reset_board.py -d $(PROG_PORT)

clean:
	rm -f *.o *.hex main

image:
	xxd -r -ps out.cap out.bin

reset:
	./reset_board.py -d $(PROG_PORT)

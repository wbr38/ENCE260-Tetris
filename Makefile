# File:   Makefile
# Author: M. P. Hayes, UCECE, William Brown, Matthew Wills
# Date:   12 Sep 2010
# Descr:  Makefile for game

# Definitions.
OBJCOPY=avr-objcopy
SIZE=avr-size
DEL=rm

CC=avr-gcc
CFLAGS= \
	-mmcu=atmega32u2 \
	-Os \
	-Wall \
	-Wstrict-prototypes \
	-Wextra \
	-g

# include paths:
CFLAGS += \
	-I. \
	-I../../utils \
	-I../../fonts \
	-I../../drivers \
	-I../../drivers/avr

# automatic dependency generation (https://stackoverflow.com/a/10202536/16999526)
CFLAGS += \
	-MMD \
	-MP

# Object files
OBJS=game.o piece.o board.o packet.o game_data.o

# from API
OBJS+=system.o \
	button.o \
	led.o \
	timer.o \
	pacer.o \
	tinygl.o \
	display.o \
	ledmat.o \
	font.o \
	task.o \
	navswitch.o \
	ir_uart.o \
	timer0.o \
	usart1.o \
	prescale.o

# Include automatically generated dependency files, if they exist
-include $(OBJS:.o=.d)

# Default target.
all: game.out

# Compile: create object files from C source files
# Because we are using automatic dependency generation, we don't need to manually specify headers
# Only the paths to look for .c files
%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

%.o: ../../utils/%.c
	$(CC) -c $(CFLAGS) $< -o $@

%.o: ../../drivers/%.c
	$(CC) -c $(CFLAGS) $< -o $@

%.o: ../../drivers/avr/%.c
	$(CC) -c $(CFLAGS) $< -o $@

# Link: create ELF output file from object files
game.out: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@ -lm
	$(SIZE) $@

# Clean: remove all generated files
.PHONY: clean
clean: 
	-$(DEL) *.o *.out *.hex *.d


# Target: program project.
.PHONY: program

program: game.out
	$(OBJCOPY) -O ihex game.out game.hex
	dfu-programmer atmega32u2 erase
	dfu-programmer atmega32u2 flash game.hex
	dfu-programmer atmega32u2 start

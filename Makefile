#
# Copyright (C) 2016 CliveLiu
# Subject to the GNU Public License, version 2.
#
# Created By:		Clive Liu<ftdstudio1990@gmail.com>
# Created Date:	2016-03-07
#
# ChangeList:
# Created in 2016-03-07 by Clive;
#
OUTPUT_FILE := serialDaemon
SRC_FILES := $(OUTPUT_FILE).o uart.o receiver.o devmem.o rgb24tobmp.o bmpfile.o

CROSS_COMPILE ?=

CC := $(CROSS_COMPILE)gcc

LIBS := -lpthread -lm
CFLAGS :=
CPPFLAGS :=

.PHONY: all
all: $(SRC_FILES)
	$(CC) $(SRC_FILES) $(LIBS) -o $(OUTPUT_FILE)

%.o: %.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $< -o $@

.PHONY: clean
clean:
	rm -rf *.o $(OUTPUT_FILE)

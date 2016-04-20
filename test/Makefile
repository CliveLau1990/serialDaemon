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
OUT_FILE1 := test1
OUT_FILE2 := test2
OUT_FILE3 := test3
OUT_FILE4 := test4
COMMON_FILES := uart.o

CROSS_COMPILE ?=

CC := $(CROSS_COMPILE)gcc

LIBS := -lpthread
CFLAGS :=
CPPFLAGS :=

.PHONY: all
all: $(OUT_FILE1) $(OUT_FILE2) $(OUT_FILE3) $(OUT_FILE4)

$(OUT_FILE1): $(OUT_FILE1).o $(COMMON_FILES)
	$(CC) $^ $(LIBS) -o $@

$(OUT_FILE2): $(OUT_FILE2).o $(COMMON_FILES)
	$(CC) $^ $(LIBS) -o $@

$(OUT_FILE3): $(OUT_FILE3).o $(COMMON_FILES)
	$(CC) $^ $(LIBS) -o $@

$(OUT_FILE4): $(OUT_FILE4).o $(COMMON_FILES)
	$(CC) $^ $(LIBS) -o $@

%.o: %.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $< -o $@

.PHONY: clean
clean:
	rm -rf *.o $(OUT_FILE1) $(OUT_FILE2) $(OUT_FILE3) $(OUT_FILE4)

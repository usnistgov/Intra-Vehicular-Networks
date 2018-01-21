DESTDIR ?=
PREFIX ?= /usr/local

MAKEFLAGS = -k

CFLAGS    = -O2 -Wall -Wno-parentheses \
	    -fno-strict-aliasing

CPPFLAGS += -Iinclude \
	    -D_FILE_OFFSET_BITS=64 \
	    -DSO_RXQ_OVFL=40 \
	    -DPF_CAN=29 \
	    -DAF_CAN=PF_CAN

PROGRAMS_ISOTP = isotpdump isotprecv isotpsend isotpsniffer isotptun isotpserver isotpperf
PROGRAMS_CANGW = cangw
PROGRAMS_SLCAN = slcan_attach slcand
PROGRAMS = can-calc-bit-timing candump can2IPwrapper can2IPwrapper2 can2IPwrapper3 cansniffer cansend canplayer cangen canbusload IPtoCanwrapper IPtoCanwrapper2\
	   log2long log2asc asc2log\
	   canlogserver bcmserver\
	   $(PROGRAMS_ISOTP)\
	   $(PROGRAMS_CANGW)\
	   $(PROGRAMS_SLCAN)\
	   slcanpty canfdtest

all: $(PROGRAMS)

clean:
	rm -f $(PROGRAMS) *.o

install:
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp -f $(PROGRAMS) $(DESTDIR)$(PREFIX)/bin

distclean:
	rm -f $(PROGRAMS) *.o *~
IPtoCanwrapper.o:   lib.h
IPtoCanwrapper2.o:   lib.h
cansend.o:	lib.h
cangen.o:	lib.h
candump.o:	lib.h
can2IPwrapper.o:	lib.h
can2IPwrapper2.o:	lib.h
can2IPwrapper3.o:	lib.h
canplayer.o:	lib.h
canlogserver.o:	lib.h
canbusload.o:	lib.h
log2long.o:	lib.h
log2asc.o:	lib.h
asc2log.o:	lib.h
canframelen.o:  canframelen.h
IPtoCanwrapper:     IPtoCanwrapper.o    lib.o
IPtoCanwrapper2:     IPtoCanwrapper2.o    lib.o
cansend:	cansend.o	lib.o
cangen:		cangen.o	lib.o
candump:	candump.o	lib.o
can2IPwrapper:	can2IPwrapper.o	lib.o
can2IPwrapper2:	can2IPwrapper2.o	lib.o
can2IPwrapper3:	can2IPwrapper3.o	lib.o
canplayer:	canplayer.o	lib.o
canlogserver:	canlogserver.o	lib.o
log2long:	log2long.o	lib.o
log2asc:	log2asc.o	lib.o
asc2log:	asc2log.o	lib.o
canbusload:	canbusload.o	canframelen.o
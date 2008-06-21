AR = ar
CC = gcc
LD = gcc
DOXYGEN = doxygen
INSTALL = install
SED = sed
MKDIR = mkdir
PKG_CONFIG = pkg-config

ARFLAGS = -cru
CFLAGS = -g -Wall -Wextra -Wundef -Wpointer-arith -Wcast-align \
	-Wwrite-strings -Wstrict-prototypes \
	-Wnested-externs -Werror -pedantic -std=c99 \
	-Wno-format-zero-length -Wformat-security -Wstrict-aliasing=2 \
	-Wmissing-format-attribute -Wunused -Wunreachable-code \
	-Wformat=2 -Werror-implicit-function-declaration \
	-Wmissing-declarations -Wmissing-prototypes
LDFLAGS = -g -L./

# Installation prefix, if not already defined (e.g. on command line)
PREFIX ?= /usr/local
DESTDIR ?=

.PHONY: all clean docs install uninstall

all: libnsgif.a bin/decode_gif
	
libnsgif.a: libnsgif.o libnsgif.pc
	${AR} ${ARFLAGS} libnsgif.a libnsgif.o

libnsgif.pc: libnsgif.pc.in
	$(SED) -e 's#PREFIX#$(PREFIX)#' libnsgif.pc.in > libnsgif.pc

%.o: %.c
	${CC} -c ${CFLAGS} -o $@ $<

bin/decode_gif: examples/decode_gif.c libnsgif.a
	${CC} ${CFLAGS} -o $@ $< libnsgif.a

docs:
	${DOXYGEN}

clean:
	rm -f $(wildcard *.o) $(wildcard *.a) libnsgif.pc
	rm -rf doc

install: libnsgif.a libnsgif.pc
	$(MKDIR) -p $(DESTDIR)$(PREFIX)/lib/pkgconfig
	$(MKDIR) -p $(DESTDIR)$(PREFIX)/lib
	$(MKDIR) -p $(DESTDIR)$(PREFIX)/include
	$(INSTALL) --mode=644 -t $(DESTDIR)$(PREFIX)/lib libnsgif.a
	$(INSTALL) --mode=644 -t $(DESTDIR)$(PREFIX)/include libnsgif.h
	$(INSTALL) --mode=644 -t $(DESTDIR)$(PREFIX)/lib/pkgconfig libnsgif.pc

uninstall:
	rm $(DESTDIR)$(PREFIX)/lib/libnsgif.a
	rm $(DESTDIR)$(PREFIX)/include/libnsgif.h
	rm $(DESTDIR)$(PREFIX)/lib/pkgconfig/libnsgif.pc

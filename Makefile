#
# This file is part of Libnsgif
#

SOURCE = libnsgif.c
HDRS = libnsgif.h utils/log.h

CFLAGS = -Wall -Wextra -Wundef -Wpointer-arith -Wcast-align \
	-Wwrite-strings -Wstrict-prototypes \
	-Wnested-externs -pedantic -std=c99 \
	-Wno-format-zero-length -Wformat-security -Wstrict-aliasing=2 \
	-Wmissing-format-attribute -Wunused -Wunreachable-code \
	-Wformat=2 -Werror-implicit-function-declaration \
	-Wmissing-declarations -Wmissing-prototypes
ARFLAGS = -cr
INSTALL = install
SED = sed
DOXYGEN = doxygen

ifeq ($(TARGET),riscos)
  GCCSDK_INSTALL_CROSSBIN ?= /home/riscos/cross/bin
  GCCSDK_INSTALL_ENV ?= /home/riscos/env
  CC := $(wildcard $(GCCSDK_INSTALL_CROSSBIN)/*gcc)
  AR := $(wildcard $(GCCSDK_INSTALL_CROSSBIN)/*ar)
  CFLAGS += -Driscos -mpoke-function-name -I$(GCCSDK_INSTALL_ENV)/include
  LIBS = -L$(GCCSDK_INSTALL_ENV)/lib
  ifneq (,$(findstring arm-unknown-riscos-gcc,$(CC)))
    EXEEXT := ,e1f
    SUBTARGET := -elf-
  else
    EXEEXT := ,ff8
    SUBTARGET := -aof-
  endif
  PREFIX = $(GCCSDK_INSTALL_ENV)
else
  CFLAGS += -g
  LIBS =
  PREFIX = /usr/local
endif

OBJDIR = $(TARGET)$(SUBTARGET)objects
LIBDIR = $(TARGET)$(SUBTARGET)lib
BINDIR = $(TARGET)$(SUBTARGET)bin

OBJS = $(addprefix $(OBJDIR)/, $(SOURCE:.c=.o))

.PHONY: all clean docs install uninstall

all: $(LIBDIR)/libnsgif.a $(BINDIR)/decode_gif$(EXEEXT)

$(LIBDIR)/libnsgif.a: $(OBJS) $(LIBDIR)/libnsgif.pc
	@echo "    LINK:" $@
	@mkdir -p $(LIBDIR)
	@$(AR) $(ARFLAGS) $@ $(OBJS)

$(LIBDIR)/libnsgif.pc: libnsgif.pc.in
	@echo "     SED:" $@
	@mkdir -p $(LIBDIR)
	@$(SED) -e 's#PREFIX#$(PREFIX)#' $^ > $@

$(BINDIR)/decode_gif$(EXEEXT): examples/decode_gif.c $(LIBDIR)/libnsgif.a
	@echo "    LINK:" $@
	@mkdir -p $(BINDIR)
	@$(CC) $(CFLAGS) -I. -o $@ $^

$(OBJDIR)/%.o: %.c $(HDRS)
	@echo " COMPILE:" $<
	@mkdir -p $(OBJDIR)
	@$(CC) $(CFLAGS) -c -o $@ $<

docs:
	${DOXYGEN}

install: $(LIBDIR)/libnsgif.a $(LIBDIR)/libnsgif.pc
	mkdir -p $(PREFIX)/lib/pkgconfig
	mkdir -p $(PREFIX)/lib
	mkdir -p $(PREFIX)/include
	$(INSTALL) --mode=644 -t $(PREFIX)/lib $(LIBDIR)/libnsgif.a
	$(INSTALL) --mode=644 -t $(PREFIX)/include libnsgif.h
	$(INSTALL) --mode=644 -t $(PREFIX)/lib/pkgconfig $(LIBDIR)/libnsgif.pc

uninstall:
	rm $(PREFIX)/lib/libnsgif.a
	rm $(PREFIX)/include/libnsgif.h
	rm $(PREFIX)/lib/pkgconfig/libnsgif.pc

clean:
	-rm $(OBJS) $(LIBDIR)/libnsgif.a $(LIBDIR)/libnsgif.pc $(BINDIR)/decode_gif$(EXEEXT)
	-rm -rf doc

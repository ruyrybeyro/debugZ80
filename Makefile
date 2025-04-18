
MAKEFILE      = Makefile

EQ            = =

####### Compiler, tools and options

CC            = clang
DEFINES       = 
CFLAGS        = -pipe -O2 $(EXPORT_ARCH_ARGS) -Wall -W -fPIC $(DEFINES)
INCPATH       = -I. -I../QtSpecem/h
DEL_FILE      = rm -f
CHK_DIR_EXISTS= test -d
MKDIR         = mkdir -p
COPY          = cp -f
COPY_FILE     = cp -f
COPY_DIR      = cp -f -R
INSTALL_FILE  = install -m 644 -p
INSTALL_PROGRAM = install -m 755 -p
INSTALL_DIR   = cp -f -R
DEL_FILE      = rm -f
SYMLINK       = ln -f -s
DEL_DIR       = rmdir
MOVE          = mv -f
TAR           = tar -cf
COMPRESS      = gzip -9f
DISTNAME      = debugz801.0.0
DISTDIR = .tmp/debugz801.0.0
LINK          = clang
LFLAGS        = -Wl
LIBS          = $(SUBLIBS)    
AR            = ar cq
RANLIB        = ranlib -s
SED           = sed
STRIP         = strip

####### Output directory

OBJECTS_DIR   = ./


SOURCES       = basic.c \
		cpuregs.c \
		debugz80.c \
		dissbl.c \
		flags.c \
		hexdump.c \
		main.c \
		mem.c \
		panic.c \
		shm_client.c \
		vars.c
OBJECTS       = basic.o \
		cpuregs.o \
		debugz80.o \
		dissbl.o \
		flags.o \
		hexdump.o \
		main.o \
		mem.o \
		panic.o \
		shm_client.o \
		vars.o
DIST          = basic.c \
		cpuregs.c \
		debugz80.c \
		dissbl.c \
		flags.c \
		hexdump.c \
		main.c \
		mem.c \
		panic.c \
		shm_client.c \
		vars.c
DESTDIR       = 
TARGET        = debugz80

####### Custom Variables
EXPORT_VALID_ARCHS = x86_64
EXPORT_ACTIVE_ARCHS = $(filter $(EXPORT_VALID_ARCHS), $(ARCHS))
EXPORT_ARCH_ARGS = $(foreach arch, $(if $(EXPORT_ACTIVE_ARCHS), $(EXPORT_ACTIVE_ARCHS), $(EXPORT_VALID_ARCHS)), -arch $(arch))


first: all
####### Build rules

all: debugz80

debugz80:  $(OBJECTS)  
	$(LINK) $(LFLAGS) -o $(TARGET) $(OBJECTS) $(OBJCOMP) $(LIBS)

Makefile: 

dist: distdir FORCE
	(cd `dirname $(DISTDIR)` && $(TAR) $(DISTNAME).tar $(DISTNAME) && $(COMPRESS) $(DISTNAME).tar) && $(MOVE) `dirname $(DISTDIR)`/$(DISTNAME).tar.gz . && $(DEL_FILE) -r $(DISTDIR)

distdir: FORCE
	@test -d $(DISTDIR) || mkdir -p $(DISTDIR)
	$(COPY_FILE) --parents $(DIST) $(DISTDIR)/
	$(COPY_FILE) --parents $(DISTDIR)/
	$(COPY_FILE) --parents basic.c cpuregs.c debugz80.c dissbl.c flags.c hexdump.c main.c mem.c panic.c shm_client.c vars.c $(DISTDIR)/

compiler_clean: clean

clean:	compiler_clean  
	-$(DEL_FILE) $(OBJECTS)
	-$(DEL_FILE) *~ core *.core 


distclean: clean 
	-$(DEL_FILE) -r debugz80


####### Sub-libraries

check: first

benchmark: first

compiler_rcc_make_all:
compiler_rcc_clean:
	-$(DEL_FILE) 

####### Compile

basic.o: basic.c ../QtSpecem/h/quirks.h \
		../QtSpecem/h/env.h \
		../QtSpecem/h/z80.h \
		../QtSpecem/h/iglobal.h
	$(CC) -c $(CFLAGS) $(INCPATH) -o basic.o basic.c

cpuregs.o: cpuregs.c ../QtSpecem/h/quirks.h \
		../QtSpecem/h/env.h \
		../QtSpecem/h/z80.h \
		../QtSpecem/h/iglobal.h
	$(CC) -c $(CFLAGS) $(INCPATH) -o cpuregs.o cpuregs.c

debugz80.o: debugz80.c ../QtSpecem/h/env.h \
		../QtSpecem/h/z80.h \
		../QtSpecem/h/quirks.h \
		../QtSpecem/h/iglobal.h
	$(CC) -c $(CFLAGS) $(INCPATH) -o debugz80.o debugz80.c

dissbl.o: dissbl.c ../QtSpecem/h/env.h \
		../QtSpecem/h/z80.h \
		../QtSpecem/h/quirks.h \
		../QtSpecem/h/iglobal.h
	$(CC) -c $(CFLAGS) $(INCPATH) -o dissbl.o dissbl.c

flags.o: flags.c ../QtSpecem/h/env.h \
		../QtSpecem/h/z80.h \
		../QtSpecem/h/quirks.h \
		../QtSpecem/h/iglobal.h
	$(CC) -c $(CFLAGS) $(INCPATH) -o flags.o flags.c

hexdump.o: hexdump.c ../QtSpecem/h/quirks.h \
		../QtSpecem/h/env.h \
		../QtSpecem/h/z80.h \
		../QtSpecem/h/iglobal.h
	$(CC) -c $(CFLAGS) $(INCPATH) -o hexdump.o hexdump.c

main.o: main.c ../QtSpecem/h/z80.h \
		../QtSpecem/h/quirks.h \
		../QtSpecem/h/iglobal.h
	$(CC) -c $(CFLAGS) $(INCPATH) -o main.o main.c

mem.o: mem.c ../QtSpecem/h/env.h \
		../QtSpecem/h/z80.h \
		../QtSpecem/h/quirks.h \
		../QtSpecem/h/iglobal.h
	$(CC) -c $(CFLAGS) $(INCPATH) -o mem.o mem.c

panic.o: panic.c 
	$(CC) -c $(CFLAGS) $(INCPATH) -o panic.o panic.c

shm_client.o: shm_client.c ../QtSpecem/h/quirks.h \
		../QtSpecem/h/env.h \
		../QtSpecem/h/z80.h \
		../QtSpecem/h/iglobal.h
	$(CC) -c $(CFLAGS) $(INCPATH) -o shm_client.o shm_client.c

vars.o: vars.c ../QtSpecem/h/quirks.h \
		../QtSpecem/h/env.h \
		../QtSpecem/h/z80.h \
		../QtSpecem/h/iglobal.h
	$(CC) -c $(CFLAGS) $(INCPATH) -o vars.o vars.c

####### Install

install:  FORCE

uninstall:  FORCE

FORCE:


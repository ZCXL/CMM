PWD = $(shell pwd)
CC = gcc
LINKER = gcc
CFLAGS = -Wall -O2 -g
LIBS = 
INCLUDE = 
OBJECTS = src/CMM/obj/main.o \
	src/CMM/obj/analyze.o \
	src/CMM/obj/cgen.o \
	src/CMM/obj/code.o \
	src/CMM/obj/parse.o \
	src/CMM/obj/scan.o \
	src/CMM/obj/symtab.o \
	src/CMM/obj/util.o

CORE_DEPS = src/CMM/src/analyze.h \
	src/CMM/src/cgen.h \
	src/CMM/src/code.h \
	src/CMM/src/globals.h \
	src/CMM/src/parse.h \
	src/CMM/src/scan.h \
	src/CMM/src/symtab.h \
	src/CMM/src/util.h 

CORE_INC = -I src/CMM/src/

defualt:src/CMM/bin/cmm

src/CMM/bin/cmm:$(OBJECTS)
	$(LINKER) -o $@ $(OBJECTS)

src/CMM/obj/main.o:	$(CORE_DEPS) \
	src/CMM/src/main.c
	$(CC) -c $(CFLAGS) $(CORE_INC) -o $@ src/CMM/src/main.c

src/CMM/obj/analyze.o: $(CORE_DEPS) \
	src/CMM/src/analyze.c
	$(CC) -c $(CFLAGS) $(CORE_INC) -o $@ src/CMM/src/analyze.c

src/CMM/obj/cgen.o: $(CORE_DEPS) \
	src/CMM/src/cgen.c
	$(CC) -c $(CFLAGS) $(CORE_INC) -o $@ src/CMM/src/cgen.c

src/CMM/obj/code.o: $(CORE_DEPS) \
	src/CMM/src/code.c
	$(CC) -c $(CFLAGS) $(CORE_INC) -o $@ src/CMM/src/code.c

src/CMM/obj/parse.o: $(CORE_DEPS) \
	src/CMM/src/parse.c
	$(CC) -c $(CFLAGS) $(CORE_INC) -o $@ src/CMM/src/parse.c

src/CMM/obj/scan.o: $(CORE_DEPS) \
	src/CMM/src/scan.c
	$(CC) -c $(CFLAGS) $(CORE_INC) -o $@ src/CMM/src/scan.c

src/CMM/obj/symtab.o: $(CORE_DEPS) \
	src/CMM/src/symtab.c
	$(CC) -c $(CFLAGS) $(CORE_INC) -o $@ src/CMM/src/symtab.c

src/CMM/obj/util.o: $(CORE_DEPS) \
	src/CMM/src/util.c
	$(CC) -c $(CFLAGS) $(CORE_INC) -o $@ src/CMM/src/util.c

all:clean install

install:src/CMM/bin/cmm
	chmod 751 src/CMM/bin/cmm
	cp src/CMM/bin/cmm /usr/local/bin

clean:
	-rm src/CMM/bin/cmm
	-rm src/CMM/obj/*

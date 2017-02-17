CC = gcc
LINKER = gcc
CFLAGS = -Wall -O2
LIBS = 
INCLUDE = 
OBJECTS = src/Linker/obj/main.o \
	src/Linker/obj/combine.o \
	src/Linker/obj/parse.o \
	src/Linker/obj/scan.o \
    src/Linker/obj/util.o

CORE_DEPS = src/Linker/src/combine.h \
	src/Linker/src/globals.h \
	src/Linker/src/scan.h \
	src/Linker/src/util.h \
    src/Linker/src/parse.h

CORE_INC = -I src/Linker/src/

defualt:src/Linker/bin/linker

src/Linker/bin/linker:$(OBJECTS)
	$(LINKER) -o $@ $(OBJECTS)

src/Linker/obj/main.o:	$(CORE_DEPS) \
	src/Linker/src/main.c
	$(CC) -c $(CFLAGS) $(CORE_INC) -o $@ src/Linker/src/main.c

src/Linker/obj/scan.o: $(CORE_DEPS) \
	src/Linker/src/scan.c
	$(CC) -c $(CFLAGS) $(CORE_INC) -o $@ src/Linker/src/scan.c

src/Linker/obj/combine.o: $(CORE_DEPS) \
	src/Linker/src/combine.c
	$(CC) -c $(CFLAGS) $(CORE_INC) -o $@ src/Linker/src/combine.c

src/Linker/obj/util.o: $(CORE_DEPS) \
	src/Linker/src/util.c
	$(CC) -c $(CFLAGS) $(CORE_INC) -o $@ src/Linker/src/util.c

src/Linker/obj/parse.o: $(CORE_DEPS) \
	src/Linker/src/parse.c
	$(CC) -c $(CFLAGS) $(CORE_INC) -o $@ src/Linker/src/parse.c

all:clean install

install:src/Linker/bin/linker
	chmod 751 src/Linker/bin/linker
	cp src/Linker/bin/linker /usr/local/bin

clean:
	-rm src/Linker/bin/linker
	-rm src/Linker/obj/*

CC = gcc
LINKER = gcc
CFLAGS = -Wall -O2
LIBS = 
INCLUDE = 
OBJECTS = src/CVM/obj/main.o \
	src/CVM/obj/excute.o \
	src/CVM/obj/scan.o \
	src/CVM/obj/util.o

CORE_DEPS = src/CVM/src/excute.h \
	src/CVM/src/globals.h \
	src/CVM/src/scan.h \
	src/CVM/src/util.h 

CORE_INC = -I src/CVM/src/

defualt:src/CVM/bin/cvm

src/CVM/bin/cvm:$(OBJECTS)
	$(LINKER) -o $@ $(OBJECTS)

src/CVM/obj/main.o:	$(CORE_DEPS) \
	src/CVM/src/main.c
	$(CC) -c $(CFLAGS) $(CORE_INC) -o $@ src/CVM/src/main.c

src/CVM/obj/scan.o: $(CORE_DEPS) \
	src/CVM/src/scan.c
	$(CC) -c $(CFLAGS) $(CORE_INC) -o $@ src/CVM/src/scan.c

src/CVM/obj/excute.o: $(CORE_DEPS) \
	src/CVM/src/excute.c
	$(CC) -c $(CFLAGS) $(CORE_INC) -o $@ src/CVM/src/excute.c

src/CVM/obj/util.o: $(CORE_DEPS) \
	src/CVM/src/util.c
	$(CC) -c $(CFLAGS) $(CORE_INC) -o $@ src/CVM/src/util.c

all:clean install

install:src/CVM/bin/cvm
	chmod 751 src/CVM/bin/cvm
	cp src/CVM/bin/cvm /usr/local/bin

clean:
	-rm src/CVM/bin/cvm
	-rm src/CVM/obj/*

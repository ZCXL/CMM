CMM_INCLUDES = $(wildcard include/*.cmm)

default: build

clean:
	$(MAKE) -f src/CMM/Makefile clean
	$(MAKE) -f src/CVM/Makefile clean
	$(MAKE) -f src/Linker/Makefile clean
	-rm bin/cmm
	-rm bin/cvm
	-rm bin/linker

build:bin/cmm \
    bin/cvm \
    bin/linker \
    compilelib

bin/cmm:
	$(MAKE) -f src/CMM/Makefile
	cp src/CMM/bin/cmm bin

bin/cvm:
	$(MAKE) -f src/CVM/Makefile
	cp src/CVM/bin/cvm bin

bin/linker:
	$(MAKE) -f src/Linker/Makefile
	cp src/Linker/bin/linker bin
    
compilelib:bin/cmm
	for file in $(CMM_INCLUDES);do\
        	bin/cmm $$file;\
    	done

install:
	$(MAKE) -f src/CMM/Makefile install
	$(MAKE) -f src/CVM/Makefile install
	$(MAKE) -f src/Linker/Makefile install

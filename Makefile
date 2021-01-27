PROGS=dasmfw
OBJS=dasmfw.o \
        Disassembler.o \
		Dasm6800.o \
		Dasm6801.o \
		Dasm6809.o \
		Dasm6301.o \
		Dasm6309.o \
		Dasm68HC11.o \
		Dasm68000.o \
		DasmAvr8.o

CPPFLAGS=-O2 -Wall -Wextra

all:	$(PROGS)

dasmfw: $(OBJS)
	$(CXX) $(CPPFLAGS) -o $@ $^
	
%.o:	%.cpp
	$(CXX) $(CPPFLAGS) $(<) -c -o $@

.PHONY clean:
	rm $(OBJS)
	rm $(PROGS)

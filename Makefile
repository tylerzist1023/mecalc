#
# Compiler flags
#
CC     = clang
CPPC   = clang++
ASMC 	= nasm
RCC		= llvm-rc
CFLAGS = -std=c99
CPPFLAGS = -std=c++14
CXXFLAGS = -DUNICODE -MD -I./include -L./lib
ASMFLAGS = -f elf64
RCCFLAGS = /FO # only on llvm-rc, remove when using windres
CXXLIBS =

#
# Project files
#
SRCDIR = ./src
SRCSCPP = $(shell find $(SRCDIR) -name *.cpp) # might need bash for this
SRCSC = $(shell find $(SRCDIR) -name *.c)
SRCSASM = $(shell find $(SRCDIR) -name *.asm)
SRCSRC = $(shell find $(SRCDIR) -name *.rc)
OBJS = $(patsubst %.cpp,%.o,$(SRCSCPP)) $(patsubst %.c,%.o,$(SRCSC)) $(patsubst %.asm,%.o,$(SRCSASM)) $(patsubst %.rc,%_rc.o,$(SRCSRC))
EXE  = main.exe

BINDIR = ./bin
#
# Debug build settings
#
DBGDIR = $(BINDIR)/debug
DBGEXE = $(DBGDIR)/$(EXE)
DBGOBJS = $(addprefix $(DBGDIR)/, $(OBJS))
DBGCXXFLAGS = -g -O0 -DDEBUG

#
# Release build settings
#
RELDIR = $(BINDIR)/release
RELEXE = $(RELDIR)/$(EXE)
RELOBJS = $(addprefix $(RELDIR)/, $(OBJS))
RELCXXFLAGS = -O3 -DRELEASE

.PHONY: all clean debug prep release remake

# Default build
all: prep debug

#
# Debug rules
#
debug: $(DBGEXE)

$(DBGEXE): $(DBGOBJS)
	$(CPPC) $(CXXFLAGS) $(DBGCXXFLAGS) -o $(DBGEXE) $^ $(CXXLIBS)

$(DBGDIR)/%.o: %.cpp
	$(CPPC) -c $(CPPFLAGS) $(CXXFLAGS) $(DBGCXXFLAGS) -o $@ $*.cpp
$(DBGDIR)/%.o: %.c
	$(CC) -c $(CFLAGS) $(CXXFLAGS) $(DBGCXXFLAGS) -o $@ $*.c
$(DBGDIR)/%.o: %.asm
	$(ASMC) $(ASMFLAGS) $*.asm -o $@
$(DBGDIR)/%_rc.o: %.rc
	$(RCC) $*.rc $(RCCFLAGS) $@

#
# Release rules
#
release: $(RELEXE)

$(RELEXE): $(RELOBJS)
	$(CPPC) $(CXXFLAGS) $(RELCXXFLAGS) -o $(RELEXE) $^ $(CXXLIBS)

$(RELDIR)/%.o: %.cpp
	$(CC) -c $(CPPFLAGS) $(CXXFLAGS) $(RELCXXFLAGS) -o $@ $*.cpp
$(RELDIR)/%.o: %.c
	$(CC) -c $(CFLAGS) $(CXXFLAGS) $(RELCXXFLAGS) -o $@ $*.c
$(RELDIR)/%.o: %.asm
	$(ASMC) $(ASMFLAGS) $*.asm -o $@ 
$(RELDIR)/%_rc.o: %.rc
	$(RCC) $*.rc $(RCCFLAGS) $@

#
# Other rules
#	
prep:
	@mkdir -p $(DBGDIR)/$(SRCDIR) $(RELDIR)/$(SRCDIR)
	@cd src && find . -type d -exec mkdir -p -- ../$(DBGDIR)/$(SRCDIR)/{} ../$(RELDIR)/$(SRCDIR)/{} \;

remake: clean all

clean:
	rm -f $(RELEXE) $(RELOBJS) $(DBGEXE) $(DBGOBJS) $(patsubst %.o,%.d,$(DBGOBJS)) $(patsubst %.o,%.d,$(RELOBJS))

hardclean:
	rm -r $(BINDIR)

Makefile:
	remake

-include $(DBGOBJS:.o=.d)
-include $(RELOBJS:.o=.d)
# aeolus
#Compiler and Linker
CC          := g++
flavour     ?= debug

#The Target Binary Program
TARGET      := aeolus

#The Directories, Source, Includes, Objects, Binary and Resources
SRCDIR      := src
INCDIR      := inc
BUILDDIR    := bin/obj
TARGETDIR   := bin/target
LIBDIR      := bin/lib
RESDIR      := res
SRCEXT      := cpp
DEPEXT      := d
OBJEXT      := o

#Flags, Libraries and Includes
CFLAGS      := -fopenmp -Wall -g
LIB         := -fopenmp -lm
INC         := -I$(INCDIR) -I/usr/local/include
INCDEP      := -I$(INCDIR)

ifeq ($(flavour), debug)
	TARGET = aeolus_debug
endif

SOURCES := cell.cpp, gas_state.cpp, flow_state.cpp, main.cpp, vertex.cpp, \
		   gas_model.cpp, fluid_block.cpp

#---------------------------------------------------------------------------------
#DO NOT EDIT BELOW THIS LINE
#---------------------------------------------------------------------------------
SOURCES     := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
OBJECTS     := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.$(OBJEXT)))
LIBSOURCES  := $(shell find $(SRCDIR)/python_api -type f -name *.$(SRCEXT))

#Default Make
all: directories $(TARGET)

# Compile only
build: directories $(OBJECTS)

#Remake
remake: cleaner all

#Make the Directories
directories:
	mkdir -p $(TARGETDIR)
	mkdir -p $(BUILDDIR)
	mkdir -p $(LIBDIR)

#Clean only Objecst
clean:
	$(RM) -rf $(BUILDDIR)

#Full Clean, Objects and Binaries
cleaner:
	$(RM) -rf bin

#Pull in dependency info for *existing* .o files
-include $(OBJECTS:.$(OBJEXT)=.$(DEPEXT))

#Link
$(TARGET): $(OBJECTS)
	$(CC) -o $(TARGETDIR)/$(TARGET) $^ $(LIB)

#Compile
$(BUILDDIR)/%.$(OBJEXT): $(SRCDIR)/%.$(SRCEXT)
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INC) -c -o $@ $<
	$(CC) $(CFLAGS) $(INCDEP) -MM $(SRCDIR)/$*.$(SRCEXT) > $(BUILDDIR)/$*.$(DEPEXT)
	@cp -f $(BUILDDIR)/$*.$(DEPEXT) $(BUILDDIR)/$*.$(DEPEXT).tmp
	@sed -e 's|.*:|$(BUILDDIR)/$*.$(OBJEXT):|' < $(BUILDDIR)/$*.$(DEPEXT).tmp > $(BUILDDIR)/$*.$(DEPEXT)
	@sed -e 's/.*://' -e 's/\\$$//' < $(BUILDDIR)/$*.$(DEPEXT).tmp | fmt -1 | sed -e 's/^ *//' -e 's/$$/:/' >> $(BUILDDIR)/$*.$(DEPEXT)
	@rm -f $(BUILDDIR)/$*.$(DEPEXT).tmp

# make the dynamic libraries
lib: $(OBJECTS)
	$(CC) -shared -Wl,-soname,libgas.so -o $(LIBDIR)/libgas.so bin/obj/python_api/libgas.o

#Non-File Targets
.PHONY: all remake clean cleaner resources

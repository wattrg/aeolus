# aeolus
#Compiler and Linker
CC          := g++
flavour     ?= debug

#The Target Binary Program
TARGET      := aeolus

#The Directories, Source, Includes, Objects, Binary and Resources
SRCDIR     := src
INCDIR     := inc
BUILDDIR   := build/obj
TARGETDIR  := build/target
INSTALLDIR := inst/bin
LIBINSTDIR := inst/lib
LIBDIR     := build/lib
RESDIR     := res
SRCEXT     := cpp
DEPEXT     := d
OBJEXT     := o

#Flags, Libraries and Includes
CFLAGS      := -fopenmp -Wall -g -fPIC
LIB         := -fopenmp -lm
INC         := -I$(INCDIR) -I/usr/local/include 
INCDEP      := -I$(INCDIR)

ifeq ($(flavour), debug)
	TARGET = aeolus_debug
endif

#---------------------------------------------------------------------------------
#DO NOT EDIT BELOW THIS LINE
#---------------------------------------------------------------------------------
SOURCES     := $(shell find $(SRCDIR) -path src/python_api -prune -o -type f -name *.$(SRCEXT) -print)
OBJECTS     := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.$(OBJEXT)))
LIBSOURCES  := $(shell find $(SRCDIR)/python_api -type f -name *.$(SRCEXT))
PYBIND11    := $(shell python -m pybind11 --includes)

#Default Make
all: directories $(TARGET) lib install install_lib
	@echo Finished

echo_sources:
	@echo $(SOURCES)

echo_objects:
	@echo $(OBJECTS)

install: build $(TARGET) 
	@mkdir -p $(INSTALLDIR)
	cp $(TARGETDIR)/* $(INSTALLDIR)
	@echo Finished installation

# Compile only
build: directories $(OBJECTS)
	@echo Finished build

#Remake
remake: cleaner all

#Make the Directories
directories:
	mkdir -p $(TARGETDIR)
	mkdir -p $(BUILDDIR)
	mkdir -p $(LIBDIR)
	mkdir -p $(BUILDDIR)/gas
	mkdir -p $(BUILDDIR)/grid

#Clean only Objecst
clean:
	$(RM) -rf $(BUILDDIR)

#Full Clean, Objects and Binaries
cleaner:
	$(RM) -rf build
	$(RM) -rf inst

#Pull in dependency info for *existing* .o files
#-include $(OBJECTS:.$(OBJEXT)=.$(DEPEXT))

#Link
$(TARGET): $(OBJECTS)
	$(CC) -o $(TARGETDIR)/$(TARGET) $^ $(LIB)

#Compile
$(BUILDDIR)/%.$(OBJEXT): $(SRCDIR)/%.$(SRCEXT)
	$(CC) $(CFLAGS) $(INC) -c -o $@ $< 
	$(CC) $(CFLAGS) $(INCDEP) -MM $(SRCDIR)/$*.$(SRCEXT) > $(BUILDDIR)/$*.$(DEPEXT)
	@cp -f $(BUILDDIR)/$*.$(DEPEXT) $(BUILDDIR)/$*.$(DEPEXT).tmp
	@sed -e 's|.*:|$(BUILDDIR)/$*.$(OBJEXT):|' < $(BUILDDIR)/$*.$(DEPEXT).tmp > $(BUILDDIR)/$*.$(DEPEXT)
	@sed -e 's/.*://' -e 's/\\$$//' < $(BUILDDIR)/$*.$(DEPEXT).tmp | fmt -1 | sed -e 's/^ *//' -e 's/$$/:/' >> $(BUILDDIR)/$*.$(DEPEXT)
	@rm -f $(BUILDDIR)/$*.$(DEPEXT).tmp

# make the dynamic libraries
$(BUILDDIR)/lib/aeolus.so: $(OBJECTS)
	$(CC) -O3 -Wall -shared -std=c++11 -fPIC $(PYBIND11) $(SRCDIR)/python_api/lib.cpp -o $(LIBDIR)/aeolus.so $^ $(LIB)

lib: directories $(BUILDDIR)/lib/aeolus.so
	@echo Finished building python library

install_lib: lib
	mkdir -p $(LIBINSTDIR)
	cp -f $(LIBDIR)/* $(LIBINSTDIR)
	@echo Finished installing python library


#Non-File Targets
.PHONY: all remake clean cleaner resources

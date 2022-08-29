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
CFLAGS      := -Wall -fPIC
LIB         := -lm
INC         := -I$(INCDIR) -I/usr/local/include 
INCDEP      := -I$(INCDIR)


#---------------------------------------------------------------------------------
#DO NOT EDIT BELOW THIS LINE
#---------------------------------------------------------------------------------
# debug build
ifeq ($(flavour), debug)
	TARGET = aeolus_debug
	CFLAGS := $(CFLAGS) -g
else 
	CFLAGS := $(CFLAGS) -o3
endif

# profiling
ifeq ($(profile), 1)
	ifneq ($(flavour), debug)
		CFLAGS := $(CFLAGS) -g
	endif
	CFLAGS := $(CFLAGS) -pg
endif

GIT_HASH := $(shell git describe --always --dirty)
GIT_BRANCH := $(shell git rev-parse --abbrev-ref HEAD)
COMPILE_TIME := $(shell date +'%Y-%m-%d %H:%M:%S AEST')
export VERSION_FLAGS=-DGIT_HASH="\"$(GIT_HASH)\"" -DCOMPILE_TIME="\"$(COMPILE_TIME)\"" -DGIT_BRANCH="\"$(GIT_BRANCH)\""

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

echo_cflags:
	@echo $(CFLAGS)

echo_makeflags:
	@echo $(MAKEFLAGS)

echo_version_flags:
	@echo $(VERSION_FLAGS)

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
	@mkdir -p $(TARGETDIR)
	@mkdir -p $(BUILDDIR)
	@mkdir -p $(LIBDIR)
	@mkdir -p $(BUILDDIR)/gas
	@mkdir -p $(BUILDDIR)/util
	@mkdir -p $(BUILDDIR)/io
	@mkdir -p $(BUILDDIR)/finite_volume
	@mkdir -p $(BUILDDIR)/solvers
	@mkdir -p $(BUILDDIR)/finite_volume/boundary_conditions
	@mkdir -p $(BUILDDIR)/finite_volume/io

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
	$(CC) $(CFLAGS) -o $(TARGETDIR)/$(TARGET) $^ $(LIB)

#Compile
$(BUILDDIR)/%.$(OBJEXT): $(SRCDIR)/%.$(SRCEXT)
	$(CC) $(CFLAGS) $(VERSION_FLAGS) $(INC) -c -o $@ $< 
	$(CC) $(CFLAGS) $(VERSION_FLAGS) $(INCDEP) -MM $(SRCDIR)/$*.$(SRCEXT) > $(BUILDDIR)/$*.$(DEPEXT)
	@cp -f $(BUILDDIR)/$*.$(DEPEXT) $(BUILDDIR)/$*.$(DEPEXT).tmp
	@sed -e 's|.*:|$(BUILDDIR)/$*.$(OBJEXT):|' < $(BUILDDIR)/$*.$(DEPEXT).tmp > $(BUILDDIR)/$*.$(DEPEXT)
	@sed -e 's/.*://' -e 's/\\$$//' < $(BUILDDIR)/$*.$(DEPEXT).tmp | fmt -1 | sed -e 's/^ *//' -e 's/$$/:/' >> $(BUILDDIR)/$*.$(DEPEXT)
	@rm -f $(BUILDDIR)/$*.$(DEPEXT).tmp

# make the dynamic libraries
$(BUILDDIR)/lib/aeolus.so: $(OBJECTS)
	$(CC) -Wall -shared -std=c++11 -fPIC $(VERSION_FLAGS) $(PYBIND11) $(SRCDIR)/python_api/lib.cpp -o $(LIBDIR)/aeolus.so $^ $(LIB)

lib: directories $(BUILDDIR)/lib/aeolus.so
	@echo Finished building python library

install_lib: lib
	mkdir -p $(LIBINSTDIR)
	cp -f $(LIBDIR)/* $(LIBINSTDIR)
	@echo Finished installing python library


#Non-File Targets
.PHONY: all remake clean cleaner resources

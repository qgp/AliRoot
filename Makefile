# Top level Makefile for AliRoot System
#
# Author: Jan-Erik Revsbech (revsbech@fys.ku.dk)
#         Developed on idea of Boris Polichtchouk (Boris.Polichtchouk@cern.ch), 15/4/2001


##### include general path/location macros #####
override ALICE_ROOT=$(shell pwd)

ifdef ALIVERBOSE
MUTE:=
else
MUTE:=@
endif

include build/Makefile.config
################################################################## 
# 
#            Where to install libraries and binaries 
#                 and common header files

LIBPATH=lib/tgt_$(ALICE_TARGET)
BINPATH=bin/tgt_$(ALICE_TARGET)
EXPORTDIR = $(ALICE_ROOT)/include
##################################################################

##################################################################
# include machine dependent macros 

-include build/Makefile.$(ALICE_TARGET)
##################################################################

##################################################################
# 
#               Check if called with debug

ifeq ($(ALIDEBUG),YES)
override ALICE_TARGET:=$(ALICE_TARGET)DEBUG
FFLAGS := -g $(filter-out -O%,$(FFLAGS))
CXXFLAGS := -g $(filter-out -O%,$(CXXLAGS))
CFLAGS := -g $(filter-out -O%,$(CLAGS))
SOFLAGS := -g $(filter-out -O%,$(SOFLAGS))
LDFLAGS := -g $(filter-out -O%,$(LDFLAGS))
endif
##################################################################

##################################################################
#
#                   Modules to build 

# COMMENTED OUT FOR TEST PURPOSES

ALIROOTMODULES:= STEER PHOS TRD TPC ZDC MUON PMD FMD TOF ITS \
      CRT RICH START STRUCT EVGEN RALICE ALIFAST VZERO \
	  THijing CONTAINERS MEVSIM TMEVSIM THbtp HBTP EMCAL HBTAN \
        THerwig

CERNMODULES:= PDF PYTHIA PYTHIA6 HIJING MICROCERN HERWIG

MODULES:=$(ALIROOTMODULES) $(CERNMODULES)

##################################################################

MODULES += ALIROOT 

MODDIRS := MODULES

#############################################################
# 
#               Default include dirs for 
#          C++, Fortran, Cint, and dependencies 
#      The module directory will be added by each module
#

CXXFLAGS += -I$(ALICE_ROOT)/include
CXXFLAGS += $(patsubst %,-I%,$(ROOTSYS)/include)

CINTFLAGS += -I$(ALICE_ROOT)/include
CINTFLAGS += $(patsubst %,-I%,$(ROOTSYS)/include)

DEPINC  += -I$(ALICE_ROOT)/include
DEPINC += $(patsubst %,-I%,$(ROOTSYS)/include)
#############################################################


#############################################################
#
#             Libraries to link binaries against
#            Libraries will be linked againstSHLIB
LIBS := $(ROOTLIBS) $(SYSLIBS) $(GLIBS)
#############################################################


# default target
default:     alilibs  aliroot


#############################################################
#
#            Each module will add to this

ALLLIBS      :=
ALLEXECS     :=
INCLUDEFILES :=
BINLIBS      := 
EXPORTFILES  := 
#############################################################

BINLIBDIRS   := -L$(ALICE_ROOT)/$(LIBPATH)


#Dependencies of module.mk files

include build/module.dep

#############################################################
# 
#        Check if module.mk is present for the library
%.mk: build/module.tpl
ifndef ALIQUIET
	@echo "***** Creating $@ file *****";
endif
	@share/alibtool mkmodule  $(patsubst %/module.mk,%,$@) > $@;
#############################################################

# **************************************************************************
#
#               If cleaning, do not include 
#             dependencies or module.mk files.

ifeq ($(findstring $(MAKECMDGOALS), clean clean-all clean-dicts clean-modules clean-depend clean-objects clean-libs clean-bins),)

#            If making modules, not not include
#                       anything

ifneq ($(findstring modules,$(MAKECMDGOALS)),modules)

#############################################################
# 
#                Include the modules
-include $(patsubst %,%/module.mk,$(MODULES))
#############################################################

#############################################################
#
#          include dependencies if not making them!
ifneq ($(MAKECMDGOALS),depend )
#           Don't include if cleaning of any sort
ifneq ($(findstring clean,$(MAKECMDGOALS)),clean)
include $(INCLUDEFILES)
endif
endif
#############################################################

endif
endif
# **************************************************************************

#############################################################
#
#              include dummy dependency file
#               *MUST* be last includefile
include build/dummy.d
#############################################################


# targets

.PHONY:		alilibs aliroot makedistr clean

modules: $(patsubst %,%/module.mk,$(MODULES)) 	

aliroot: $(BINPATH) $(ALLEXECS) alilibs bin

alilibs: $(LIBPATH) $(ALLLIBS) lib modules

# Single Makefile "distribution": Makefile + modules + mkdepend scripts
makedistr: $(MODULES)	 
	 tar -cvf MakeDistr.tar $(patsubst %,%/*.pkg,$(MODULES)) \
		Makefile create build/* 

all: aliroot

depend: $(INCLUDEFILES) 

debug:
ifndef ALIQUIET
	@echo "***** Entering DEBUG mode. *****"
endif
	@(export ALIDEBUG=YES && $(MAKE))
lib: 
	@mkdir lib
	@mkdir lib/tgt_$(ALICE_TARGET)

bin: 
	@mkdir bin
	@mkdir bin/tgt_$(ALICE_TARGET)

$(MODULES):
ifndef ALIQUIET
	@echo "***** Making $@ *****"
endif
	@mkdir -p $@

$(BINPATH):
ifndef ALIQUIET
	@echo "***** Making $@ *****"
endif
	@mkdir -p $@

$(LIBPATH):
ifndef ALIQUIET
	@echo "***** Making $@ *****"
endif
	@mkdir -p $@

build/dummy.d: $(EXPORTFILES)
	@(if [ ! -f $@ ] ; then \
	   touch $@; \
	fi)

clean:
	@echo "***** No targen clean, use one of these *****"
	@echo "	clean-aliroot     : Clean up all aliroot libraries"
	@echo "	clean-MODULENAME  : Clean everything from module MODULENAME"
	@echo "	clean-all         : Cleans up everything, including cern libraires"
	@echo "	clean-modules     : Clean all module.mk file in all modules"
	@echo "	clean-libs        : Clean all libraries (not object files)"
	@echo "********************************************"

clean-all: clean-modules clean-libs clean-bins
ifndef ALIQUIET
	@echo "***** Cleaning up everything ****"
endif
	$(MUTE)rm -rf $(patsubst %,%/tgt_$(ALICE_TARGET),$(MODULES))
	$(MUTE)rm -rf $(EXPORTDIR)

#This cleans only libraries that are not CERN-libraries

clean-aliroot:   $(patsubst %,%/module.mk,$(ALIROOTMODULES)) $(patsubst %,clean-%,$(ALIROOTMODULES))

clean-dicts:
ifndef ALIQUIET
	@echo "***** Cleaning up G__ files *****"
endif
	$(MUTE)rm -rf */tgt_$(ALICE_TARGET)/G__*

clean-modules:
ifndef ALIQUIET
	@echo "***** Cleaning up module.mk files *****"
endif
	$(MUTE)rm -rf $(patsubst %,%/module.mk,$(MODULES)) 

clean-depend:
ifndef ALIQUIET
	@echo "***** Cleaning up dependencies *****"
endif
	$(MUTE)echo rm `find . -name "*.d"`

clean-objects:
ifndef ALIQUIET
	@echo "***** Cleaning up .o files *****"
endif
	$(MUTE)echo rm `find . -name "*.o"`

clean-libs:
ifndef ALIQUIET
	@echo "***** Cleaning up library files *****"
endif
	$(MUTE)rm -rf lib/tgt_$(ALICE_TARGET)/*

clean-bins:
ifndef ALIQUIET
	@echo "***** Cleaning up binary files *****"
endif
	$(MUTE)rm -rf bin/tgt_$(ALICE_TARGET)

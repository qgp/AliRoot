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

# IRST coding rule check
IRST_INSTALLDIR=$(ALICE)/local/IRST
CLASSPATH=$(IRST_INSTALLDIR)
export CLASSPATH IRST_INSTALLDIR
CODE_CHECK=java rules.ALICE.ALICERuleChecker
REV_ENG=$(IRST_INSTALLDIR)/scripts/revEng.sh


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

# Uncomment to show some output
#$(warning MAKECMDGOALS=$(MAKECMDGOALS))

ALIROOTMODULES:= STEER PHOS TRD TPC ZDC MUON PMD FMD TOF ITS \
      CRT RICH START STRUCT EVGEN RALICE ALIFAST VZERO \
      THijing CONTAINERS MEVSIM TMEVSIM THbtp HBTP HBTAN \
      THerwig TEPEMGEN EPEMGEN FASTSIM
#EMCAL 
CERNMODULES:= PDF PYTHIA PYTHIA6 HIJING MICROCERN HERWIG

MODULES:=$(ALIROOTMODULES) $(CERNMODULES) 

ifeq ($(findstring TFluka,$(MAKECMDGOALS)),TFluka)
MODULES += TFluka
endif

ifeq ($(findstring Flugg,$(MAKECMDGOALS)),Flugg)
MODULES += Flugg
endif

##################################################################

MODULES += ALIROOT 

MODDIRS := $(MODULES)

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
LIBS := $(GLIBS) $(ROOTLIBS) $(SYSLIBS)
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

#
#
#############################################################

#############################################################
#
#          include dependencies if not making them!
ifneq ($(MAKECMDGOALS),depend )
#           Don't include if cleaning of any sort
ifneq ($(findstring clean,$(MAKECMDGOALS)),clean)
#$(warning INCLUDEFILES=$(INCLUDEFILES))
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

.PHONY:		alilibs aliroot makedistr clean htmldoc

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

CHECKMODULES := $(ALIROOTMODULES)
CHECKMODULES := $(filter-out HBTP,$(CHECKMODULES))
CHECKMODULES := $(filter-out MEVSIM,$(CHECKMODULES))
CHECKMODULES := $(filter-out EPEMGEN,$(CHECKMODULES))

check-all:			$(patsubst %,%/module.mk,$(CHECKMODULES)) $(patsubst %,check-%,$(CHECKMODULES))

reveng-all:			$(patsubst %,%/module.mk,$(CHECKMODULES)) $(patsubst %,reveng-%,$(CHECKMODULES))

revdisp-all:		$(patsubst %,%/module.mk,$(CHECKMODULES)) $(patsubst %,revdisp-%,$(CHECKMODULES))

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

clean-check-all:			$(patsubst %,%/module.mk,$(CHECKMODULES)) $(patsubst %,clean-check-%,$(CHECKMODULES))

clean-reveng-all:			$(patsubst %,%/module.mk,$(CHECKMODULES)) $(patsubst %,clean-reveng-%,$(CHECKMODULES))

htmldoc:
	@rm -rf html/roothtml
	@rm -f  html/picts
	@rm -f /tmp/macros
	@cd html ;\
	aliroot -q -b "mkhtml.C(0,1)" ;\
	ls ../macros/*.C > /tmp/macros ;\
	for i in $(ALIROOTMODULES) ; do \
		ls ../$$i/*.C 2>/dev/null >> /tmp/macros ;\
	done ;\
	for i in `cat /tmp/macros` ; do \
		echo $$i ; \
		aliroot -b -q "mkhtml.C(\"$$i\")" > /dev/null ;\
	done ;\
	./makeExampleList ;
	@ln -s ../picts html/picts
	@ln -s ../../picts html/roothtml/picts
	@ln -s ../../../picts html/roothtml/src/picts
	@ln -s ../../../picts html/roothtml/examples/picts

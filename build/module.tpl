#-*- Mode: Makefile -*-


ifndef PACKFFLAGS
@PACKAGE@FFLAGS:=$(FFLAGS)
else
@PACKAGE@FFLAGS:=$(PACKFFLAGS)
endif

ifndef PACKCFLAGS
@PACKAGE@CFLAGS:=$(CFLAGS)
else
@PACKAGE@CFLAGS:=$(PACKCFLAGS)
endif

ifndef PACKCXXFLAGS
@PACKAGE@CXXFLAGS:=$(CXXFLAGS)
else
@PACKAGE@CXXFLAGS:=$(PACKCXXFLAGS)
endif

ifndef PACKSOFLAGS
@PACKAGE@SOFLAGS:=$(SOFLAGS)
else
@PACKAGE@SOFLAGS:=$(PACKSOFLAGS)
endif

ifdef DYEXT
ifndef PACKDYFLAGS
@PACKAGE@DYFLAGS:=$(DYFLAGS)
else
@PACKAGE@DYFLAGS:=$(PACKDYFLAGS)
endif
endif

ifndef PACKLDFLAGS
@PACKAGE@LDFLAGS:=$(LDFLAGS)
else
@PACKAGE@LDFLAGS:=$(PACKLDFLAGS)
endif

ifndef PACKDCXXFLAGS
@PACKAGE@DCXXFLAGS:=$(CXXFLAGSNO)
else
@PACKAGE@DCXXFLAGS:=$(PACKDCXXFLAGS)
endif

ifndef PACKBLIBS
@PACKAGE@BLIBS:=$(LIBS)
else
@PACKAGE@BLIBS:=$(PACKBLIBS)
endif


ifdef DHDR
WITHDICT=YES
else
WITHDICT=
endif

# Headerfiles exported to common place:
@PACKAGE@EXPORT:=$(EXPORT)
@PACKAGE@EXPORTDEST:=$(patsubst %,$(EXPORTDIR)/%,$(EXPORT))


#Extra include,libs, defines etc.

@PACKAGE@DEFINE:=$(EDEFINE) -D__MODULE__=\"@MODULE@\"

@PACKAGE@INC:=$(patsubst %,-I%,$(EINCLUDE)) -I@MODULE@

@PACKAGE@ELIBS:=$(patsubst %,-l%,$(ELIBS))
@PACKAGE@ELIBSDEP:=$(patsubst %,lib/tgt_$(ALICE_TARGET)/lib%.$(SOEXT),$(ELIBS))
@PACKAGE@ELIBSDIR:=$(patsubst %,-L%,$(ELIBSDIR))

#c sources and headers

@PACKAGE@CS:=$(patsubst %,$(MODDIR)/%,$(CSRCS))
@PACKAGE@CH:=$(patsubst %,$(MODDIR)/%,$(CHDRS))

#Fortran sources 
@PACKAGE@FS:=$(patsubst %,$(MODDIR)/%,$(FSRCS))

#c++ sources and header
@PACKAGE@S:=$(patsubst %,$(MODDIR)/%,$(SRCS))
@PACKAGE@H:=$(patsubst %,$(MODDIR)/%,$(HDRS)) $(EHDRS)

#############################################################################
#
#            If special rootcint headerfiles is specified use them
#                         else use all headers

ifndef CINTHDRS
@PACKAGE@CINTHDRS:=$(@PACKAGE@H) 
else
@PACKAGE@CINTHDRS:=$(CINTHDRS)
endif
#############################################################################

# Package Dictionary 
@PACKAGE@DH:=$(MODDIR)/$(DHDR)

#All objects
@PACKAGE@CO:=$(patsubst %,$(MODDIRO)/%, $(CSRCS:.c=.o))
TEMP:=$(FSRCS:.F=.o)
@PACKAGE@FO:=$(patsubst %,$(MODDIRO)/%, $(TEMP:.f=.o))
@PACKAGE@O:= $(patsubst %,$(MODDIRO)/%, $(SRCS:.cxx=.o)) $(@PACKAGE@FO) $(@PACKAGE@CO)



ifdef WITHDICT
  @PACKAGE@DS:=$(MODDIRO)/G__@PACKAGE@.cxx
  @PACKAGE@DO:=$(MODDIRO)/G__@PACKAGE@.o
  @PACKAGE@DDEP:=$(@PACKAGE@DO:.o=.d)
  @PACKAGE@DEP:=$(@PACKAGE@O:.o=.d) $(@PACKAGE@DDEP)
else
  @PACKAGE@DS:=
  @PACKAGE@DO:=
  @PACKAGE@DDEP:=
  @PACKAGE@DEP:=$(@PACKAGE@O:.o=.d)
endif


#The actual library file

@PACKAGE@LIB:=$(LIBPATH)/lib@PACKAGE@.$(SOEXT)

ifneq ($(DYEXT),)
@PACKAGE@DLIB:=$(LIBPATH)/lib@PACKAGE@.$(DYEXT)
endif

@PACKAGE@ALIB:=$(LIBPATH)/lib@PACKAGE@.$(AEXT)

#Add this to the modules libs
@MODULE@LIBS += $(@PACKAGE@LIB)
@MODULE@ALIBS += $(@PACKAGE@ALIB)
ifneq ($(DYEXT),)
@MODULE@DLIBS += $(@PACKAGE@DLIB)
endif

#The actual binary file

@PACKAGE@BIN:=$(BINPATH)/@PACKAGE@

#Add to modules list of binaries
@MODULE@BINS += $(@PACKAGE@BIN)

# Use in the main Makefile

ifeq ($(TYPE),lib)
ALLLIBS += $(@PACKAGE@LIB)
ALLALIBS += $(@PACKAGE@ALIB)
ifneq ($(DYEXT),)
ALLLIBS += $(@PACKAGE@DLIB)
endif
BINLIBS += -l@PACKAGE@
else
ALLEXECS += $(@PACKAGE@BIN)
endif

ifeq ($(DYEXT),)
@PACKAGE@LIB := $(@PACKAGE@LIB)
else
@PACKAGE@LIB := $(@PACKAGE@LIB)
endif

# include all dependence files
INCLUDEFILES +=$(@PACKAGE@DEP)

EXPORTFILES += $(@PACKAGE@EXPORTDEST)

#local rules

#The exportfiles only include if any!!

ifdef @PACKAGE@EXPORT
#$(@PACKAGE@EXPORTDEST): $(patsubst %,@MODULE@/%,$(@PACKAGE@EXPORT))

$(@PACKAGE@EXPORTDEST): $(EXPORTDIR)/%.h: @MODULE@/%.h
ifndef ALIQUIET
	  @echo "***** Copying file $^ to $@ *****"
endif
	  @[ -d $(dir $@) ] || mkdir -p $(dir $@)
	  @cp $^ $@	
endif

#------------------------------------------------------------------------

ifneq (,$(findstring macosx,$(ALICE_TARGET)))
$(@PACKAGE@LIB): $(@PACKAGE@DLIB) $(@PACKAGE@O) $(@PACKAGE@DO) @MODULE@/module.mk
ifndef ALIQUIET
	  @echo "***** Linking library $@ *****"
endif
	  $(MUTE)rm -f $@; cd $(dir $@); ln -s $(notdir $(@PACKAGE@DLIB)) $(notdir $@)
else
$(@PACKAGE@LIB):$(@PACKAGE@O) $(@PACKAGE@DO) @MODULE@/module.mk
ifndef ALIQUIET
	  @echo "***** Linking library $@ *****"
endif
	  $(MUTE)TMPDIR=/tmp/@MODULE@$$$$.`date +%M%S` ; \
	  export TMPDIR; mkdir -p $$TMPDIR ; cd $$TMPDIR ; \
	  find $(CURDIR)/@MODULE@/tgt_$(ALICE_TARGET) -name '*.o' -exec ln -s {} . \; ;\
	  \rm -f $(CURDIR)/$@ ;\
	  TMPLIB=$(notdir $(@PACKAGE@LIB)); export TMPLIB;\
	  $(SHLD) $(@PACKAGE@SOFLAGS) -o $(CURDIR)/$@ $(notdir $(@PACKAGE@O) $(@PACKAGE@DO))  $(@PACKAGE@ELIBSDIR) $(@PACKAGE@ELIBS) $(SHLIB);\
	  chmod a-w $(CURDIR)/$@ ;\
	  cd $(ALICE_ROOT) ; \rm -rf $$TMPDIR
endif

ifneq ($(DYEXT),)
$(@PACKAGE@DLIB):$(@PACKAGE@O) $(@PACKAGE@DO) @MODULE@/module.mk
ifndef ALIQUIET
	  @echo "***** Linking library $@ *****"
endif
	  $(MUTE)TMPDIR=/tmp/@MODULE@$$$$.`date +%M%S` ; \
	  export TMPDIR; mkdir -p $$TMPDIR ; cd $$TMPDIR ; \
	  find $(CURDIR)/@MODULE@/tgt_$(ALICE_TARGET) -name '*.o' -exec ln -s {} . \; ;\
	  \rm -f $(CURDIR)/$@ ;\
	  $(DYLD) $(@PACKAGE@DYFLAGS) -o $(CURDIR)/$@ $(notdir $(@PACKAGE@O) $(@PACKAGE@DO))  $(@PACKAGE@ELIBSDIR) $(@PACKAGE@ELIBS) $(DYLIB);\
	  chmod a-w $(CURDIR)/$@ ;\
	  cd $(ALICE_ROOT) ; \rm -rf $$TMPDIR
endif

#------------------------------------------------------------------------

$(@PACKAGE@ALIB):$(@PACKAGE@O) $(@PACKAGE@DO) @MODULE@/module.mk
ifndef ALIQUIET
	  @echo "***** Linking static library $@ *****"
endif
	  $(MUTE)TMPDIR=/tmp/@MODULE@$$$$.`date +%M%S` ; \
	  export TMPDIR; mkdir -p $$TMPDIR ; cd $$TMPDIR ; \
	  find $(CURDIR)/@MODULE@/tgt_$(ALICE_TARGET) -name '*.o' -exec ln -s {} . \; ;\
	  \rm -f $(CURDIR)/$@ ;\
	  TMPLIB=$(notdir $(@PACKAGE@LIB)); export TMPLIB;\
	  $(ALLD) $(ALFLAGS) $(CURDIR)/$@ $(notdir $(@PACKAGE@O) $(@PACKAGE@DO))  $(@PACKAGE@ELIBSDIR) $(@PACKAGE@ELIBS) $(ALLIB);\
      cd $(CURDIR) ; \rm -rf $$TMPDIR
	  $(MUTE)chmod a-w $@


$(@PACKAGE@BIN):$(@PACKAGE@O) $(@PACKAGE@DO) @MODULE@/module.mk
ifndef ALIQUIET
	  @echo "***** Making executable $@ *****"
endif
ifeq ($(ALIPROFILE),YES)
	$(MUTE)$(LD) $(@PACKAGE@LDFLAGS) $(@PACKAGE@O) $(ARLIBS) $(SHLIBS) $(@PACKAGE@BLIBS) $(EXEFLAGS) -o $@
else
	  $(MUTE)$(LD) $(@PACKAGE@LDFLAGS) $(@PACKAGE@O) $(@PACKAGE@DO) $(BINLIBDIRS) $(@PACKAGE@ELIBSDIR) $(@PACKAGE@ELIBS) $(@PACKAGE@BLIBS) $(EXEFLAGS) -o $@
endif

$(@PACKAGE@DS): $(@PACKAGE@CINTHDRS) $(@PACKAGE@DH) @MODULE@/module.mk @MODULE@/tgt_$(ALICE_TARGET)/@PACKAGE@_srcslist
ifndef ALIQUIET
	 @echo "***** Creating $@ *****";	
endif
	 @(if [ ! -d '$(dir $@)' ]; then echo "***** Making directory $(dir $@) *****"; mkdir -p $(dir $@); fi;)
	 @\rm -f $(patsubst %.cxx,%.d, $@)
	 $(MUTE)rootcint -f $@ -c $(@PACKAGE@DEFINE) $(CINTFLAGS) $(@PACKAGE@INC) $(@PACKAGE@CINTHDRS) $(@PACKAGE@DH) 

$(@PACKAGE@DO): $(@PACKAGE@DS) 
ifndef ALIQUIET
		@echo "***** Compiling $< *****";
endif
		$(MUTE)$(CXX) $(@PACKAGE@DEFINE) -c $(@PACKAGE@INC)  -I$(ALICE_ROOT) $< -o $@ $(@PACKAGE@DCXXFLAGS)

$(MODDIRO)/@PACKAGE@_srcslist: @MODULE@/@TYPE@@PACKAGE@.pkg
	$(MUTE)if [ ! -d '$(dir $@)' ]; then echo "***** Making directory $(dir $@) *****"; mkdir -p $(dir $@); fi
	$(MUTE)for i in $(@PACKAGE@CS) $(@PACKAGE@S) xyz; do echo $$i; done | sort > $@.new
	$(MUTE)for j in `diff -w $@ $@.new 2>/dev/null | awk '/^\</{sub(".c.*",".",$$2); print $$2}' | $(XARGS) basename` ;\
	do grep -l $$j `find */tgt_$(ALICE_TARGET) -name "*.d"` | $(XARGS) echo \rm -f ;\
	(find @MODULE@/tgt_$(ALICE_TARGET) -name "$${j}d" ; find @MODULE@/tgt_$(ALICE_TARGET) -name "$${j}o") | $(XARGS) echo \rm -f ;\
	done
	$(MUTE)diff -q -w >/dev/null 2>&1 $@ $@.new ;\
	if [ $$? -ne 0 ]; then \mv $@.new $@; else \rm $@.new; fi

#Different targets for the module

ifeq ($(TYPE),lib)
all-@PACKAGE@: $(@PACKAGE@LIB)
ifneq ($(DYEXT),)
all-@PACKAGE@: $(@PACKAGE@DLIB)
endif
else
all-@PACKAGE@: $(@PACKAGE@BIN)
endif

depend-@PACKAGE@: $(@PACKAGE@DEP)

# determination of object files
$(MODDIRO)/%.o: $(MODDIR)/%.cxx $(MODDIRO)/%.d 
ifndef ALIQUIET
	@echo "***** Compiling $< *****";
endif
	@(if [ ! -d '$(dir $@)' ]; then echo "***** Making directory $(dir $@) *****"; mkdir -p $(dir $@); fi;)
	$(MUTE)$(CXX) $(@PACKAGE@DEFINE) -c $(@PACKAGE@INC)   $< -o $@ $(@PACKAGE@CXXFLAGS)

$(MODDIRO)/%.o: $(MODDIR)/%.F $(MODDIRO)/%.d 
ifndef ALIQUIET
	@echo "***** Compiling $< *****";
endif
	@(if [ ! -d '$(dir $@)' ]; then echo "***** Making directory $(dir $@) *****"; mkdir -p $(dir $@); fi;)
	$(MUTE)$(F77) -c $(@PACKAGE@INC)  $< -o $@ $(@PACKAGE@FFLAGS)

$(MODDIRO)/%.o: $(MODDIR)/%.f $(MODDIRO)/%.d 
ifndef ALIQUIET
	@echo "***** Compiling $< *****";
endif
	@(if [ ! -d '$(dir $@)' ]; then echo "***** Making directory $(dir $@) *****"; mkdir -p $(dir $@); fi;)
	$(MUTE)$(F77) -c $(@PACKAGE@INC)  $< -o $@ $(@PACKAGE@FFLAGS)

$(MODDIRO)/%.o: $(MODDIR)/%.c $(MODDIRO)/%.d 
ifndef ALIQUIET
	@echo "***** Compiling $< *****";
endif
	@(if [ ! -d '$(dir $@)' ]; then echo "***** Making directory $(dir $@) *****"; mkdir -p $(dir $@); fi;)
	$(MUTE)$(CC) $(@PACKAGE@DEFINE) -c  $(@PACKAGE@INC)  $< -o $@   $(@PACKAGE@CFLAGS)

$(@PACKAGE@DDEP): $(@PACKAGE@DS)
ifndef ALIQUIET
		@echo "***** Making dependences for $< *****";
endif
		@(if [ ! -d '$(dir $@)' ]; then echo "***** Making directory $(dir $@) *****"; mkdir -p $(dir $@); fi;)
		@share/alibtool depend "$(@PACKAGE@ELIBSDIR) $(@PACKAGE@INC) $(DEPINC)  $<" > $@

$(MODDIRO)/%.d: $(MODDIRS)/%.cxx
ifndef ALIQUIET
		@echo "***** Making dependences for $< *****";
endif
		@(if [ ! -d '$(dir $@)' ]; then echo "***** Making directory $(dir $@) *****"; mkdir -p $(dir $@); fi;)
		@share/alibtool depend "$(@PACKAGE@DEFINE) $(@PACKAGE@ELIBSDIR) $(@PACKAGE@INC) $(DEPINC)  $<" > $@
$(MODDIRO)/%.d: $(MODDIRS)/%.f
ifndef ALIQUIET
		@echo "***** Making dependences for $< *****";
endif
		@(if [ ! -d '$(dir $@)' ]; then echo "***** Making directory $(dir $@) *****"; mkdir -p $(dir $@); fi;)
		@share/alibtool dependF "$(@PACKAGE@ELIBSDIR) $(@PACKAGE@INC) $(DEPINC)  $<" > $@
$(MODDIRO)/%.d: $(MODDIRS)/%.F
ifndef ALIQUIET
		@echo "***** Making dependences for $< *****";
endif
		@(if [ ! -d '$(dir $@)' ]; then echo "***** Making directory $(dir $@) *****"; mkdir -p $(dir $@); fi;)
		$(MUTE)share/alibtool dependF "$(@PACKAGE@ELIBSDIR) $(@PACKAGE@INC) $(DEPINC)  $<" > $@
$(MODDIRO)/%.d: $(MODDIRS)/%.c
ifndef ALIQUIET
		@echo "***** Making dependences for $< *****";
endif
		@(if [ ! -d '$(dir $@)' ]; then echo "***** Making directory $(dir $@) *****"; mkdir -p $(dir $@); fi;)
		@share/alibtool depend "$(@PACKAGE@DEFINE) $(@PACKAGE@ELIBSDIR) $(@PACKAGE@INC) $(DEPINC) $<" > $@

.PRECIOUS: $(patsubst %.cxx,$(MODDIRO)/%.d,$(SRCS))
.PRECIOUS: $(patsubst %.c,$(MODDIRO)/%.d,$(CSRCS))
.PRECIOUS: $(patsubst %.F,$(MODDIRO)/%.d,$(patsubst %.f,$(MODDIRO)/%.d,$(FSRCS)))

@PACKAGE@CHECKS := $(patsubst %.cxx,@MODULE@/check/%.viol,$(SRCS))

check-@MODULE@: $(@PACKAGE@CHECKS)

# IRST coding rule check 
@MODULE@/check/%.i : @MODULE@/%.cxx @MODULE@/tgt_$(ALICE_TARGET)/%.d
	@[ -d $(dir $@) ] || mkdir -p $(dir $@)
	$(MUTE)$(CXX) -E $(@PACKAGE@DEFINE) $(@PACKAGE@INC) -I. $< > $@ $(@PACKAGE@CXXFLAGS)
	@cd $(dir $@) ; $(IRST_INSTALLDIR)/patch/patch4alice.prl $(notdir $@)

# IRST coding rule check
@MODULE@/check/%.viol : @MODULE@/check/%.i
	$(MUTE)echo $@ ; $(CODE_CHECK) $< $(shell echo $(dir $<) | sed -e 's:/check::') > $@

@PACKAGE@PREPROC       = $(patsubst %.viol,%.i,$(@PACKAGE@CHECKS))

@PACKAGE@REVENGS       = $(patsubst %.viol,%.ii,$(@PACKAGE@CHECKS))

.SECONDARY: $(@PACKAGE@REVENGS) $(@PACKAGE@PREPROC)

PACKREVENG += $(@PACKAGE@PREPROC)


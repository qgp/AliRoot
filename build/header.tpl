

#********** This part is for package @PACKAGE@ ***********

#Determine if it's a library or a executable
TYPE=@TYPE@
    
# Package head directory, source and include directories
MODDIR:=@MODULE@
MODDIRS:=$(MODDIR)
MODDIRI:=$(MODDIR)
MODDIRO:=$(MODDIR)/tgt_$(ALICE_TARGET)

# Reseting variables before importing pkg-file
SRCS:=
HDRS:=
FSRCS:=
DHDR:=
CSRCS:=
CHDRS:=
EINCLUDE:=
EDEFINE:=
ELIBS:=
ELIBSDIR:=
PACKFFLAGS:=
PACKCXXFLAGS:=
PACKCFLAGS:=
EXPORT:=
EHDRS:=
CINTHDRS:=
ARLIBS:=
SHLIBS:=
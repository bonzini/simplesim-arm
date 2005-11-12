#
# Makefile - simulator suite make file
#
# This file is a part of the SimpleScalar tool suite written by
# Todd M. Austin as a part of the Multiscalar Research Project.
#  
# The tool suite is currently maintained by Doug Burger and Todd M. Austin.
# 
# Copyright (C) 1994, 1995, 1996, 1997, 1998 by Todd M. Austin
#
# This source file is distributed "as is" in the hope that it will be
# useful.  It is distributed with no warranty, and no author or
# distributor accepts any responsibility for the consequences of its
# use. 
#
# Everyone is granted permission to copy, modify and redistribute
# this source file under the following conditions:
#
#    This tool set is distributed for non-commercial use only. 
#    Please contact the maintainer for restrictions applying to 
#    commercial use of these tools.
#
#    Permission is granted to anyone to make or distribute copies
#    of this source code, either as received or modified, in any
#    medium, provided that all copyright notices, permission and
#    nonwarranty notices are preserved, and that the distributor
#    grants the recipient permission for further redistribution as
#    permitted by this document.
#
#    Permission is granted to distribute this file in compiled
#    or executable form under the same conditions that apply for
#    source code, provided that either:
#
#    A. it is accompanied by the corresponding machine-readable
#       source code,
#    B. it is accompanied by a written offer, with no time limit,
#       to give anyone a machine-readable copy of the corresponding
#       source code in return for reimbursement of the cost of
#       distribution.  This written offer must permit verbatim
#       duplication by anyone, or
#    C. it is distributed by someone who received only the
#       executable form, and is accompanied by a copy of the
#       written offer of source code that they received concurrently.
#
# In other words, you are welcome to use, share and improve this
# source file.  You are forbidden to forbid anyone else to use, share
# and improve what you give them.
#
# INTERNET: dburger@cs.wisc.edu
# US Mail:  1210 W. Dayton Street, Madison, WI 53706
#
# $Id: Makefile,v 1.1.1.1 2000/11/29 14:53:54 cu-cs Exp $
#
# $Log: Makefile,v $
# Revision 1.1.1.1  2000/11/29 14:53:54  cu-cs
# Grand unification of arm sources.
#
#
# Revision 1.1.1.1.2.14  2000/11/14 05:32:21  taustin
# Sim-profile now works for ARM (mostly).
#
# Revision 1.1.1.1.2.13  2000/11/10 15:48:36  ernstd
# created header file for shared variables in sim-outorder.  Also added ptrace support for viewing uops.
#
# Revision 1.1.1.1.2.12  2000/11/09 19:44:52  taustin
# Fixed static's that were removed.
#
# Revision 1.1.1.1.2.11  2000/11/08 20:50:45  kimns
# Makefile for Wattch porting version...
#
# Revision 1.1.1.1.2.10  2000/11/08 20:30:19  kimns
# Files necessary to compile Wattch Model ....
# SimpleScalar/ARM
#
# Revision 1.1.1.1.2.9  2000/11/04 05:30:22  taustin
# Cleaned up the code.
#
# Revision 1.1.1.1.2.8  2000/10/13 17:36:41  chriswea
# changed back to level zero optimization
#
# Revision 1.1.1.1.2.7  2000/09/26 15:45:11  taustin
# Attempted to get sim-outorder working, so so...
#
# Revision 1.1.1.1.2.6  2000/09/19 17:24:06  taustin
# Dynamic dependency checker, for debugging .DEF file dependency specifications.
#
# Revision 1.1.1.1.2.5  2000/09/11 12:22:37  taustin
# Added sim-uop build.
#
# Revision 1.1.1.1.2.4  2000/09/05 13:56:00  taustin
# SimpleScalar/ARM fuzz buster - random co-simulation tester, very handy!
#
# Revision 1.1.1.1.2.3  2000/08/22 18:38:49  taustin
# More progress on the SimpleScalar/ARM target.
#
# Revision 1.1.1.1.2.2  2000/08/02 08:51:56  taustin
# SimpleScalar/ARM co-simulation component, based on the ARMulator.
#
# Revision 1.1.1.1.2.1  2000/07/21 18:30:54  taustin
# More progress on the SimpleScalar/ARM target.
#
# Revision 1.1.1.1  2000/05/26 15:18:57  taustin
# SimpleScalar Tool Set
#
#
# Revision 1.12  1999/12/13 18:56:48  taustin
# improved RCSDIFF handling
#
# Revision 1.11  1999/12/13 18:38:08  taustin
# big and little pisa support added
#
# Revision 1.10  1999/03/08 06:22:35  taustin
# added Solaris host config
#
# Revision 1.9  1998/09/03 22:26:58  taustin
# Alpha OSF1 host support added
# PA-RISC HPUX host support added
# SPARC SunOS host support added
# RS/6000 AIX Unix host support added
# now cleans both tests-alpha and tests-pisa, independent of config
#
# Revision 1.8  1998/08/31 17:05:49  taustin
# added support for MS VC++ NMAKE builds (works with v5.0 SR3 or later)
#
# Revision 1.7  1998/08/27 07:40:03  taustin
# reorganized Makefile: it now works with MS VC++ NMAKE, and many host
#    configurations are supplied in the header; added target configuration
#    support; converted "sim-tests" target to use "-redir:sim" and
#    "-redir:prog" options, this eliminates the need for the "redir"
#    scripts
#
# Revision 1.6  1997/04/16  22:08:40  taustin
# added standalone loader support
#
# Revision 1.5  1997/03/11  01:04:13  taustin
# updated copyright
# CC target now supported
# RANLIB target now supported
# MAKE target now supported
# CFLAGS reorganized
# MFLAGS and MLIBS to improve portability
#
# Revision 1.1  1996/12/05  18:56:09  taustin
# Initial revision
#
# 

##################################################################
#
# Modify the following definitions to suit your build environment,
# NOTE: most platforms should not require any changes
#
##################################################################

#
# Define below C compiler and flags, machine-specific flags and libraries,
# build tools and file extensions, these are specific to a host environment,
# pre-tested environments follow...
#

##
## vanilla Unix, GCC build
##
## NOTE: the SimpleScalar simulators must be compiled with an ANSI C
## compatible compiler.
##
## tested hosts:
##
##	Slackware Linux version 2.0.33, GNU GCC version 2.7.2.2
##	FreeBSD version 3.0-current, GNU egcs version 2.91.50
##	Alpha OSF1 version 4.0, GNU GCC version 2.7.2
##	PA-RISC HPUX version B.10.01, GNU GCC version 2.7-96q3
##	SPARC SunOS version 5.5.1, GNU egcs-2.90.29
##	RS/6000 AIX Unix version 4, GNU GCC version cygnus-2.7-96q4
##	Windows NT version 4.0, Cygnus CygWin/32 beta 19
##
CC = gcc
OFLAGS = -O0 -g -Wall -DMIN_SYSCALL_MODE
#OFLAGS = -O3
MFLAGS = `./sysprobe -flags`
MLIBS  = `./sysprobe -libs` -lm
ENDIAN = `./sysprobe -s`
MAKE = make
AR = ar qcv
AROPT =
RANLIB = ranlib
RM = rm -f
RMDIR = rm -f
LN = ln -s
LNDIR = ln -s
DIFF = diff
OEXT = o
LEXT = a
EEXT =
CS = ;
X=/

##
## Solaris 2.6, GNU GCC version 2.7.2.3
##
#CC = gcc # /s/gcc-2.7.2.3/bin/gcc
#OFLAGS = -O0 -g -Wall
#MFLAGS = `./sysprobe -flags`
#MLIBS  = `./sysprobe -libs` -lm -lsocket -lnsl
#ENDIAN = `./sysprobe -s`
#MAKE = make
#AR = ar qcv
#AROPT =
#RANLIB = ranlib
#RM = rm -f
#RMDIR = rm -f
#LN = ln -s
#LNDIR = ln -s
#DIFF = diff
#OEXT = o
#LEXT = a
#EEXT =
#CS = ;
#X=/

##
## Alpha OSF1 version 4.0, DEC C compiler version V5.2-036
##
#CC = cc -std
#OFLAGS = -O0 -g -w
#MFLAGS = `./sysprobe -flags`
#MLIBS  = `./sysprobe -libs` -lm
#ENDIAN = `./sysprobe -s`
#MAKE = make
#AR = ar qcv
#AROPT =
#RANLIB = ranlib
#RM = rm -f
#RMDIR = rm -f
#LN = ln -s
#LNDIR = ln -s
#DIFF = diff
#OEXT = o
#LEXT = a
#EEXT =
#CS = ;
#X=/

##
## PA-RISC HPUX version B.10.01, c89 HP C compiler version A.10.31.02
##
#CC = c89 +e -D__CC_C89
#OFLAGS = -g
#MFLAGS = `./sysprobe -flags`
#MLIBS  = `./sysprobe -libs` -lm
#ENDIAN = `./sysprobe -s`
#MAKE = make
#AR = ar qcv
#AROPT =
#RANLIB = ranlib
#RM = rm -f
#RMDIR = rm -f
#LN = ln -s
#LNDIR = ln -s
#DIFF = diff
#OEXT = o
#LEXT = a
#EEXT =
#CS = ;
#X=/

##
## SPARC SunOS version 5.5.1, Sun WorkShop C Compiler (acc) version 4.2
##
#CC = /opt/SUNWspro/SC4.2/bin/acc
#OFLAGS = -O0 -g
#MFLAGS = `./sysprobe -flags`
#MLIBS  = `./sysprobe -libs` -lm
#ENDIAN = `./sysprobe -s`
#MAKE = make
#AR = ar qcv
#AROPT =
#RANLIB = ranlib
#RM = rm -f
#RMDIR = rm -f
#LN = ln -s
#LNDIR = ln -s
#DIFF = diff
#OEXT = o
#LEXT = a
#EEXT =
#CS = ;
#X=/

##
## RS/6000 AIX Unix version 4, xlc compiler build
##
#CC = xlc -D__CC_XLC
#OFLAGS = -g
#MFLAGS = `./sysprobe -flags`
#MLIBS  = `./sysprobe -libs` -lm
#ENDIAN = `./sysprobe -s`
#MAKE = make
#AR = ar qcv
#AROPT =
#RANLIB = ranlib
#RM = rm -f
#RMDIR = rm -f
#LN = ln -s
#LNDIR = ln -s
#DIFF = diff
#OEXT = o
#LEXT = a
#EEXT =
#CS = ;
#X=/

##
## WinNT, MS VC++ build
##
## NOTE: requires MS VC++ version 5.0 + service pack 3 or later
## NOTE1: before configuring the simulator, delete the symbolic link "tests/"
##
#CC = cl /Za /nologo
#OFLAGS = /W3 /Zi
#MFLAGS = -DBYTES_LITTLE_ENDIAN -DWORDS_LITTLE_ENDIAN -DFAST_SRL -DFAST_SRA
#MLIBS  =
#ENDIAN = little
#MAKE = nmake /nologo
#AR = lib
#AROPT = -out:
#RANLIB = dir
#RM = del/f/q
#RMDIR = del/s/f/q
#LN = copy
#LNDIR = xcopy/s/e/i
#DIFF = dir
#OEXT = obj
#LEXT = lib
#EEXT = .exe
#CS = &&
#X=\\\\

#
# Compilation-specific feature flags
#
# -DDEBUG	- turns on debugging features
# -DBFD_LOADER	- use libbfd.a to load programs (also required BINUTILS_INC
#		  and BINUTILS_LIB to be defined, see below)
# -DGZIP_PATH	- specifies path to GZIP executable, only needed if SYSPROBE
#		  cannot locate binary
# -DSLOW_SHIFTS	- emulate all shift operations, only used for testing as
#		  sysprobe will auto-detect if host can use fast shifts
#
FFLAGS = -DDEBUG

#
# Point the Makefile to your Simplescalar-based bunutils, these definitions
# should indicate where the include and library directories reside.
# NOTE: these definitions are only required if BFD_LOADER is defined.
#
#BINUTILS_INC = -I../include
#BINUTILS_LIB = -L../lib

#
#


##################################################################
#
# YOU SHOULD NOT NEED TO MODIFY ANYTHING BELOW THIS COMMENT
#
##################################################################

#
# complete flags
#
CFLAGS = $(MFLAGS) $(FFLAGS) $(OFLAGS) $(BINUTILS_INC) $(BINUTILS_LIB) -DARMULATOR -DMODE32 -DMODET

#
# all the sources
#
SRCS =	main.c sim-fast.c sim-safe.c sim-cache.c sim-profile.c \
	sim-eio.c sim-bpred.c sim-cheetah.c sim-outorder.c \
	memory.c regs.c cache.c bpred.c ptrace.c eventq.c \
	resource.c endian.c dlite.c symbol.c eval.c options.c range.c \
	eio.c stats.c endian.c misc.c \
	target-pisa/pisa.c target-pisa/loader.c target-pisa/syscall.c \
	target-pisa/symbol.c \
	target-alpha/alpha.c target-alpha/loader.c target-alpha/syscall.c \
	target-alpha/symbol.c

HDRS =	syscall.h memory.h regs.h sim.h loader.h cache.h bpred.h ptrace.h \
	eventq.h resource.h endian.h dlite.h symbol.h eval.h bitmap.h \
	eio.h range.h version.h endian.h misc.h
#
# common objects
#
OBJS =	main.$(OEXT) syscall.$(OEXT) memory.$(OEXT) regs.$(OEXT) \
	loader.$(OEXT) endian.$(OEXT) dlite.$(OEXT) symbol.$(OEXT) \
	eval.$(OEXT) options.$(OEXT) stats.$(OEXT) eio.$(OEXT) \
	range.$(OEXT) misc.$(OEXT) machine.$(OEXT)

#
# programs to build
#
PROGS = sim-fast$(EEXT) sim-safe$(EEXT) sim-uop$(EEXT) im-eio$(EEXT) \
	sim-bpred$(EEXT) sim-profile$(EEXT) \
	sim-cheetah$(EEXT) sim-cache$(EEXT) sim-outorder$(EEXT) \
	sim-armulator$(EEXT) sim-fuzz$(EEXT) sim-dis$(EEXT) \
	sim-depchk$(EEXT)

#
# all targets, NOTE: library ordering is important...
#
all: sim-safe sim-uop sim-profile sim-cache sim-cheetah sim-bpred sim-outorder # sim-armulator sim-dis sim-depchk $(PROGS)
	@echo "my work is done here..."

config-pisa:
	-$(RM) config.h machine.h machine.c machine.def loader.c symbol.c syscall.c
	$(LN) target-pisa$(X)config.h config.h
	$(LN) target-pisa$(X)pisa.h machine.h
	$(LN) target-pisa$(X)pisa.c machine.c
	$(LN) target-pisa$(X)pisa.def machine.def
	$(LN) target-pisa$(X)loader.c loader.c
	$(LN) target-pisa$(X)symbol.c symbol.c
	$(LN) target-pisa$(X)syscall.c syscall.c
	-$(RMDIR) tests
	$(LNDIR) tests-pisa tests

config-pisabig:
	-$(RM) config.h machine.h machine.c machine.def loader.c symbol.c syscall.c
	$(LN) target-pisa$(X)configbig.h config.h
	$(LN) target-pisa$(X)pisa.h machine.h
	$(LN) target-pisa$(X)pisa.c machine.c
	$(LN) target-pisa$(X)pisa.def machine.def
	$(LN) target-pisa$(X)loader.c loader.c
	$(LN) target-pisa$(X)symbol.c symbol.c
	$(LN) target-pisa$(X)syscall.c syscall.c
	-$(RMDIR) tests
	$(LNDIR) tests-pisa tests

config-pisalit:
	-$(RM) config.h machine.h machine.c machine.def loader.c symbol.c syscall.c
	$(LN) target-pisa$(X)configlit.h config.h
	$(LN) target-pisa$(X)pisa.h machine.h
	$(LN) target-pisa$(X)pisa.c machine.c
	$(LN) target-pisa$(X)pisa.def machine.def
	$(LN) target-pisa$(X)loader.c loader.c
	$(LN) target-pisa$(X)symbol.c symbol.c
	$(LN) target-pisa$(X)syscall.c syscall.c
	-$(RMDIR) tests
	$(LNDIR) tests-pisa tests

config-alpha:
	-$(RM) config.h machine.h machine.c machine.def loader.c symbol.c syscall.c
	$(LN) target-alpha$(X)config.h config.h
	$(LN) target-alpha$(X)alpha.h machine.h
	$(LN) target-alpha$(X)alpha.c machine.c
	$(LN) target-alpha$(X)alpha.def machine.def
	$(LN) target-alpha$(X)loader.c loader.c
	$(LN) target-alpha$(X)symbol.c symbol.c
	$(LN) target-alpha$(X)syscall.c syscall.c
	-$(RMDIR) tests
	$(LNDIR) tests-alpha tests

config-arm:
	-$(RM) config.h machine.h machine.c machine.def loader.c symbol.c syscall.c
	$(LN) target-arm$(X)config.h config.h
	$(LN) target-arm$(X)arm.h machine.h
	$(LN) target-arm$(X)arm.c machine.c
	$(LN) target-arm$(X)arm.def machine.def
	$(LN) target-arm$(X)loader.c loader.c
	$(LN) target-arm$(X)symbol.c symbol.c
	$(LN) target-arm$(X)syscall.c syscall.c
	-$(RMDIR) tests
	$(LNDIR) tests-arm tests

sysprobe$(EEXT):	sysprobe.c
	$(CC) $(FFLAGS) -o sysprobe$(EEXT) sysprobe.c
	@echo endian probe results: $(ENDIAN)
	@echo probe flags: $(MFLAGS)
	@echo probe libs: $(MLIBS)

sim-fast$(EEXT):	sysprobe$(EEXT) sim-fast.$(OEXT) $(OBJS) libexo/libexo.$(LEXT)
	$(CC) -o sim-fast$(EEXT) $(CFLAGS) sim-fast.$(OEXT) $(OBJS) libexo/libexo.$(LEXT) $(MLIBS)

sim-depchk$(EEXT):	sysprobe$(EEXT) sim-depchk.$(OEXT) $(OBJS) libexo/libexo.$(LEXT)
	$(CC) -o sim-depchk$(EEXT) $(CFLAGS) sim-depchk.$(OEXT) $(OBJS) libexo/libexo.$(LEXT) $(MLIBS)

sim-safe$(EEXT):	sysprobe$(EEXT) sim-safe.$(OEXT) $(OBJS) libexo/libexo.$(LEXT)
	$(CC) -o sim-safe$(EEXT) $(CFLAGS) sim-safe.$(OEXT) $(OBJS) libexo/libexo.$(LEXT) $(MLIBS)

sim-uop$(EEXT):	sysprobe$(EEXT) sim-uop.$(OEXT) $(OBJS) libexo/libexo.$(LEXT)
	$(CC) -o sim-uop$(EEXT) $(CFLAGS) sim-uop.$(OEXT) $(OBJS) libexo/libexo.$(LEXT) $(MLIBS)

sim-dis$(EEXT):	sysprobe$(EEXT) sim-dis.$(OEXT) $(OBJS) libexo/libexo.$(LEXT)
	$(CC) -o sim-dis$(EEXT) $(CFLAGS) sim-dis.$(OEXT) $(OBJS) libexo/libexo.$(LEXT) $(MLIBS)

sim-armulator$(EEXT):	sysprobe$(EEXT) sim-armulator.$(OEXT) armulator.o $(OBJS) libexo/libexo.$(LEXT)
	$(CC) -o sim-armulator$(EEXT) $(CFLAGS) sim-armulator.$(OEXT) $(OBJS) armulator.o libexo/libexo.$(LEXT) $(MLIBS)

sim-fuzz$(EEXT):	sysprobe$(EEXT) sim-fuzz.$(OEXT) armulator.o $(OBJS) libexo/libexo.$(LEXT)
	$(CC) -o sim-fuzz$(EEXT) $(CFLAGS) sim-fuzz.$(OEXT) $(OBJS) armulator.o libexo/libexo.$(LEXT) $(MLIBS)

armulator.o: armulator.c
	$(CC) -o armulator.o -c $(CFLAGS) armulator.c

sim-profile$(EEXT):	sysprobe$(EEXT) sim-profile.$(OEXT) $(OBJS) libexo/libexo.$(LEXT)
	$(CC) -o sim-profile$(EEXT) $(CFLAGS) sim-profile.$(OEXT) $(OBJS) libexo/libexo.$(LEXT) $(MLIBS)

sim-eio$(EEXT):	sysprobe$(EEXT) sim-eio.$(OEXT) $(OBJS) libexo/libexo.$(LEXT)
	$(CC) -o sim-eio$(EEXT) $(CFLAGS) sim-eio.$(OEXT) $(OBJS) libexo/libexo.$(LEXT) $(MLIBS)

sim-bpred$(EEXT):	sysprobe$(EEXT) sim-bpred.$(OEXT) bpred.$(OEXT) $(OBJS) libexo/libexo.$(LEXT)
	$(CC) -o sim-bpred$(EEXT) $(CFLAGS) sim-bpred.$(OEXT) bpred.$(OEXT) $(OBJS) libexo/libexo.$(LEXT) $(MLIBS)

sim-cheetah$(EEXT):	sysprobe$(EEXT) sim-cheetah.$(OEXT) $(OBJS) libcheetah/libcheetah.$(LEXT) libexo/libexo.$(LEXT)
	$(CC) -o sim-cheetah$(EEXT) $(CFLAGS) sim-cheetah.$(OEXT) $(OBJS) libcheetah/libcheetah.$(LEXT) libexo/libexo.$(LEXT) $(MLIBS)

sim-cache$(EEXT):	sysprobe$(EEXT) sim-cache.$(OEXT) cache.$(OEXT) $(OBJS) libexo/libexo.$(LEXT)
	$(CC) -o sim-cache$(EEXT) $(CFLAGS) sim-cache.$(OEXT) cache.$(OEXT) $(OBJS) libexo/libexo.$(LEXT) $(MLIBS)

sim-outorder$(EEXT):	sysprobe$(EEXT) sim-outorder.$(OEXT) cache.$(OEXT) bpred.$(OEXT) resource.$(OEXT) ptrace.$(OEXT) $(OBJS) libexo/libexo.$(LEXT)
	$(CC) -o sim-outorder$(EEXT) $(CFLAGS) sim-outorder.$(OEXT) cache.$(OEXT) bpred.$(OEXT) resource.$(OEXT) ptrace.$(OEXT) $(OBJS) libexo/libexo.$(LEXT) $(MLIBS)

exo libexo/libexo.$(LEXT): sysprobe$(EEXT)
	cd libexo $(CS) \
	$(MAKE) "MAKE=$(MAKE)" "CC=$(CC)" "AR=$(AR)" "AROPT=$(AROPT)" "RANLIB=$(RANLIB)" "CFLAGS=$(MFLAGS) $(FFLAGS) $(OFLAGS)" "OEXT=$(OEXT)" "LEXT=$(LEXT)" "EEXT=$(EEXT)" "X=$(X)" "RM=$(RM)" libexo.$(LEXT)

cheetah libcheetah/libcheetah.$(LEXT): sysprobe$(EEXT)
	cd libcheetah $(CS) \
	$(MAKE) "MAKE=$(MAKE)" "CC=$(CC)" "AR=$(AR)" "AROPT=$(AROPT)" "RANLIB=$(RANLIB)" "CFLAGS=$(MFLAGS) $(FFLAGS) $(OFLAGS)" "OEXT=$(OEXT)" "LEXT=$(LEXT)" "EEXT=$(EEXT)" "X=$(X)" "RM=$(RM)" libcheetah.$(LEXT)

.c.$(OEXT):
	$(CC) $(CFLAGS) -c $*.c

filelist:
	@echo $(SRCS) $(HDRS) Makefile

diffs:
	-rcsdiff RCS/*
	-cd config; rcsdiff RCS/*
	-cd libcheetah; rcsdiff RCS/*
	-cd libexo; rcsdiff RCS/*
	-cd target-alpha; rcsdiff RCS/*
	-cd target-pisa; rcsdiff RCS/*

sim-tests sim-tests-nt: sysprobe$(EEXT) $(PROGS)
	cd tests $(CS) \
	$(MAKE) "MAKE=$(MAKE)" "RM=$(RM)" "ENDIAN=$(ENDIAN)" tests \
		"DIFF=$(DIFF)" "SIM_DIR=.." "SIM_BIN=sim-fast$(EEXT)" \
		"X=$(X)" "CS=$(CS)" $(CS) \
	cd ..
	cd tests $(CS) \
	$(MAKE) "MAKE=$(MAKE)" "RM=$(RM)" "ENDIAN=$(ENDIAN)" tests \
		"DIFF=$(DIFF)" "SIM_DIR=.." "SIM_BIN=sim-safe$(EEXT)" \
		"X=$(X)" "CS=$(CS)" $(CS) \
	cd ..
	cd tests $(CS) \
	$(MAKE) "MAKE=$(MAKE)" "RM=$(RM)" "ENDIAN=$(ENDIAN)" tests \
		"DIFF=$(DIFF)" "SIM_DIR=.." "SIM_BIN=sim-cache$(EEXT)" \
		"X=$(X)" "CS=$(CS)" $(CS) \
	cd ..
	cd tests $(CS) \
	$(MAKE) "MAKE=$(MAKE)" "RM=$(RM)" "ENDIAN=$(ENDIAN)" tests \
		"DIFF=$(DIFF)" "SIM_DIR=.." "SIM_BIN=sim-cheetah$(EEXT)" \
		"X=$(X)" "CS=$(CS)" $(CS) \
	cd ..
	cd tests $(CS) \
	$(MAKE) "MAKE=$(MAKE)" "RM=$(RM)" "ENDIAN=$(ENDIAN)" tests \
		"DIFF=$(DIFF)" "SIM_DIR=.." "SIM_BIN=sim-bpred$(EEXT)" \
		"X=$(X)" "CS=$(CS)" $(CS) \
	cd ..
	cd tests $(CS) \
	$(MAKE) "MAKE=$(MAKE)" "RM=$(RM)" "ENDIAN=$(ENDIAN)" tests \
		"DIFF=$(DIFF)" "SIM_DIR=.." "SIM_BIN=sim-profile$(EEXT)" \
		"X=$(X)" "CS=$(CS)" "SIM_OPTS=-all" $(CS) \
	cd ..
	cd tests $(CS) \
	$(MAKE) "MAKE=$(MAKE)" "RM=$(RM)" "ENDIAN=$(ENDIAN)" tests \
		"DIFF=$(DIFF)" "SIM_DIR=.." "SIM_BIN=sim-outorder$(EEXT)" \
		"X=$(X)" "CS=$(CS)" $(CS) \
	cd ..

clean:
	-$(RM) *.o *.obj core *~ Makefile.bak sysprobe$(EEXT) $(PROGS)
	cd libcheetah $(CS) $(MAKE) "RM=$(RM)" "CS=$(CS)" clean $(CS) cd ..
	cd libexo $(CS) $(MAKE) "RM=$(RM)" "CS=$(CS)" clean $(CS) cd ..
	cd tests-alpha $(CS) $(MAKE) "RM=$(RM)" "CS=$(CS)" clean $(CS) cd ..
	cd tests-pisa $(CS) $(MAKE) "RM=$(RM)" "CS=$(CS)" clean $(CS) cd ..

unpure:
	rm -f sim.pure *pure*.o sim.pure.pure_hardlink sim.pure.pure_linkinfo

depend:
	makedepend.local -n -x $(BINUTILS_INC) $(SRCS)


# DO NOT DELETE THIS LINE -- make depend depends on it.

main.$(OEXT): host.h misc.h machine.h machine.def endian.h version.h dlite.h
main.$(OEXT): regs.h memory.h options.h stats.h eval.h loader.h sim.h
sim-fast.$(OEXT): host.h misc.h machine.h machine.def regs.h memory.h
sim-fast.$(OEXT): options.h stats.h eval.h loader.h syscall.h dlite.h sim.h
sim-safe.$(OEXT): host.h misc.h machine.h machine.def regs.h memory.h
sim-safe.$(OEXT): options.h stats.h eval.h loader.h syscall.h dlite.h sim.h
sim-cache.$(OEXT): host.h misc.h machine.h machine.def regs.h memory.h
sim-cache.$(OEXT): options.h stats.h eval.h cache.h loader.h syscall.h
sim-cache.$(OEXT): dlite.h sim.h
sim-profile.$(OEXT): host.h misc.h machine.h machine.def regs.h memory.h
sim-profile.$(OEXT): options.h stats.h eval.h loader.h syscall.h dlite.h
sim-profile.$(OEXT): symbol.h sim.h
sim-eio.$(OEXT): host.h misc.h machine.h machine.def regs.h memory.h
sim-eio.$(OEXT): options.h stats.h eval.h loader.h syscall.h dlite.h eio.h
sim-eio.$(OEXT): range.h sim.h
sim-bpred.$(OEXT): host.h misc.h machine.h machine.def regs.h memory.h
sim-bpred.$(OEXT): options.h stats.h eval.h loader.h syscall.h dlite.h
sim-bpred.$(OEXT): bpred.h sim.h
sim-cheetah.$(OEXT): host.h misc.h machine.h machine.def regs.h memory.h
sim-cheetah.$(OEXT): options.h stats.h eval.h loader.h syscall.h dlite.h
sim-cheetah.$(OEXT): libcheetah/libcheetah.h sim.h
sim-outorder.$(OEXT): host.h misc.h machine.h machine.def regs.h memory.h
sim-outorder.$(OEXT): options.h stats.h eval.h cache.h loader.h syscall.h
sim-outorder.$(OEXT): bpred.h resource.h bitmap.h ptrace.h range.h dlite.h
sim-outorder.$(OEXT): sim.h 
memory.$(OEXT): host.h misc.h machine.h machine.def options.h stats.h eval.h
memory.$(OEXT): memory.h
regs.$(OEXT): host.h misc.h machine.h machine.def loader.h regs.h memory.h
regs.$(OEXT): options.h stats.h eval.h
cache.$(OEXT): host.h misc.h machine.h machine.def cache.h memory.h options.h
cache.$(OEXT): stats.h eval.h
bpred.$(OEXT): host.h misc.h machine.h machine.def bpred.h stats.h eval.h
ptrace.$(OEXT): host.h misc.h machine.h machine.def range.h ptrace.h
eventq.$(OEXT): host.h misc.h machine.h machine.def eventq.h bitmap.h
resource.$(OEXT): host.h misc.h resource.h
endian.$(OEXT): endian.h loader.h host.h misc.h machine.h machine.def regs.h
endian.$(OEXT): memory.h options.h stats.h eval.h
dlite.$(OEXT): host.h misc.h machine.h machine.def version.h eval.h regs.h
dlite.$(OEXT): memory.h options.h stats.h sim.h symbol.h loader.h range.h
dlite.$(OEXT): dlite.h
symbol.$(OEXT): host.h misc.h loader.h machine.h
symbol.$(OEXT): machine.def regs.h memory.h options.h stats.h eval.h symbol.h
eval.$(OEXT): host.h misc.h eval.h machine.h machine.def
options.$(OEXT): host.h misc.h options.h
range.$(OEXT): host.h misc.h machine.h machine.def symbol.h loader.h regs.h
range.$(OEXT): memory.h options.h stats.h eval.h range.h
eio.$(OEXT): host.h misc.h machine.h machine.def regs.h memory.h options.h
eio.$(OEXT): stats.h eval.h loader.h libexo/libexo.h host.h misc.h machine.h
eio.$(OEXT): syscall.h sim.h endian.h eio.h
stats.$(OEXT): host.h misc.h machine.h machine.def eval.h stats.h
endian.$(OEXT): endian.h loader.h host.h misc.h machine.h machine.def regs.h
endian.$(OEXT): memory.h options.h stats.h eval.h
misc.$(OEXT): host.h misc.h machine.h machine.def
pisa.$(OEXT): host.h misc.h machine.h machine.def eval.h regs.h
loader.$(OEXT): host.h misc.h machine.h machine.def endian.h regs.h memory.h
loader.$(OEXT): options.h stats.h eval.h sim.h eio.h loader.h
syscall.$(OEXT): host.h misc.h machine.h machine.def regs.h memory.h
syscall.$(OEXT): options.h stats.h eval.h loader.h sim.h endian.h eio.h
syscall.$(OEXT): syscall.h
symbol.$(OEXT): host.h misc.h loader.h machine.h
symbol.$(OEXT): machine.def regs.h memory.h options.h stats.h eval.h symbol.h
alpha.$(OEXT): host.h misc.h machine.h machine.def eval.h regs.h
loader.$(OEXT): host.h misc.h machine.h machine.def endian.h regs.h memory.h
loader.$(OEXT): options.h stats.h eval.h sim.h eio.h loader.h
syscall.$(OEXT): host.h misc.h machine.h machine.def regs.h memory.h
syscall.$(OEXT): options.h stats.h eval.h loader.h sim.h endian.h eio.h
syscall.$(OEXT): syscall.h
symbol.$(OEXT): host.h misc.h loader.h machine.h machine.def regs.h memory.h
symbol.$(OEXT): options.h stats.h eval.h symbol.h

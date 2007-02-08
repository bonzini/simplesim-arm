AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = -Wall -DMIN_SYSCALL_MODE $(BINUTILS_INC) $(FFLAGS) -DARMULATOR -DMODE32 -DMODET
AM_CFLAGS = `./sysprobe -flags`
AM_LDFLAGS  = `./sysprobe -libs` $(BINUTILS_LIB)

AR = ar
RANLIB = @RANLIB@
RM = rm -f
RMDIR = rm -f
LN = @LN_S@
LNDIR = @LN_S@
DIFF = diff
CS = &&

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
# all the sources
#
noinst_LIBRARIES = libsim.a libsim-power.a
libsim_a_SOURCES = \
	main.c \
	memory.c regs.c cache.c bpred.c ptrace.c \
	resource.c endian.c dlite.c eval.c options.c range.c \
	eio.c stats.c endian.c misc.c \
	loader.c symbol.c machine.c syscall.c \
	power.c time.c cacti/io.c

libsim_power_a_SOURCES = \
	main.c \
	memory.c regs.c cache.c bpred.c ptrace.c \
	resource.c endian.c dlite.c eval.c options.c range.c \
	eio.c stats.c endian.c misc.c \
	loader.c symbol.c machine.c syscall.c

dist_noinst_DATA = eventq.c eventq.h

noinst_HEADERS = \
	syscall.h memory.h regs.h sim.h loader.h cache.h bpred.h ptrace.h \
	resource.h endian.h dlite.h symbol.h eval.h bitmap.h \
	eio.h range.h version.h endian.h misc.h

bin_PROGRAMS = sim-fast sim-safe sim-cache sim-profile sim-bpred \
	sim-cheetah sim-outorder sim-dis sim-depchk sim-uop sim-panalyzer \
	sim-wattch

EXTRA_PROGRAMS = sim-armulator sim-eio sysprobe


sim_fast_LDADD = libsim.a libexo/libexo.a
sim_safe_LDADD = libsim.a libexo/libexo.a
sim_cache_LDADD = libsim.a libexo/libexo.a
sim_profile_LDADD = libsim.a libexo/libexo.a
sim_eio_LDADD = libsim.a libexo/libexo.a
sim_bpred_LDADD = libsim.a libexo/libexo.a
sim_cheetah_LDADD = libsim.a libcheetah/libcheetah.a libexo/libexo.a
sim_outorder_LDADD = libsim.a libexo/libexo.a
sim_wattch_LDADD = libsim.a libexo/libexo.a
sim_armulator_LDADD = libsim.a libexo/libexo.a
sim_dis_LDADD = libsim.a libexo/libexo.a
sim_depchk_LDADD = libsim.a libexo/libexo.a
sim_uop_LDADD = libsim.a libexo/libexo.a
sim_panalyzer_LDADD = libsim-power.a libexo/libexo.a libpanalyzer/libpanalyzer.a

sim_fast_DEPENDENCIES = libsim.a libexo/libexo.a
sim_safe_DEPENDENCIES = libsim.a libexo/libexo.a
sim_cache_DEPENDENCIES = libsim.a libexo/libexo.a
sim_profile_DEPENDENCIES = libsim.a libexo/libexo.a
sim_eio_DEPENDENCIES = libsim.a libexo/libexo.a
sim_bpred_DEPENDENCIES = libsim.a libexo/libexo.a
sim_cheetah_DEPENDENCIES = libsim.a libcheetah/libcheetah.a libexo/libexo.a
sim_outorder_DEPENDENCIES = libsim.a libexo/libexo.a
sim_wattch_DEPENDENCIES = libsim.a libexo/libexo.a
sim_armulator_DEPENDENCIES = libsim.a libexo/libexo.a
sim_dis_DEPENDENCIES = libsim.a libexo/libexo.a
sim_depchk_DEPENDENCIES = libsim.a libexo/libexo.a
sim_uop_DEPENDENCIES = libsim.a libexo/libexo.a
sim_panalyzer_DEPENDENCIES = libsim-power.a libexo/libexo.a libpanalyzer/libpanalyzer.a

sim_panalyzer_CFLAGS = -DVANALYZER_H -DPANALYZER_H -DLV2_PANALYZER_H
libsim_power_a_CFLAGS = -DVANALYZER_H -DPANALYZER_H -DLV2_PANALYZER_H
# -DLV2_PANALYZER_H
# -DLV1_PANALYZER_H
# -DLV1_CLOCK_PANALYZER_H
# -DLV1_IO_PANALYZER_H
# -DPA_TRACE_WINDOW_H

libexo/libexo.a:
	cd libexo && $(MAKE) libexo.a LEXT=a OEXT=$(OBJEXT) \
	       AR="$(AR)" RANLIB="$(RANLIB)" ARFLAGS="$(ARFLAGS)" \
	       CC="$(CC)" CFLAGS="$(CFLAGS)"

libcheetah/libcheetah.a:
	cd libcheetah && $(MAKE) libcheetah.a LEXT=a OEXT=$(OBJEXT) \
	       AR="$(AR)" RANLIB="$(RANLIB)" ARFLAGS="$(ARFLAGS)" \
	       CC="$(CC)" CFLAGS="$(CFLAGS)"

libpa_lv1/libpa_lv1.a:
	cd libpa_lv1 && $(MAKE) libpa_lv1.a LEXT=a OEXT=$(OBJEXT) \
	       AR="$(AR)" RANLIB="$(RANLIB)" ARFLAGS="$(ARFLAGS)" \
	       CC="$(CC)" CFLAGS="$(CFLAGS)"

libpanalyzer/libpanalyzer.a:
	cd libpanalyzer && $(MAKE) libpanalyzer.a LEXT=a OEXT=$(OBJEXT) \
	       AR="$(AR)" RANLIB="$(RANLIB)" ARFLAGS="$(ARFLAGS)" \
	       CC="$(CC)" CFLAGS="$(CFLAGS)"

sysprobe$(EXEEXT):	sysprobe.c
	$(CC) $(FFLAGS) -o sysprobe$(EXEEXT) sysprobe.c
	@echo endian probe results: `./sysprobe -s`
	@echo probe flags: $(AM_CFLAGS) $(FFLAGS)
	@echo probe libs: $(AM_LDFLAGS) $(LIBS)

sysprobe$(EXEEXT): AM_CFLAGS = 
sysprobe$(EXEEXT): AM_LDFLAGS = -lm $(BINUTILS_LIB)

sim-tests sim-tests-nt: sysprobe$(EXEEXT) $(PROGS)
	cd tests $(CS) \
	$(MAKE) "MAKE=$(MAKE)" "RM=$(RM)" "ENDIAN=`./sysprobe -s`" tests \
		"DIFF=$(DIFF)" "SIM_DIR=.." "SIM_BIN=sim-fast$(EXEEXT)" \
		"X=/" "CS=$(CS)" $(CS) \
	cd ..
	cd tests $(CS) \
	$(MAKE) "MAKE=$(MAKE)" "RM=$(RM)" "ENDIAN=`./sysprobe -s`" tests \
		"DIFF=$(DIFF)" "SIM_DIR=.." "SIM_BIN=sim-safe$(EXEEXT)" \
		"X=/" "CS=$(CS)" $(CS) \
	cd ..
	cd tests $(CS) \
	$(MAKE) "MAKE=$(MAKE)" "RM=$(RM)" "ENDIAN=`./sysprobe -s`" tests \
		"DIFF=$(DIFF)" "SIM_DIR=.." "SIM_BIN=sim-cache$(EXEEXT)" \
		"X=/" "CS=$(CS)" $(CS) \
	cd ..
	cd tests $(CS) \
	$(MAKE) "MAKE=$(MAKE)" "RM=$(RM)" "ENDIAN=`./sysprobe -s`" tests \
		"DIFF=$(DIFF)" "SIM_DIR=.." "SIM_BIN=sim-cheetah$(EXEEXT)" \
		"X=/" "CS=$(CS)" $(CS) \
	cd ..
	cd tests $(CS) \
	$(MAKE) "MAKE=$(MAKE)" "RM=$(RM)" "ENDIAN=`./sysprobe -s`" tests \
		"DIFF=$(DIFF)" "SIM_DIR=.." "SIM_BIN=sim-bpred$(EXEEXT)" \
		"X=/" "CS=$(CS)" $(CS) \
	cd ..
	cd tests $(CS) \
	$(MAKE) "MAKE=$(MAKE)" "RM=$(RM)" "ENDIAN=`./sysprobe -s`" tests \
		"DIFF=$(DIFF)" "SIM_DIR=.." "SIM_BIN=sim-profile$(EXEEXT)" \
		"X=/" "CS=$(CS)" "SIM_OPTS=-all" $(CS) \
	cd ..
	cd tests $(CS) \
	$(MAKE) "MAKE=$(MAKE)" "RM=$(RM)" "ENDIAN=`./sysprobe -s`" tests \
		"DIFF=$(DIFF)" "SIM_DIR=.." "SIM_BIN=sim-outorder$(EXEEXT)" \
		"X=/" "CS=$(CS)" $(CS) \
	cd ..
	cd tests $(CS) \
	$(MAKE) "MAKE=$(MAKE)" "RM=$(RM)" "ENDIAN=`./sysprobe -s`" tests \
		"DIFF=$(DIFF)" "SIM_DIR=.." "SIM_BIN=sim-wattch$(EXEEXT)" \
		"X=/" "CS=$(CS)" $(CS) \
	cd ..


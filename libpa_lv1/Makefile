#
# all the sources
#
SRC	= technology.c lv1_panalyzer.c lv1_cache_panalyzer.c lv1_uarch_panalyzer.c lv1_alu_panalyzer.c lv1_fpu_panalyzer.c lv1_mult_panalyzer.c lv1_rf_panalyzer.c lv1_bpred_panalyzer.c lv1_clock_panalyzer.c lv1_io_panalyzer.c
#
# common header files
#
HDR	= technology.h lv1_panalyzer.c lv1_cache_panalyzer.h lv1_uarch_panalyzer.h lv1_alu_panalyzer.h lv1_fpu_panalyzer.h lv1_mult_panalyzer.h lv1_rf_panalyzer.h lv1_bpred_panalyzer.h lv1_clock_panalyzer.h lv1_io_panalyzer.h
#
# common objects
#
OBJ	= technology.$(OEXT) lv1_panalyzer.$(OEXT) lv1_cache_panalyzer.$(OEXT) lv1_uarch_panalyzer.$(OEXT) lv1_alu_panalyzer.$(OEXT) lv1_fpu_panalyzer.$(OEXT) lv1_mult_panalyzer.$(OEXT) lv1_rf_panalyzer.$(OEXT) lv1_bpred_panalyzer.$(OEXT) lv1_clock_panalyzer.$(OEXT) lv1_io_panalyzer.$(OEXT)
	
all: libpa_lv1.$(LEXT)

libpa_lv1.$(LEXT): $(OBJ) 
	$(RM) libpa_lv1.$(LEXT) 
	$(AR) $(ARFLAGS) libpa_lv1.$(LEXT) $(OBJ) 
	$(RANLIB) libpa_lv1.$(LEXT)

.c.$(OEXT):
	$(CC) $(CFLAGS) -c $*.c 

clean:
	-$(RM) libpa_lv1.a libpa_lv1.lib *.o *.obj *.exe core *~ Makefile.bak

depend:
	makedepend.local -n -x $(SRC)

# DO NOT DELETE THIS LINE -- make depend depends on it.

libpa_lv1.$(OEXT): ../host.h ../machine.h ../misc.h ../machine.def 
libpa_lv1.$(OEXT): ../memory.h ../cache.h 
libpa_lv1.$(OEXT): technology.h lv1_panalyzer.h lv1_cache_panalyzer.h lv1_uarch_panalyzer.h lv1_alu_panalyzer.h lv1_fpu_panalyzer.h lv1_mult_panalyzer.h lv1_rf_panalyzer.h lv1_bpred_panalyzer.h lv1_clock_panalyzer.h lv1_io_panalyzer.h


#
# all the sources
#
SRC	= technology.c panalyzer.c circuit_analyzer.c clock_panalyzer.c io_panalyzer.c memory_panalyzer.c cache_panalyzer.c uarch_panalyzer.c library.c microstrip.c logic_panalyzer.c alu_panalyzer.c mult_panalyzer.c fpu_panalyzer.c
#
# common header files
#
HDR	= technology.h panalyzer.h circuit_analyzer.h clock_panalyzer.h io_panalyzer.h memory_panalyzer.h cache_panalyzer.h uarch_panalyzer.h library.h microstrip.h logic_panalyzer.h alu_panalyzer.h mult_panalyzer.h fpu_panalyzer.h
#
# common objects
#
OBJ	= technology.$(OEXT) panalyzer.$(OEXT) circuit_analyzer.$(OEXT) clock_panalyzer.$(OEXT) io_panalyzer.$(OEXT) memory_panalyzer.$(OEXT) cache_panalyzer.$(OEXT) uarch_panalyzer.$(OEXT) library.$(OEXT) microstrip.$(OEXT) logic_panalyzer.$(OEXT) alu_panalyzer.$(OEXT) mult_panalyzer.$(OEXT) fpu_panalyzer.$(OEXT)
	
all: libpanalyzer.$(LEXT)

libpanalyzer.$(LEXT): $(OBJ) 
	$(RM) libpanalyzer.$(LEXT) 
	$(AR) $(ARFLAGS) libpanalyzer.$(LEXT) $(OBJ) 
	$(RANLIB) libpanalyzer.$(LEXT)

.c.$(OEXT):
	$(CC) $(CFLAGS) -c $*.c 

clean:
	-$(RM) libpanalyzer.a libpanalyzer.lib *.o *.obj *.exe core *~ Makefile.bak

depend:
	makedepend.local -n -x $(SRC)

# DO NOT DELETE THIS LINE -- make depend depends on it.

libpanalyzer.$(OEXT): ../host.h ../machine.h ../misc.h ../machine.def 
libpanalyzer.$(OEXT): ../memory.h ../cache.h 
libpanalyzer.$(OEXT): technology.h panalyzer.h circuit_analyzer.h clock_panalyzer.h io_panalyzer.h memory_panalyzer.h cache_panalyzer.h uarch_panalyzer.h library.h microstrip.h logic_panalyzer.h alu_panalyzer.h mult_panalyzer.h fpu_panalyzer.h 

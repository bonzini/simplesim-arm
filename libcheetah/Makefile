## these are set in ../Makefile
## CC, AR, AROPT, CFLAGS, RANLIB


#
# all the sources
#
SRC     = libcheetah.c saclru.c util.c sacopt.c ppopt.c faclru.c \
	  facopt.c dmvl.c
HDR     = libcheetah.h util.h

#
# common objects
#
OBJ     = libcheetah.$(OEXT) saclru.$(OEXT) util.$(OEXT) sacopt.$(OEXT) \
	  ppopt.$(OEXT) faclru.$(OEXT) facopt.$(OEXT) dmvl.$(OEXT)


libcheetah.$(LEXT):	$(OBJ)
	$(RM) libcheetah.$(LEXT)
	$(AR) $(AROPT)libcheetah.$(LEXT) $(OBJ)
	$(RANLIB) libcheetah.$(LEXT)

.c.$(OEXT):
	$(CC) $(CFLAGS) -c $*.c

clean:
	-$(RM) libcheetah.a libcheetah.lib *.o *.obj *.exe core *~ Makefile.bak

depend:
	makedepend.local -n -x $(SRC)

test:
	$(MAKE) cheetah ascbin
	$(MAKE) /tmp/trace_file
	cheetah -Csa -Fbasic -Rlru -a5 -b14 -l4 -n2 -t1000000 < /tmp/trace_file > /tmp/test.results
	cheetah -Csa -Fbasic -Ropt -a5 -b14 -l4 -n2 -t1000000 < /tmp/trace_file >> /tmp/test.results
	cheetah -Cfa -Fbasic -Rlru -M65536 -i64 -t1000000 < /tmp/trace_file >> /tmp/test.results
	cheetah -Cfa -Fbasic -Ropt -M65536 -i64 -t1000000 < /tmp/trace_file >> /tmp/test.results
	cheetah -Cdm -Fbasic -c13 -a3 -b8 -t1000000 < /tmp/trace_file >> /tmp/test.results
	echo "The following diff should be empty"
	diff orig_results /tmp/test.results


# DO NOT DELETE THIS LINE -- make depend depends on it.

libcheetah.$(OEXT): ../host.h ../misc.h ../machine.h ../host.h ../misc.h
libcheetah.$(OEXT): ../machine.def util.h libcheetah.h
saclru.$(OEXT): ../host.h ../misc.h ../machine.h ../host.h ../misc.h
saclru.$(OEXT): ../machine.def util.h libcheetah.h
util.$(OEXT): ../host.h ../misc.h ../machine.h ../host.h ../misc.h
util.$(OEXT): ../machine.def util.h libcheetah.h
sacopt.$(OEXT): ../host.h ../misc.h ../machine.h ../host.h ../misc.h
sacopt.$(OEXT): ../machine.def util.h libcheetah.h
ppopt.$(OEXT): ../host.h ../misc.h ../machine.h ../host.h ../misc.h
ppopt.$(OEXT): ../machine.def libcheetah.h
faclru.$(OEXT): ../host.h ../misc.h ../machine.h ../host.h ../misc.h
faclru.$(OEXT): ../machine.def util.h libcheetah.h
facopt.$(OEXT): ../host.h ../misc.h ../machine.h ../host.h ../misc.h
facopt.$(OEXT): ../machine.def util.h libcheetah.h
dmvl.$(OEXT): ../host.h ../misc.h ../machine.h ../host.h ../misc.h
dmvl.$(OEXT): ../machine.def util.h libcheetah.h

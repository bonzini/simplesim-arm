#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT(simplesim, 1.0)
AC_CONFIG_SRCDIR([sim-outorder.c])
AM_INIT_AUTOMAKE

# Checks for programs.
AC_PROG_CC
AC_PROG_LN_S
AC_PROG_RANLIB
AC_PROG_MAKE_SET

AC_SEARCH_LIBS(dlsym, dl, [LIBS="$LIBS -ldl"])
AC_SEARCH_LIBS(sin, m, [LIBS="$LIBS -lm"])

AC_CONFIG_LINKS(config.h:target-arm/config.h)
AC_CONFIG_LINKS(machine.h:target-arm/arm.h)
AC_CONFIG_LINKS(machine.c:target-arm/arm.c)
AC_CONFIG_LINKS(machine.def:target-arm/arm.def)
AC_CONFIG_LINKS(loader.c:target-arm/loader.c)
AC_CONFIG_LINKS(symbol.c:target-arm/symbol.c)
AC_CONFIG_LINKS(syscall.c:target-arm/syscall.c)
dnl AC_CONFIG_LINKS(tests:tests-arm)

AC_CONFIG_FILES([Makefile])
AC_CONFIG_COMMANDS([sysprobe], [make sysprobe])
AC_OUTPUT

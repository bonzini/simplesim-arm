/*
 * endian.c - host endian probes
 *
 * This file is a part of the SimpleScalar tool suite written by
 * Todd M. Austin as a part of the Multiscalar Research Project.
 *  
 * The tool suite is currently maintained by Doug Burger and Todd M. Austin.
 * 
 * Copyright (C) 1994, 1995, 1996, 1997, 1998 by Todd M. Austin
 *
 * This source file is distributed "as is" in the hope that it will be
 * useful.  The tool set comes with no warranty, and no author or
 * distributor accepts any responsibility for the consequences of its
 * use. 
 * 
 * Everyone is granted permission to copy, modify and redistribute
 * this tool set under the following conditions:
 * 
 *    This source code is distributed for non-commercial use only. 
 *    Please contact the maintainer for restrictions applying to 
 *    commercial use.
 *
 *    Permission is granted to anyone to make or distribute copies
 *    of this source code, either as received or modified, in any
 *    medium, provided that all copyright notices, permission and
 *    nonwarranty notices are preserved, and that the distributor
 *    grants the recipient permission for further redistribution as
 *    permitted by this document.
 *
 *    Permission is granted to distribute this file in compiled
 *    or executable form under the same conditions that apply for
 *    source code, provided that either:
 *
 *    A. it is accompanied by the corresponding machine-readable
 *       source code,
 *    B. it is accompanied by a written offer, with no time limit,
 *       to give anyone a machine-readable copy of the corresponding
 *       source code in return for reimbursement of the cost of
 *       distribution.  This written offer must permit verbatim
 *       duplication by anyone, or
 *    C. it is distributed by someone who received only the
 *       executable form, and is accompanied by a copy of the
 *       written offer of source code that they received concurrently.
 *
 * In other words, you are welcome to use, share and improve this
 * source file.  You are forbidden to forbid anyone else to use, share
 * and improve what you give them.
 *
 * INTERNET: dburger@cs.wisc.edu
 * US Mail:  1210 W. Dayton Street, Madison, WI 53706
 *
 * $Id: endian.c,v 1.1.1.1 2000/11/29 14:53:54 cu-cs Exp $
 *
 * $Log: endian.c,v $
 * Revision 1.1.1.1  2000/11/29 14:53:54  cu-cs
 * Grand unification of arm sources.
 *
 *
 * Revision 1.1.1.1  2000/05/26 15:18:57  taustin
 * SimpleScalar Tool Set
 *
 *
 * Revision 1.5  1998/08/27 08:23:44  taustin
 * implemented host interface description in host.h
 *
 * Revision 1.4  1997/03/11  01:10:18  taustin
 * updated copyright
 *
 * Revision 1.3  1997/01/06  15:58:32  taustin
 * comments updated
 *
 * Revision 1.1  1996/12/05  18:52:32  taustin
 * Initial revision
 *
 *
 */

#include "endian.h"
#ifndef HOST_ONLY
#include "loader.h"
#endif

/* probe host (simulator) byte endian format */
enum endian_t
endian_host_byte_order(void)
{
  int i = 1, *p;

  p = &i;
  if (*((char *)p) == 1)
    return endian_little;
  else if (*((char *)p) == 0)
    return endian_big;
  else
    return endian_unknown;
}

/* probe host (simulator) double word endian format */
enum endian_t
endian_host_word_order(void)
{
  int *p;
  union {
    double d;
    int i[2];
  } x;

  x.d = 1.0;

  /* NOTE: this check assumes IEEE floating point format */
  if (x.i[0] == 0)
    return endian_little;
  else if (x.i[0] == 0x3ff00000)
    return endian_big;
  else
    return endian_unknown;
}

#ifndef HOST_ONLY

/* probe target (simulated program) byte endian format, only
   valid after program has been loaded */
enum endian_t
endian_target_byte_order(void)
{
  return ld_target_big_endian ? endian_big : endian_little;
}

/* probe target (simulated program) double word endian format,
   only valid after program has been loaded */
enum endian_t
endian_target_word_order(void)
{
  /* same as byte sex for SimpleScalar target */
  return endian_target_byte_order();
}

#endif /* !HOST_ONLY */

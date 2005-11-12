/*
 * ptrace.h - pipeline tracing definitions and interfaces
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
 * $Id: ptrace.h,v 1.1.1.1 2000/11/29 14:53:54 cu-cs Exp $
 *
 * $Log: ptrace.h,v $
 * Revision 1.1.1.1  2000/11/29 14:53:54  cu-cs
 * Grand unification of arm sources.
 *
 *
 * Revision 1.1.1.1.2.2  2000/11/10 15:48:37  ernstd
 * created header file for shared variables in sim-outorder.  Also added ptrace support for viewing uops.
 *
 * Revision 1.1.1.1.2.1  2000/10/13 17:45:14  chriswea
 * updated
 *
 * Revision 1.3  2000/08/10 18:22:45  chriswea
 * added ptracepipe
 *
 * Revision 1.2  2000/07/26 21:07:04  chriswea
 * added the visualization pipetrace new_stage_lat
 *
 * Revision 1.2  1998/08/27 15:50:04  taustin
 * implemented host interface description in host.h
 * added target interface support
 *
 * Revision 1.1  1997/03/11  01:32:28  taustin
 * Initial revision
 *
 *
 */

#ifndef PTRACE_H
#define PTRACE_H

#include "host.h"
#include "misc.h"
#include "machine.h"
#include "range.h"
#include "stats.h"


/*
 * pipeline events:
 *
 *	+ <iseq> <pc> <addr> <inst>	- new instruction def
 *	- <iseq>			- instruction squashed or retired
 *	@ <cycle>			- new cycle def
 *	* <iseq> <stage> <events>	- instruction stage transition
 *
 */

/*
	[IF]   [DA]   [EX]   [WB]   [CT]
         aa     dd     jj     ll     nn
         bb     ee     kk     mm     oo
         cc                          pp
 */

/* pipeline stages */
#define PST_IFETCH		"IF"
#define PST_DISPATCH		"DA"
#define PST_EXECUTE		"EX"
#define PST_WRITEBACK		"WB"
#define PST_COMMIT		"CT"

/* pipeline events */
#define PEV_CACHEMISS		0x00000001	/* I/D-cache miss */
#define PEV_TLBMISS		0x00000002	/* I/D-tlb miss */
#define PEV_MPOCCURED		0x00000004	/* mis-pred branch occurred */
#define PEV_MPDETECT		0x00000008	/* mis-pred branch detected */
#define PEV_AGEN		0x00000010	/* address generation */

/* pipetrace file */
extern FILE *ptrace_outfd;

/* pipetrace pipe */
extern int ptracepipe;

/* pipetracing is active */
extern int ptrace_active;

/* pipetracing range */
extern struct range_range_t ptrace_range;

/* one-shot switch for pipetracing */
extern int ptrace_oneshot;

/* open pipeline trace */
void
ptrace_open(char *range,		/* trace range */
	    char *fname);		/* output filename */

/* close pipeline trace */
void
ptrace_close(void);

/* NOTE: pipetracing is a one-shot switch, since turning on a trace more than
   once will mess up the pipetrace viewer */
#define ptrace_check_active(PC, ICNT, CYCLE)				\
  ((ptrace_outfd != NULL						\
    && !range_cmp_range1(&ptrace_range, (PC), (ICNT), (CYCLE)))		\
   ? (!ptrace_oneshot ? (ptrace_active = ptrace_oneshot = TRUE) : FALSE)\
   : (ptrace_active = FALSE))

/* main interfaces, with fast checks */
#define ptrace_newinst(A,B,C,D)						\
  if (ptrace_active) __ptrace_newinst((A),(B),(C),(D))
#define ptrace_newuop(A,B,C,D,E)					\
  if (ptrace_active) __ptrace_newuop((A),(B),(C),(D),(E))
#define ptrace_endinst(A)						\
  if (ptrace_active) __ptrace_endinst((A))
#define ptrace_newcycle(A)						\
  if (ptrace_active) __ptrace_newcycle((A))
#define ptrace_newstage(A,B,C,D,E)						\
  if (ptrace_active) __ptrace_newstage_lat((A),(B),(C),(D),(E))

#define ptrace_active(A,I,C)						\
  (ptrace_outfd != NULL	&& !range_cmp_range(&ptrace_range, (A), (I), (C)))

/* declare a new instruction */
void
__ptrace_newinst(unsigned int iseq,	/* instruction sequence number */
		 md_inst_t inst,	/* new instruction */
		 md_addr_t pc,		/* program counter of instruction */
		 md_addr_t addr);	/* address referenced, if load/store */

/* declare a new uop */
void
__ptrace_newuop(unsigned int iseq,	/* instruction sequence number */
		md_inst_t inst,		/* new uop description */
		enum md_opcode op,
		md_addr_t pc,		/* program counter of instruction */
		md_addr_t addr);	/* address referenced, if load/store */

/* declare instruction retirement or squash */
void
__ptrace_endinst(unsigned int iseq);	/* instruction sequence number */

/* declare a new cycle */
void
__ptrace_newcycle(tick_t cycle);	/* new cycle */

/* indicate instruction transition to a new pipeline stage */
void
__ptrace_newstage(unsigned int iseq,	/* instruction sequence number */
		  char *pstage,		/* pipeline stage entered */
		  unsigned int pevents);/* pipeline events while in stage */

void
__ptrace_newstage_lat(unsigned int iseq,	/* instruction sequence number */
		  char *pstage,		/* pipeline stage entered */
		  unsigned int pevents, unsigned int latency, unsigned int longerlat);/* pipeline events while in stage */

/* print the value of stat variable STAT */
void
ptrace_print_stat(struct stat_sdb_t *sdb,	/* stat database */
		struct stat_stat_t *stat,/* stat variable */
		FILE *fd);		/* output stream */

/* print the value of all stat variables in stat database SDB */
void
ptrace_print_stats(struct stat_sdb_t *sdb,/* stat database */
		 FILE *fd);		/* output stream */




#endif /* PTRACE_H */

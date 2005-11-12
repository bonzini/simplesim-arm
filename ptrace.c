/*
 * ptrace.c - pipeline tracing routines
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
 * $Id: ptrace.c,v 1.1.1.1 2000/11/29 14:53:54 cu-cs Exp $
 *
 * $Log: ptrace.c,v $
 * Revision 1.1.1.1  2000/11/29 14:53:54  cu-cs
 * Grand unification of arm sources.
 *
 *
 * Revision 1.1.1.1.2.2  2000/11/10 15:48:37  ernstd
 * created header file for shared variables in sim-outorder.  Also added ptrace support for viewing uops.
 *
 * Revision 1.1.1.1.2.1  2000/10/13 17:45:04  chriswea
 * updated
 *
 * Revision 1.3  2000/08/10 18:22:33  chriswea
 * added ptracepipe
 *
 * Revision 1.2  2000/07/26 21:06:35  chriswea
 * added the new visualization pipetraces
 *
 * Revision 1.2  1998/08/27 15:49:17  taustin
 * implemented host interface description in host.h
 * added target interface support
 * using new target-dependent myprintf() package
 *
 * Revision 1.1  1997/03/11  01:32:15  taustin
 * Initial revision
 *
 *
 */

#include <stdio.h>
#include <stdlib.h>

#include "host.h"
#include "misc.h"
#include "machine.h"
#include "range.h"
#include "ptrace.h"
#include <fcntl.h>
#include <unistd.h>
#include <sys/file.h>

/* pipetrace file */
FILE *ptrace_outfd = NULL;

/* pipetracing is active */
int ptrace_active = FALSE;

/* file pipe to communicate with gpv */
int ptracepipe;

/* pipetracing range */
struct range_range_t ptrace_range;

/* one-shot switch for pipetracing */
int ptrace_oneshot = FALSE;

char buftest[128] ="Does this work??"; 

struct flock filelockinfo;
/* open pipeline trace */
void
ptrace_open(char *fname,		/* output filename */
	    char *range)		/* trace range */
{
  char *errstr;

  /* parse the output range */
  if (!range)
    {
      /* no range */
      errstr = range_parse_range(":", &ptrace_range);
      if (errstr)
	panic("cannot parse pipetrace range, use: {<start>}:{<end>}");
      ptrace_active = TRUE;
    }
  else
    {
      errstr = range_parse_range(range, &ptrace_range);
      if (errstr)
	fatal("cannot parse pipetrace range, use: {<start>}:{<end>}");
      ptrace_active = FALSE;
    }

  if (ptrace_range.start.ptype != ptrace_range.end.ptype)
    fatal("range endpoints are not of the same type");

  /* open output trace file */
  //else we are setting up a pipe to the gpv
  if(ptracepipe != 0)
    { 

      //open another FILE pointer to stdio that we do not redirect
      ptrace_outfd = fdopen (1,"w");

     if (!ptrace_outfd)
     	fatal("cannot open pipe for output %d", ptrace_outfd);

     }     

  else if (!fname || !strcmp(fname, "-") || !strcmp(fname, "stderr"))
  {    
        ptrace_outfd = stderr;
  }
  else if (!strcmp(fname, "stdout"))
    ptrace_outfd = stdout;

  else
    {
      ptrace_outfd = fopen(fname, "w");
      if (!ptrace_outfd)
	fatal("cannot open pipetrace output file `%s'", fname);
    }
}

/* close pipeline trace */
void
ptrace_close(void)
{
    fprintf(ptrace_outfd, "<END VISUAL>\n");
  if (ptrace_outfd != NULL && ptrace_outfd != stderr && ptrace_outfd != stdout)
    fclose(ptrace_outfd);
}

/* declare a new instruction */
void
__ptrace_newinst(unsigned int iseq,	/* instruction sequence number */
		 md_inst_t inst,	/* new instruction */
		 md_addr_t pc,		/* program counter of instruction */
		 md_addr_t addr)	/* address referenced, if load/store */
{
  myfprintf(ptrace_outfd, "+ %u 0x%08p 0x%08p ", iseq, pc, addr);
  md_print_insn(inst, addr, ptrace_outfd);
  fprintf(ptrace_outfd, "\n");

  if (ptrace_outfd == stderr || ptrace_outfd == stdout)
    fflush(ptrace_outfd);
}

/* declare a new uop */
void
__ptrace_newuop(unsigned int iseq,	/* instruction sequence number */
		md_inst_t inst,		/* new uop description */
		enum md_opcode op,
		md_addr_t pc,		/* program counter of instruction */
		md_addr_t addr)		/* address referenced, if load/store */
{
  myfprintf(ptrace_outfd,
	    "+ %u 0x%08p 0x%08p ", iseq, pc, addr);
  md_print_uop(op,inst,pc,ptrace_outfd);
  fprintf(ptrace_outfd, "\n");

  if (ptrace_outfd == stderr || ptrace_outfd == stdout)
    fflush(ptrace_outfd);
}

/* declare instruction retirement or squash */
void
__ptrace_endinst(unsigned int iseq)	/* instruction sequence number */
{
  fprintf(ptrace_outfd, "- %u\n", iseq);

  if (ptrace_outfd == stderr || ptrace_outfd == stdout)
    fflush(ptrace_outfd);
}

/* declare a new cycle */
void
__ptrace_newcycle(tick_t cycle)		/* new cycle */
{
  fprintf(ptrace_outfd, "@ %.0f\n", (double)cycle);

  if (ptrace_outfd == stderr || ptrace_outfd == stdout)
    fflush(ptrace_outfd);
}

/* indicate instruction transition to a new pipeline stage */
void
__ptrace_newstage(unsigned int iseq,	/* instruction sequence number */
		  char *pstage,		/* pipeline stage entered */
		  unsigned int pevents)/* pipeline events while in stage */
{
  fprintf(ptrace_outfd, "* %u %s 0x%08x\n", iseq, pstage, pevents);

  if (ptrace_outfd == stderr || ptrace_outfd == stdout)
    fflush(ptrace_outfd);
}

/* indicate instruction transition to a new pipeline stage */
void
__ptrace_newstage_lat(unsigned int iseq,	/* instruction sequence number */
		  char *pstage,		                /* pipeline stage entered */
		  unsigned int pevents,             /* pipeline events while in stage */
		  unsigned int latency,             /* latency of miss if applicable */
		  unsigned int longerlat)          /* which event was longer? */ 
{
  unsigned int my_latency;
  my_latency=pevents ? latency : 0;
  
  fprintf(ptrace_outfd, "* %u %s 0x%03x %d 0x%03x\n", iseq, pstage, pevents, my_latency, longerlat);

  if (ptrace_outfd == stderr || ptrace_outfd == stdout)
    fflush(ptrace_outfd);
}


/* print the value of stat variable STAT */
void
ptrace_print_stat(struct stat_sdb_t *sdb,	/* stat database */
		struct stat_stat_t *stat,/* stat variable */
		FILE *fd)		/* output stream */
{
  struct eval_value_t val;

  switch (stat->sc)
    {
    case sc_int:
      fprintf(fd, "%s>", stat->name);
      myfprintf(fd, stat->format, *stat->variant.for_int.var);
      //fprintf(fd, " # %s", stat->desc);
      break;
    case sc_uint:
      fprintf(fd, "%s>", stat->name);
      myfprintf(fd, stat->format, *stat->variant.for_uint.var);
      //fprintf(fd, " # %s", stat->desc);
      break;
#ifdef HOST_HAS_QWORD
    case sc_qword:
      {
	char buf[128];

	fprintf(fd, "%s>", stat->name);
	mysprintf(buf, stat->format, *stat->variant.for_qword.var);
	fprintf(fd, "%s", buf);
      }
      break;
    case sc_sqword:
      {
	char buf[128];

	fprintf(fd, "%s>", stat->name);
	mysprintf(buf, stat->format, *stat->variant.for_sqword.var);
	fprintf(fd, "%s", buf);
      }
      break;
#endif /* HOST_HAS_QWORD */
    case sc_float:
      fprintf(fd, "%s>", stat->name);
      myfprintf(fd, stat->format, (double)*stat->variant.for_float.var);
      //fprintf(fd, " # %s", stat->desc);
      break;
    case sc_double:
      fprintf(fd, "%s>", stat->name);
      myfprintf(fd, stat->format, *stat->variant.for_double.var);
      //fprintf(fd, " # %s", stat->desc);
      break;
    case sc_dist:
      print_dist(stat, fd);
      break;
    case sc_sdist:
      print_sdist(stat, fd);
      break;
    case sc_formula:
      {
	/* instantiate a new evaluator to avoid recursion problems */
	struct eval_state_t *es = eval_new(stat_eval_ident, sdb);
	char *endp;

	fprintf(fd, "%s>", stat->name);
	val = eval_expr(es, stat->variant.for_formula.formula, &endp);
	if (eval_error != ERR_NOERR || *endp != '\0')
	  fprintf(fd, "<error: %s>", eval_err_str[eval_error]);
	else
	  myfprintf(fd, stat->format, eval_as_double(val));
	//fprintf(fd, " # %s", stat->desc);

	/* done with the evaluator */
	eval_delete(es);
      }
      break;
    default:
      panic("bogus stat class");
    }
  fprintf(fd, "\n");
}

/* print the value of all stat variables in stat database SDB */
void
ptrace_print_stats(struct stat_sdb_t *sdb,/* stat database */
		 FILE *fd)		/* output stream */
{
  struct stat_stat_t *stat;

  if (!sdb)
    {
      /* no stats */
      return;
    }

  for (stat=sdb->stats; stat != NULL; stat=stat->next)
    {
      fprintf(fd,"<");
      ptrace_print_stat(sdb, stat, fd);

    }
}

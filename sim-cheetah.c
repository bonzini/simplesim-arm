/*
 * sim-cheetah.c - single-pass multiple-configuration cache simulator
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
 * $Id: sim-cheetah.c,v 1.1.1.1 2000/11/29 14:53:54 cu-cs Exp $
 *
 * $Log: sim-cheetah.c,v $
 * Revision 1.1.1.1  2000/11/29 14:53:54  cu-cs
 * Grand unification of arm sources.
 *
 *
 * Revision 1.1.1.1.2.1  2000/11/04 05:30:22  taustin
 * Cleaned up the code.
 *
 * Revision 1.1.1.1  2000/05/26 15:18:59  taustin
 * SimpleScalar Tool Set
 *
 *
 * Revision 1.4  1999/12/31 18:47:18  taustin
 * quad_t naming conflicts removed
 *
 * Revision 1.3  1999/12/13 18:45:58  taustin
 * cross endian execution support added
 *
 * Revision 1.2  1998/08/27 15:58:37  taustin
 * implemented host interface description in host.h
 * added target interface support
 * added support for register and memory contexts
 * added option ("-max:inst") to limit number of instructions analyzed
 * instruction predecoding moved to loader module
 * Alpha target support added
 * added support for qword's
 * -max:inst's limited to 2147483647 for sim-cheetah due to integer
 *       overflow problems in libcheetah (to be fixed...)
 * "none" option added to disable libcheetah analysis
 * added fault support
 *
 * Revision 1.1  1997/03/11  01:33:09  taustin
 * Initial revision
 *
 * Revision 1.3  1996/12/27  15:54:04  taustin
 * updated comments
 * integrated support for options and stats packages
 * added sim_init() code
 *
 * Revision 1.1  1996/12/05  18:52:32  taustin
 * Initial revision
 *
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "host.h"
#include "misc.h"
#include "machine.h"
#include "regs.h"
#include "memory.h"
#include "loader.h"
#include "syscall.h"
#include "dlite.h"
#include "options.h"
#include "stats.h"
#include "libcheetah/libcheetah.h"
#include "sim.h"

/*
 * This file implements a functional simulator driver for Cheetah.  Cheetah
 * is a cache simulation package written by Rabin Sugumar and Santosh Abraham
 * which can efficiently simulate multiple cache configurations in a single
 * run of a program.  Specifically, Cheetah can simulate ranges of single
 * level set-associative and fully-associative caches.  See the directory
 * libcheetah/ for more details on Cheetah.
 */

/* FIXME: libcheetah bugs out after 2^31 instructions */
#define LIBCHEETAH_MAX_INST	2147483647

/* simulated registers */
static struct regs_t regs;

/* simulated memory */
static struct mem_t *mem = NULL;

/* track number of UOPs executed */
static counter_t sim_num_uops = 0;

/* track number of insn and refs */
static counter_t sim_num_refs = 0;

/* maximum number of inst's to execute */
static unsigned int max_insts;

/* replacement policy, i.e., lru or opt */
static char *repl_str;

/* cache configuration, i.e., fa, sa, or dm */
static char *conf_str;

/* minimum number of sets to analyze (log base 2) */
static int min_sets;

/* minimum number of sets to analyze (log base 2) */
static int max_sets;

/* line size of the caches (log base 2) */
static int line_size;

/* max degree of associativity to analyze (log base 2) */
static int max_assoc;

/* cache size intervals at which miss ratio is shown */
static int cache_interval;

/* maximum cache size of interest */
static int max_cache;

/* size of cache (log base 2) for DM analysis */
static int cache_size;

/* reference stream to analyze, i.e., {inst|data|unified} */
static char *ref_stream;

/* reference stream to analyze */
#define REFS_INST		0x01
#define REFS_DATA		0x02
static int refs;

/* register simulator-specific options */
void
sim_reg_options(struct opt_odb_t *odb)
{
  opt_reg_header(odb, 
"sim-cheetah: This program implements a functional simulator driver for\n"
"Cheetah.  Cheetah is a cache simulation package written by Rabin Sugumar\n"
"and Santosh Abraham which can efficiently simulate multiple cache\n"
"configurations in a single run of a program.  Specifically, Cheetah can\n"
"simulate ranges of single level set-associative and fully-associative\n"
"caches.  See the directory libcheetah/ for more details on Cheetah.\n"
		 );

  /* instruction limit */
  opt_reg_uint(odb, "-max:inst", "maximum number of inst's to execute",
	       &max_insts, /* default */LIBCHEETAH_MAX_INST,
	       /* print */TRUE, /* format */NULL);

  opt_reg_string(odb, "-refs",
		 "reference stream to analyze, i.e., {none|inst|data|unified}",
		 &ref_stream, "data", /* print */TRUE, /* format */NULL);

  opt_reg_string(odb, "-R", "replacement policy, i.e., lru or opt",
		 &repl_str, "lru", /* print */TRUE, /* format */NULL);

  opt_reg_string(odb, "-C", "cache configuration, i.e., fa, sa, or dm",
		 &conf_str, "sa", /* print */TRUE, /* format */NULL);

  opt_reg_int(odb, "-a", "min number of sets (log base 2, line size for DM)",
	      &min_sets, 7, /* print */TRUE, /* format */NULL);

  opt_reg_int(odb, "-b", "max number of sets (log base 2, line size for DM)",
	      &max_sets, 14, /* print */TRUE, /* format */NULL);

  opt_reg_int(odb, "-l", "line size of the caches (log base 2)",
	      &line_size, 4, /* print */TRUE, /* format */NULL);

  opt_reg_int(odb, "-n", "max degree of associativity to analyze (log base 2)",
	      &max_assoc, 1, /* print */TRUE, /* format */NULL);

  opt_reg_int(odb, "-in", "cache size intervals at which miss ratio is shown",
	      &cache_interval, 512, /* print */TRUE, /* format */NULL);

  opt_reg_int(odb, "-M", "maximum cache size of interest",
	      &max_cache, 524288, /* print */TRUE, /* format */NULL);

  opt_reg_int(odb, "-c", "size of cache (log base 2) for DM analysis",
	      &cache_size, 16, /* print */TRUE, /* format */NULL);
}

/* libcheetah argument count and string vector */
static int lib_argc = 0;
static char *lib_argv[16];

/* check simulator-specific option values */
void
sim_check_options(struct opt_odb_t *odb, int argc, char **argv)
{
  char buf[512];

  fprintf(stderr,
	  "Portions Copyright (C) 1989-1993 by "
	  "Rabin A. Sugumar and Santosh G. Abraham.\n");

  if (max_insts == 0 || max_insts > LIBCHEETAH_MAX_INST)
    {
      warn("sim-cheetah can only process %d instructions",
	   LIBCHEETAH_MAX_INST);
      max_insts = LIBCHEETAH_MAX_INST;
    }

  if (!strcmp(ref_stream, "none"))
    refs = 0;
  else if (!strcmp(ref_stream, "inst"))
    refs = REFS_INST;
  else if (!strcmp(ref_stream, "data"))
    refs = REFS_DATA;
  else if (!strcmp(ref_stream, "unified"))
    refs = (REFS_INST|REFS_DATA);
  else
    fatal("bad reference stream specifier, use {inst|data|unified}");

  /* marshall up the libcheetah arguments */
  lib_argc = 0;

  sprintf(buf, "-R%s", repl_str);
  lib_argv[lib_argc++] = mystrdup(buf);

  sprintf(buf, "-C%s", conf_str);
  lib_argv[lib_argc++] = mystrdup(buf);

  sprintf(buf, "-a%d", min_sets);
  lib_argv[lib_argc++] = mystrdup(buf);

  sprintf(buf, "-b%d", max_sets);
  lib_argv[lib_argc++] = mystrdup(buf);

  sprintf(buf, "-l%d", line_size);
  lib_argv[lib_argc++] = mystrdup(buf);

  sprintf(buf, "-n%d", max_assoc);
  lib_argv[lib_argc++] = mystrdup(buf);

  sprintf(buf, "-i%d", cache_interval);
  lib_argv[lib_argc++] = mystrdup(buf);

  sprintf(buf, "-M%d", max_cache);
  lib_argv[lib_argc++] = mystrdup(buf);

  sprintf(buf, "-c%d", cache_size);
  lib_argv[lib_argc++] = mystrdup(buf);
}

/* register simulator-specific statistics */
void
sim_reg_stats(struct stat_sdb_t *sdb)
{
  stat_reg_counter(sdb, "sim_num_insn",
		   "total number of instructions executed",
		   &sim_num_insn, sim_num_insn, NULL);
  stat_reg_counter(sdb, "sim_num_uops",
		   "total number of UOPs executed",
		   &sim_num_uops, sim_num_uops, NULL);
  stat_reg_formula(sdb, "sim_avg_flowlen",
		   "uops per instruction",
		   "sim_num_uops / sim_num_insn", NULL);
  stat_reg_counter(sdb, "sim_num_refs",
		   "total number of loads and stores executed",
		   &sim_num_refs, 0, NULL);
  stat_reg_int(sdb, "sim_elapsed_time",
	       "total simulation time in seconds",
	       (int *)&sim_elapsed_time, 0, NULL);
  stat_reg_formula(sdb, "sim_inst_rate",
		   "simulation speed (in insts/sec)",
		   "sim_num_insn / sim_elapsed_time", NULL);
}

/* initialize the simulator */
void
sim_init(void)
{
  sim_num_refs = 0;

  /* allocate and initialize register file */
  regs_init(&regs);

  /* allocate and initialize memory space */
  mem = mem_create("mem");
  mem_init(mem);
}

/* local machine state accessor */
static char *					/* err str, NULL for no err */
cheetah_mstate_obj(FILE *stream,		/* output stream */
		   char *cmd,			/* optional command string */
		   struct regs_t *regs,		/* registers to access */
		   struct mem_t *mem)		/* memory to access */
{
  sim_end_time = time((time_t *)NULL);
  sim_elapsed_time = MAX(sim_end_time - sim_start_time, 1);

  /* print simulation stats */
  fprintf(stream, "\nsim: ** simulation statistics **\n");
  stat_print_stats(sim_sdb, stream);

  /* print libcheetah stats */
  cheetah_stats(stream, /* mid */TRUE);

  /* no error */
  return NULL;
}

/* load program into simulated state */
void
sim_load_prog(char *fname,		/* program to load */
	      int argc, char **argv,	/* program arguments */
	      char **envp)		/* program environment */
{
  /* load program text and data, set up environment, memory, and regs */
  ld_load_prog(fname, argc, argv, envp, &regs, mem, TRUE);

  /* initialize the DLite debugger */
  dlite_init(md_reg_obj, dlite_mem_obj, cheetah_mstate_obj);

  /* initialize libcheetah */
  cheetah_init(lib_argc, lib_argv);
}

/* print simulator-specific configuration information */
void
sim_aux_config(FILE *stream)		/* output stream */
{
  /* print libcheetah configuration */
  cheetah_config(stream);
}

/* dump simulator-specific auxiliary simulator statistics */
void
sim_aux_stats(FILE *stream)		/* output stream */
{
  /* print libcheetah stats */
  cheetah_stats(stream, /* final */FALSE);
}

/* un-initialize simulator-specific state */
void
sim_uninit(void)
{
  /* nada */
}


/*
 * configure the execution engine
 */

/*
 * precise architected register accessors
 */

/* next program counter */
#define SET_NPC(EXPR)		(regs.regs_NPC = (EXPR))

/* current program counter */
#define CPC			(regs.regs_PC)

/* general purpose registers */
#define GPR(N)			(regs.regs_R[N])
#define SET_GPR(N,EXPR)							\
  ((void)(((N) == 15) ? setPC++ : 0), regs.regs_R[N] = (EXPR))

#if defined(TARGET_PISA)

/* floating point registers, L->word, F->single-prec, D->double-prec */
#define FPR_L(N)		(regs.regs_F.l[(N)])
#define SET_FPR_L(N,EXPR)	(regs.regs_F.l[(N)] = (EXPR))
#define FPR_F(N)		(regs.regs_F.f[(N)])
#define SET_FPR_F(N,EXPR)	(regs.regs_F.f[(N)] = (EXPR))
#define FPR_D(N)		(regs.regs_F.d[(N) >> 1])
#define SET_FPR_D(N,EXPR)	(regs.regs_F.d[(N) >> 1] = (EXPR))

/* miscellaneous register accessors */
#define SET_HI(EXPR)		(regs.regs_C.hi = (EXPR))
#define HI			(regs.regs_C.hi)
#define SET_LO(EXPR)		(regs.regs_C.lo = (EXPR))
#define LO			(regs.regs_C.lo)
#define FCC			(regs.regs_C.fcc)
#define SET_FCC(EXPR)		(regs.regs_C.fcc = (EXPR))

#elif defined(TARGET_ALPHA)

/* floating point registers, L->word, F->single-prec, D->double-prec */
#define FPR_Q(N)		(regs.regs_F.q[N])
#define SET_FPR_Q(N,EXPR)	(regs.regs_F.q[N] = (EXPR))
#define FPR(N)			(regs.regs_F.d[N])
#define SET_FPR(N,EXPR)		(regs.regs_F.d[N] = (EXPR))

/* miscellaneous register accessors */
#define FPCR			(regs.regs_C.fpcr)
#define SET_FPCR(EXPR)		(regs.regs_C.fpcr = (EXPR))
#define UNIQ			(regs.regs_C.uniq)
#define SET_UNIQ(EXPR)		(regs.regs_C.uniq = (EXPR))

#elif defined(TARGET_ARM)

/* processor status register */
#define PSR			(regs.regs_C.cpsr)
#define SET_PSR(EXPR)		(regs.regs_C.cpsr = (EXPR))

#define PSR_N			_PSR_N(regs.regs_C.cpsr)
#define SET_PSR_N(EXPR)		_SET_PSR_N(regs.regs_C.cpsr, (EXPR))
#define PSR_C			_PSR_C(regs.regs_C.cpsr)
#define SET_PSR_C(EXPR)		_SET_PSR_C(regs.regs_C.cpsr, (EXPR))
#define PSR_Z			_PSR_Z(regs.regs_C.cpsr)
#define SET_PSR_Z(EXPR)		_SET_PSR_Z(regs.regs_C.cpsr, (EXPR))
#define PSR_V			_PSR_V(regs.regs_C.cpsr)
#define SET_PSR_V(EXPR)		_SET_PSR_V(regs.regs_C.cpsr, (EXPR))

/* floating point conversions */
union x { float f; word_t w; };
#define DTOW(D)		({ union x fw; fw.f = (float)(D); fw.w; })
#define WTOD(W)		({ union x fw; fw.w = (W); (double)fw.f; })
#define QSWP(Q)								\
  ((((Q) << 32) & ULL(0xffffffff00000000)) | (((Q) >> 32) & ULL(0xffffffff)))

/* floating point registers, L->word, F->single-prec, D->double-prec */
#define FPR_Q(N)		(QSWP(regs.regs_F.q[N]))
#define SET_FPR_Q(N,EXPR)	(regs.regs_F.q[N] = QSWP((EXPR)))
#define FPR_W(N)		(DTOW(regs.regs_F.d[N]))
#define SET_FPR_W(N,EXPR)	(regs.regs_F.d[N] = (WTOD(EXPR)))
#define FPR(N)			(regs.regs_F.d[(N)])
#define SET_FPR(N,EXPR)		(regs.regs_F.d[(N)] = (EXPR))

/* miscellaneous register accessors */
#define FPSR			(regs.regs_C.fpsr)
#define SET_FPSR(EXPR)		(regs.regs_C.fpsr = (EXPR))

#else
#error No ISA target defined...
#endif

/* precise architected memory state accessor macros */
#define READ_BYTE(SRC, FAULT)						\
  (addr = (SRC),							\
   ((refs & REFS_DATA) ? cheetah_access(addr) : (void)0),		\
   (FAULT) = md_fault_none, MEM_READ_BYTE(mem, addr))
#define READ_HALF(SRC, FAULT)						\
  (addr = (SRC),							\
   ((refs & REFS_DATA) ? cheetah_access(addr) : (void)0),		\
   (FAULT) = md_fault_none, MEM_READ_HALF(mem, addr))
#define READ_WORD(SRC, FAULT)						\
  (addr = (SRC),							\
   ((refs & REFS_DATA) ? cheetah_access(addr) : (void)0),		\
   (FAULT) = md_fault_none, MEM_READ_WORD(mem, addr))
#ifdef HOST_HAS_QWORD
#define READ_QWORD(SRC, FAULT)						\
  (addr = (SRC),							\
   ((refs & REFS_DATA) ? cheetah_access(addr) : (void)0),		\
   (FAULT) = md_fault_none, MEM_READ_QWORD(mem, addr))
#endif /* HOST_HAS_QWORD */

#define WRITE_BYTE(SRC, DST, FAULT)					\
  (addr = (DST),							\
   ((refs & REFS_DATA) ? cheetah_access(addr) : (void)0),		\
   (FAULT) = md_fault_none, MEM_WRITE_BYTE(mem, addr, (SRC)))
#define WRITE_HALF(SRC, DST, FAULT)					\
  (addr = (DST),							\
   ((refs & REFS_DATA) ? cheetah_access(addr) : (void)0),		\
   (FAULT) = md_fault_none, MEM_WRITE_HALF(mem, addr, (SRC)))
#define WRITE_WORD(SRC, DST, FAULT)					\
  (addr = (DST),							\
   ((refs & REFS_DATA) ? cheetah_access(addr) : (void)0),		\
   (FAULT) = md_fault_none, MEM_WRITE_WORD(mem, addr, (SRC)))
#ifdef HOST_HAS_QWORD
#define WRITE_QWORD(SRC, DST, FAULT)					\
  (addr = (DST),							\
   ((refs & REFS_DATA) ? cheetah_access(addr) : (void)0),		\
   (FAULT) = md_fault_none, MEM_WRITE_QWORD(mem, addr, (SRC)))
#endif /* HOST_HAS_QWORD */

/* system call memory access function */
enum md_fault_type
cheetah_access_fn(struct mem_t *mem,	/* memory space to access */
		  enum mem_cmd cmd,	/* memory access cmd, Read or Write */
		  md_addr_t addr,	/* data address to access */
		  void *p,		/* data input/output buffer */
		  int nbytes)		/* number of bytes to access */
{
  if (refs & REFS_DATA)
    cheetah_access(addr);
  return mem_access(mem, cmd, addr, p, nbytes);
}

/* system call handler macro */
#define SYSCALL(INST)	sys_syscall(&regs, cheetah_access_fn, mem, INST, TRUE)

/* start simulation, program loaded, processor precise state initialized */
void
sim_main(void)
{
  md_inst_t inst;
  register md_addr_t addr;
  enum md_opcode op;
  register int is_write;
  enum md_fault_type fault;
  int setPC, in_flow = FALSE, flow_index, nflow;
  struct md_uop_t flowtab[MD_MAX_FLOWLEN];

  fprintf(stderr, "sim: ** starting functional simulation **\n");

  /* set up initial default next PC */
  regs.regs_NPC = regs.regs_PC + sizeof(md_inst_t);

  /* synchronize register files... */
  regs.regs_R[MD_REG_PC] = regs.regs_PC;

  /* check for DLite debugger entry condition */
  if (dlite_check_break(regs.regs_PC, /* no access */0, /* addr */0, 0, 0))
    dlite_main(regs.regs_PC - sizeof(md_inst_t), regs.regs_PC,
	       sim_num_insn, &regs, mem);

  while (TRUE)
    {
      /* maintain $r0 semantics */
#ifndef TARGET_ARM
      regs.regs_R[MD_REG_ZERO] = 0;
#endif
#ifdef TARGET_ALPHA
      regs.regs_F.d[MD_REG_ZERO] = 0.0;
#endif /* TARGET_ALPHA */

      /* set default reference address and access mode */
      addr = 0; is_write = FALSE;

      /* set default fault - none */
      fault = md_fault_none;

      if (!in_flow)
	{
	  /* keep an instruction count */
	  sim_num_insn++;

	  /* get the next instruction to execute */
	  if (refs & REFS_INST)
	    cheetah_access(regs.regs_PC);
	  MD_FETCH_INST(inst, mem, regs.regs_PC);

	  /* decode the instruction */
	  MD_SET_OPCODE(op, inst);

	  if (MD_OP_FLAGS(op) & F_CISC)
	    {
	      /* get instruction flow */
	      nflow = md_get_flow(op, inst, flowtab);
	      if (nflow > 0)
		{
		  in_flow = TRUE;
		  flow_index = 0;
		}
	      else
		fatal("could not locate CISC flow");
	      sim_num_uops += nflow;
	    }
	  else
	    sim_num_uops++;
	}
      if (in_flow)
	{
	  op = flowtab[flow_index].op;
	  inst = flowtab[flow_index++].inst;
	  if (flow_index == nflow)
	    in_flow = FALSE;
	}

      if (op == NA)
	panic("bogus opcode detected @ 0x%08p", regs.regs_PC);
      if (MD_OP_FLAGS(op) & F_CISC)
	panic("CISC opcode decoded");

      setPC = 0;
      regs.regs_R[MD_REG_PC] = regs.regs_PC;

      /* execute the instruction */
      switch (op)
	{
#define DEFINST(OP,MSK,NAME,OPFORM,RES,FLAGS,O1,O2,O3,I1,I2,I3,I4)	\
	case OP:							\
          SYMCAT(OP,_IMPL);						\
          break;
#define DEFUOP(OP,NAME,OPFORM,RES,FLAGS,O1,O2,O3,I1,I2,I3,I4)		\
	case OP:							\
          SYMCAT(OP,_IMPL);						\
          break;
#define DEFLINK(OP,MSK,NAME,MASK,SHIFT)					\
        case OP:							\
          panic("attempted to execute a linking opcode");
#define CONNECT(OP)
#define DECLARE_FAULT(FAULT)						\
	  { fault = (FAULT); break; }
#include "machine.def"
	default:
	  panic("attempted to execute a bogus opcode");
      }

      if (fault != md_fault_none)
	fatal("fault (%d) detected @ 0x%08p", fault, regs.regs_PC);

      if (setPC != 0/* regs.regs_R[MD_REG_PC] != regs.regs_PC */)
	regs.regs_NPC = regs.regs_R[MD_REG_PC];

      if (MD_OP_FLAGS(op) & F_MEM)
	{
	  sim_num_refs++;
	  if (MD_OP_FLAGS(op) & F_STORE)
	    is_write = TRUE;
	}

      /* check for DLite debugger entry condition */
      if (dlite_check_break(regs.regs_NPC,
			    is_write ? ACCESS_WRITE : ACCESS_READ,
			    addr, sim_num_insn, sim_num_insn))
	dlite_main(regs.regs_PC, regs.regs_NPC, sim_num_insn, &regs, mem);

      /* go to the next instruction */
      if (!in_flow)
	{
	  regs.regs_PC = regs.regs_NPC;
	  regs.regs_NPC += sizeof(md_inst_t);
	}

      /* finish early? */
      if (max_insts && sim_num_insn >= max_insts)
	return;
    }
}

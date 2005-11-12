/*
 * sim-armulator.c - reference ARM functional simulator implementation
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
 * $Id: sim-armulator.c,v 1.1.1.1 2000/11/29 14:53:54 cu-cs Exp $
 *
 * $Log: sim-armulator.c,v $
 * Revision 1.1.1.1  2000/11/29 14:53:54  cu-cs
 * Grand unification of arm sources.
 *
 *
 * Revision 1.1.2.4  2000/08/26 21:04:14  taustin
 * Added -trigger:inst <N> option to enable verbose dumps mid-execution.
 *
 * Revision 1.1.2.3  2000/08/12 20:15:29  taustin
 * Fixed loader problems.
 * Improved heap management.
 * Added improved bogus address checks.
 *
 * Revision 1.1.2.2  2000/08/02 09:44:54  taustin
 * Fixed stat system call emulation.
 *
 * Revision 1.1.2.1  2000/08/02 08:51:57  taustin
 * SimpleScalar/ARM co-simulation component, based on the ARMulator.
 *
 * Revision 1.1.1.1.2.3  2000/07/28 21:37:08  taustin
 * Added ld_text_bound and ld_data_bound to loader exports.
 *
 * Revision 1.1.1.1.2.2  2000/07/26 05:01:32  taustin
 * Added more debug output.
 *
 * Revision 1.1.1.1.2.1  2000/07/21 18:30:55  taustin
 * More progress on the SimpleScalar/ARM target.
 *
 * Revision 1.1.1.1  2000/05/26 15:18:58  taustin
 * SimpleScalar Tool Set
 *
 *
 * Revision 1.9  1999/12/31 18:53:24  taustin
 * quad_t naming conflicts removed
 *
 * Revision 1.8  1999/12/13 18:47:13  taustin
 * cross endian execution support added
 *
 * Revision 1.7  1998/08/31 17:11:01  taustin
 * added register checksuming support, viewable with "-v" flag
 *
 * Revision 1.6  1998/08/27 16:38:25  taustin
 * implemented host interface description in host.h
 * added target interface support
 * added support for register and memory contexts
 * instruction predecoding moved to loader module
 * Alpha target support added
 * added support for qword's
 * added fault support
 * added option ("-max:inst") to limit number of instructions analyzed
 * added target-dependent myprintf() support
 *
 * Revision 1.5  1997/03/11  17:14:57  taustin
 * updated copyright
 * long/int tweaks made for ALPHA target support
 * supported added for non-GNU C compilers
 *
 * Revision 1.4  1997/01/06  16:06:28  taustin
 * updated comments
 * opt_reg_header() call now prints simulator overview message
 * access variable now generalized to is_write boolean flag
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
#include "sim.h"

#include "armdefs.h"
#include "armemu.h"

/*
 * This file implements a functional simulator.  This functional simulator is
 * the simplest, most user-friendly simulator in the simplescalar tool set.
 * Unlike sim-fast, this functional simulator checks for all instruction
 * errors, and the implementation is crafted for clarity rather than speed.
 */

/* simulated registers */
static struct regs_t regs;

/* simulated memory */
static struct mem_t *mem = NULL;

/* track number of refs */
static counter_t sim_num_refs = 0;

/* maximum number of inst's to execute */
static unsigned int max_insts;

static unsigned int trigger_inst;

/* register simulator-specific options */
void
sim_reg_options(struct opt_odb_t *odb)
{
  opt_reg_header(odb, 
"sim-safe: This simulator implements a functional simulator.  This\n"
"functional simulator is the simplest, most user-friendly simulator in the\n"
"simplescalar tool set.  Unlike sim-fast, this functional simulator checks\n"
"for all instruction errors, and the implementation is crafted for clarity\n"
"rather than speed.\n"
		 );

  /* instruction limit */
  opt_reg_uint(odb, "-max:inst", "maximum number of inst's to execute",
	       &max_insts, /* default */0,
	       /* print */TRUE, /* format */NULL);

  opt_reg_uint(odb, "-trigger:inst", "trigger instruction",
               &trigger_inst, /* default */0,
               /* print */TRUE, /* format */NULL);
}

/* check simulator-specific option values */
void
sim_check_options(struct opt_odb_t *odb, int argc, char **argv)
{
  /* nada */
}

/* register simulator-specific statistics */
void
sim_reg_stats(struct stat_sdb_t *sdb)
{
  stat_reg_counter(sdb, "sim_num_insn",
		   "total number of instructions executed",
		   &sim_num_insn, sim_num_insn, NULL);
  stat_reg_counter(sdb, "sim_num_refs",
		   "total number of loads and stores executed",
		   &sim_num_refs, 0, NULL);
  stat_reg_int(sdb, "sim_elapsed_time",
	       "total simulation time in seconds",
	       &sim_elapsed_time, 0, NULL);
  stat_reg_formula(sdb, "sim_inst_rate",
		   "simulation speed (in insts/sec)",
		   "sim_num_insn / sim_elapsed_time", NULL);
  ld_reg_stats(sdb);
  mem_reg_stats(mem, sdb);
}

#ifdef ARMULATOR

ARMul_State *state = NULL;

#endif

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

#ifdef ARMULATOR

  ARMul_EmulateInit();
  state = ARMul_NewState();
  state->Mode = USER32MODE;
  state->prog32Sig = HIGH;
  state->bigendSig = LOW;
  state->verbose = 0;

#endif
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
  dlite_init(md_reg_obj, dlite_mem_obj, dlite_mstate_obj);
}

/* print simulator-specific configuration information */
void
sim_aux_config(FILE *stream)		/* output stream */
{
  /* nothing currently */
}

/* dump simulator-specific auxiliary simulator statistics */
void
sim_aux_stats(FILE *stream)		/* output stream */
{
  /* nada */
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

#if 0
/* next program counter */
#define SET_NPC(EXPR)		(regs.regs_NPC = (EXPR))

/* current program counter */
#define CPC			(regs.regs_PC)

/* general purpose registers */
#define GPR(N)			(regs.regs_R[N])
#define SET_GPR(N,EXPR)		(regs.regs_R[N] = (EXPR))

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
#define FPR(N)			(regs.regs_F.d[(N)])
#define SET_FPR(N,EXPR)		(regs.regs_F.d[(N)] = (EXPR))

/* miscellaneous register accessors */
#define FPCR			(regs.regs_C.fpcr)
#define SET_FPCR(EXPR)		(regs.regs_C.fpcr = (EXPR))
#define UNIQ			(regs.regs_C.uniq)
#define SET_UNIQ(EXPR)		(regs.regs_C.uniq = (EXPR))

#else
#error No ISA target defined...
#endif

/* precise architected memory state accessor macros */
#define READ_BYTE(SRC, FAULT)						\
  ((FAULT) = md_fault_none, MEM_READ_BYTE(mem, addr = (SRC)))
#define READ_HALF(SRC, FAULT)						\
  ((FAULT) = md_fault_none, MEM_READ_HALF(mem, addr = (SRC)))
#define READ_WORD(SRC, FAULT)						\
  ((FAULT) = md_fault_none, MEM_READ_WORD(mem, addr = (SRC)))
#ifdef HOST_HAS_QWORD
#define READ_QWORD(SRC, FAULT)						\
  ((FAULT) = md_fault_none, MEM_READ_QWORD(mem, addr = (SRC)))
#endif /* HOST_HAS_QWORD */

#define WRITE_BYTE(SRC, DST, FAULT)					\
  ((FAULT) = md_fault_none, MEM_WRITE_BYTE(mem, addr = (DST), (SRC)))
#define WRITE_HALF(SRC, DST, FAULT)					\
  ((FAULT) = md_fault_none, MEM_WRITE_HALF(mem, addr = (DST), (SRC)))
#define WRITE_WORD(SRC, DST, FAULT)					\
  ((FAULT) = md_fault_none, MEM_WRITE_WORD(mem, addr = (DST), (SRC)))
#ifdef HOST_HAS_QWORD
#define WRITE_QWORD(SRC, DST, FAULT)					\
  ((FAULT) = md_fault_none, MEM_WRITE_QWORD(mem, addr = (DST), (SRC)))
#endif /* HOST_HAS_QWORD */

/* system call handler macro */
#define SYSCALL(INST)	sys_syscall(&regs, mem_access, mem, INST, TRUE)
#endif

word_t
GetWord(void *p, md_addr_t addr)
{
  if (addr < 0x2000000)
    myfprintf(stderr, "--rd @ %p(%n): addr:%x, val:%x....\n", 
	regs.regs_R[15], sim_num_insn, addr, MEM_READ_WORD(mem, (addr & ~3)));
	
  return MEM_READ_WORD(mem, (addr & ~3));
}

void
PutWord(void *p, md_addr_t addr, word_t data)
{
  if (addr < 0x2000000)
    myfprintf(stderr, "--wr @ %p(%n): addr:%x, val:%x....\n", 
	regs.regs_R[15], sim_num_insn, addr, data);

  MEM_WRITE_WORD(mem, (addr & ~3), data);
}

void
resynch_regs(struct regs_t *regs, ARMul_State *state, int to)
{
  int i;

  if (to)
    {
      for (i=0; i < 16; i++)
	regs->regs_R[i] = state->Reg[i];
      regs->regs_C.cpsr = state->Cpsr;
      regs->regs_C.cpsr = ((regs->regs_C.cpsr & ~NBIT) | (!!NFLAG << 31));
      regs->regs_C.cpsr = ((regs->regs_C.cpsr & ~ZBIT) | (!!ZFLAG << 30));
      regs->regs_C.cpsr = ((regs->regs_C.cpsr & ~CBIT) | (!!CFLAG << 29));
      regs->regs_C.cpsr = ((regs->regs_C.cpsr & ~VBIT) | (!!VFLAG << 28));
      regs->regs_C.spsr = state->Spsr[state->Bank];
    }
  else /* from */
    {
      for (i=0; i < 16; i++)
	state->Reg[i] = regs->regs_R[i];
      state->Cpsr = regs->regs_C.cpsr;
      NFLAG = (regs->regs_C.cpsr >> 31) & 1;
      ZFLAG = (regs->regs_C.cpsr >> 30) & 1;
      CFLAG = (regs->regs_C.cpsr >> 29) & 1;
      VFLAG = (regs->regs_C.cpsr >> 28) & 1;
      state->Spsr[state->Bank] = regs->regs_C.spsr;
    }
}

unsigned
ARMul_OSHandleSWI (ARMword instr, ARMul_State * state, ARMword number)
{
  resynch_regs(&regs, state, /* to */TRUE);
  sys_syscall(&regs, mem_access, mem, instr, TRUE);
  resynch_regs(&regs, state, /* from */FALSE);
  return TRUE;
}

/* start simulation, program loaded, processor precise state initialized */
void
sim_main(void)
{
  md_inst_t inst, decoded, loaded;
  register md_addr_t addr, pc;
  enum md_opcode op;
  register int is_write;
  enum md_fault_type fault;
  int trigger = FALSE;

  fprintf(stderr, "sim: ** starting functional simulation **\n");

  /* set up initial default next PC */
#if 0
  regs.regs_PC = ld_text_base;
#endif
  regs.regs_NPC = regs.regs_PC + sizeof(md_inst_t);

  /* synchronize register files... */
  resynch_regs(&regs, state, /* from */FALSE);
  state->pc = regs.regs_PC;
  state->Reg[15] = regs.regs_PC;
  state->NextInstr = RESUME;

  /* check for DLite debugger entry condition */
  if (dlite_check_break(regs.regs_PC, /* !access */0, /* addr */0, 0, 0))
    dlite_main(regs.regs_PC - sizeof(md_inst_t),
	       regs.regs_PC, sim_num_insn, &regs, mem);

  while (TRUE)
    {
      /* maintain $r0 semantics */
#ifdef TARGET_ARM
      /* regs.regs_R[MD_REG_PC] = regs.regs_PC; */
#endif
#ifndef TARGET_ARM
      regs.regs_R[MD_REG_ZERO] = 0;
#endif
#ifdef TARGET_ALPHA
      regs.regs_F.d[MD_REG_ZERO] = 0.0;
#endif /* TARGET_ALPHA */

#if 0
      if (regs.regs_PC >= ld_text_bound)
	{
	  info("all instructions decoded...");
	  exit(0);
	}
#endif

      /* keep an instruction count */
      sim_num_insn++;

#ifdef ARMULATOR

      if (state->Emulate == STOP)
	fatal("ARMulator simulation stopped");
      if ((state->prog32Sig && ARMul_MODE32BIT) == 0)
	fatal("ARMulator has left 32-bit mode");

      if (state->NextInstr < PRIMEPIPE)
	{
	  decoded = state->decoded;
	  loaded = state->loaded;
	  pc = state->pc;
	}

#ifdef MODET
      if (TFLAG)
	{
	  isize = 2;
	}
      else
#endif
	isize = 4;
      switch (state->NextInstr)
	{
	case SEQ:
	  state->Reg[15] += isize; /* Advance the pipeline, and an S cycle */
	  pc += isize;
	  inst = decoded;
	  decoded = loaded;
	  /* get the next instruction to execute */
	  MD_FETCH_INST(loaded, mem, pc + (isize * 2));
	  break;

	case NONSEQ:
	  state->Reg[15] += isize; /* Advance the pipeline, and an N cycle */
	  pc += isize;
	  inst = decoded;
	  decoded = loaded;
	  /* get the next instruction to execute */
	  MD_FETCH_INST(loaded, mem, pc + (isize * 2));
	  NORMALCYCLE;
	  break;

	case PCINCEDSEQ:
	  pc += isize;		/* Program counter advanced, and an S cycle */
	  inst = decoded;
	  decoded = loaded;
	  /* get the next instruction to execute */
	  MD_FETCH_INST(loaded, mem, pc + (isize * 2));
	  NORMALCYCLE;
	  break;

	case PCINCEDNONSEQ:
	  pc += isize;		/* Program counter advanced, and an N cycle */
	  inst = decoded;
	  decoded = loaded;
	  /* get the next instruction to execute */
	  MD_FETCH_INST(loaded, mem, pc + (isize * 2));
	  NORMALCYCLE;
	  break;

	case RESUME:		/* The program counter has been changed */
	  pc = state->Reg[15];
#ifndef MODE32
	  pc = pc & R15PCBITS;
#endif
	  state->Reg[15] = pc + (isize * 2);
	  state->Aborted = 0;
	  /* get the next instruction to execute */
	  MD_FETCH_INST(inst, mem, pc);
	  /* get the next instruction to execute */
	  MD_FETCH_INST(decoded, mem, pc + isize);
	  /* get the next instruction to execute */
	  MD_FETCH_INST(loaded, mem, pc + isize*2);
	  NORMALCYCLE;
	  break;

	default:		/* The program counter has been changed */
	  pc = state->Reg[15];
#ifndef MODE32
	  pc = pc & R15PCBITS;
#endif
	  state->Reg[15] = pc + (isize * 2);
	  state->Aborted = 0;
	  /* get the next instruction to execute */
	  MD_FETCH_INST(inst, mem, pc);
	  /* get the next instruction to execute */
	  MD_FETCH_INST(decoded, mem, pc + isize);
	  /* get the next instruction to execute */
	  MD_FETCH_INST(loaded, mem, pc + (isize*2));
	  NORMALCYCLE;
	  break;
	}
      if (state->EventSet)
	abort(); /* XXX ARMul_EnvokeEvent (state); */

      if (state->Exception)
	{			/* Any exceptions */
	  abort(); /* XXX */
	  if (state->NresetSig == LOW)
	    {
	      abort(); /* XXX ARMul_Abort (state, ARMul_ResetV); */
	      break;
	    }
	  else if (!state->NfiqSig && !FFLAG)
	    {
	      abort(); /* XXX ARMul_Abort (state, ARMul_FIQV); */
	      break;
	    }
	  else if (!state->NirqSig && !IFLAG)
	    {
	      abort(); /* XXX ARMul_Abort (state, ARMul_IRQV); */
	      break;
	    }
	}

      if (state->CallDebug > 0)
	{
	  abort();
	}
      else if (state->Emulate < ONCE)
	{
	  state->NextInstr = RESUME;
	  break;
	}

      state->Emulate = RUN;
      ARMul_Emulate32(pc, inst, state);
      /* synchronize register files... */
      resynch_regs(&regs, state, /* to */TRUE);

      state->decoded = decoded;
      state->loaded = loaded;
      state->pc = pc;

      /* pick up the next PC */
      regs.regs_NPC = regs.regs_R[MD_REG_PC];

      if (verbose && sim_num_insn >= trigger_inst)
	{
	  myfprintf(stderr, "%10n [xor: 0x%08x] @ 0x%08p: ",
		    sim_num_insn, md_xor_regs(&regs), pc);
	  md_print_insn(inst, pc, stderr);
	  fprintf(stderr, "\n");
	  md_print_iregs(regs.regs_R, stderr);
	  md_print_cregs(regs.regs_C, stderr);
	  /* fflush(stderr); */
	}

#else /* !ARMULATOR */

      /* set default reference address and access mode */
      addr = 0; is_write = FALSE;

      /* set default fault - none */
      fault = md_fault_none;

      /* decode the instruction */
      MD_SET_OPCODE(op, inst);

      /* execute the instruction */
      switch (op)
	{
#define DEFINST(OP,MSK,NAME,OPFORM,RES,FLAGS,O1,O2,I1,I2,I3,I4)		\
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

      if (verbose)
	{
	  myfprintf(stderr, "%10n [xor: 0x%08x] @ 0x%08p: ",
		    sim_num_insn, md_xor_regs(&regs), regs.regs_PC);
	  md_print_insn(inst, regs.regs_PC, stderr);
	  if (MD_OP_FLAGS(op) & F_MEM)
	    myfprintf(stderr, "  mem: 0x%08p", addr);
	  fprintf(stderr, "\n");
	  myfprintf(stderr, "           op: %d, inst: 0x%08x\n", op, inst);
	  md_print_iregs(&regs, stderr);
	  /* fflush(stderr); */
	}

      if (MD_OP_FLAGS(op) & F_MEM)
	{
	  sim_num_refs++;
	  if (MD_OP_FLAGS(op) & F_STORE)
	    is_write = TRUE;
	}

#endif

      /* check for DLite debugger entry condition */
      if (dlite_check_break(regs.regs_NPC,
			    is_write ? ACCESS_WRITE : ACCESS_READ,
			    addr, sim_num_insn, sim_num_insn))
	dlite_main(regs.regs_PC, regs.regs_NPC, sim_num_insn, &regs, mem);

      /* go to the next instruction */
      regs.regs_PC = regs.regs_NPC;
      regs.regs_NPC += sizeof(md_inst_t);

      /* finish early? */
      if (max_insts && sim_num_insn >= max_insts)
	return;
    }
}

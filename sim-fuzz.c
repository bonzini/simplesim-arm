/*
 * sim-fuzz.c - ARM fuzz tester harness
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
 * $Id: sim-fuzz.c,v 1.1.1.1 2000/11/29 14:53:54 cu-cs Exp $
 *
 * $Log: sim-fuzz.c,v $
 * Revision 1.1.1.1  2000/11/29 14:53:54  cu-cs
 * Grand unification of arm sources.
 *
 *
 * Revision 1.1.2.2  2000/09/19 17:25:32  taustin
 * Completed UOP decomposition code, all UOPs now take nominally one cycle on a
 * simple pipeline, including loads and stores.  Initial testing has the code
 * working fine, however, extensive random testing will occur when sim-fuzz.c
 * gets extended to support multiple functional units...
 *
 * Revision 1.1.2.1  2000/09/05 13:56:02  taustin
 * SimpleScalar/ARM fuzz buster - random co-simulation tester, very handy!
 *
 * Revision 1.1.1.1.2.8  2000/08/30 21:23:39  taustin
 * Fixed r15 update semantics (now catches r15 updated to value of PC, this happens
 * when a tail recursive function returns to itself - oye!)...
 *
 * Revision 1.1.1.1.2.7  2000/08/29 14:18:09  taustin
 * Fixed word and qword size fp register accesses.
 *
 * Revision 1.1.1.1.2.6  2000/08/26 21:04:14  taustin
 * Added -trigger:inst <N> option to enable verbose dumps mid-execution.
 *
 * Revision 1.1.1.1.2.5  2000/08/26 06:53:53  taustin
 * Simplified SimpleScalar/ARM PC handling - seems to work...
 *
 * Revision 1.1.1.1.2.4  2000/08/22 18:38:49  taustin
 * More progress on the SimpleScalar/ARM target.
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

#define rand_word()	((word_t)random())
#define rand_byte()	((byte_t)random())

/* simulated registers */
static struct regs_t test_regs;

static ARMul_State *ref_state = NULL;

#define STORETAB_LOG_SZ	12
#define STORETAB_SZ	(1 << STORETAB_LOG_SZ)
byte_t storetab[STORETAB_SZ];

#define TADDR(X)							\
  ((((((X) >> (2*(STORETAB_LOG_SZ-2)))					\
      ^ ((X) >> (STORETAB_LOG_SZ-2))					\
      ^ (X))								\
     & ((1 << STORETAB_LOG_SZ) - 1))					\
    & ~3)								\
   | ((X) & 3))

/* track number of refs */
static counter_t sim_valid_insn = 0;
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
  stat_reg_counter(sdb, "sim_valid_insn",
		   "total number of valid instructions executed",
		   &sim_valid_insn, sim_valid_insn, NULL);
  stat_reg_counter(sdb, "sim_num_refs",
		   "total number of loads and stores executed",
		   &sim_num_refs, 0, NULL);
  stat_reg_int(sdb, "sim_elapsed_time",
	       "total simulation time in seconds",
	       &sim_elapsed_time, 0, NULL);
  stat_reg_formula(sdb, "sim_inst_rate",
		   "simulation speed (in insts/sec)",
		   "sim_num_insn / sim_elapsed_time", NULL);
//  ld_reg_stats(sdb);
//  mem_reg_stats(mem, sdb);

  /* microarchitecture stats */
  md_reg_stats(sdb);
}

/* initialize the simulator */
void
sim_init(void)
{
  int i;
  sim_num_refs = 0;

  /* allocate and initialize register file */
  regs_init(&test_regs);

  /* allocate and initialize memory space */
//  mem = mem_create("mem");
//  mem_init(mem);

  ARMul_EmulateInit();
  ref_state = ARMul_NewState();
  ref_state->Mode = USER32MODE;
  ref_state->Bank = 0;
  ref_state->prog32Sig = HIGH;
  ref_state->bigendSig = LOW;
  ref_state->verbose = 0;

  /* initialize store table */
  for (i=0; i < STORETAB_SZ; i++)
    storetab[i] = rand_byte();
}

/* load program into simulated state */
void
sim_load_prog(char *fname,		/* program to load */
	      int argc, char **argv,	/* program arguments */
	      char **envp)		/* program environment */
{
//  /* load program text and data, set up environment, memory, and regs */
//  ld_load_prog(fname, argc, argv, envp, &regs, mem, TRUE);

//  /* initialize the DLite debugger */
//  dlite_init(md_reg_obj, dlite_mem_obj, dlite_mstate_obj);
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

/* next program counter */
#define SET_NPC(EXPR)		(test_regs.regs_NPC = (EXPR))

/* current program counter */
#define CPC			(test_regs.regs_PC)

/* general purpose registers */
#define GPR(N)			(test_regs.regs_R[N])
#define SET_GPR(N,EXPR)							\
  ((void)(((N) == 15) ? setPC++ : 0), test_regs.regs_R[N] = (EXPR))

/* processor status register */
#define PSR			(test_regs.regs_C.cpsr)
#define SET_PSR(EXPR)		(test_regs.regs_C.cpsr = (EXPR))

#define PSR_N			_PSR_N(test_regs.regs_C.cpsr)
#define SET_PSR_N(EXPR)		_SET_PSR_N(test_regs.regs_C.cpsr, (EXPR))
#define PSR_C			_PSR_C(test_regs.regs_C.cpsr)
#define SET_PSR_C(EXPR)		_SET_PSR_C(test_regs.regs_C.cpsr, (EXPR))
#define PSR_Z			_PSR_Z(test_regs.regs_C.cpsr)
#define SET_PSR_Z(EXPR)		_SET_PSR_Z(test_regs.regs_C.cpsr, (EXPR))
#define PSR_V			_PSR_V(test_regs.regs_C.cpsr)
#define SET_PSR_V(EXPR)		_SET_PSR_V(test_regs.regs_C.cpsr, (EXPR))

/* floating point conversions */
union x { float f; word_t w; };
#define DTOW(D)		({ union x fw; fw.f = (float)(D); fw.w; })
#define WTOD(W)		({ union x fw; fw.w = (W); (double)fw.f; })
#define QSWP(Q)								\
  ((((Q) << 32) & ULL(0xffffffff00000000)) | (((Q) >> 32) & ULL(0xffffffff)))

/* floating point registers, L->word, F->single-prec, D->double-prec */
#define FPR_Q(N)		(QSWP(test_regs.regs_F.q[N]))
#define SET_FPR_Q(N,EXPR)	(test_regs.regs_F.q[N] = QSWP((EXPR)))
#define FPR_W(N)		(DTOW(test_regs.regs_F.d[N]))
#define SET_FPR_W(N,EXPR)	(test_regs.regs_F.d[N] = (WTOD(EXPR)))
#define FPR(N)			(test_regs.regs_F.d[(N)])
#define SET_FPR(N,EXPR)		(test_regs.regs_F.d[(N)] = (EXPR))

/* miscellaneous register accessors */
#define FPSR			(test_regs.regs_C.fpsr)
#define SET_FPSR(EXPR)		(test_regs.regs_C.fpsr = (EXPR))


word_t
word_align(md_addr_t addr, word_t data)
{
  addr = (addr & 3) << 3;
  return ((data >> addr) | (data << (32 - addr)));
}

/* precise architected memory state accessor macros */
#define READ_BYTE(SRC, FAULT)						\
  ((FAULT) = md_fault_none, *(byte_t *)(&storetab[TADDR(SRC)]))
#define READ_HALF(SRC, FAULT)						\
  ((FAULT) = md_fault_none, *(half_t *)(&storetab[TADDR(SRC) & ~1]))
#define READ_WORD(SRC, FAULT)						\
  ((FAULT) = md_fault_none,						\
   (((SRC) & 3)								\
    ? word_align((SRC), *(word_t *)(&storetab[TADDR(SRC) & ~3]))	\
    : *(word_t *)(&storetab[TADDR(SRC)])))
#ifdef HOST_HAS_QWORD
#define READ_QWORD(SRC, FAULT)	((FAULT) = md_fault_invalid, 0)
#endif /* HOST_HAS_QWORD */

#define WRITE_BYTE(SRC, DST, FAULT)					\
  ((FAULT) = md_fault_none,						\
   test_stores[test_nstores].size = 1,					\
   addr = test_stores[test_nstores].addr = (DST),			\
   test_stores[test_nstores++].data = (word_t)(byte_t)(SRC))
#define WRITE_HALF(SRC, DST, FAULT)					\
  ((FAULT) = md_fault_none,						\
   test_stores[test_nstores].size = 2,					\
   addr = test_stores[test_nstores].addr = (DST),			\
   test_stores[test_nstores++].data = (word_t)(half_t)(SRC))
#define WRITE_WORD(SRC, DST, FAULT)					\
  ((FAULT) = md_fault_none,						\
   test_stores[test_nstores].size = 4,					\
   addr = test_stores[test_nstores].addr = (DST),			\
   test_stores[test_nstores++].data = (word_t)(SRC))
#ifdef HOST_HAS_QWORD
#define WRITE_QWORD(SRC, DST, FAULT)	((FAULT) = md_fault_invalid)
#endif /* HOST_HAS_QWORD */

/* system call handler macro */
#define SYSCALL(INST)	(/* nada... */(void)0)

unsigned
ARMul_OSHandleSWI (ARMword instr, ARMul_State * state, ARMword number)
{
  /* nada... */
  return TRUE;
}

struct storebuf_t { word_t size; md_addr_t addr; word_t data; };

#define MAX_STORES	32

int ref_nstores = 0;
struct storebuf_t ref_stores[MAX_STORES];

int test_nstores = 0;
struct storebuf_t test_stores[MAX_STORES];

/* return a deterministic value based on the address */
word_t
GetWord(void *p, md_addr_t addr)
{
  if (TADDR(addr) >= 4096)
    panic("bogus taddr");
  
  return *((word_t *)(&storetab[TADDR(addr) & ~3]));
}

void
PutWord(void *p, md_addr_t addr, word_t data)
{
  /* MEM_WRITE_WORD(mem, (addr & ~3), data); */
  ref_stores[ref_nstores].addr = addr;
  ref_stores[ref_nstores++].data = data;
}

/* start simulation, program loaded, processor precise state initialized */
void
sim_main(void)
{
  md_inst_t inst, test_inst, ref_inst;
  md_addr_t addr, ref_pc, ref_npc;
  enum md_opcode op;
  register int is_write;
  enum md_fault_type fault;
  int ref_ok, test_ok;
  int i, setPC;
  extern ARMword isize;

  fprintf(stderr, "sim: ** starting fuzz testing **\n");

  isize = 4;
  while (TRUE)
    {
      do
	{
	  /* initialize execution experiment */
	  ref_state->NextInstr = SEQ;
	  test_regs.regs_PC = ref_pc = (rand_word() & ~0x3);
	  ref_state->Reg[MD_REG_PC] = ref_pc + 8;
	  test_regs.regs_NPC = test_regs.regs_PC + 4;
	  test_regs.regs_R[MD_REG_PC] = test_regs.regs_PC;
	  test_inst = ref_inst = rand_word();
	  for (i=0; i < 15; i++)
	    test_regs.regs_R[i] = ref_state->Reg[i] = rand_word();
	  test_regs.regs_C.cpsr = ref_state->Cpsr = rand_word();
	  ref_state->NFlag = ((test_regs.regs_C.cpsr >> 31) & 1);
	  ref_state->ZFlag = ((test_regs.regs_C.cpsr >> 30) & 1);
	  ref_state->CFlag = ((test_regs.regs_C.cpsr >> 29) & 1);
	  ref_state->VFlag = ((test_regs.regs_C.cpsr >> 28) & 1);
	  test_nstores = ref_nstores = 0;

	  /* keep an instruction count */
	  sim_num_insn++;

	  /* execute on reference simulator */
	  ref_ok = ARMul_Emulate32(ref_pc, ref_inst, ref_state);
	  // if (verbose && ref_ok == 0)
	  //   warn("tried instruction 0x%08x, but failed...", ref_inst);
	}
      while (ref_ok == 0);

      /* compute the ARMulator next PC */
      switch (ref_state->NextInstr)
	{
	case SEQ:
	  ref_npc = ref_pc + 4;
	  break;

	case NONSEQ:
	  ref_npc = ref_pc + 4;
	  break;

	case PCINCEDSEQ:
	  ref_npc = ref_pc + 4;
	  break;

	case PCINCEDNONSEQ:
	  ref_npc = ref_pc + 4;
	  break;

	case RESUME:		/* The program counter has been changed */
	  ref_npc = ref_state->Reg[MD_REG_PC];
	  break;

	default:		/* The program counter has been changed */
	  ref_npc = ref_state->Reg[MD_REG_PC];
	  break;
	}

      /* execute on the SimpleScalar/ARM functional component */
      inst = test_inst;

      test_ok = COND_VALID(PSR) ? 2 : 1;

      if (test_ok == 2)
	{
	  /* keep a valid instruction count */
	  sim_valid_insn++;

	  /* decode the instruction */
	  MD_SET_OPCODE(op, inst);
	  if (op == NA)
	    fatal("bogus opcode detected @ %n", sim_num_insn);

	  /* set default reference address and access mode */
	  addr = 0; is_write = FALSE;

	  /* set default fault - none */
	  fault = md_fault_none;

	  /* execute the instruction */
	  setPC = 0;
	  switch (op)
	    {
#define DEFINST(OP,MSK,NAME,OPFORM,RES,FLAGS,O1,O2,O3,I1,I2,I3,I4)	\
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

	  if (setPC != 0/* regs.regs_R[MD_REG_PC] != regs.regs_PC */)
	    test_regs.regs_NPC = test_regs.regs_R[MD_REG_PC];

	  if (verbose && sim_num_insn >= trigger_inst)
	    {
	      myfprintf(stderr, "%10n [xor: 0x%08x] @ 0x%08p: ",
			sim_num_insn, md_xor_regs(&test_regs),
			test_regs.regs_PC);
	      md_print_insn(inst, test_regs.regs_PC, stderr);
	      fprintf(stderr, "\n");
	      // md_print_iregs(regs.regs_R, stderr);
	      // md_print_fpregs(regs.regs_F, stderr);
	      // md_print_cregs(regs.regs_C, stderr);
	      /* fflush(stderr); */
	    }

	  if (fault != md_fault_none)
	    fatal("fault (%d) detected @ 0x%08p", fault, test_regs.regs_PC);
	}

      /* compare the results */
      if (test_regs.regs_PC != ref_pc)
	fatal("PC mismatch");
      if ((test_regs.regs_NPC & ~3) != (ref_npc & ~3))
	fatal("NPC mismatch");
      if (test_ok != ref_ok)
	fatal("predicate mismatch");
      for (i=0; i < 15; i++)
	{
	  if (test_regs.regs_R[i] != ref_state->Reg[i])
	    fatal("integer register mismatch");
	}
      if (ref_state->NFlag != ((test_regs.regs_C.cpsr >> 31) & 1))
	fatal("N flag mismatch");
      if (ref_state->ZFlag != ((test_regs.regs_C.cpsr >> 30) & 1))
	fatal("Z flag mismatch");
      if (ref_state->CFlag != ((test_regs.regs_C.cpsr >> 29) & 1))
	fatal("C flag mismatch");
      if (ref_state->VFlag != ((test_regs.regs_C.cpsr >> 28) & 1))
	fatal("V flag mismatch");

      if (test_nstores != ref_nstores)
	fatal("nstores mismatch");
      if (test_nstores >= MAX_STORES)
	fatal("too many stores");
      for (i=0; i < test_nstores; i++)
	{
	  if (test_stores[i].addr != ref_stores[i].addr)
	    fatal("store address mismatch");
	  switch (test_stores[i].size)
	    {
	    case 1:
	      ref_stores[i].data =
		(ref_stores[i].data >> ((test_stores[i].addr & 3) * 8))
		& 0xff;
	      break;
	    case 2:
	      if (test_stores[i].addr & 2)
		ref_stores[i].data = (ref_stores[i].data >> 16) & 0xffff;
	      else
		ref_stores[i].data = ref_stores[i].data & 0xffff;
	      break;
	    case 4:
	      break;
	    default:
	      abort();
	    }
	  if (test_stores[i].data != ref_stores[i].data)
	    fatal("store data mismatch");
	}

#if 0
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
      regs.regs_PC = regs.regs_NPC;
      regs.regs_NPC += sizeof(md_inst_t);
#endif

      if ((sim_num_insn % 5000000) == 0)
	info("%n instructions probed...", sim_num_insn);

      /* finish early? */
      if (max_insts && sim_num_insn >= max_insts)
	return;
    }
}

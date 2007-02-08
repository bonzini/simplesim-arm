/*
 * arm.c - ARM ISA definition routines
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
 * $Id: arm.c,v 1.1.1.1 2000/11/29 14:53:54 cu-cs Exp $
 *
 * $Log: arm.c,v $
 * Revision 1.1.1.1  2000/11/29 14:53:54  cu-cs
 * Grand unification of arm sources.
 *
 *
 * Revision 1.1.2.21  2000/10/19 14:38:42  taustin
 * More work on sim-outorder for SimpleScalar/ARM.
 *
 * Revision 1.1.2.20  2000/09/26 15:45:31  taustin
 * Attempted to get sim-outorder working, so so...
 * Dependency fixes.
 *
 * Revision 1.1.2.19  2000/09/22 01:58:16  taustin
 * Finished UOP flow generation support.
 *
 * Revision 1.1.2.18  2000/09/19 17:25:35  taustin
 * Completed UOP decomposition code, all UOPs now take nominally one cycle on a
 * simple pipeline, including loads and stores.  Initial testing has the code
 * working fine, however, extensive random testing will occur when sim-fuzz.c
 * gets extended to support multiple functional units...
 *
 * Revision 1.1.2.17  2000/09/11 12:23:07  taustin
 * Added UOP decomposition routines, used by sim-uop, soon to used by perf
 * simulators.
 *
 * Revision 1.1.2.16  2000/09/05 13:56:31  taustin
 * Lots of fixes after simulating using the new SimpleScalar/ARM fuzz buster to
 * perform random co-simulation testing, very handy!
 *
 * Revision 1.1.2.15  2000/08/22 18:38:52  taustin
 * More progress on the SimpleScalar/ARM target.
 *
 * Revision 1.1.2.14  2000/08/02 08:52:15  taustin
 * SimpleScalar/ARM co-simulation component, based on the ARMulator.
 * More fixes to the SimpleScalar/ARM target.
 *
 * Revision 1.1.2.13  2000/07/28 21:37:22  taustin
 * More debugging of the SimpleScalar/ARM target.
 *
 * Revision 1.1.2.12  2000/07/28 20:32:04  omutlu
 * *** empty log message ***
 *
 * Revision 1.1.2.11  2000/07/27 21:56:11  omutlu
 * corrections to shifts
 *
 * Revision 1.1.2.10  2000/07/27 21:45:03  taustin
 * Added umpteen half word and signed byte loads.
 *
 * Revision 1.1.2.9  2000/07/27 20:36:25  omutlu
 * added the undefined instruction
 *
 * Revision 1.1.2.8  2000/07/27 19:12:07  omutlu
 * some fixes to cdp instruction. needs more work
 *
 * Revision 1.1.2.7  2000/07/26 12:21:55  taustin
 * Fixed CPRT_LINK in ARM decode table.
 *
 * Revision 1.1.2.6  2000/07/26 06:32:16  omutlu
 *
 * More fixes to floating-point instructions
 *
 * Revision 1.1.2.5  2000/07/26 05:01:50  taustin
 * More disassembler fixes...
 *
 * Revision 1.1.2.4  2000/07/25 18:29:00  omutlu
 * Modified the branch offset calculation, cmp, ldm
 *
 * Revision 1.1.2.3  2000/07/21 18:30:58  taustin
 * More progress on the SimpleScalar/ARM target.
 *
 * Revision 1.1.2.2  2000/07/13 03:09:55  taustin
 * More progress on the SimpleScalar/ARM target.
 *
 * Revision 1.1.2.1  2000/05/31 19:25:59  taustin
 * ARM definition files
 *
 * Revision 1.1.1.1  2000/05/26 15:22:27  taustin
 * SimpleScalar Tool Set
 *
 *
 * Revision 1.4  1999/12/31 18:55:19  taustin
 * quad_t naming conflicts removed
 * decoders are more bulletproof (check for array overflows)
 *
 * Revision 1.3  1999/12/13 18:58:56  taustin
 * cross endian execution support added
 *
 * Revision 1.2  1998/08/31 17:15:58  taustin
 * added register checksuming support
 *
 * Revision 1.1  1998/08/27 16:53:27  taustin
 * Initial revision
 *
 * Revision 1.1  1998/05/06  01:08:39  calder
 * Initial revision
 *
 * Revision 1.4  1997/03/11  01:40:38  taustin
 * updated copyrights
 * long/int tweaks made for ALPHA target support
 * supported added for non-GNU C compilers
 * ss_print_insn() can now tolerate bogus insts (for DLite!)
 *
 * Revision 1.3  1997/01/06  16:07:24  taustin
 * comments updated
 * functional unit definitions moved from ss.def
 *
 * Revision 1.1  1996/12/05  18:52:32  taustin
 * Initial revision
 *
 *
 */

#include <stdio.h>
#include <stdlib.h>

#include "host.h"
#include "misc.h"
#include "machine.h"
#include "eval.h"
#include "regs.h"
#include "stats.h"

#ifdef LV2_PANALYZER_H
#include "./libpanalyzer/technology.h"
#else
#include "power.h"
#endif

#if 0 /* cross-endian execution now works with EIO trace files only... */
/* FIXME: currently SimpleScalar/AXP only builds on little-endian... */
#if !defined(BYTES_LITTLE_ENDIAN) || !defined(WORDS_LITTLE_ENDIAN)
#error SimpleScalar/ARM only builds on little-endian machines...
#endif
#endif

/* FIXME: currently SimpleScalar/AXP only builds with qword support... */
#if !defined(HOST_HAS_QWORD)
#error SimpleScalar/ARM only builds on hosts with builtin qword support...
#error Try building with GNU GCC, as it supports qwords on most machines.
#endif

/* preferred nop instruction definition */
md_inst_t MD_NOP_INST = 0xe1a00000;		/* mov r0,r0 */

/* opcode mask -> enum md_opcodem, used by decoder (MD_OP_ENUM()) */
enum md_opcode md_mask2op[MD_MAX_MASK+1];
unsigned int md_opoffset[OP_MAX];

/* enum md_opcode -> mask for decoding next level */
unsigned int md_opmask[OP_MAX] = {
  0, /* NA */
#define DEFINST(OP,MSK,NAME,OPFORM,RES,FLAGS,O1,O2,O3,I1,I2,I3,I4) 0,
#define DEFUOP(OP,NAME,OPFORM,RES,FLAGS,O1,O2,O3,I1,I2,I3,I4) 0,
#define DEFLINK(OP,MSK,NAME,SHIFT,MASK) MASK,
#define CONNECT(OP)
#include "machine.def"
};

/* enum md_opcode -> shift for decoding next level */
unsigned int md_opshift[OP_MAX] = {
  0, /* NA */
#define DEFINST(OP,MSK,NAME,OPFORM,RES,FLAGS,O1,O2,O3,I1,I2,I3,I4) 0,
#define DEFUOP(OP,NAME,OPFORM,RES,FLAGS,O1,O2,O3,I1,I2,I3,I4) 0,
#define DEFLINK(OP,MSK,NAME,SHIFT,MASK) SHIFT,
#define CONNECT(OP)
#include "machine.def"
};

/* enum md_opcode -> description string */
char *md_op2name[OP_MAX] = {
  NULL, /* NA */
#define DEFINST(OP,MSK,NAME,OPFORM,RES,FLAGS,O1,O2,O3,I1,I2,I3,I4) NAME,
#define DEFUOP(OP,NAME,OPFORM,RES,FLAGS,O1,O2,O3, I1,I2,I3,I4) NAME,
#define DEFLINK(OP,MSK,NAME,MASK,SHIFT) NAME,
#define CONNECT(OP)
#include "machine.def"
};

/* enum md_opcode -> opcode operand format, used by disassembler */
char *md_op2format[OP_MAX] = {
  NULL, /* NA */
#define DEFINST(OP,MSK,NAME,OPFORM,RES,FLAGS,O1,O2,O3,I1,I2,I3,I4) OPFORM,
#define DEFUOP(OP,NAME,OPFORM,RES,FLAGS,O1,O2,O3,I1,I2,I3,I4) OPFORM,
#define DEFLINK(OP,MSK,NAME,MASK,SHIFT) NULL,
#define CONNECT(OP)
#include "machine.def"
};

/* enum md_opcode -> enum md_fu_class, used by performance simulators */
enum md_fu_class md_op2fu[OP_MAX] = {
  FUClamd_NA, /* NA */
#define DEFINST(OP,MSK,NAME,OPFORM,RES,FLAGS,O1,O2,O3,I1,I2,I3,I4) RES,
#define DEFUOP(OP,NAME,OPFORM,RES,FLAGS,O1,O2,O3,I1,I2,I3,I4) RES,
#define DEFLINK(OP,MSK,NAME,MASK,SHIFT) FUClamd_NA,
#define CONNECT(OP)
#include "machine.def"
};

/* enum md_fu_class -> description string */
char *md_fu2name[NUM_FU_CLASSES] = {
  "NONE", /* NA */
  "fu-int-ALU",
  "fu-int-multiply",
  "fu-int-divide",
  "fu-FP-add/sub",
  "fu-FP-comparison",
  "fu-FP-conversion",
  "fu-FP-multiply",
  "fu-FP-divide",
  "fu-FP-sqrt",
  "rd-port",
  "wr-port"
};

char *md_amode_str[md_amode_NUM] =
{
  "(const)",		/* immediate addressing mode */
  "(gp + const)",	/* global data access through global pointer */
  "(sp + const)",	/* stack access through stack pointer */
  "(fp + const)",	/* stack access through frame pointer */
  "(reg + const)",	/* (reg + const) addressing */
  "(reg + reg)"		/* (reg + reg) addressing */
};

/* symbolic register names, parser is case-insensitive */
struct md_reg_names_t md_reg_names[] =
{
  /* name */	/* file */	/* reg */

  /* integer register file */
  { "$r0",	rt_gpr,		0 },
  { "$v0",	rt_gpr,		0 },
  { "$a0",	rt_gpr,		0 },
  { "$r1",	rt_gpr,		1 },
  { "$a1",	rt_gpr,		1 },
  { "$r2",	rt_gpr,		2 },
  { "$a2",	rt_gpr,		2 },
  { "$r3",	rt_gpr,		3 },
  { "$a3",	rt_gpr,		3 },
  { "$r4",	rt_gpr,		4 },
  { "$r5",	rt_gpr,		5 },
  { "$r6",	rt_gpr,		6 },
  { "$r7",	rt_gpr,		7 },
  { "$r8",	rt_gpr,		8 },
  { "$r9",	rt_gpr,		9 },
  { "$r10",	rt_gpr,		10 },
  { "$r11",	rt_gpr,		11 },
  { "$fp",	rt_gpr,		11 },
  { "$r12",	rt_gpr,		12 },
  { "$ip",	rt_gpr,		12 },
  { "$r13",	rt_gpr,		13 },
  { "$sp",	rt_gpr,		13 },
  { "$r14",	rt_gpr,		14 },
  { "$lr",	rt_gpr,		14 },
  { "$r15",	rt_gpr,		15 },
  { "$pc",	rt_gpr,		15 },

  /* floating point register file - double precision */
  { "$f0",	rt_fpr,		0 },
  { "$f1",	rt_fpr,		1 },
  { "$f2",	rt_fpr,		2 },
  { "$f3",	rt_fpr,		3 },
  { "$f4",	rt_fpr,		4 },
  { "$f5",	rt_fpr,		5 },
  { "$f6",	rt_fpr,		6 },
  { "$f7",	rt_fpr,		7 },

  /* floating point register file - integer precision */
  { "$l0",	rt_lpr,		0 },
  { "$l1",	rt_lpr,		1 },
  { "$l2",	rt_lpr,		2 },
  { "$l3",	rt_lpr,		3 },
  { "$l4",	rt_lpr,		4 },
  { "$l5",	rt_lpr,		5 },
  { "$l6",	rt_lpr,		6 },
  { "$l7",	rt_lpr,		7 },

  /* miscellaneous registers */
  { "$cpsr",	rt_ctrl,	0 },
  { "$spsr",	rt_ctrl,	1 },
  { "$fpsr",	rt_ctrl,	2 },

  /* program counters */
  { "$pc",	rt_PC,		0 },
  { "$npc",	rt_NPC,		0 },

  /* sentinel */
  { NULL,	rt_gpr,		0 }
};

/* returns a register name string */
char *
md_reg_name(enum md_reg_type rt, int reg)
{
  int i;

  for (i=0; md_reg_names[i].str != NULL; i++)
    {
      if (md_reg_names[i].file == rt && md_reg_names[i].reg == reg)
	return md_reg_names[i].str;
    }

  /* no found... */
  return NULL;
}

char *						/* err str, NULL for no err */
md_reg_obj(struct regs_t *regs,			/* registers to access */
	   int is_write,			/* access type */
	   enum md_reg_type rt,			/* reg bank to probe */
	   int reg,				/* register number */
	   struct eval_value_t *val)		/* input, output */
{
  switch (rt)
    {
    case rt_gpr:
      if (reg < 0 || reg >= MD_NUM_IREGS)
	return "register number out of range";

      if (!is_write)
	{
	  val->type = et_qword;
	  val->value.as_qword = regs->regs_R[reg];
	}
      else
	regs->regs_R[reg] = eval_as_qword(*val);
      break;

    case rt_lpr:
      if (reg < 0 || reg >= MD_NUM_FREGS)
	return "register number out of range";

      if (!is_write)
	{
	  val->type = et_qword;
	  val->value.as_qword = regs->regs_F.q[reg];
	}
      else
	regs->regs_F.q[reg] = eval_as_qword(*val);
      break;

    case rt_fpr:
      if (reg < 0 || reg >= MD_NUM_FREGS)
	return "register number out of range";

      if (!is_write)
	{
	  val->type = et_double;
	  val->value.as_double = regs->regs_F.d[reg];
	}
      else
	regs->regs_F.d[reg] = eval_as_double(*val);
      break;

#if XXX
    case rt_ctrl:
      switch (reg)
	{
	case /* FPSR */0:
	  if (!is_write)
	    {
	      val->type = et_qword;
	      val->value.as_qword = regs->regs_C.fpsr;
	    }
	  else
	    regs->regs_C.fpsr = eval_as_qword(*val);
	  break;

	case /* UNIQ */1:
	  if (!is_write)
	    {
	      val->type = et_qword;
	      val->value.as_qword = regs->regs_C.uniq;
	    }
	  else
	    regs->regs_C.uniq = eval_as_qword(*val);
	  break;

	default:
	  return "register number out of range";
	}
      break;
#endif

    case rt_PC:
      if (!is_write)
	{
	  val->type = et_addr;
	  val->value.as_addr = regs->regs_PC;
	}
      else
	regs->regs_PC = eval_as_addr(*val);
      break;

    case rt_NPC:
      if (!is_write)
	{
	  val->type = et_addr;
	  val->value.as_addr = regs->regs_NPC;
	}
      else
	regs->regs_NPC = eval_as_addr(*val);
      break;

    default:
      panic("bogus register bank");
    }

  /* no error */
  return NULL;
}

/* print integer REG(S) to STREAM */
void
md_print_ireg(md_gpr_t regs, int reg, FILE *stream)
{
  myfprintf(stream, "%4s: %16d/0x%08x",
	    md_reg_name(rt_gpr, reg), regs[reg], regs[reg]);
}

void
md_print_iregs(md_gpr_t regs, FILE *stream)
{
  int i;

  for (i=0; i < MD_NUM_IREGS; i += 2)
    {
      md_print_ireg(regs, i, stream);
      fprintf(stream, "  ");
      md_print_ireg(regs, i+1, stream);
      fprintf(stream, "\n");
    }
}

/* print floating point REGS to STREAM */
void
md_print_fpreg(md_fpr_t regs, int reg, FILE *stream)
{
  myfprintf(stream, "%4s: %16ld/0x%012lx/%f",
	    md_reg_name(rt_fpr, reg), regs.q[reg], regs.q[reg], regs.d[reg]);
}

void
md_print_fpregs(md_fpr_t regs, FILE *stream)
{
  int i;

  /* floating point registers */
  for (i=0; i < MD_NUM_FREGS; i += 2)
    {
      md_print_fpreg(regs, i, stream);
      fprintf(stream, "\n");

      md_print_fpreg(regs, i+1, stream);
      fprintf(stream, "\n");
    }
}

void
md_print_creg(md_ctrl_t regs, int reg, FILE *stream)
{
  /* index is only used to iterate over these registers, hence no enums... */
  switch (reg)
    {
    case 0:
      myfprintf(stream, "CPSR: 0x%08x", regs.cpsr);
      break;

    case 1:
      myfprintf(stream, "SPSR: 0x%08x", regs.spsr);
      break;

    case 2:
      myfprintf(stream, "FPSR: 0x%08x", regs.fpsr);
      break;

    default:
      panic("bogus control register index");
    }
}

void
md_print_cregs(md_ctrl_t regs, FILE *stream)
{
  md_print_creg(regs, 0, stream);
  fprintf(stream, "  ");
  md_print_creg(regs, 1, stream);
  fprintf(stream, "  ");
  md_print_creg(regs, 2, stream);
  fprintf(stream, "  ");
}

/* xor checksum registers */
word_t
md_xor_regs(struct regs_t *regs)
{
  int i;
  qword_t checksum = 0;

  for (i=0; i < 15; i++)
    checksum ^= regs->regs_R[i];

  //  for (i=0; i < MD_NUM_FREGS; i++)
  //    checksum ^= regs->regs_F.q[i];

  checksum ^= regs->regs_C.cpsr;
  checksum ^= regs->regs_C.spsr;
  checksum ^= regs->regs_C.fpsr;
  //  checksum ^= regs->regs_PC;
  //  checksum ^= regs->regs_NPC;

  return (word_t)((checksum >> 32) ^ checksum);
}


/* enum md_opcode -> opcode flags, used by simulators */
unsigned int md_op2flags[OP_MAX] = {
  NA, /* NA */
#define DEFINST(OP,MSK,NAME,OPFORM,RES,FLAGS,O1,O2,O3,I1,I2,I3,I4) FLAGS,
#define DEFUOP(OP,NAME,OPFORM,RES,FLAGS,O1,O2,O3,I1,I2,I3,I4) FLAGS,
#define DEFLINK(OP,MSK,NAME,MASK,SHIFT) NA,
#define CONNECT(OP)
#include "machine.def"
};


static unsigned long
md_set_decoder(char *name,
	       unsigned long mskbits, unsigned long offset,
	       enum md_opcode op, unsigned long max_offset)
{
  unsigned long msk_base = mskbits & 0xff;
  unsigned long msk_bound = (mskbits >> 8) & 0xff;
  unsigned long msk;

  msk = msk_base;
  do {
    if ((msk + offset) >= MD_MAX_MASK)
      panic("MASK_MAX is too small, inst=`%s', index=%d",
	    name, msk + offset);
    if (debugging && md_mask2op[msk + offset])
      warn("doubly defined opcode, inst=`%s', index=%d",
	    name, msk + offset);

    md_mask2op[msk + offset] = op;
    msk++;
  } while (msk <= msk_bound);

  return MAX(max_offset, (msk-1) + offset);
}


/* intialize the inst decoder, this function builds the ISA decode tables */
void
md_init_decoder(void)
{
  unsigned long max_offset = 0;
  unsigned long offset = 0;

#define DEFINST(OP,MSK,NAME,OPFORM,RES,FLAGS,O1,O2,O3,I1,I2,I3,I4)	\
  max_offset = md_set_decoder(NAME, (MSK), offset, (OP), max_offset);
#define DEFLINK(OP,MSK,NAME,MASK,SHIFT)					\
  max_offset = md_set_decoder(NAME, (MSK), offset, (OP), max_offset);
#define CONNECT(OP)							\
    offset = max_offset+1;						\
    if (debugging && md_opoffset[OP])					\
      warn("doubly defined opoffset, inst=`%s', op=%d, offset=%d",	\
	    #OP, (int)(OP), offset);					\
    md_opoffset[OP] = offset;

#include "machine.def"

  if (max_offset >= MD_MAX_MASK)
    panic("MASK_MAX is too small, index==%d", max_offset);
}

/* instruction assemblers for UOP flow generator */
#define AGEN(C,URD,URN,SHAMT,SHFT,RM)					\
  (0x00000000 | ((C) << 28) | ((URN) << 17) | ((URD) << 12)		\
   | ((SHAMT) << 7) | ((SHFT) << 5) | (RM))
#define AGENI(C,URD,URN,IMM)						\
  (0x00000000 | ((C) << 28) | ((URN) << 17) | ((URD) << 12) | (IMM))
#define LDSTP(C,RD,URN)							\
  (0x00000000 | ((C) << 28) | ((RD) << 12) | ((URN) << 17))

#if 0
#define MOVA(C,RM)		(0x00000000 | ((C) << 28) | (RM))
#define ADD(C,RD,RN,SHAMT,SHFT,RM)					\
  (0x00800000 | ((C) << 28) | ((RN) << 16) | ((RD) << 12)		\
   | ((SHAMT) << 7) | ((SHFT) << 5) | (RM))
#define ADDI(C,RD,RN,SHFT,IMM)						\
  (0x02800000 | ((C) << 28) | ((RN) << 16) | ((RD) << 12)		\
   | ((SHFT) << 8) | (IMM))
#define SUB(C,RD,RN,SHAMT,SHFT,RM)					\
  (0x00400000 | ((C) << 28) | ((RN) << 16) | ((RD) << 12)		\
   | ((SHAMT) << 7) | ((SHFT) << 5) | (RM))
#define SUBI(C,RD,RN,SHFT,IMM)						\
  (0x02400000 | ((C) << 28) | ((RN) << 16) | ((RD) << 12)		\
   | ((SHFT) << 8) | (IMM))
#define STR(C,P,U,W,RD,RN,OFS)						\
  (0x04000000 | ((C) << 28) | ((P) << 24) | ((U) << 23) | ((W) << 21)	\
   | ((RD) << 12) | ((RN) << 16) | (OFS))
#define LDR(C,P,U,W,RD,RN,OFS)						\
  (0x04100000 | ((C) << 28) | ((P) << 24) | ((U) << 23) | ((W) << 21)	\
   | ((RD) << 12) | ((RN) << 16) | (OFS))
/* FIXME: bits missing! */
#define STFD(C,P,U,W,FD,RN,FPOFS)					\
  (0x00000000 | ((C) << 28) | ((P) << 24) | ((U) << 23) | ((W) << 21)	\
   | ((FD) << 12) | ((RN) << 16) | (FPOFS))
#define LDFD(C,P,U,W,FD,RN,FPOFS)					\
  (0x00000000 | ((C) << 28) | ((P) << 24) | ((U) << 23) | ((W) << 21)	\
   | ((FD) << 12) | ((RN) << 16) | (FPOFS))
#define AGEN(C,RD,RN,SHAMT,SHFT,RM)					\
  (0x00800000 | ((C) << 28) | ((RN) << 16) | ((RD) << 12)		\
   | ((SHAMT) << 7) | ((SHFT) << 5) | (RM))
#define AGENI(C,RD,RN,IMM)						\
  (0x02800000 | ((C) << 28) | ((RN) << 16) | ((RD) << 12) | (IMM))
#endif

enum type_spec { type_byte, type_sbyte, type_half, type_shalf, type_word,
		 type_float, type_double, type_extended };
enum ofs_spec { ofs_rm, ofs_shiftrm, ofs_offset };

int
md_emit_ldst(struct md_uop_t *flow,
	     int cond, int load, int pre, int writeback,
	     /* type and format */enum type_spec type, enum ofs_spec ofs,
	     /* dest */int rd,
	     /* src1 */int urn,
	     /* src2 */int shamt, int shift_type, int rm,
	     /* src2 */int sign, unsigned offset)
{
  int nuops = 0, eareg;

  if (pre)
    {
      /* pre-update */
      if (writeback)
	eareg = urn;
      else
	eareg = MD_REG_TMP0;

      /* emit base register update */
      switch (ofs)
	{
	case ofs_rm:
	  flow[nuops].op = (sign == 1) ? AGEN_U : AGEN;
	  flow[nuops++].inst = AGEN(cond, eareg, urn, 0, 0, rm);
	  break;
	case ofs_shiftrm:
	  flow[nuops].op = (sign == 1) ? AGEN_U : AGEN;
	  flow[nuops++].inst = AGEN(cond, eareg, urn, shamt, shift_type, rm);
	  break;
	case ofs_offset:
	  flow[nuops].op = (sign == 1) ? AGENI_U : AGENI;
	  flow[nuops++].inst = AGENI(cond, eareg, urn, offset);
	  break;
	default:
	  panic("bogus offset specifier");
	}

      /* emit load/store with base register access */
      switch (type)
	{
	case type_byte:
	  flow[nuops].op = load ? LDP_B : STP_B;
	  flow[nuops++].inst = LDSTP(cond, rd, eareg);
	  break;
	case type_sbyte:
	  flow[nuops].op = load ? LDP_SB : STP_B;
	  flow[nuops++].inst = LDSTP(cond, rd, eareg);
	  break;
	case type_half:
	  flow[nuops].op = load ? LDP_H : STP_H;
	  flow[nuops++].inst = LDSTP(cond, rd, eareg);
	  break;
	case type_shalf:
	  flow[nuops].op = load ? LDP_SH : STP_H;
	  flow[nuops++].inst = LDSTP(cond, rd, eareg);
	  break;
	case type_word:
	  flow[nuops].op = load ? LDP_W : STP_W;
	  flow[nuops++].inst = LDSTP(cond, rd, eareg);
	  break;
	case type_float:
	  flow[nuops].op = load ? LDP_S : STP_S;
	  flow[nuops++].inst = LDSTP(cond, /* FD */rd & 0x07, eareg);
	  break;
	case type_double:
	  flow[nuops].op = load ? LDP_D : STP_D;
	  flow[nuops++].inst = LDSTP(cond, /* FD */rd & 0x07, eareg);
	  break;
	case type_extended:
	  flow[nuops].op = load ? LDP_E : STP_E;
	  flow[nuops++].inst = LDSTP(cond, /* FD */rd & 0x07, eareg);
	  break;
	default:
	  panic("bogus type specifier");
	}
    }
  else
    {
      /* post-update */
      if (!writeback)
	panic("post-update without writeback");

      /* emit load/store with base register access */
      switch (type)
	{
	case type_byte:
	  flow[nuops].op = load ? LDP_B : STP_B;
	  flow[nuops++].inst = LDSTP(cond, rd, urn);
	  break;
	case type_sbyte:
	  flow[nuops].op = load ? LDP_SB : STP_B;
	  flow[nuops++].inst = LDSTP(cond, rd, urn);
	  break;
	case type_half:
	  flow[nuops].op = load ? LDP_H : STP_H;
	  flow[nuops++].inst = LDSTP(cond, rd, urn);
	  break;
	case type_shalf:
	  flow[nuops].op = load ? LDP_SH : STP_H;
	  flow[nuops++].inst = LDSTP(cond, rd, urn);
	  break;
	case type_word:
	  flow[nuops].op = load ? LDP_W : STP_W;
	  flow[nuops++].inst = LDSTP(cond, rd, urn);
	  break;
	case type_float:
	  flow[nuops].op = load ? LDP_S : STP_S;
	  flow[nuops++].inst = LDSTP(cond, /* FD */rd & 0x07, urn);
	  break;
	case type_double:
	  flow[nuops].op = load ? LDP_D : STP_D;
	  flow[nuops++].inst = LDSTP(cond, /* FD */rd & 0x07, urn);
	  break;
	case type_extended:
	  flow[nuops].op = load ? LDP_E : STP_E;
	  flow[nuops++].inst = LDSTP(cond, /* FD */rd & 0x07, urn);
	  break;
	default:
	  panic("bogus type specifier");
	}

      /* emit base register update */
      switch (ofs)
	{
	case ofs_rm:
	  flow[nuops].op = (sign == 1) ? AGEN_U : AGEN;
	  flow[nuops++].inst = AGEN(cond, urn, urn, 0, 0, rm);
	  break;
	case ofs_shiftrm:
	  flow[nuops].op = (sign == 1) ? AGEN_U : AGEN;
	  flow[nuops++].inst = AGEN(cond, urn, urn, shamt, shift_type, rm);
	  break;
	case ofs_offset:
	  flow[nuops].op = (sign == 1) ? AGENI_U : AGENI;
	  flow[nuops++].inst = AGENI(cond, urn, urn, offset);
	  break;
	default:
	  panic("bogus offset specifier");
	}
    }
  return nuops;
}

/* UOP flow generator, returns a small non-cyclic program implementing OP,
   returns length of flow returned */
int
md_get_flow(enum md_opcode op, md_inst_t inst,
	    struct md_uop_t flow[MD_MAX_FLOWLEN])
{
  int nuops = 0;
  int nregs = ONES(REGLIST);
  int cond = (inst >> 28) & 0x0f;
  int offset, sign, pre, writeback, load;
  enum type_spec type;
  enum ofs_spec ofs;

  switch (op)
    {
    case STM:
      offset = nregs*4 - 4;
      sign = -1;
      goto do_STM;
    case STM_U:
      offset = 0;
      sign = 1;
      goto do_STM;
    case STM_PU:
      offset = 4;
      sign = 1;
      goto do_STM;
    case STM_P:
      offset = nregs*4;
      sign = -1;
      goto do_STM;

    do_STM:
      {
	int i, rd = 0, rn = (RN);

	for (i=0; i < nregs; i++,rd++)
	  {
	    while ((REGLIST & (1 << rd)) == 0) rd++;
	    nuops +=
	      md_emit_ldst(&flow[nuops],
			   cond, /* st */FALSE, /* pre */TRUE, /* !wb */FALSE,
			   /* type and format */type_word, ofs_offset,
			   /* dest */rd,
			   /* src1 */rn,
			   /* src2 */0, 0, 0,
			   /* src2 */sign, sign*i*4+offset);
#if 0
	    flow[nuops].op = (sign == 1) ? STR_PU : STR_P;
	    flow[nuops++].inst =
	      STR(cond,/*P*/1,/*U*/(sign == 1),/*W*/0,
		  /*RD*/rd,/*RN*/rn,/*OFS*/sign*i*4+offset);
#endif
	  }
      }
      break; /* return nuops; */

    case LDM_L:
      offset = nregs*4 - 4;
      sign = -1;
      goto do_LDM;
    case LDM_UL:
      offset = 0;
      sign = 1;
      goto do_LDM;
    case LDM_PUL:
      offset = 4;
      sign = 1;
      goto do_LDM;
    case LDM_PL:
      offset = nregs*4;
      sign = -1;
      goto do_LDM;

    do_LDM:
      {
	int i, rd = 0, rn = (RN);

	flow[nuops].op = AGENI_U;
	flow[nuops++].inst = AGENI(cond, MD_REG_TMP1, rn, /* mov */0);
#if 0
	flow[nuops].op = MOVA;
	flow[nuops++].inst = MOVA(cond,/*RM*/rn);
#endif

	for (i=0; i < nregs; i++,rd++)
	  {
	    while ((REGLIST & (1 << rd)) == 0) rd++;
	    nuops +=
	      md_emit_ldst(&flow[nuops],
			   cond, /* ld */TRUE, /* pre */TRUE, /* !wb */FALSE,
			   /* type and format */type_word, ofs_offset,
			   /* dest */rd,
			   /* src1 */MD_REG_TMP1,
			   /* src2 */0, 0, 0,
			   /* src2 */sign, sign*i*4+offset);
#if 0
	    flow[nuops].op = (sign == 1) ? LDA_PU : LDA_P;
	    flow[nuops++].inst =
	      LDR(cond,/*P*/1,/*U*/(sign == 1),/*W*/0,
		  /*RD*/rd,/*RN*/0,/*OFS*/sign*i*4+offset);
#endif
	  }
      }
      break; /* return nuops; */

    case STM_W:
      offset = nregs*4 - 4;
      sign = -1;
      goto do_STM_W;
    case STM_UW:
      offset = 0;
      sign = 1;
      goto do_STM_W;
    case STM_PUW:
      offset = 4;
      sign = 1;
      goto do_STM_W;
    case STM_PW:
      offset = nregs*4;
      sign = -1;
      goto do_STM_W;

    do_STM_W:
      {
	int i, rd = 0, rn = (RN);

	flow[nuops].op = AGENI_U;
	flow[nuops++].inst = AGENI(cond, MD_REG_TMP1, rn, /* mov */0);
#if 0
	flow[nuops].op = MOVA;
	flow[nuops++].inst = MOVA(cond,/*RM*/rn);
#endif

	while ((REGLIST & (1 << rd)) == 0) rd++;
	nuops +=
	  md_emit_ldst(&flow[nuops],
		       cond, /* st */FALSE, /* pre */TRUE, /* !wb */FALSE,
		       /* type and format */type_word, ofs_offset,
		       /* dest */rd,
		       /* src1 */MD_REG_TMP1,
		       /* src2 */0, 0, 0,
		       /* src2 */sign, sign*0*4+offset);
#if 0
	flow[nuops].op = (sign == 1) ? STA_PU : STA_P;
	flow[nuops++].inst =
	  STR(cond,/*P*/1,/*U*/(sign == 1),/*W*/0,
	      /*RD*/rd,/*RN*/0,/*OFS*/sign*0*4+offset);
#endif

	flow[nuops].op = (sign == 1) ? AGENI_U : AGENI;
	flow[nuops++].inst = AGENI(cond, rn, rn, nregs*4);
#if 0
	flow[nuops].op = (sign == 1) ? ADDI : SUBI;
	flow[nuops++].inst =
	  ((sign == 1)
	   ? ADDI(cond, rn, rn, 0, nregs*4)
	   : SUBI(cond, rn, rn, 0, nregs*4));
#endif

	for (rd++,i=1; i < nregs; i++,rd++)
	  {
	    while ((REGLIST & (1 << rd)) == 0) rd++;
	    nuops +=
	      md_emit_ldst(&flow[nuops],
			   cond, /* st */FALSE, /* pre */TRUE, /* !wb */FALSE,
			   /* type and format */type_word, ofs_offset,
			   /* dest */rd,
			   /* src1 */MD_REG_TMP1,
			   /* src2 */0, 0, 0,
			   /* src2 */sign, sign*i*4+offset);
#if 0
	    flow[nuops].op = (sign == 1) ? STA_PU : STA_P;
	    flow[nuops++].inst =
	      STR(cond,/*P*/1,/*U*/(sign == 1),/*W*/0,
		  /*RD*/rd,/*RN*/0,/*OFS*/sign*i*4+offset);
#endif
	  }
      }
      break; /* return nuops; */

    case LDM_WL:
      offset = nregs*4 - 4;
      sign = -1;
      goto do_LDM_W;
    case LDM_UWL:
      offset = 0;
      sign = 1;
      goto do_LDM_W;
    case LDM_PUWL:
      offset = 4;
      sign = 1;
      goto do_LDM_W;
    case LDM_PWL:
      offset = nregs*4;
      sign = -1;
      goto do_LDM_W;

    do_LDM_W:
      {
	int i, rd = 0, rn = (RN);

	flow[nuops].op = AGENI_U;
	flow[nuops++].inst = AGENI(cond, MD_REG_TMP1, rn, /* mov */0);

	while ((REGLIST & (1 << rd)) == 0) rd++;
	nuops +=
	  md_emit_ldst(&flow[nuops],
		       cond, /* ld */TRUE, /* pre */TRUE, /* !wb */FALSE,
		       /* type and format */type_word, ofs_offset,
		       /* dest */rd,
		       /* src1 */MD_REG_TMP1,
		       /* src2 */0, 0, 0,
		       /* src2 */sign, sign*0*4+offset);
#if 0
	flow[nuops].op = (sign == 1) ? LDA_PU : LDA_P;
	flow[nuops++].inst =
	  LDR(cond,/*P*/1,/*U*/(sign == 1),/*W*/0,
	      /*RD*/rd,/*RN*/0,/*OFS*/sign*0*4+offset);
#endif

	flow[nuops].op = (sign == 1) ? AGENI_U : AGENI;
	flow[nuops++].inst = AGENI(cond, rn, rn, nregs*4);

	for (rd++,i=1; i < nregs; i++,rd++)
	  {
	    while ((REGLIST & (1 << rd)) == 0) rd++;
	    nuops +=
	      md_emit_ldst(&flow[nuops],
			   cond, /* ld */TRUE, /* pre */TRUE, /* !wb */FALSE,
			   /* type and format */type_word, ofs_offset,
			   /* dest */rd,
			   /* src1 */MD_REG_TMP1,
			   /* src2 */0, 0, 0,
			   /* src2 */sign, sign*i*4+offset);
#if 0
	    flow[nuops].op = (sign == 1) ? LDA_PU : LDA_P;
	    flow[nuops++].inst =
	      LDR(cond,/*P*/1,/*U*/(sign == 1),/*W*/0,
		  /*RD*/rd,/*RN*/0,/*OFS*/sign*i*4+offset);
#endif
	  }
      }
      break; /* return nuops; */

    case SFM_P:
      sign = -1;
      goto do_SFM_P;

    case SFM_PU:
      sign = 1;
      goto do_SFM_P;

    do_SFM_P:
      {
	int i, rn = (RN);
	int count = FCNT ? FCNT : 4;

	for (i=0; i < count; i++)
	  {
	    nuops +=
	      md_emit_ldst(&flow[nuops],
			   cond, /* st */FALSE, /* pre */TRUE, /* !wb */FALSE,
			   /* type and format */type_double, ofs_offset,
			   /* dest */(FD)+i,
			   /* src1 */rn,
			   /* src2 */0, 0, 0,
			   /* src2 */sign, sign*i*8+FPOFS);
#if 0
	    flow[nuops].op = (sign == 1) ? STFD_PU : STFD_P;
	    flow[nuops++].inst =
	      STFD(cond,/*P*/1,/*U*/(sign == 1),/*W*/0,
		  /*RD*/(FD)+i,/*RN*/rn,/*OFS*/sign*i*8+FPOFS);
#endif
	  }
      }
      break; /* return nuops; */

    case LFM_PL:
      sign = -1;
      goto do_LFM_P;

    case LFM_PUL:
      sign = 1;
      goto do_LFM_P;

    do_LFM_P:
      {
	int i, rn = (RN);
	int count = FCNT ? FCNT : 4;

	for (i=0; i < count; i++)
	  {
	    nuops +=
	      md_emit_ldst(&flow[nuops],
			   cond, /* ld */TRUE, /* pre */TRUE, /* !wb */FALSE,
			   /* type and format */type_double, ofs_offset,
			   /* dest */(FD)+i,
			   /* src1 */rn,
			   /* src2 */0, 0, 0,
			   /* src2 */sign, sign*i*8+FPOFS);
#if 0
	    flow[nuops].op = (sign == 1) ? LDFD_PUL : LDFD_PL;
	    flow[nuops++].inst =
	      LDFD(cond,/*P*/1,/*U*/(sign == 1),/*W*/0,
		  /*RD*/(FD)+i,/*RN*/rn,/*OFS*/sign*i*8+FPOFS);
#endif
	  }
      }
      break; /* return nuops; */

    case SFM_W:
      sign = -1;
      pre = FALSE;
      goto do_SFM_PW;

    case SFM_UW:
      sign = 1;
      pre = FALSE;
      goto do_SFM_PW;

    case SFM_PW:
      sign = -1;
      pre = TRUE;
      goto do_SFM_PW;

    case SFM_PUW:
      sign = 1;
      pre = TRUE;
      goto do_SFM_PW;

    do_SFM_PW:
      {
	int i, rn = (RN);
	int count = FCNT ? FCNT : 4;

	if (pre)
	  {
	    flow[nuops].op = (sign == 1) ? AGENI_U : AGENI;
	    flow[nuops++].inst = AGENI(cond, rn, rn, FPOFS);
	  }
	for (i=0; i < count; i++)
	  {
	    nuops +=
	      md_emit_ldst(&flow[nuops],
			   cond, /* st */FALSE, /* pre */TRUE, /* !wb */FALSE,
			   /* type and format */type_double, ofs_offset,
			   /* dest */(FD)+i,
			   /* src1 */rn,
			   /* src2 */0, 0, 0,
			   /* src2 */1, i*8);
#if 0
	    flow[nuops].op = (sign == 1) ? STFD_PU : STFD_P;
	    flow[nuops++].inst =
	      STFD(cond,/*P*/1,/*U*/(sign == 1),/*W*/0,
		  /*RD*/(FD)+i,/*RN*/rn,/*OFS*/i*8);
#endif
	  }
	if (!pre)
	  {
	    flow[nuops].op = (sign == 1) ? AGENI_U : AGENI;
	    flow[nuops++].inst = AGENI(cond, rn, rn, FPOFS);
	  }
      }
      break; /* return nuops; */

    case LFM_WL:
      sign = -1;
      pre = FALSE;
      goto do_LFM_PW;

    case LFM_UWL:
      sign = 1;
      pre = FALSE;
      goto do_LFM_PW;

    case LFM_PWL:
      sign = -1;
      pre = TRUE;
      goto do_LFM_PW;

    case LFM_PUWL:
      sign = 1;
      pre = TRUE;
      goto do_LFM_PW;

    do_LFM_PW:
      {
	int i, rn = (RN);
	int count = FCNT ? FCNT : 4;

	if (pre)
	  {
	    flow[nuops].op = (sign == 1) ? AGENI_U : AGENI;
	    flow[nuops++].inst = AGENI(cond, rn, rn, FPOFS);
	  }
	for (i=0; i < count; i++)
	  {
	    nuops +=
	      md_emit_ldst(&flow[nuops],
			   cond, /* ld */TRUE, /* pre */TRUE, /* !wb */FALSE,
			   /* type and format */type_double, ofs_offset,
			   /* dest */(FD)+i,
			   /* src1 */rn,
			   /* src2 */0, 0, 0,
			   /* src2 */1, i*8);
#if 0
	    flow[nuops].op = (sign == 1) ? LDFD_PUL : LDFD_PL;
	    flow[nuops++].inst =
	      LDFD(cond,/*P*/1,/*U*/(sign == 1),/*W*/0,
		  /*RD*/(FD)+i,/*RN*/rn,/*OFS*/i*8);
#endif
	  }
	if (!pre)
	  {
	    flow[nuops].op = (sign == 1) ? AGENI_U : AGENI;
	    flow[nuops++].inst = AGENI(cond, rn, rn, FPOFS);
	  }
      }
      break; /* return nuops; */

    case STRH_R:
      load = FALSE;
      type = type_half;
      pre = FALSE;
      writeback = TRUE;
      ofs = ofs_rm;
      sign = -1;
      offset = 0;
      goto do_STRLDR;
    case STRH_O:
      load = FALSE;
      type = type_half;
      pre = FALSE;
      writeback = TRUE;
      ofs = ofs_offset;
      sign = -1;
      offset = HOFS;
      goto do_STRLDR;
    case STRH_RU:
      load = FALSE;
      type = type_half;
      pre = FALSE;
      writeback = TRUE;
      ofs = ofs_rm;
      sign = 1;
      offset = 0;
      goto do_STRLDR;
    case STRH_OU:
      load = FALSE;
      type = type_half;
      pre = FALSE;
      writeback = TRUE;
      ofs = ofs_offset;
      sign = 1;
      offset = HOFS;
      goto do_STRLDR;
    case STRH_PR:
      load = FALSE;
      type = type_half;
      pre = TRUE;
      writeback = FALSE;
      ofs = ofs_rm;
      sign = -1;
      offset = 0;
      goto do_STRLDR;
    case STRH_PRW:
      load = FALSE;
      type = type_half;
      pre = TRUE;
      writeback = TRUE;
      ofs = ofs_rm;
      sign = -1;
      offset = 0;
      goto do_STRLDR;
    case STRH_PO:
      load = FALSE;
      type = type_half;
      pre = TRUE;
      writeback = FALSE;
      ofs = ofs_offset;
      sign = -1;
      offset = HOFS;
      goto do_STRLDR;
    case STRH_POW:
      load = FALSE;
      type = type_half;
      pre = TRUE;
      writeback = TRUE;
      ofs = ofs_offset;
      sign = -1;
      offset = HOFS;
      goto do_STRLDR;
    case STRH_PRU:
      load = FALSE;
      type = type_half;
      pre = TRUE;
      writeback = FALSE;
      ofs = ofs_rm;
      sign = 1;
      offset = 0;
      goto do_STRLDR;
    case STRH_PRUW:
      load = FALSE;
      type = type_half;
      pre = TRUE;
      writeback = TRUE;
      ofs = ofs_rm;
      sign = 1;
      offset = 0;
      goto do_STRLDR;
    case STRH_POU:
      load = FALSE;
      type = type_half;
      pre = TRUE;
      writeback = FALSE;
      ofs = ofs_offset;
      sign = 1;
      offset = HOFS;
      goto do_STRLDR;
    case STRH_POUW:
      load = FALSE;
      type = type_half;
      pre = TRUE;
      writeback = TRUE;
      ofs = ofs_offset;
      sign = 1;
      offset = HOFS;
      goto do_STRLDR;
    case STR:
      load = FALSE;
      type = type_word;
      pre = FALSE;
      writeback = TRUE;
      ofs = ofs_offset;
      sign = -1;
      offset = OFS;
      goto do_STRLDR;
    case STR_B:
      load = FALSE;
      type = type_byte;
      pre = FALSE;
      writeback = TRUE;
      ofs = ofs_offset;
      sign = -1;
      offset = OFS;
      goto do_STRLDR;
    case STR_U:
      load = FALSE;
      type = type_word;
      pre = FALSE;
      writeback = TRUE;
      ofs = ofs_offset;
      sign = 1;
      offset = OFS;
      goto do_STRLDR;
    case STR_UB:
      load = FALSE;
      type = type_byte;
      pre = FALSE;
      writeback = TRUE;
      ofs = ofs_offset;
      sign = 1;
      offset = OFS;
      goto do_STRLDR;
    case STR_P:
      load = FALSE;
      type = type_word;
      pre = TRUE;
      writeback = FALSE;
      ofs = ofs_offset;
      sign = -1;
      offset = OFS;
      goto do_STRLDR;
    case STR_PW:
      load = FALSE;
      type = type_word;
      pre = TRUE;
      writeback = TRUE;
      ofs = ofs_offset;
      sign = -1;
      offset = OFS;
      goto do_STRLDR;
    case STR_PB:
      load = FALSE;
      type = type_byte;
      pre = TRUE;
      writeback = FALSE;
      ofs = ofs_offset;
      sign = -1;
      offset = OFS;
      goto do_STRLDR;
    case STR_PBW:
      load = FALSE;
      type = type_byte;
      pre = TRUE;
      writeback = TRUE;
      ofs = ofs_offset;
      sign = -1;
      offset = OFS;
      goto do_STRLDR;
    case STR_PU:
      load = FALSE;
      type = type_word;
      pre = TRUE;
      writeback = FALSE;
      ofs = ofs_offset;
      sign = 1;
      offset = OFS;
      goto do_STRLDR;
    case STR_PUW:
      load = FALSE;
      type = type_word;
      pre = TRUE;
      writeback = TRUE;
      ofs = ofs_offset;
      sign = 1;
      offset = OFS;
      goto do_STRLDR;
    case STR_PUB:
      load = FALSE;
      type = type_byte;
      pre = TRUE;
      writeback = FALSE;
      ofs = ofs_offset;
      sign = 1;
      offset = OFS;
      goto do_STRLDR;
    case STR_PUBW:
      load = FALSE;
      type = type_byte;
      pre = TRUE;
      writeback = TRUE;
      ofs = ofs_offset;
      sign = 1;
      offset = OFS;
      goto do_STRLDR;
    case STR_R:
      load = FALSE;
      type = type_word;
      pre = FALSE;
      writeback = TRUE;
      ofs = ofs_shiftrm;
      sign = -1;
      offset = 0;
      goto do_STRLDR;
    case STR_RB:
      load = FALSE;
      type = type_byte;
      pre = FALSE;
      writeback = TRUE;
      ofs = ofs_shiftrm;
      sign = -1;
      offset = 0;
      goto do_STRLDR;
    case STR_RU:
      load = FALSE;
      type = type_word;
      pre = FALSE;
      writeback = TRUE;
      ofs = ofs_shiftrm;
      sign = 1;
      offset = 0;
      goto do_STRLDR;
    case STR_RUB:
      load = FALSE;
      type = type_byte;
      pre = FALSE;
      writeback = TRUE;
      ofs = ofs_shiftrm;
      sign = 1;
      offset = 0;
      goto do_STRLDR;
    case STR_RP:
      load = FALSE;
      type = type_word;
      pre = TRUE;
      writeback = FALSE;
      ofs = ofs_shiftrm;
      sign = -1;
      offset = 0;
      goto do_STRLDR;
    case STR_RPW:
      load = FALSE;
      type = type_word;
      pre = TRUE;
      writeback = TRUE;
      ofs = ofs_shiftrm;
      sign = -1;
      offset = 0;
      goto do_STRLDR;
    case STR_RPB:
      load = FALSE;
      type = type_byte;
      pre = TRUE;
      writeback = FALSE;
      ofs = ofs_shiftrm;
      sign = -1;
      offset = 0;
      goto do_STRLDR;
    case STR_RPBW:
      load = FALSE;
      type = type_byte;
      pre = TRUE;
      writeback = TRUE;
      ofs = ofs_shiftrm;
      sign = -1;
      offset = 0;
      goto do_STRLDR;
    case STR_RPU:
      load = FALSE;
      type = type_word;
      pre = TRUE;
      writeback = FALSE;
      ofs = ofs_shiftrm;
      sign = 1;
      offset = 0;
      goto do_STRLDR;
    case STR_RPUW:
      load = FALSE;
      type = type_word;
      pre = TRUE;
      writeback = TRUE;
      ofs = ofs_shiftrm;
      sign = 1;
      offset = 0;
      goto do_STRLDR;
    case STR_RPUB:
      load = FALSE;
      type = type_byte;
      pre = TRUE;
      writeback = FALSE;
      ofs = ofs_shiftrm;
      sign = 1;
      offset = 0;
      goto do_STRLDR;
    case STR_RPUBW:
      load = FALSE;
      type = type_byte;
      pre = TRUE;
      writeback = TRUE;
      ofs = ofs_shiftrm;
      sign = 1;
      offset = 0;
      goto do_STRLDR;

    case STFS_W:
      load = FALSE;
      type = type_float;
      pre = FALSE;
      writeback = TRUE;
      ofs = ofs_offset;
      sign = -1;
      offset = FPOFS;
      goto do_STRLDR;
    case STFD_W:
      load = FALSE;
      type = type_double;
      pre = FALSE;
      writeback = TRUE;
      ofs = ofs_offset;
      sign = -1;
      offset = FPOFS;
      goto do_STRLDR;
    case STFE_W:
      load = FALSE;
      type = type_extended;
      pre = FALSE;
      writeback = TRUE;
      ofs = ofs_offset;
      sign = -1;
      offset = FPOFS;
      goto do_STRLDR;
    case STFS_UW:
      load = FALSE;
      type = type_float;
      pre = FALSE;
      writeback = TRUE;
      ofs = ofs_offset;
      sign = 1;
      offset = FPOFS;
      goto do_STRLDR;
    case STFD_UW:
      load = FALSE;
      type = type_double;
      pre = FALSE;
      writeback = TRUE;
      ofs = ofs_offset;
      sign = 1;
      offset = FPOFS;
      goto do_STRLDR;
    case STFE_UW:
      load = FALSE;
      type = type_extended;
      pre = FALSE;
      writeback = TRUE;
      ofs = ofs_offset;
      sign = 1;
      offset = FPOFS;
      goto do_STRLDR;
    case STFS_P:
      load = FALSE;
      type = type_float;
      pre = TRUE;
      writeback = FALSE;
      ofs = ofs_offset;
      sign = -1;
      offset = FPOFS;
      goto do_STRLDR;
    case STFD_P:
      load = FALSE;
      type = type_double;
      pre = TRUE;
      writeback = FALSE;
      ofs = ofs_offset;
      sign = -1;
      offset = FPOFS;
      goto do_STRLDR;
    case STFS_PW:
      load = FALSE;
      type = type_float;
      pre = TRUE;
      writeback = TRUE;
      ofs = ofs_offset;
      sign = -1;
      offset = FPOFS;
      goto do_STRLDR;
    case STFD_PW:
      load = FALSE;
      type = type_double;
      pre = TRUE;
      writeback = TRUE;
      ofs = ofs_offset;
      sign = -1;
      offset = FPOFS;
      goto do_STRLDR;
    case STFE_P:
      load = FALSE;
      type = type_extended;
      pre = TRUE;
      writeback = FALSE;
      ofs = ofs_offset;
      sign = -1;
      offset = FPOFS;
      goto do_STRLDR;
    case STFE_PW:
      load = FALSE;
      type = type_extended;
      pre = TRUE;
      writeback = TRUE;
      ofs = ofs_offset;
      sign = -1;
      offset = FPOFS;
      goto do_STRLDR;
    case STFS_PU:
      load = FALSE;
      type = type_float;
      pre = TRUE;
      writeback = FALSE;
      ofs = ofs_offset;
      sign = 1;
      offset = FPOFS;
      goto do_STRLDR;
    case STFD_PU:
      load = FALSE;
      type = type_double;
      pre = TRUE;
      writeback = FALSE;
      ofs = ofs_offset;
      sign = 1;
      offset = FPOFS;
      goto do_STRLDR;
    case STFS_PUW:
      load = FALSE;
      type = type_float;
      pre = TRUE;
      writeback = TRUE;
      ofs = ofs_offset;
      sign = 1;
      offset = FPOFS;
      goto do_STRLDR;
    case STFD_PUW:
      load = FALSE;
      type = type_double;
      pre = TRUE;
      writeback = TRUE;
      ofs = ofs_offset;
      sign = 1;
      offset = FPOFS;
      goto do_STRLDR;
    case STFE_PU:
      load = FALSE;
      type = type_extended;
      pre = TRUE;
      writeback = FALSE;
      ofs = ofs_offset;
      sign = 1;
      offset = FPOFS;
      goto do_STRLDR;
    case STFE_PUW:
      load = FALSE;
      type = type_extended;
      pre = TRUE;
      writeback = TRUE;
      ofs = ofs_offset;
      sign = 1;
      offset = FPOFS;
      goto do_STRLDR;

    case LDRH_RL:
      load = TRUE;
      type = type_half;
      pre = FALSE;
      writeback = TRUE;
      ofs = ofs_rm;
      sign = -1;
      offset = 0;
      goto do_STRLDR;
    case LDRH_OL:
      load = TRUE;
      type = type_half;
      pre = FALSE;
      writeback = TRUE;
      ofs = ofs_offset;
      sign = -1;
      offset = HOFS;
      goto do_STRLDR;
    case LDRH_RUL:
      load = TRUE;
      type = type_half;
      pre = FALSE;
      writeback = TRUE;
      ofs = ofs_rm;
      sign = 1;
      offset = 0;
      goto do_STRLDR;
    case LDRH_OUL:
      load = TRUE;
      type = type_half;
      pre = FALSE;
      writeback = TRUE;
      ofs = ofs_offset;
      sign = 1;
      offset = HOFS;
      goto do_STRLDR;
    case LDRH_PRL:
      load = TRUE;
      type = type_half;
      pre = TRUE;
      writeback = FALSE;
      ofs = ofs_rm;
      sign = -1;
      offset = 0;
      goto do_STRLDR;
    case LDRH_PRWL:
      load = TRUE;
      type = type_half;
      pre = TRUE;
      writeback = TRUE;
      ofs = ofs_rm;
      sign = -1;
      offset = 0;
      goto do_STRLDR;
    case LDRH_POL:
      load = TRUE;
      type = type_half;
      pre = TRUE;
      writeback = FALSE;
      ofs = ofs_offset;
      sign = -1;
      offset = HOFS;
      goto do_STRLDR;
    case LDRH_POWL:
      load = TRUE;
      type = type_half;
      pre = TRUE;
      writeback = TRUE;
      ofs = ofs_offset;
      sign = -1;
      offset = HOFS;
      goto do_STRLDR;
    case LDRH_PRUL:
      load = TRUE;
      type = type_half;
      pre = TRUE;
      writeback = FALSE;
      ofs = ofs_rm;
      sign = 1;
      offset = 0;
      goto do_STRLDR;
    case LDRH_PRUWL:
      load = TRUE;
      type = type_half;
      pre = TRUE;
      writeback = TRUE;
      ofs = ofs_rm;
      sign = 1;
      offset = 0;
      goto do_STRLDR;
    case LDRH_POUL:
      load = TRUE;
      type = type_half;
      pre = TRUE;
      writeback = FALSE;
      ofs = ofs_offset;
      sign = 1;
      offset = HOFS;
      goto do_STRLDR;
    case LDRH_POUWL:
      load = TRUE;
      type = type_half;
      pre = TRUE;
      writeback = TRUE;
      ofs = ofs_offset;
      sign = 1;
      offset = HOFS;
      goto do_STRLDR;
    case LDR_L:
      load = TRUE;
      type = type_word;
      pre = FALSE;
      writeback = TRUE;
      ofs = ofs_offset;
      sign = -1;
      offset = OFS;
      goto do_STRLDR;
    case LDR_BL:
      load = TRUE;
      type = type_byte;
      pre = FALSE;
      writeback = TRUE;
      ofs = ofs_offset;
      sign = -1;
      offset = OFS;
      goto do_STRLDR;
    case LDR_UL:
      load = TRUE;
      type = type_word;
      pre = FALSE;
      writeback = TRUE;
      ofs = ofs_offset;
      sign = 1;
      offset = OFS;
      goto do_STRLDR;
    case LDR_UBL:
      load = TRUE;
      type = type_byte;
      pre = FALSE;
      writeback = TRUE;
      ofs = ofs_offset;
      sign = 1;
      offset = OFS;
      goto do_STRLDR;
    case LDR_PL:
      load = TRUE;
      type = type_word;
      pre = TRUE;
      writeback = FALSE;
      ofs = ofs_offset;
      sign = -1;
      offset = OFS;
      goto do_STRLDR;
    case LDR_PWL:
      load = TRUE;
      type = type_word;
      pre = TRUE;
      writeback = TRUE;
      ofs = ofs_offset;
      sign = -1;
      offset = OFS;
      goto do_STRLDR;
    case LDR_PBL:
      load = TRUE;
      type = type_byte;
      pre = TRUE;
      writeback = FALSE;
      ofs = ofs_offset;
      sign = -1;
      offset = OFS;
      goto do_STRLDR;
    case LDR_PBWL:
      load = TRUE;
      type = type_byte;
      pre = TRUE;
      writeback = TRUE;
      ofs = ofs_offset;
      sign = -1;
      offset = OFS;
      goto do_STRLDR;
    case LDR_PUL:
      load = TRUE;
      type = type_word;
      pre = TRUE;
      writeback = FALSE;
      ofs = ofs_offset;
      sign = 1;
      offset = OFS;
      goto do_STRLDR;
    case LDR_PUWL:
      load = TRUE;
      type = type_word;
      pre = TRUE;
      writeback = TRUE;
      ofs = ofs_offset;
      sign = 1;
      offset = OFS;
      goto do_STRLDR;
    case LDR_PUBL:
      load = TRUE;
      type = type_byte;
      pre = TRUE;
      writeback = FALSE;
      ofs = ofs_offset;
      sign = 1;
      offset = OFS;
      goto do_STRLDR;
    case LDR_PUBWL:
      load = TRUE;
      type = type_byte;
      pre = TRUE;
      writeback = TRUE;
      ofs = ofs_offset;
      sign = 1;
      offset = OFS;
      goto do_STRLDR;
    case LDR_RL:
      load = TRUE;
      type = type_word;
      pre = FALSE;
      writeback = TRUE;
      ofs = ofs_shiftrm;
      sign = -1;
      offset = 0;
      goto do_STRLDR;
    case LDR_RBL:
      load = TRUE;
      type = type_byte;
      pre = FALSE;
      writeback = TRUE;
      ofs = ofs_shiftrm;
      sign = -1;
      offset = 0;
      goto do_STRLDR;
    case LDR_RUL:
      load = TRUE;
      type = type_word;
      pre = FALSE;
      writeback = TRUE;
      ofs = ofs_shiftrm;
      sign = 1;
      offset = 0;
      goto do_STRLDR;
    case LDR_RUBL:
      load = TRUE;
      type = type_byte;
      pre = FALSE;
      writeback = TRUE;
      ofs = ofs_shiftrm;
      sign = 1;
      offset = 0;
      goto do_STRLDR;
    case LDR_RPL:
      load = TRUE;
      type = type_word;
      pre = TRUE;
      writeback = FALSE;
      ofs = ofs_shiftrm;
      sign = -1;
      offset = 0;
      goto do_STRLDR;
    case LDR_RPWL:
      load = TRUE;
      type = type_word;
      pre = TRUE;
      writeback = TRUE;
      ofs = ofs_shiftrm;
      sign = -1;
      offset = 0;
      goto do_STRLDR;
    case LDR_RPBL:
      load = TRUE;
      type = type_byte;
      pre = TRUE;
      writeback = FALSE;
      ofs = ofs_shiftrm;
      sign = -1;
      offset = 0;
      goto do_STRLDR;
    case LDR_RPBWL:
      load = TRUE;
      type = type_byte;
      pre = TRUE;
      writeback = TRUE;
      ofs = ofs_shiftrm;
      sign = -1;
      offset = 0;
      goto do_STRLDR;
    case LDR_RPUL:
      load = TRUE;
      type = type_word;
      pre = TRUE;
      writeback = FALSE;
      ofs = ofs_shiftrm;
      sign = 1;
      offset = 0;
      goto do_STRLDR;
    case LDR_RPUWL:
      load = TRUE;
      type = type_word;
      pre = TRUE;
      writeback = TRUE;
      ofs = ofs_shiftrm;
      sign = 1;
      offset = 0;
      goto do_STRLDR;
    case LDR_RPUBL:
      load = TRUE;
      type = type_byte;
      pre = TRUE;
      writeback = FALSE;
      ofs = ofs_shiftrm;
      sign = 1;
      offset = 0;
      goto do_STRLDR;
    case LDR_RPUBWL:
      load = TRUE;
      type = type_byte;
      pre = TRUE;
      writeback = TRUE;
      ofs = ofs_shiftrm;
      sign = 1;
      offset = 0;
      goto do_STRLDR;

    case LDFS_WL:
      load = TRUE;
      type = type_float;
      pre = FALSE;
      writeback = TRUE;
      ofs = ofs_offset;
      sign = -1;
      offset = FPOFS;
      goto do_STRLDR;
    case LDFD_WL:
      load = TRUE;
      type = type_double;
      pre = FALSE;
      writeback = TRUE;
      ofs = ofs_offset;
      sign = -1;
      offset = FPOFS;
      goto do_STRLDR;
    case LDFE_WL:
      load = TRUE;
      type = type_extended;
      pre = FALSE;
      writeback = TRUE;
      ofs = ofs_offset;
      sign = -1;
      offset = FPOFS;
      goto do_STRLDR;
    case LDFS_UWL:
      load = TRUE;
      type = type_float;
      pre = FALSE;
      writeback = TRUE;
      ofs = ofs_offset;
      sign = 1;
      offset = FPOFS;
      goto do_STRLDR;
    case LDFD_UWL:
      load = TRUE;
      type = type_double;
      pre = FALSE;
      writeback = TRUE;
      ofs = ofs_offset;
      sign = 1;
      offset = FPOFS;
      goto do_STRLDR;
    case LDFE_UWL:
      load = TRUE;
      type = type_extended;
      pre = FALSE;
      writeback = TRUE;
      ofs = ofs_offset;
      sign = 1;
      offset = FPOFS;
      goto do_STRLDR;
    case LDFS_PL:
      load = TRUE;
      type = type_float;
      pre = TRUE;
      writeback = FALSE;
      ofs = ofs_offset;
      sign = -1;
      offset = FPOFS;
      goto do_STRLDR;
    case LDFD_PL:
      load = TRUE;
      type = type_double;
      pre = TRUE;
      writeback = FALSE;
      ofs = ofs_offset;
      sign = -1;
      offset = FPOFS;
      goto do_STRLDR;
    case LDFS_PWL:
      load = TRUE;
      type = type_float;
      pre = TRUE;
      writeback = TRUE;
      ofs = ofs_offset;
      sign = -1;
      offset = FPOFS;
      goto do_STRLDR;
    case LDFD_PWL:
      load = TRUE;
      type = type_double;
      pre = TRUE;
      writeback = TRUE;
      ofs = ofs_offset;
      sign = -1;
      offset = FPOFS;
      goto do_STRLDR;
    case LDFE_PL:
      load = TRUE;
      type = type_extended;
      pre = TRUE;
      writeback = FALSE;
      ofs = ofs_offset;
      sign = -1;
      offset = FPOFS;
      goto do_STRLDR;
    case LDFE_PWL:
      load = TRUE;
      type = type_extended;
      pre = TRUE;
      writeback = TRUE;
      ofs = ofs_offset;
      sign = -1;
      offset = FPOFS;
      goto do_STRLDR;
    case LDFS_PUL:
      load = TRUE;
      type = type_float;
      pre = TRUE;
      writeback = FALSE;
      ofs = ofs_offset;
      sign = 1;
      offset = FPOFS;
      goto do_STRLDR;
    case LDFD_PUL:
      load = TRUE;
      type = type_double;
      pre = TRUE;
      writeback = FALSE;
      ofs = ofs_offset;
      sign = 1;
      offset = FPOFS;
      goto do_STRLDR;
    case LDFS_PUWL:
      load = TRUE;
      type = type_float;
      pre = TRUE;
      writeback = TRUE;
      ofs = ofs_offset;
      sign = 1;
      offset = FPOFS;
      goto do_STRLDR;
    case LDFD_PUWL:
      load = TRUE;
      type = type_double;
      pre = TRUE;
      writeback = TRUE;
      ofs = ofs_offset;
      sign = 1;
      offset = FPOFS;
      goto do_STRLDR;
    case LDFE_PUL:
      load = TRUE;
      type = type_extended;
      pre = TRUE;
      writeback = FALSE;
      ofs = ofs_offset;
      sign = 1;
      offset = FPOFS;
      goto do_STRLDR;
    case LDFE_PUWL:
      load = TRUE;
      type = type_extended;
      pre = TRUE;
      writeback = TRUE;
      ofs = ofs_offset;
      sign = 1;
      offset = FPOFS;
      goto do_STRLDR;

    case LDRSB_RL:
      load = TRUE;
      type = type_sbyte;
      pre = FALSE;
      writeback = TRUE;
      ofs = ofs_rm;
      sign = -1;
      offset = 0;
      goto do_STRLDR;
    case LDRSB_OL:
      load = TRUE;
      type = type_sbyte;
      pre = FALSE;
      writeback = TRUE;
      ofs = ofs_offset;
      sign = -1;
      offset = HOFS;
      goto do_STRLDR;
    case LDRSB_RUL:
      load = TRUE;
      type = type_sbyte;
      pre = FALSE;
      writeback = TRUE;
      ofs = ofs_rm;
      sign = 1;
      offset = 0;
      goto do_STRLDR;
    case LDRSB_OUL:
      load = TRUE;
      type = type_sbyte;
      pre = FALSE;
      writeback = TRUE;
      ofs = ofs_offset;
      sign = 1;
      offset = HOFS;
      goto do_STRLDR;
    case LDRSH_RL:
      load = TRUE;
      type = type_shalf;
      pre = FALSE;
      writeback = TRUE;
      ofs = ofs_rm;
      sign = -1;
      offset = 0;
      goto do_STRLDR;
    case LDRSH_OL:
      load = TRUE;
      type = type_shalf;
      pre = FALSE;
      writeback = TRUE;
      ofs = ofs_offset;
      sign = -1;
      offset = HOFS;
      goto do_STRLDR;
    case LDRSH_RUL:
      load = TRUE;
      type = type_shalf;
      pre = FALSE;
      writeback = TRUE;
      ofs = ofs_rm;
      sign = 1;
      offset = 0;
      goto do_STRLDR;
    case LDRSH_OUL:
      load = TRUE;
      type = type_shalf;
      pre = FALSE;
      writeback = TRUE;
      ofs = ofs_offset;
      sign = 1;
      offset = HOFS;
      goto do_STRLDR;
    case LDRSB_PRL:
      load = TRUE;
      type = type_sbyte;
      pre = TRUE;
      writeback = FALSE;
      ofs = ofs_rm;
      sign = -1;
      offset = 0;
      goto do_STRLDR;
    case LDRSB_PRWL:
      load = TRUE;
      type = type_sbyte;
      pre = TRUE;
      writeback = TRUE;
      ofs = ofs_rm;
      sign = -1;
      offset = 0;
      goto do_STRLDR;
    case LDRSB_POL:
      load = TRUE;
      type = type_sbyte;
      pre = TRUE;
      writeback = FALSE;
      ofs = ofs_offset;
      sign = -1;
      offset = HOFS;
      goto do_STRLDR;
    case LDRSB_POWL:
      load = TRUE;
      type = type_sbyte;
      pre = TRUE;
      writeback = TRUE;
      ofs = ofs_offset;
      sign = -1;
      offset = HOFS;
      goto do_STRLDR;
    case LDRSB_PRUL:
      load = TRUE;
      type = type_sbyte;
      pre = TRUE;
      writeback = FALSE;
      ofs = ofs_rm;
      sign = 1;
      offset = 0;
      goto do_STRLDR;
    case LDRSB_PRUWL:
      load = TRUE;
      type = type_sbyte;
      pre = TRUE;
      writeback = TRUE;
      ofs = ofs_rm;
      sign = 1;
      offset = 0;
      goto do_STRLDR;
    case LDRSB_POUL:
      load = TRUE;
      type = type_sbyte;
      pre = TRUE;
      writeback = FALSE;
      ofs = ofs_offset;
      sign = 1;
      offset = HOFS;
      goto do_STRLDR;
    case LDRSB_POUWL:
      load = TRUE;
      type = type_sbyte;
      pre = TRUE;
      writeback = TRUE;
      ofs = ofs_offset;
      sign = 1;
      offset = HOFS;
      goto do_STRLDR;
    case LDRSH_PRL:
      load = TRUE;
      type = type_shalf;
      pre = TRUE;
      writeback = FALSE;
      ofs = ofs_rm;
      sign = -1;
      offset = 0;
      goto do_STRLDR;
    case LDRSH_PRWL:
      load = TRUE;
      type = type_shalf;
      pre = TRUE;
      writeback = TRUE;
      ofs = ofs_rm;
      sign = -1;
      offset = 0;
      goto do_STRLDR;
    case LDRSH_POL:
      load = TRUE;
      type = type_shalf;
      pre = TRUE;
      writeback = FALSE;
      ofs = ofs_offset;
      sign = -1;
      offset = HOFS;
      goto do_STRLDR;
    case LDRSH_POWL:
      load = TRUE;
      type = type_shalf;
      pre = TRUE;
      writeback = TRUE;
      ofs = ofs_offset;
      sign = -1;
      offset = HOFS;
      goto do_STRLDR;
    case LDRSH_PRUL:
      load = TRUE;
      type = type_shalf;
      pre = TRUE;
      writeback = FALSE;
      ofs = ofs_rm;
      sign = 1;
      offset = 0;
      goto do_STRLDR;
    case LDRSH_PRUWL:
      load = TRUE;
      type = type_shalf;
      pre = TRUE;
      writeback = TRUE;
      ofs = ofs_rm;
      sign = 1;
      offset = 0;
      goto do_STRLDR;
    case LDRSH_POUL:
      load = TRUE;
      type = type_shalf;
      pre = TRUE;
      writeback = FALSE;
      ofs = ofs_offset;
      sign = 1;
      offset = HOFS;
      goto do_STRLDR;
    case LDRSH_POUWL:
      load = TRUE;
      type = type_shalf;
      pre = TRUE;
      writeback = TRUE;
      ofs = ofs_offset;
      sign = 1;
      offset = HOFS;
      goto do_STRLDR;

    do_STRLDR:
      nuops +=
	md_emit_ldst(&flow[nuops],
		     cond, load, pre, writeback,
		     /* type and format */type, ofs,
		     /* dest */RD,
		     /* src1 */RN,
		     /* src2 */SHIFT_SHAMT, SHIFT_TYPE, RM,
		     /* src2 */sign, offset);
      break; /* return nuops; */

    default:
      panic("inst `%d' is not a CISC flow", (int)op);
    }

  if (nuops >= MD_MAX_FLOWLEN)
    panic("uop flow buffer overflow, increase MD_MAX_FLOWLEN");
  return nuops;
}

double md_fpimm[8] =
  { 0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 0.5, 10.0 };

int
md_ones(word_t val)
{
  int i, cnt = 0;

  for (i=0; i < 32; i++)
    {
      if ((val & (1 << i)) != 0)
	cnt++;
    }
  return cnt;
}

#define POS(i) ( (~(i)) >> 31 )
#define NEG(i) ( (i) >> 31 )

int
md_addc(word_t lhs, word_t rhs, word_t res)
{
  int cflag;

  if (((lhs | rhs) >> 30) != 0)
    cflag = ((NEG(lhs) && NEG(rhs))
	     || (NEG(lhs) && POS(res))
	     || (NEG(rhs) && POS(res)));
  else
    cflag = 0;

  return cflag;
}

int
md_addv(word_t lhs, word_t rhs, word_t res)
{
  int vflag;

  if (((lhs | rhs) >> 30) != 0)
    vflag = ((NEG(lhs) && NEG(rhs) && POS(res))
	     || (POS(lhs) && POS(rhs) && NEG(res)));
  else
    vflag = 0;

  return vflag;
}

int
md_subc(word_t lhs, word_t rhs, word_t res)
{
  int cflag;

  if ((lhs >= rhs) || ((rhs | lhs) >> 31) != 0)
    cflag = ((NEG(lhs) && POS(rhs))
	     || (NEG(lhs) && POS(res))
	     || (POS(rhs) && POS(res)));
  else
    cflag = 0;

  return cflag;
}

int
md_subv(word_t lhs, word_t rhs, word_t res)
{
  int vflag;

  if ((lhs >= rhs) || ((rhs | lhs) >> 31) != 0)
    vflag = ((NEG(lhs) && POS(rhs) && POS(res))
	     || (POS(lhs) && NEG(rhs) && NEG(res)));
  else
    vflag = 0;

  return vflag;
}

int
md_cond_ok(md_inst_t inst, word_t psr)
{
  int res;

  /* bit 0 of the mask => Z
     bit 1 of the mask => C
     bit 2 of the mask => N
     bit 3 of the mask => V */
  static int mask[16] =
    { 0xAAAA,			   /* COND_EQ: _PSR_Z(psr) */
      0x5555,			   /* COND_NE: !_PSR_Z(psr); */
      0xCCCC,			   /* COND_CS: _PSR_C(psr); */
      0x3333,			   /* COND_CC: !_PSR_C(psr); */
      0xF0F0,			   /* COND_MI: _PSR_N(psr); */
      0x0F0F,			   /* COND_PL: !_PSR_N(psr); */
      0xFF00,			   /* COND_VS: _PSR_V(psr); */
      0x00FF,			   /* COND_VC: !_PSR_V(psr); */
      0xCCCC & 0x5555,		   /* COND_HI: _PSR_C(psr) && !_PSR_Z(psr); */
      0x3333 | 0xAAAA,		   /* COND_LS: !_PSR_C(psr) || _PSR_Z(psr); */
      0x0F0F ^ 0xFF00,		   /* COND_GE: !PSR_N(psr) != _PSR_V(psr); */
      0xF0F0 ^ 0xFF00,		   /* COND_LT: PSR_N(psr) != _PSR_V(psr); */
      (0x0F0F ^ 0xFF00) & 0x5555,  /* COND_GT: COND_GE & COND_NE */
      (0xF0F0 ^ 0xFF00) | 0xAAAA,  /* COND_LE: COND_LT | COND_EQ */
      0xFFFF, /* COND_AL */
      0, /* COND_NV */
    };

  res = mask[COND];
  res >>= ((_PSR_Z(psr) & 1) << 0)
	| ((_PSR_C(psr) & 1) << 1)
	| ((_PSR_N(psr) & 1) << 2)
	| ((_PSR_V(psr) & 1) << 3);
  return res & 1;
}

word_t
md_shiftrm(md_inst_t inst, word_t rmval, word_t rsval, word_t cfval)
{
  if (SHIFT_REG)
    {
      int shamt = rsval & 0xff;

      switch (SHIFT_TYPE)
	{
	case SHIFT_LSL:
	  if (shamt == 0)
	    return rmval;
	  else if (shamt >= 32)
	    return 0;
	  else
	    return rmval << shamt;

	case SHIFT_LSR:
	  if (shamt == 0)
	    return rmval;
	  else if (shamt >= 32)
	    return 0;
	  else
	    return rmval >> shamt;

	case SHIFT_ASR:
	  if (shamt == 0)
	    return rmval;
	  else if (shamt >= 32)
	    return (word_t)(((sword_t)rmval) >> 31);
	  else
	    return (word_t)(((sword_t)rmval) >> shamt);

	case SHIFT_ROR:
	  shamt = shamt & 0x1f;
	  if (shamt == 0)
	    return rmval;
	  else
	    return (rmval << (32 - shamt)) | (rmval >> shamt);

	default:
	  panic("bogus shift type");
	}
    }
  else /* SHIFT IMM */
    {
      switch (SHIFT_TYPE)
	{
	case SHIFT_LSL:
	  return rmval << SHIFT_SHAMT;

	case SHIFT_LSR:
	  if (SHIFT_SHAMT == 0)
	    return 0;
	  else
	    return rmval >> SHIFT_SHAMT;

	case SHIFT_ASR:
	  if (SHIFT_SHAMT == 0)
	    return (word_t)(((sword_t)rmval) >> 31);
	  else
	    return (word_t)(((sword_t)rmval) >> SHIFT_SHAMT);

	case SHIFT_ROR:
	  if (SHIFT_SHAMT == 0)
	    return (rmval >> 1) | ((!!cfval) << 31);
	  else
	    return (rmval << (32 - SHIFT_SHAMT)) | (rmval >> SHIFT_SHAMT);

	default:
	  panic("bogus shift type");
	}
    }
}

word_t
md_shiftc(md_inst_t inst, word_t rmval, word_t rsval, word_t cfval)
{
  if (SHIFT_REG)
    {
      int shamt = rsval & 0xff;

      switch (SHIFT_TYPE)
	{
	case SHIFT_LSL:
	  if (shamt == 0)
	    return !!cfval;
	  else if (shamt >= 32)
	    {
	      if (shamt == 32)
		return (rmval & 1);
	      else if (shamt > 32)
		return 0;
	    }
	  else
	    return (rmval >> (32 - shamt)) & 1;

	case SHIFT_LSR:
	  if (shamt == 0)
	    return !!cfval;
	  else if (shamt >= 32)
	    {
	      if (shamt == 32)
		return ((rmval >> 31) & 1);
	      else if (shamt > 32)
		return 0;
	    }
	  else
	    return (rmval >> (shamt - 1)) & 1;

	case SHIFT_ASR:
	  if (shamt == 0)
	    return !!cfval;
	  else if (shamt >= 32)
	    return (rmval >> 31) & 1;
	  else
	    return ((word_t)((sword_t)rmval >> (shamt - 1))) & 1;

	case SHIFT_ROR:
	  if (shamt == 0)
	    return !!cfval;
	  shamt = shamt & 0x1f;
	  if (shamt == 0)
	    return (rmval >> 31) & 1;
	  else
	    return (rmval >> (shamt - 1)) & 1;

	default:
	  panic("bogus shift type");
	}
    }
  else /* SHIFT IMM */
    {
      switch (SHIFT_TYPE)
	{
	case SHIFT_LSL:
	  return (rmval >> (32 - SHIFT_SHAMT)) & 1;

	case SHIFT_LSR:
	  if (SHIFT_SHAMT == 0)
	    return (rmval >> 31) & 1;
	  else
	    return (rmval >> (SHIFT_SHAMT - 1)) & 1;

	case SHIFT_ASR:
	  if (SHIFT_SHAMT == 0)
	    return (rmval >> 31) & 1;
	  else
	    return ((word_t)((sword_t)rmval >> (SHIFT_SHAMT - 1))) & 1;

	case SHIFT_ROR:
	  if (SHIFT_SHAMT == 0)
	    return (rmval & 1) & 1;
	  else
	    return (rmval >> (SHIFT_SHAMT - 1)) & 1;

	default:
	  panic("bogus shift type");
	}
    }
}

static char *md_cond[16] =
  { "eq", "ne", "cs", "cc", "mi", "pl", "vs", "vc",
    "hi", "ls", "ge", "lt", "gt", "le", "", "nv" };
static char *md_shift_type[4] = { "lsl", "lsr", "asr", "ror" };
static char *md_pu_code[4] = { "da", "ia", "db", "ib" };
static char *md_ef_size[4] = { "s", "d", "e", "??" };
static char *md_gh_rndmode[4] = { "", "p", "m", "z" };
static char *md_fp_imm[8] =
  { "0.0", "1.0", "2.0", "3.0", "4.0", "5.0", "0.5", "10.0" };

void
md_print_ifmt(char *fmt,
	      md_inst_t inst,		/* instruction to disassemble */
	      md_addr_t pc,		/* addr of inst, used for PC-rels */
	      FILE *stream)
{
  int i;
  char *s = fmt;

  while (*s)
    {
      if (*s == '%')
	{
	  s++;
	  switch (*s)
	    {
	    case 'c':
	      fputs(md_cond[COND], stream);
	      break;

	    case 'd':
	      fprintf(stream, "r%d", RD);
	      break;

	    case 'v':
	      if (URD < 16)
		fprintf(stream, "r%d", URD);
	      else
		fprintf(stream, "tmp%d", URD-16);
	      break;

	    case 'n':
	      fprintf(stream, "r%d", RN);
	      break;
	    case 'u':
	      if (URN < 16)
		fprintf(stream, "r%d", URN);
	      else
		fprintf(stream, "tmp%d", URN-16);
	      break;

	    case 's':
	      fprintf(stream, "r%d", RS);
	      break;
	    case 'w':
	      fprintf(stream, "r%d", RM);
	      break;
	    case 'm':
	      if (!SHIFT_REG && !SHIFT_SHAMT)
		fprintf(stream, "r%d", RM);
	      else if (SHIFT_REG && !SHIFT_REG_PAD)
		fprintf(stream,
			"r%d, %s r%d", RM, md_shift_type[SHIFT_TYPE], RS);
	      else if (SHIFT_REG && SHIFT_REG_PAD)
		fprintf(stream, "%s r%d (invalid pad!!!)",
			md_shift_type[SHIFT_TYPE], RS);
	      else
		fprintf(stream,
			"r%d, %s #%d", RM, md_shift_type[SHIFT_TYPE], SHIFT_SHAMT);
	      break;

	    case 'D':
	      fprintf(stream, "f%d", FD);
	      break;
	    case 'N':
	      fprintf(stream, "f%d", FN);
	      break;
	    case 'M':
	      fprintf(stream, "f%d", FM);
	      break;

	    case 'i':
	      fprintf(stream, "%d (%d >>> %d)",
		      ROTR(ROTIMM, ROTAMT << 1), ROTIMM, ROTAMT);
	      break;
	    case 'o':
	      fprintf(stream, "%d", OFS);
	      break;
	    case 'h':
	      fprintf(stream, "%d", HOFS);
	      break;
	    case 'O':
	      fprintf(stream, "%d", FPOFS);
	      break;
	    case 'I':
	      fprintf(stream, "%s", md_fp_imm[FPIMMBITS]);
	      break;

	    case 'a':
	      fprintf(stream, "%s", md_pu_code[LDST_PU]);
	      break;
	    case 'R':
	      {
		int first = TRUE;

		fprintf(stream, "{");
		for (i=0; i < 16; i++)
		  {
		    if (REGLIST & (1 << i))
		      {
			if (!first)
			  fprintf(stream, ",");
			fprintf(stream, "r%d", i);
			first = FALSE;
		      }
		  }
		fprintf(stream, "}");
	      }
	      break;
	    case 't':
	      fprintf(stream, "%s", md_ef_size[EF_SIZE]);
	      break;
	    case 'T':
	      fprintf(stream, "%s", md_ef_size[LDST_EF_SIZE]);
	      break;
	    case 'r':
	      fprintf(stream, "%s", md_gh_rndmode[GH_RNDMODE]);
	      break;
	    case 'C':
	      fprintf(stream, "%d", FCNT ? FCNT : 4);
	      break;

	    case 'j':
	      myfprintf(stream, "0x%p", pc + BOFS + 8);
	      break;

	    case 'S':
	      fprintf(stream, "0x%08x", SYSCODE);
	      break;
	    case 'P':
	      fprintf(stream, "%d", RS);
	      break;
	    case 'p':
	      fprintf(stream, "%d", CPOPC);
	      break;
	    case 'g':
	      fprintf(stream, "%d", CPEXT);
	      break;

	    default:
	      panic("unknown disassembler escape code `%c'", *s);
	    }
	}
      else
	fputc(*s, stream);
      s++;
    }
}

/* disassemble an Alpha instruction */
void
md_print_insn(md_inst_t inst,		/* instruction to disassemble */
	      md_addr_t pc,		/* addr of inst, used for PC-rels */
	      FILE *stream)		/* output stream */
{
  enum md_opcode op;

  /* use stderr as default output stream */
  if (!stream)
    stream = stderr;

  /* decode the instruction, assumes predecoded text segment */
  MD_SET_OPCODE(op, inst);

  /* disassemble the instruction */
  if (op <= OP_NA || op >= OP_MAX) 
    {
      /* bogus instruction */
		fprintf(stream, "<invalid inst: 0x%08x>, op = %d", inst, op);
    }
  else if ((((inst >> 25) & 0x7) == 0x3) && ((inst >> 4) & 0x1)) {
  		fprintf(stream, "<undefined>");
	 }
  else
    {
      md_print_ifmt(MD_OP_NAME(op), inst, pc, stream);
      fprintf(stream, "  ");
      md_print_ifmt(MD_OP_FORMAT(op), inst, pc, stream);
    }
}

/* disassemble an Alpha instruction */
void
md_print_uop(enum md_opcode op,
	     md_inst_t inst,		/* instruction to disassemble */
	     md_addr_t pc,		/* addr of inst, used for PC-rels */
	     FILE *stream)		/* output stream */
{
  /* use stderr as default output stream */
  if (!stream)
    stream = stderr;

  /* disassemble the instruction */
  if (op <= OP_NA || op >= OP_MAX) 
    {
      /* bogus instruction */
		fprintf(stream, "<invalid inst: 0x%08x>, op = %d", inst, op);
    }
  else if ((((inst >> 25) & 0x7) == 0x3) && ((inst >> 4) & 0x1)) {
  		fprintf(stream, "<undefined>");
	 }
  else
    {
      md_print_ifmt(MD_OP_NAME(op), inst, pc, stream);
      fprintf(stream, "  ");
      md_print_ifmt(MD_OP_FORMAT(op), inst, pc, stream);
    }
}


counter_t afu_count = 0;
double afu_max_power = 0, afu_total_power = 0;
double *_afu_vdd, *_afu_clk_freq;

/* execute AFU instructions */
double
(*_afu1)(mem_access_fn mem_fn,              /* generic memory accessor */
     struct mem_t *mem,                 /* memory space to access */
     word_t *out1, word_t *out2,
     word_t in1, word_t in2, word_t in3, word_t in4) = NULL;

/* execute AFU instructions */
double
(*_afu2)(mem_access_fn mem_fn,              /* generic memory accessor */
     struct mem_t *mem,                 /* memory space to access */
     word_t *out1, word_t *out2,
     word_t in1, word_t in2, word_t in3, word_t in4) = NULL;

/* execute AFU instructions */
double
(*_afu3)(mem_access_fn mem_fn,              /* generic memory accessor */
     struct mem_t *mem,                 /* memory space to access */
     word_t *out1, word_t *out2,
     word_t in1, word_t in2, word_t in3, word_t in4) = NULL;

/* execute AFU instructions */
double
(*_afu4)(mem_access_fn mem_fn,              /* generic memory accessor */
     struct mem_t *mem,                 /* memory space to access */
     word_t *out1, word_t *out2,
     word_t in1, word_t in2, word_t in3, word_t in4) = NULL;

/* execute AFU instructions */
double
(*_afu5)(mem_access_fn mem_fn,              /* generic memory accessor */
     struct mem_t *mem,                 /* memory space to access */
     word_t *out1, word_t *out2,
     word_t in1, word_t in2, word_t in3, word_t in4) = NULL;

/* execute AFU instructions */
double
(*_afu6)(mem_access_fn mem_fn,              /* generic memory accessor */
     struct mem_t *mem,                 /* memory space to access */
     word_t *out1, word_t *out2,
     word_t in1, word_t in2, word_t in3, word_t in4) = NULL;

/* execute AFU instructions */
double
(*_afu7)(mem_access_fn mem_fn,              /* generic memory accessor */
     struct mem_t *mem,                 /* memory space to access */
     word_t *out1, word_t *out2,
     word_t in1, word_t in2, word_t in3, word_t in4) = NULL;

int *_afu_lat = NULL;

void
afu_panalyzer_set_params (double freq)
{
  if (_afu_clk_freq)
    *_afu_clk_freq = freq * 1e6;
  if (_afu_vdd)
    *_afu_vdd = Vdd;
}

void
afu_panalyzer (double watt_power)
{
  if (watt_power > afu_max_power)
    afu_max_power = watt_power;
  afu_total_power += watt_power;
}

void
md_reg_stats(struct stat_sdb_t *sdb)
{
  stat_reg_counter(sdb, "afu_count",
                   "total number of AFU instructions executed",
                   &afu_count, /* initial value */ 0, /* format */ NULL);

  if (_afu_vdd)
    {
      stat_reg_double(sdb, "afu_peak",
                       "afu peak power dissipation",
                       &afu_max_power, /* initial value */ 0, /* format */ NULL);
      stat_reg_double(sdb, "afu_pdissipation",
                       "afu total power dissipation",
                       &afu_total_power, /* initial value */ 0, /* format */ NULL);
      stat_reg_formula(sdb, "afu_avgpdissipation",
                       "afu average power dissipation",
                       "afu_pdissipation / sim_cycle", /* format */ NULL);
    }
}


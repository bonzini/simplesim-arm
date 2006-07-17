/*
 * arm.h - ARM ISA definitions
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
 * $Id: arm.h,v 1.1.1.1 2000/11/29 14:53:54 cu-cs Exp $
 *
 * $Log: arm.h,v $
 * Revision 1.1.1.1  2000/11/29 14:53:54  cu-cs
 * Grand unification of arm sources.
 *
 *
 * Revision 1.1.2.17  2000/11/21 14:52:08  taustin
 * Completed changes for SA-1 core validation.
 *
 * Revision 1.1.2.16  2000/09/26 15:45:38  taustin
 * Attempted to get sim-outorder working, so so...
 * Dependency fixes.
 *
 * Revision 1.1.2.15  2000/09/22 01:58:19  taustin
 * Finished UOP flow generation support.
 *
 * Revision 1.1.2.14  2000/09/19 17:25:38  taustin
 * Completed UOP decomposition code, all UOPs now take nominally one cycle on a
 * simple pipeline, including loads and stores.  Initial testing has the code
 * working fine, however, extensive random testing will occur when sim-fuzz.c
 * gets extended to support multiple functional units...
 *
 * Revision 1.1.2.13  2000/09/11 12:23:10  taustin
 * Added UOP decomposition routines, used by sim-uop, soon to used by perf
 * simulators.
 *
 * Revision 1.1.2.12  2000/09/05 13:56:34  taustin
 * Lots of fixes after simulating using the new SimpleScalar/ARM fuzz buster to
 * perform random co-simulation testing, very handy!
 *
 * Revision 1.1.2.11  2000/08/29 14:18:27  taustin
 * Fixed word and qword size fp register accesses.
 * Addes time() and times() system call support (for Perl).
 *
 * Revision 1.1.2.10  2000/08/26 06:54:11  taustin
 * Simplified SimpleScalar/ARM PC handling - seems to work...
 * More SimpleScalar/ARM instruction definition debug work.
 *
 * Revision 1.1.2.9  2000/08/22 18:38:54  taustin
 * More progress on the SimpleScalar/ARM target.
 *
 * Revision 1.1.2.8  2000/08/02 08:52:15  taustin
 * SimpleScalar/ARM co-simulation component, based on the ARMulator.
 * More fixes to the SimpleScalar/ARM target.
 *
 * Revision 1.1.2.7  2000/07/27 21:45:06  taustin
 * Added umpteen half word and signed byte loads.
 *
 * Revision 1.1.2.6  2000/07/27 19:12:09  omutlu
 * some fixes to cdp instruction. needs more work
 *
 * Revision 1.1.2.5  2000/07/26 05:01:52  taustin
 * More disassembler fixes...
 *
 * Revision 1.1.2.4  2000/07/25 18:29:06  omutlu
 * Modified the branch offset calculation, cmp, ldm
 *
 * Revision 1.1.2.3  2000/07/21 18:31:00  taustin
 * More progress on the SimpleScalar/ARM target.
 *
 * Revision 1.1.2.2  2000/07/13 03:09:56  taustin
 * More progress on the SimpleScalar/ARM target.
 *
 * Revision 1.1.2.1  2000/05/31 19:26:00  taustin
 * ARM definition files
 *
 * Revision 1.1.1.1  2000/05/26 15:22:27  taustin
 * SimpleScalar Tool Set
 *
 *
 * Revision 1.4  1999/12/31 18:58:15  taustin
 * quad_t naming conflicts removed
 * MD_FETCH_INST() macro fixed
 * Alpha instruction set extensions (MVI, BWX, AMASK, IMPLVER, etc...)
 *
 * Revision 1.3  1999/12/13 18:59:40  taustin
 * cross endian execution support added
 *
 * Revision 1.2  1998/08/31 17:15:23  taustin
 * fixed non-portable SEXT definitions
 *
 * Revision 1.1  1998/08/27 16:53:36  taustin
 * Initial revision
 *
 * Revision 1.1  1998/05/06  01:09:18  calder
 * Initial revision
 *
 * Revision 1.5  1997/03/11  01:38:10  taustin
 * updated copyrights
 * long/int tweaks made for ALPHA target support
 * IFAIL() hook now allows simulators to declare instruction faults
 * IDIV()/IMOD()/FDIV() hooks now support simulator fault masking
 * supported added for non-GNU C compilers
 *
 * Revision 1.4  1997/01/06  16:08:10  taustin
 * comments updated
 * functional unit definitions moved from ss.def
 *
 * Revision 1.3  1996/12/27  15:55:37  taustin
 * fixed system header collision with MAXINT
 *
 * Revision 1.1  1996/12/05  18:50:23  taustin
 * Initial revision
 *
 */

#ifndef ARM_H
#define ARM_H

#include <stdio.h>

#include "host.h"
#include "misc.h"
#include "config.h"
#include "endian.h"


/*
 * This file contains various definitions needed to decode, disassemble, and
 * execute ARM instructions.
 */

/* build for ARM target */
#define TARGET_ARM

/* probe cross-endian execution */
#if defined(BYTES_BIG_ENDIAN)
#define MD_CROSS_ENDIAN
#endif

/* not applicable/available, usable in most definition contexts */
#define NA		0

/*
 * target-dependent type definitions
 */

/* define MD_QWORD_ADDRS if the target requires 64-bit (qword) addresses */
/* #define MD_QWORD_ADDRS */

/* address type definition */
typedef word_t md_addr_t;


/*
 * target-dependent memory module configuration
 */

/* physical memory page size (must be a power-of-two) */
#define MD_PAGE_SIZE		4096
#define MD_LOG_PAGE_SIZE	12


/*
 * target-dependent instruction faults
 */

enum md_fault_type {
  md_fault_none = 0,		/* no fault */
  md_fault_access,		/* storage access fault */
  md_fault_alignment,		/* storage alignment fault */
  md_fault_overflow,		/* signed arithmetic overflow fault */
  md_fault_div0,		/* division by zero fault */
  md_fault_invalid,             /* invalid arithmetic operation */ 
                                /* added to allow SQRT{S,T} in FIX exts */
  md_fault_break,		/* BREAK instruction fault */
  md_fault_unimpl,		/* unimplemented instruction fault */
  md_fault_internal		/* internal S/W fault */
};


/*
 * target-dependent register file definitions, used by regs.[hc]
 */

/* number of integer registers */
#define MD_NUM_IREGS		(/* arch */16 + /* Ucode */16)

/* number of floating point registers */
#define MD_NUM_FREGS		8

/* number of control registers */
#define MD_NUM_CREGS		3

/* total number of registers, excluding PC and NPC */
#define MD_TOTAL_REGS							\
  (/*int*/32 + /*fp*/8 + /*misc*/3 + /*tmp*/1 + /*mem*/1 + /*ctrl*/1)

/* general purpose (integer) register file entry type */
typedef word_t md_gpr_t[MD_NUM_IREGS];

/* floating point register file entry type */
typedef union {
  qword_t q[MD_NUM_FREGS];	/* integer qword view */
  dfloat_t d[MD_NUM_FREGS];	/* double-precision floating point view */
} md_fpr_t;

/* control register file contents */
typedef struct {
  word_t cpsr;			/* processor status register */
  word_t spsr;
  word_t fpsr;			/* floating point status register */
} md_ctrl_t;

/* well known registers */
enum md_reg_names {
  MD_REG_R0 = 0,
  MD_REG_V0 = 0,	/* return value reg */
  MD_REG_A0 = 0,	/* argument regs */
  MD_REG_R1 = 1,
  MD_REG_A1 = 1,
  MD_REG_R2 = 2,
  MD_REG_A2 = 2,
  MD_REG_R3 = 3,
  MD_REG_A3 = 3,
  MD_REG_R4 = 4,
  MD_REG_R5 = 5,
  MD_REG_R6 = 6,
  MD_REG_R7 = 7,
  MD_REG_R8 = 8,
  MD_REG_R9 = 9,
  MD_REG_R10 = 10,
  MD_REG_SL = 10,
  MD_REG_R11 = 11,
  MD_REG_FP = 11,	/* frame pointer */
  MD_REG_R12 = 12,
  MD_REG_IP = 12,
  MD_REG_R13 = 13,
  MD_REG_SP = 13,	/* stack pointer */
  MD_REG_R14 = 14,
  MD_REG_LR = 14,	/* link register */
  MD_REG_R15 = 15,
  MD_REG_PC = 15,	/* link register */
  MD_REG_TMP0 = 16,	/* temp registers - used by Ucode, 16 total */
  MD_REG_TMP1 = 17,
  MD_REG_TMP2 = 18,
  MD_REG_TMP3 = 19
};

/*
 * target-dependent instruction format definition
 */

/* instruction formats */
typedef word_t md_inst_t;

/* preferred nop instruction definition */
extern md_inst_t MD_NOP_INST;

/* target swap support */
#ifdef MD_CROSS_ENDIAN

#define MD_SWAPH(X)		SWAP_HALF(X)
#define MD_SWAPW(X)		SWAP_WORD(X)
#define MD_SWAPQ(X)		SWAP_QWORD(X)
#define MD_SWAPI(X)		SWAP_WORD(X)

#else /* !MD_CROSS_ENDIAN */

#define MD_SWAPH(X)		(X)
#define MD_SWAPW(X)		(X)
#define MD_SWAPQ(X)		(X)
#define MD_SWAPD(X)		(X)
#define MD_SWAPI(X)		(X)

#endif

/* fetch an instruction.  the strange XOR is a hack to turn BX instructions
   into MOV PC, Rnn.  The encoding should be changed from c12FFF1s (where
   c is the condition and s is the source) to c1A0F00s.  */
#define MD_FETCH_INST(INST, MEM, PC)					\
  { (INST) = MEM_READ_WORD((MEM), (PC)),				\
    (INST) ^= (((INST) & 0xFFFFFF0) == 0x12FFF10) ? 0x8F0F10 : 0; }

/*
 * target-dependent loader module configuration
 */

/* maximum size of argc+argv+envp environment */
#define MD_MAX_ENVIRON		16384


/*
 * machine.def specific definitions
 */

/* fprintf (stderr, "%s %x %d %d\n", md_op2name[OP], ((INST >> md_opshift[OP]) & md_opmask[OP]), md_opshift[OP], md_opmask[OP]) */

/* inst -> enum md_opcode mapping, use this macro to decode insts */
#define MD_TOP_OP(INST)		(((INST) >> 24) & 0x0f)
#define MD_SET_OPCODE(OP, INST)						\
  { OP = md_mask2op[MD_TOP_OP(INST)];					\
    while (md_opmask[OP])						\
      OP = md_mask2op[((INST >> md_opshift[OP]) & md_opmask[OP])	\
		      + md_opoffset[OP]]; }

/* largest opcode field value (currently upper 8-bit are used for pre/post-
    incr/decr operation specifiers */
#define MD_MAX_MASK		2048

/* global opcode names, these are returned by the decoder (MD_OP_ENUM()) */
enum md_opcode {
  OP_NA = 0,	/* NA */
#define DEFINST(OP,MSK,NAME,OPFORM,RES,FLAGS,O1,O2,O3,I1,I2,I3,I4) OP,
#define DEFUOP(OP,NAME,OPFORM,RES,FLAGS,O1,O2,O3,I1,I2,I3,I4) OP,
#define DEFLINK(OP,MSK,NAME,MASK,SHIFT) OP,
#define CONNECT(OP)
#include "machine.def"
  OP_MAX	/* number of opcodes + NA */
};

/* internal decoder state */
extern enum md_opcode md_mask2op[];
extern unsigned int md_opoffset[];
extern unsigned int md_opmask[];
extern unsigned int md_opshift[];

/* enum md_opcode -> description string */
#define MD_OP_NAME(OP)		(md_op2name[OP])
extern char *md_op2name[];

/* enum md_opcode -> opcode operand format, used by disassembler */
#define MD_OP_FORMAT(OP)	(md_op2format[OP])
extern char *md_op2format[];

/* enum md_opcode -> latency for operations in FUClamd_NA class.  */
#define MD_OPLAT(OP)		((OP) >= AFU1  && (OP) <= AFU7 && _afu_lat \
				 ? _afu_lat[(OP) - AFU1] : -1)

/* function unit classes, update md_fu2name if you update this definition */
enum md_fu_class {
  FUClamd_NA = 0,	/* inst does not use a functional unit */
  IntALU,		/* integer ALU */
  IntMULT,		/* integer multiplier */
  IntDIV,		/* integer divider */
  FloatADD,		/* floating point adder/subtractor */
  FloatCMP,		/* floating point comparator */
  FloatCVT,		/* floating point<->integer converter */
  FloatMULT,		/* floating point multiplier */
  FloatDIV,		/* floating point divider */
  FloatSQRT,		/* floating point square root */
  RdPort,		/* memory read port */
  WrPort,		/* memory write port */
  NUM_FU_CLASSES	/* total functional unit classes */
};

/* enum md_opcode -> enum md_fu_class, used by performance simulators */
#define MD_OP_FUCLASS(OP)	(md_op2fu[OP])
extern enum md_fu_class md_op2fu[];

/* enum md_fu_class -> description string */
#define MD_FU_NAME(FU)		(md_fu2name[FU])
extern char *md_fu2name[];

/* instruction flags */
#define F_ICOMP		0x00000001	/* integer computation */
#define F_FCOMP		0x00000002	/* FP computation */
#define F_CTRL		0x00000004	/* control inst */
#define F_UNCOND	0x00000008	/*   unconditional change */
#define F_COND		0x00000010	/*   conditional change */
#define F_MEM		0x00000020	/* memory access inst */
#define F_LOAD		0x00000040	/*   load inst */
#define F_STORE		0x00000080	/*   store inst */
#define F_DISP		0x00000100	/*   displaced (R+C) addr mode */
#define F_RR		0x00000200	/*   R+R addr mode */
#define F_DIRECT	0x00000400	/*   direct addressing mode */
#define F_TRAP		0x00000800	/* traping inst */
#define F_LONGLAT	0x00001000	/* long latency inst (for sched) */
#define F_DIRJMP	0x00002000	/* direct jump */
#define F_INDIRJMP	0x00004000	/* indirect jump */
#define F_CALL		0x00008000	/* function call */
#define F_FPCOND	0x00010000	/* FP conditional branch */
#define F_IMM		0x00020000	/* instruction has immediate operand */
#define F_CISC		0x00040000	/* CISC instruction */
#define F_AGEN		0x00080000	/* AGEN micro-instruction */

/* enum md_opcode -> opcode flags, used by simulators */
#define MD_OP_FLAGS(OP)		(md_op2flags[OP])
extern unsigned int md_op2flags[];


/* integer register specifiers */
#define RN		((inst >> 16) & 0x0f)
#define URN		((inst >> 17) & 0x1f)	/* Ucode RN */
#define RD		((inst >> 12) & 0x0f)
#define URD		((inst >> 12) & 0x1f)	/* Ucode RD */
#define RS		((inst >> 8) & 0x0f)
#define RM		(inst & 0x0f)

/* floating point register specifiers */
#define FN		((inst >> 16) & 0x07)
#define FD		((inst >> 12) & 0x07)
#define FM		(inst & 0x07)

/* register shift accessors */
#define SHIFT_BITS	((inst >> 4) & 0xff)
#define SHIFT_REG	((inst >> 4) & 0x01)
#define SHIFT_REG_PAD	((inst >> 7) & 0x01)
#define SHIFT_TYPE	((inst >> 5) & 0x03)
#define SHIFT_SHAMT	((inst >> 7) & 0x1f)

/* register shift types */
#define SHIFT_LSL	0x00
#define SHIFT_LSR	0x01
#define SHIFT_ASR	0x02
#define SHIFT_ROR	0x03

/* rotated immediate accessors */
#define ROTIMM		(inst & 0xff)
#define ROTAMT		((inst >> 8) & 0x0f)

/* rotate operator */
#define ROTR(VAL,N)							\
  (((VAL) >> (int)((N) & 31)) | ((VAL) << (32 - (int)((N) & 31))))

/* load/store 12-bit unsigned offset field value */
#define OFS		((word_t)(inst & 0xfff))

/* load/store 8-bit unsigned offset field value */
#define HOFS		((word_t)((RS << 4) + RM))

/* fp load/store 8-bit unsigned offset field value */
#define FPOFS		((word_t)((inst & 0xff) << 2))

/* returns 24-bit signed immediate field value - made 2's complement - Onur 07/24/00 */
#define BOFS								\
  ((inst & 0x800000)							\
   ? (0xfc000000 | ((inst & 0xffffff) << 2))				\
   : ((inst & 0x7fffff) << 2))

#if 0
#define BOFS		((((word_t)(inst & 0xffffff)) >> 23) ? ((word_t)(-67108864 + (word_t)(((inst & 0xffffff) << 2)))) : ((word_t)((inst & 0xffffff)) << 2))
#endif

/* coprocessor operation code for CDP instruction */
#define CPOPC ((inst >> 20) & 0x0f)
#define CPEXT ((inst >> 5) & 0x07)

/* sign-extend operands */
#define SEXT24(X)							\
  (((X) & 0x800000) ? ((sword_t)(X) | 0xff800000)) : (sqword_t)(X))

/* load/store opcode accessors */
#define LDST_PU		((inst >> 23) & 0x03)
#define REGLIST		(inst & 0xffff)

/* floating point opcode accessors */
#define FPIMMBITS	(inst & 0x07)
#define FPIMM		(md_fpimm[FPIMMBITS])
extern double md_fpimm[8];

#define EF_SIZE		(((inst >> 18) & 0x02) | ((inst >> 7) & 0x01))
#define LDST_EF_SIZE	(((inst >> 21) & 0x02) | ((inst >> 15) & 0x01))
#define GH_RNDMODE	((inst >> 5) & 0x03)
#define FCNT		(((inst >> 21) & 0x02) | ((inst >> 15) & 0x01))

/* ones counter */
#define ONES(EXPR)	(md_ones(EXPR))

/* ones counter function */
int md_ones(word_t val);

/* arithmetic flags */
#define ADDC(LHS, RHS, RES)		md_addc((LHS), (RHS), (RES))
#define ADDV(LHS, RHS, RES)		md_addv((LHS), (RHS), (RES))
#define SUBC(LHS, RHS, RES)		md_subc((LHS), (RHS), (RES))
#define SUBV(LHS, RHS, RES)		md_subv((LHS), (RHS), (RES))

int md_addc(word_t lhs, word_t rhs, word_t res);
int md_addv(word_t lhs, word_t rhs, word_t res);
int md_subc(word_t lhs, word_t rhs, word_t res);
int md_subv(word_t lhs, word_t rhs, word_t res);

/* SWI accessors */
#define SYSCODE		(inst & 0xffffff)

/* default target PC handling */
#ifndef SET_TPC
#define SET_TPC(PC)	(void)0
#endif /* SET_TPC */

/* processor status register accessors */
#define _PSR_N(PSR)		(((PSR) >> 31) & 1)
#define _SET_PSR_N(PSR, VAL)						\
  ((PSR) = (((PSR) & ~(1 << 31)) | (((VAL) & 1) << 31)))
#define _PSR_Z(PSR)		(((PSR) >> 30) & 1)
#define _SET_PSR_Z(PSR, VAL)						\
  ((PSR) = (((PSR) & ~(1 << 30)) | (((VAL) & 1) << 30)))
#define _PSR_C(PSR)		(((PSR) >> 29) & 1)
#define _SET_PSR_C(PSR, VAL)						\
  ((PSR) = (((PSR) & ~(1 << 29)) | (((VAL) & 1) << 29)))
#define _PSR_V(PSR)		(((PSR) >> 28) & 1)
#define _SET_PSR_V(PSR, VAL)						\
  ((PSR) = (((PSR) & ~(1 << 28)) | (((VAL) & 1) << 28)))

#define _PSR_IRQ(PSR)		(((PSR) >> 7) & 1)
#define _PSR_FIQ(PSR)		(((PSR) >> 6) & 1)
#define _PSR_MODE(PSR)		((PSR) & 0x1f)

/* condition code values */
#define COND_EQ		0x00
#define COND_NE		0x01
#define COND_CS		0x02
#define COND_CC		0x03
#define COND_MI		0x04
#define COND_PL		0x05
#define COND_VS		0x06
#define COND_VC		0x07
#define COND_HI		0x08
#define COND_LS		0x09
#define COND_GE		0x0a
#define COND_LT		0x0b
#define COND_GT		0x0c
#define COND_LE		0x0d
#define COND_AL		0x0e
#define COND_NV		0x0f

/* condition opcode accessor */
#define COND		((inst >> 28) & 0x0f)

/* test instruction condition value */
#define COND_VALID(PSR)	(((COND) == COND_AL) || md_cond_ok(inst, PSR))

/* full capability instruction condition evaluator */
extern int md_cond_ok(md_inst_t inst, word_t psr);

/* compute shifted register RM value */
#define SHIFTRM(RMVAL, RSVAL, CFVAL)					\
  (((SHIFT_BITS)==0) ? (RMVAL) : md_shiftrm(inst, (RMVAL), (RSVAL), (CFVAL)))

/* full capability shifted register RM routine */
extern word_t
md_shiftrm(md_inst_t inst, word_t rmval, word_t rs_val, word_t psr);

#define SHIFTC(RMVAL, RSVAL, CFVAL)					\
  (((SHIFT_BITS)==0) ? (CFVAL) : md_shiftc(inst, (RMVAL), (RSVAL), (CFVAL)))

/* full capability shifted register CF routine */
word_t
md_shiftc(md_inst_t inst, word_t rmval, word_t rsval, word_t cfval);

/*
 * various other helper macros/functions
 */

/* non-zero if system call is an exit() */
#define OSF_SYS_exit			1
#define MD_EXIT_SYSCALL(REGS)						\
  ((REGS)->regs_R[MD_REG_V0] == OSF_SYS_exit)

/* non-zero if system call is a write to stdout/stderr */
#define OSF_SYS_write			4
#define MD_OUTPUT_SYSCALL(REGS)						\
  ((REGS)->regs_R[MD_REG_V0] == OSF_SYS_write				\
   && ((REGS)->regs_R[MD_REG_A0] == /* stdout */1			\
       || (REGS)->regs_R[MD_REG_A0] == /* stderr */2))

/* returns stream of an output system call, translated to host */
#define MD_STREAM_FILENO(REGS)		((REGS)->regs_R[MD_REG_A0])

/* returns non-zero if instruction is a function call */
#define MD_IS_CALL(OP)			((OP) == BRL)

/* returns non-zero if instruction is a function return */
/* FIXME: this will never return TRUE */
#define MD_IS_RETURN(OP)		((OP) == MOV)

/* returns non-zero if instruction is an indirect jump */
/* FIXME: !!! */
#define MD_IS_INDIR(OP)			((OP) == MOV)

/* addressing mode probe, enums and strings */
enum md_amode_type {
  md_amode_imm,		/* immediate addressing mode */
  md_amode_gp,		/* global data access through global pointer */
  md_amode_sp,		/* stack access through stack pointer */
  md_amode_fp,		/* stack access through frame pointer */
  md_amode_disp,	/* (reg + const) addressing */
  md_amode_rr,		/* (reg + reg) addressing */
  md_amode_NUM
};
extern char *md_amode_str[md_amode_NUM];

/* addressing mode pre-probe FSM, must see all instructions */
#define MD_AMODE_PREPROBE(OP, FSM)		{ (FSM) = 0; }

/* compute addressing mode, only for loads/stores */
#define MD_AMODE_PROBE(AM, OP, FSM)					\
  {									\
    if (MD_OP_FLAGS(OP) & F_DISP)					\
      {									\
	if ((RB) == MD_REG_GP)						\
	  (AM) = md_amode_gp;						\
	else if ((RB) == MD_REG_SP)					\
	  (AM) = md_amode_sp;						\
	else if ((RB) == MD_REG_FP) /* && bind_to_seg(addr) == seg_stack */\
	  (AM) = md_amode_fp;						\
	else								\
	  (AM) = md_amode_disp;						\
      }									\
    else if (MD_OP_FLAGS(OP) & F_RR)					\
      (AM) = md_amode_rr;						\
    else								\
      panic("cannot decode addressing mode");				\
  }

/* addressing mode pre-probe FSM, after all loads and stores */
#define MD_AMODE_POSTPROBE(FSM)			/* nada... */


/*
 * EIO package configuration/macros
 */

/* expected EIO file format */
#define MD_EIO_FILE_FORMAT		EIO_ALPHA_FORMAT

#define MD_MISC_REGS_TO_EXO(REGS)					\
  exo_new(ec_list,							\
	  /*icnt*/exo_new(ec_integer, (exo_integer_t)sim_num_insn),	\
	  /*PC*/exo_new(ec_address, (exo_integer_t)(REGS)->regs_PC),	\
	  /*NPC*/exo_new(ec_address, (exo_integer_t)(REGS)->regs_NPC),	\
	  NULL)

#if XXX
	  /*FPCR*/exo_new(ec_integer, (exo_integer_t)(REGS)->regs_C.fpcr),
	  /*UNIQ*/exo_new(ec_integer, (exo_integer_t)(REGS)->regs_C.uniq),
#endif

#define MD_IREG_TO_EXO(REGS, IDX)					\
  exo_new(ec_address, (exo_integer_t)(REGS)->regs_R[IDX])

#define MD_FREG_TO_EXO(REGS, IDX)					\
  exo_new(ec_address, (exo_integer_t)(REGS)->regs_F.q[IDX])

#define MD_EXO_TO_MISC_REGS(EXO, ICNT, REGS)				\
  /* check EXO format for errors... */					\
  if (!exo								\
      || exo->ec != ec_list						\
      || !exo->as_list.head						\
      || exo->as_list.head->ec != ec_integer				\
      || !exo->as_list.head->next					\
      || exo->as_list.head->next->ec != ec_address			\
      || !exo->as_list.head->next->next					\
      || exo->as_list.head->next->next->ec != ec_address		\
      || !exo->as_list.head->next->next->next				\
      || exo->as_list.head->next->next->next->ec != ec_integer		\
      || !exo->as_list.head->next->next->next->next			\
      || exo->as_list.head->next->next->next->next->ec != ec_integer	\
      || exo->as_list.head->next->next->next->next->next != NULL)	\
    fatal("could not read EIO misc regs");				\
  (ICNT) = (counter_t)exo->as_list.head->as_integer.val;		\
  (REGS)->regs_PC = (md_addr_t)exo->as_list.head->next->as_integer.val;	\
  (REGS)->regs_NPC =							\
    (md_addr_t)exo->as_list.head->next->next->as_integer.val;

#if XXX
  (REGS)->regs_C.fpcr =
    (qword_t)exo->as_list.head->next->next->next->as_integer.val;
  (REGS)->regs_C.uniq =
    (qword_t)exo->as_list.head->next->next->next->next->as_integer.val;
#endif

#define MD_EXO_TO_IREG(EXO, REGS, IDX)					\
  ((REGS)->regs_R[IDX] = (qword_t)(EXO)->as_integer.val)

#define MD_EXO_TO_FREG(EXO, REGS, IDX)					\
  ((REGS)->regs_F.q[IDX] = (qword_t)(EXO)->as_integer.val)

#define MD_EXO_CMP_IREG(EXO, REGS, IDX)					\
  ((REGS)->regs_R[IDX] != (qword_t)(EXO)->as_integer.val)

#define MD_FIRST_IN_REG			0
#define MD_LAST_IN_REG			21

#define MD_FIRST_OUT_REG		0
#define MD_LAST_OUT_REG			21


/*
 * configure the EXO package
 */

/* EXO pointer class */
typedef qword_t exo_address_t;

/* EXO integer class, 64-bit encoding */
typedef qword_t exo_integer_t;

/* EXO floating point class, 64-bit encoding */
typedef double exo_float_t;


/*
 * configure the stats package
 */

/* counter stats */
#define stat_reg_counter		stat_reg_sqword
#define sc_counter			sc_sqword
#define for_counter			for_sqword

/* address stats */
#define stat_reg_addr			stat_reg_uint


/*
 * configure the DLite! debugger
 */

/* register bank specifier */
enum md_reg_type {
  rt_gpr,		/* general purpose register */
  rt_lpr,		/* integer-precision floating pointer register */
  rt_fpr,		/* single-precision floating pointer register */
  rt_dpr,		/* double-precision floating pointer register */
  rt_ctrl,		/* control register */
  rt_PC,		/* program counter */
  rt_NPC,		/* next program counter */
  rt_NUM
};

/* register name specifier */
struct md_reg_names_t {
  char *str;			/* register name */
  enum md_reg_type file;	/* register file */
  int reg;			/* register index */
};

/* symbolic register names, parser is case-insensitive */
extern struct md_reg_names_t md_reg_names[];

/* returns a register name string */
char *md_reg_name(enum md_reg_type rt, int reg);

/* default register accessor object */
struct eval_value_t;
struct regs_t;
char *						/* err str, NULL for no err */
md_reg_obj(struct regs_t *regs,			/* registers to access */
	   int is_write,			/* access type */
	   enum md_reg_type rt,			/* reg bank to probe */
	   int reg,				/* register number */
	   struct eval_value_t *val);		/* input, output */

/* print integer REG(S) to STREAM */
void md_print_ireg(md_gpr_t regs, int reg, FILE *stream);
void md_print_iregs(md_gpr_t regs, FILE *stream);

/* print floating point REG(S) to STREAM */
void md_print_fpreg(md_fpr_t regs, int reg, FILE *stream);
void md_print_fpregs(md_fpr_t regs, FILE *stream);

/* print control REG(S) to STREAM */
void md_print_creg(md_ctrl_t regs, int reg, FILE *stream);
void md_print_cregs(md_ctrl_t regs, FILE *stream);

/* xor checksum registers */
word_t md_xor_regs(struct regs_t *regs);


/*
 * configure sim-outorder specifics
 */

/* primitive operation used to compute addresses within pipeline */
#define MD_AGEN_OP		ADDQ

/* NOP operation when injected into the pipeline */
#define MD_NOP_OP		OP_NA

/* non-zero for a valid address, used to determine if speculative accesses
   should access the DL1 data cache */
#define MD_VALID_ADDR(ADDR)						\
  (((ADDR) >= ld_text_base && (ADDR) < (ld_text_base + ld_text_size))	\
   || ((ADDR) >= ld_data_base && (ADDR) < ld_brk_point)			\
   || ((ADDR) >= (ld_stack_base - 16*1024*1024) && (ADDR) < ld_stack_base))

/*
 * configure branch predictors
 */

/* shift used to ignore branch address least significant bits, usually
   log2(sizeof(md_inst_t)) */
#define MD_BR_SHIFT		2	/* log2(4) */


/*
 * target-dependent routines
 */

/* intialize the inst decoder, this function builds the ISA decode tables */
void md_init_decoder(void);

/* ARM UOP definition */
#define MD_MAX_FLOWLEN		64
struct md_uop_t {
  enum md_opcode op;		/* decoded opcode of the UOP */
  md_inst_t inst;		/* instruction bits of UOP */
};

/* UOP flow generator, returns a small non-cyclic program implementing OP,
   returns length of flow returned */
int
md_get_flow(enum md_opcode op, md_inst_t inst,
            struct md_uop_t flow[MD_MAX_FLOWLEN]);

/* disassemble a SimpleScalar instruction */
void
md_print_insn(md_inst_t inst,		/* instruction to disassemble */
	      md_addr_t pc,		/* addr of inst, used for PC-rels */
	      FILE *stream);		/* output stream */

/* disassemble an Alpha instruction */
void
md_print_uop(enum md_opcode op,
	     md_inst_t inst,		/* instruction to disassemble */
	     md_addr_t pc,		/* addr of inst, used for PC-rels */
	     FILE *stream);		/* output stream */

#endif /* ARM_H */

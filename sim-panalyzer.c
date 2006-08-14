#define DEBUG_ARMUL

/*
 * sim-panalyzer.c - sample out-of-order issue perf simulator implementation
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
 * $Id: sim-panalyzer.c,v 1.13 2004/01/06 18:05:30 panalyzer Exp $
 *
 * $Log: sim-panalyzer.c,v $
 * Revision 1.13  2004/01/06 18:05:30  panalyzer
 * *** empty log message ***
 *
 * Revision 1.12  2004/01/04 13:30:52  panalyzer
 * *** empty log message ***
 *
 * Revision 1.11  2004/01/04 12:47:35  panalyzer
 * *** empty log message ***
 *
 * Revision 1.10  2004/01/04 07:16:13  panalyzer
 * *** empty log message ***
 *
 * Revision 1.9  2003/12/22 06:18:55  panalyzer
 * *** empty log message ***
 *
 * Revision 1.8  2003/12/21 21:24:57  panalyzer
 * *** empty log message ***
 *
 * Revision 1.7  2003/12/18 20:31:59  panalyzer
 * *** empty log message ***
 *
 * Revision 1.6  2003/11/02 22:52:01  panalyzer
 * *** empty log message ***
 *
 * Revision 1.5  2003/10/05 00:54:48  panalyzer
 * *** empty log message ***
 *
 * Revision 1.4  2003/09/23 02:18:57  panalyzer
 * *** empty log message ***
 *
 * Revision 1.3  2003/09/18 19:35:49  panalyzer
 * *** empty log message ***
 *
 * Revision 1.2  2003/09/18 04:34:46  panalyzer
 * *** empty log message ***
 *
 * Revision 1.1.1.1  2003/09/18 00:57:54  panalyzer
 *
 *
 * Revision 1.1.1.1  2003/09/18 00:18:44  panalyzer
 *
 *
 * Revision 1.1.1.1  2003/09/16 18:48:14  gdm
 *
 *
 * Revision 1.2  2000/11/29 21:01:57  cu-cs
 * quad_t => qword_t ...
 *
 * Revision 1.1.1.1  2000/11/29 14:53:54  cu-cs
 * Grand unification of arm sources.
 *
 *
 * Revision 1.1.1.1.2.17  2000/11/22 07:18:39  kimns
 * Wattch porting version supporting data sensitivity of BTB, level 1/2 instruction and data caches.
 *
 * Revision 1.1.1.1.2.16  2000/11/21 14:52:03  taustin
 * Completed changes for SA-1 core validation.
 *
 * Revision 1.1.1.1.2.15  2000/11/20 05:27:27  kimns
 * Bug fix related to Wattch branch predictor power estimation
 *
 * Revision 1.1.1.1.2.14  2000/11/14 20:10:09  kimns
 * Wattch Conditional Compilation Version
 *
 * Revision 1.1.1.1.2.13  2000/11/11 07:48:11  taustin
 * Added support for non-pipelined memory systems.
 *
 * Revision 1.1.1.1.2.12  2000/11/11 03:18:55  taustin
 * Fixed missing "cond_valid" expression.
 *
 * Revision 1.1.1.1.2.11  2000/11/10 23:43:35  kimns
 * Cleaned up Version of sim-outorder.c
 *
 * Revision 1.1.1.1.2.10  2000/11/10 15:48:37  ernstd
 * created header file for shared variables in sim-outorder.  Also added ptrace support for viewing uops.
 *
 * Revision 1.1.1.1.2.7  2000/11/06 13:18:28  taustin
 * Fixed a commit bug.
 * Revision 1.1.1.1.2.9  2000/11/09 19:44:52  taustin
 * Fixed static's that were removed.
 *
 * Revision 1.1.1.1.2.8  2000/11/08 18:35:52  kimns
 * Wattch Portig Version...
 *
 * Added perfect disambiguation support.
 *
 * Revision 1.1.1.1.2.6  2000/10/21 06:14:10  taustin
 * Fixed multiple load retirement bug.
 * Fixed uninitialized dependency bug.
 * Fixed undefined F_CISC flows.
 *
 * Revision 1.1.1.1.2.5  2000/10/20 16:00:06  taustin
 * Yet more work on the SimpleScalar/ARM performance simulator.  Almost there...
 *
 * Revision 1.1.1.1.2.4  2000/10/19 14:38:40  taustin
 * More work on sim-outorder for SimpleScalar/ARM.
 *
 * Revision 1.1.1.1.2.3  2000/10/13 17:37:00  chriswea
 * added ptrace stuff
 *
 * Revision 1.1.1.1.2.2  2000/09/26 15:45:16  taustin
 * Attempted to get sim-outorder working, so so...
 *
 * Revision 1.1.1.1.2.1  2000/09/22 01:58:11  taustin
 * Finished UOP flow generation support.
 *
 * Revision 1.1.1.1  2000/05/26 15:18:58  taustin
 * SimpleScalar Tool Set
 *
 *
 * Revision 1.7  1999/12/31 18:50:38  taustin
 * quad_t naming conflicts removed
 * added retirement tracing to sim-outorder (enable with -v)
 * speculative execution should now be deterministic (uninit bugs fixed...)
 * sim-outorder now stops after sim_num_insn
 *
 * Revision 1.6  1999/12/13 18:46:40  taustin
 * cross endian execution support added
 *
 * Revision 1.5  1998/08/27 16:27:48  taustin
 * implemented host interface description in host.h
 * added target interface support
 * added support for register and memory contexts
 * instruction predecoding moved to loader module
 * Alpha target support added
 * added support for qword's
 * added fault support
 * added option ("-max:inst") to limit number of instructions analyzed
 * explicit BTB sizing option added to branch predictors, use
 *       "-btb" option to configure BTB
 * added queue statistics for IFQ, RUU, and LSQ; all terms of Little's
 *       law are measured and reports; also, measures fraction of cycles
 *       in which queue is full
 * added fast forward option ("-fastfwd") that skips a specified number
 *       of instructions (using functional simulation) before starting timing
 *       simulation
 * sim-outorder speculative loads no longer allocate memory pages,
 *       this significantly reduces memory requirements for programs with
 *       lots of mispeculation (e.g., cc1)
 * branch predictor updates can now optionally occur in ID, WB,
 *       or CT
 * added target-dependent myprintf() support
 * fixed speculative qword store bug (missing most significant word)
 * sim-outorder now computes correct result when non-speculative register
 *       operand is first defined speculative within the same inst
 * speculative fault handling simplified
 * dead variable "no_ea_dep" removed
 *
 * Revision 1.4  1997/04/16  22:10:23  taustin
 * added -commit:width support (from kskadron)
 * fixed "bad l2 D-cache parms" fatal string
 *
 * Revision 1.3  1997/03/11  17:17:06  taustin
 * updated copyright
 * `-pcstat' option support added
 * long/int tweaks made for ALPHA target support
 * better defaults defined for caches/TLBs
 * "mstate" command supported added for DLite!
 * supported added for non-GNU C compilers
 * buglet fixed in speculative trace generation
 * multi-level cache hierarchy now supported
 * two-level predictor supported added
 * I/D-TLB supported added
 * many comments added
 * options package supported added
 * stats package support added
 * resource configuration options extended
 * pipetrace support added
 * DLite! support added
 * writeback throttling now supported
 * decode and issue B/W now decoupled
 * new and improved (and more precise) memory scheduler added
 * cruft for TLB paper removed
 *
 * Revision 1.1  1996/12/05  18:52:32  taustin
 * Initial revision
 *
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <signal.h>

#include "host.h"
#include "misc.h"
#include "machine.h"
#include "regs.h"
#include "memory.h"
#include "cache.h"
#include "loader.h"
#include "syscall.h"
#include "bpred.h"
#include "resource.h"
#include "bitmap.h"
#include "options.h"
#include "eval.h"
#include "stats.h"
#include "ptrace.h"
#include "dlite.h"
#include "sim.h"

/* added for power analysis */
#ifdef PANALYZER_H
#ifdef LV1_PANALYZER_H
#include "libpa_lv1/lv1_opts.h" 
#include "libpa_lv1/lv1_panalyzer.h"
#endif

#ifdef LV2_PANALYZER_H
#include "./libpanalyzer/technology.h"
#include "./libpanalyzer/panalyzer.h"
#include "./libpanalyzer/clock_panalyzer.h"
#include "./libpanalyzer/io_panalyzer.h"
#include "./libpanalyzer/memory_panalyzer.h"
#include "./libpanalyzer/cache_panalyzer.h"
#include "./libpanalyzer/logic_panalyzer.h"
#ifdef FP_PANALYZER_H
#include "./libpanalyzer/alu_panalyzer.h"
#include "./libpanalyzer/mult_panalyzer.h"
#include "./libpanalyzer/fpu_panalyzer.h"
#endif
#include "./libpanalyzer/uarch_panalyzer.h"
#endif

#endif

/*
 * This file implements a very detailed out-of-order issue superscalar processor with a two-level memory system and speculative execution support.  
 * This simulator is a performance simulator, tracking the latency of all pipeline operations.
 */

/* simulated registers */
static struct regs_t regs;

/* simulated memory */
static struct mem_t *mem = NULL;
static int in_flow = FALSE, flow_index, nflow;

/* ADDED */
static int jmp_flow = FALSE;
static struct md_uop_t flowtab[MD_MAX_FLOWLEN];

/*
 * simulator options
 */

/* Ludo: global variables to print a trace to a tracefile */
static unsigned int trace_me;
static char * tracefilename;
FILE * tracefile = NULL;


/* maximum number of inst's to execute */
static unsigned int max_insts;

/* number of inst's to execute before verbose output starts */
static unsigned int trigger_inst;

/* number of insts skipped before timing starts */
static int fastfwd_count;

/* pipeline trace range and output filename */
static int ptrace_nelt = 0;
static char *ptrace_opts[2];

/* profiles the IPC values if on */
static int profile_mode;
 
/* instruction fetch queue size (in insts) */
static int ruu_ifq_size;

/* extra branch mis-prediction latency */
static int ruu_branch_penalty;

/* speed of front-end of machine relative to execution core */
static int fetch_speed;

/* branch predictor type {nottaken|taken|perfect|bimod|2lev} */
static char *pred_type;

/* bimodal predictor config (<table_size>) */
static int bimod_nelt = 1;
int bimod_config[1] =
  { /* bimod tbl size */2048 };

/* 2-level predictor config (<l1size> <l2size> <hist_size> <xor>) */
static int twolev_nelt = 4;
int twolev_config[4] =
  { /* l1size */1, /* l2size */1024, /* hist */8, /* xor */FALSE};

/* combining predictor config (<meta_table_size> */
static int comb_nelt = 1;
int comb_config[1] =
  { /* meta_table_size */1024 };

/* return address stack (RAS) size */
int ras_size = 8;

/* BTB predictor config (<num_sets> <associativity>) */
static int btb_nelt = 2;
int btb_config[2] =
  { /* nsets */512, /* assoc */4 };

/* instruction decode B/W (insts/cycle) */
int ruu_decode_width;

/* instruction issue B/W (insts/cycle) */
int ruu_issue_width;

/* run pipeline with in-order issue */
static int ruu_inorder_issue;
/* issue instructions down wrong execution paths */
static int ruu_include_spec = TRUE;

/* instruction commit B/W (insts/cycle) */
int ruu_commit_width;

/* register update unit (RUU) size */
int RUU_size = 8;

/* load/store queue (LSQ) size */
int LSQ_size = 4;

/* l1 data cache config, i.e., {<config>|none} */
char *cache_dl1_opt;

/* l1 data cache hit latency (in cycles) */
static int cache_dl1_lat;

/* l2 data cache config, i.e., {<config>|none} */
char *cache_dl2_opt;

/* l2 data cache hit latency (in cycles) */
static int cache_dl2_lat;

/* l1 instruction cache config, i.e., {<config>|dl1|dl2|none} */
char *cache_il1_opt;

/* l1 instruction cache hit latency (in cycles) */
static int cache_il1_lat;

/* l2 instruction cache config, i.e., {<config>|dl1|dl2|none} */
char *cache_il2_opt;

/* l2 instruction cache hit latency (in cycles) */
static int cache_il2_lat;

/* flush caches on system calls */
static int flush_on_syscalls;

/* convert 64-bit inst addresses to 32-bit inst equivalents */
static int compress_icache_addrs;

/* memory access latency (<first_chunk> <inter_chunk>) */
static int mem_nelt = 2;
static int mem_lat[2] =
	{ /* lat to first chunk */6, /* lat between remaining chunks */2 };

/* memory access bus width (in bytes) */
/* static */ int mem_bus_width;

/* memory accesses are fully pipelined */
static int mem_pipelined;

/* instruction TLB config, i.e., {<config>|none} */
char *itlb_opt;

/* data TLB config, i.e., {<config>|none} */
char *dtlb_opt;

/* inst/data TLB miss latency (in cycles) */
static int tlb_miss_lat;

/* total number of integer ALU's available */
int res_ialu;

/* total number of integer multiplier/dividers available */
static int res_imult;

/* total number of memory system ports available (to CPU) */
int res_memport;

/* total number of floating point ALU's available */
int res_fpalu;

/* total number of floating point multiplier/dividers available */
static int res_fpmult;

/* power analysis options */ 
#ifdef PANALYZER_H
#ifdef LV1_PANALYZER_H
static double lv1_volt;
static double lv1_freq;
static double lv1_alu_Ceff;
static double lv1_mult_Ceff;
static double lv1_rf_Ceff;
static double lv1_bpred_Ceff;
static double lv1_clock_Ceff;
static double lv1_fpu_Ceff;
static char *lv1_io_option;
static fu_lv1_io_arg_t lv1_io_arg;
static fu_lv1_uarch_arg_t lv1_uarch_arg;
static fu_lv1_pwr_frame_t * lv1_pwr_frame;
static fu_lv1_cache_Ceff_t lv1_il1_Ceff;
static fu_lv1_cache_Ceff_t lv1_il2_Ceff;
static fu_lv1_cache_Ceff_t lv1_dl1_Ceff;
static fu_lv1_cache_Ceff_t lv1_dl2_Ceff;
static fu_lv1_cache_Ceff_t lv1_itlb_Ceff;
static fu_lv1_cache_Ceff_t lv1_dtlb_Ceff;

#ifdef PA_TRACE_WINDOW_H
static fu_lv1_pa_trace_arg_t lv1_pa_trace_arg;
#endif

#ifdef PA_TRACE_WINDOW_H
static double pa_trace_window[100000];
static char pa_trace_miss_window[100000];
#endif

#ifdef PA_TRANS_COUNT
static int pa_aio_ham_window[100000];
static int pa_dio_ham_window[100000];
#endif

#endif

#ifdef LV2_PANALYZER_H

#ifdef CLOCK_PANALYZER_H
/* clock panalyzer config, i.e., {<config>|none} */
static char *panalyzer_clock_opt;
static fu_clock_pspec_t *clock_pspec;
#endif /* CLOCK_PANALYZER_H */

#ifdef IO_PANALYZER_H
/* address io panalyzer config, i.e., {<config>|none} */
static char *panalyzer_aio_opt;
static fu_io_pspec_t *aio_pspec;

/* data io panalyzer config, i.e., {<config>|none} */
static char *panalyzer_dio_opt;
static fu_io_pspec_t *dio_pspec;
#endif /* IO_PANALYZER_H */

#ifdef MEMORY_PANALYZER_H
/* number of physical integer register file */
#define MD_NUM_PIREGS 64
static char *panalyzer_irf_opt;
static fu_sbank_pspec_t *irf_pspec = NULL;

/* number of physical floating point register file */
#define MD_NUM_PFREGS 64
static char *panalyzer_fprf_opt;
static fu_sbank_pspec_t *fprf_pspec = NULL;
#endif /* MEMORY_PANALYZER_H */

#ifdef CACHE_PANALYZER_H
/* cache panalyzer config, i.e., {<config>|none} */
static char *panalyzer_il1_opt;
static fu_cache_pspec_t *il1_pspec = NULL;

static char *panalyzer_il2_opt;
static fu_cache_pspec_t *il2_pspec = NULL;

static char *panalyzer_dl1_opt;
static fu_cache_pspec_t *dl1_pspec = NULL;

static char *panalyzer_dl2_opt;
static fu_cache_pspec_t *dl2_pspec = NULL;

static char *panalyzer_itlb_opt;
static fu_cache_pspec_t *itlb_pspec = NULL;

static char *panalyzer_dtlb_opt;
static fu_cache_pspec_t *dtlb_pspec = NULL;

static char *panalyzer_btb_opt;
static fu_cache_pspec_t *btb_pspec = NULL;
#endif /* CACHE_PANALYZER_H */

#ifdef MEMORY_PANALYZER_H
static char *panalyzer_bimod_opt;
static fu_sbank_pspec_t *bimod_pspec = NULL;

static char *panalyzer_lev1_opt;
static fu_sbank_pspec_t *lev1_pspec = NULL;

static char *panalyzer_lev2_opt;
static fu_sbank_pspec_t *lev2_pspec = NULL;

static char *panalyzer_ras_opt;
static fu_sbank_pspec_t *ras_pspec = NULL;
#endif /* MEMORY_PANALYZER_H */

/* uarch panalyzer config, i.e., {<config>} */
#ifdef UARCH_PANALYZER_H
static char *panalyzer_uarch_opt;
static fu_uarch_pspec_t *uarch_pspec = NULL;
#endif /* UARCH_PANALYZER_H */

#endif

#ifdef LOGIC_PANALYZER_H 
static char *panalyzer_logic_opt;
static fu_logic_pspec_t *logic_pspec = NULL;
#endif

#ifdef FP_PANALYZER_H
static double alu_Ceff;
static double mult_Ceff;
static double fpu_Ceff;

static fu_alu_pspec_t * alu_pspec = NULL;
static fu_mult_pspec_t * mult_pspec = NULL;
static fu_fpu_pspec_t * fpu_pspec = NULL;
#endif
#endif 

/* text-based stat profiles */
#define MAX_PCSTAT_VARS 8
static int pcstat_nelt = 0;
static char *pcstat_vars[MAX_PCSTAT_VARS];

/* convert 64-bit inst text addresses to 32-bit inst equivalents */
#ifdef TARGET_PISA
#define IACOMPRESS(A) (compress_icache_addrs ? ((((A) - ld_text_base) >> 1) + ld_text_base) : (A))
#define ISCOMPRESS(SZ) (compress_icache_addrs ? ((SZ) >> 1) : (SZ))
#else /* !TARGET_PISA */
#define IACOMPRESS(A)		(A)
#define ISCOMPRESS(SZ)		(SZ)
#endif /* TARGET_PISA */

/* operate in backward-compatible bugs mode (for testing only) */
static int bugcompat_mode;

/* perfect memory disambiguation */
static int perfect_disambig = FALSE;

/*
 * functional unit resource configuration
 */

/* resource pool indices, NOTE: update these if you change FU_CONFIG */
#define FU_IALU_INDEX			0
#define FU_IMULT_INDEX			1
#define FU_MEMPORT_INDEX		2
#define FU_FPALU_INDEX			3
#define FU_FPMULT_INDEX			4

/* resource pool definition, NOTE: update FU_*_INDEX defs if you change this */
struct res_desc fu_config[] = {
	{
		"integer-ALU",
		4,
		0,
		{
			{ IntALU, 1, 1 }
		}
	},
	{
		"integer-MULT/DIV",
		1,
		0,
		{
			{ IntMULT, 3, 1 },
			{ IntDIV, 20, 19 }
		}
	},
	{
		"memory-port",
		2,
		0,
		{
			{ RdPort, 1, 1 },
			{ WrPort, 1, 1 }
		}
	},
	{
		"FP-adder",
		4,
		0,
		{
			{ FloatADD, 2, 1 },
			{ FloatCMP, 2, 1 },
			{ FloatCVT, 2, 1 }
		}
	},
	{
		"FP-MULT/DIV",
		1,
		0,
		{
			{ FloatMULT, 4, 1 },
			{ FloatDIV, 12, 12 },
			{ FloatSQRT, 24, 24 }
		}
	}
};


/*
 * simulator stats
 */
/* stats database for ptrace */
struct stat_sdb_t *ptrace_sdb;

/*profiling statistics */
static int max_ipc_time=0;         /* max ipc*/
static int min_ipc_time=0;         /* min ipc*/

    /* max and min ipc when sim_num_insn > 10mil */
static int maxm_ipc_time=0;        
static int minm_ipc_time=0;
 
static float max_ipc_value=0;         /* max ipc*/
static float min_ipc_value=0;         /* min ipc*/
    /* max and min ipc when sim_num_insn > 10mil */
static float maxm_ipc_value=0;        
static float minm_ipc_value=0;
    /* counters that mark the start of ranges with large IPC changes */
static int range[5];
    /* range values  */
static float range_val[5];
static float range_max=0;
static float range_min=0;

/* SLIP variable */
static counter_t sim_slip = 0;

/* total number of instructions executed */
static counter_t sim_total_insn = 0;

/* track number of UOPs executed */
static counter_t sim_num_uops = 0;

/* total number of memory references committed */
static counter_t sim_num_refs = 0;

/* total number of memory references executed */
static counter_t sim_total_refs = 0;

/* total number of loads committed */
static counter_t sim_num_loads = 0;

/* total number of loads executed */
static counter_t sim_total_loads = 0;

/* total number of branches committed */
static counter_t sim_num_branches = 0;

/* total number of branches executed */
static counter_t sim_total_branches = 0;

/* cycle counter */
//tick_t sim_cycle = 1;
static tick_t sim_cycle = 0;

/* occupancy counters */
static counter_t IFQ_count;		/* cumulative IFQ occupancy */
static counter_t IFQ_fcount;	/* cumulative IFQ full count */
static counter_t RUU_count;		/* cumulative RUU occupancy */
static counter_t RUU_fcount;	/* cumulative RUU full count */
static counter_t LSQ_count;		/* cumulative LSQ occupancy */
static counter_t LSQ_fcount;	/* cumulative LSQ full count */

/* total non-speculative bogus addresses seen (debug var) */
static counter_t sim_invalid_addrs;

/* simulator state variables */


/* instruction sequence counter, used to assign unique id's to insts */
static unsigned int inst_seq = 0;

/* pipetrace instruction sequence counter */
static unsigned int ptrace_seq = 0;

/* speculation mode, non-zero when mis-speculating, i.e., executing instructions down the wrong path, 
 * thus state recovery will eventually have to occur that resets processor register and memory state back to the last precise state */
static int spec_mode = FALSE;

/* cycles until fetch issue resumes */
static unsigned ruu_fetch_issue_delay = 0;

/* perfect prediction enabled */
static int pred_perfect = FALSE;

/* speculative bpred-update enabled */
static char *bpred_spec_opt;
static enum { spec_ID, spec_WB, spec_CT } bpred_spec_update;

/* level 1 instruction cache, entry level instruction cache */
static struct cache_t *cache_il1;

/* level 1 instruction cache */
static struct cache_t *cache_il2;

/* level 1 data cache, entry level data cache */
static struct cache_t *cache_dl1;

/* level 2 data cache */
static struct cache_t *cache_dl2;

/* instruction TLB */
static struct cache_t *itlb;

/* data TLB */
static struct cache_t *dtlb;

/* branch predictor */
static struct bpred_t *pred;

/* functional unit resource pool */
static struct res_pool *fu_pool = NULL;

/* text-based stat profiles */
static struct stat_stat_t *pcstat_stats[MAX_PCSTAT_VARS];
static counter_t pcstat_lastvals[MAX_PCSTAT_VARS];
static struct stat_stat_t *pcstat_sdists[MAX_PCSTAT_VARS];

/* wedge all stat values into a counter_t */
#define STATVAL(STAT)							\
  ((STAT)->sc == sc_int							\
   ? (counter_t)*((STAT)->variant.for_int.var)			\
   : ((STAT)->sc == sc_uint						\
      ? (counter_t)*((STAT)->variant.for_uint.var)		\
      : ((STAT)->sc == sc_counter					\
	 ? *((STAT)->variant.for_counter.var)				\
	 : (panic("bad stat class"), 0))))


/* cycle when memory is ready next */
static time_t mem_ready = 0;


/* by Taeho Kgil */
static counter_t concurrent_misses;
// ends here

/* memory access latency, assumed to not cross a page boundary */
static unsigned int	/* total latency of access */
mem_access_latency(int blk_sz) /* block size accessed */
{
	int chunks = (blk_sz + (mem_bus_width - 1)) / mem_bus_width;

	assert(chunks > 0);

	if(mem_pipelined) 
	{
		return (/* first chunk latency */mem_lat[0] + (/* remainder chunk latency */mem_lat[1] * (chunks - 1)));
	}
	else /* !mem_pipelined */
	{
		unsigned int total_lat, lat = (/* first chunk latency */mem_lat[0] + (/* remainder chunk latency */mem_lat[1] * (chunks - 1)));
		if(mem_ready > sim_cycle)
		{
			total_lat = (lat + (mem_ready - sim_cycle)); 
			mem_ready += lat;
		}
		else /* mem_ready <= sim_cycle */
		{
			total_lat = lat;
			mem_ready = sim_cycle + lat;
		}
		return total_lat;
	}
}


/*
 * cache miss handlers
 */

/* l1 data cache l1 block miss handler function */
/*
enum mem_cmd cmd,			: access cmd, Read or Write
md_addr_t baddr,			: block address to access 
int bsize,					: size of block to access 
struct cache_blk_t *blk,	: ptr to block in upper level 
tick_t now					: time of access 
*/
static unsigned int			/* latency of block access */
dl1_access_fn(enum mem_cmd cmd, md_addr_t baddr, int bsize, struct cache_blk_t *blk, tick_t now)		
{
	unsigned int lat;

	if(cache_dl2)
	{
		/* access next level of data cache hierarchy */
#ifdef VANALYZER_H
		lat = cache_access(cache_dl2, cmd, baddr, &blk->data[0], bsize, /* now */now, /* pudata */NULL, /* repl addr */NULL);

#ifdef PANALYZER_H
#ifdef LV1_PANALYZER_H
		lv1_panalyzer(lv1_dl2,(int) cmd);
#endif
#endif

#ifdef PANALYZER_H
#ifdef LV2_PANALYZER_H
#ifdef CACHE_PANALYZER_H
        cache_panalyzer(dl2_pspec, cmd, baddr, &blk->data[0], bsize, now, lat);
#endif /* CACHE_PANALYZER_H */
#endif /* LV2_PANALYZER_H */
#endif /* PANALYZER_H */


#else /* VANALYZER_H */
		lat = cache_access(cache_dl2, cmd, baddr, NULL, bsize, /* now */now, /* pudata */NULL, /* repl addr */NULL);

#ifdef PANALYZER_H
#ifdef LV2_PANALYZER_H
#ifdef CACHE_PANALYZER_H
        cache_panalyzer(dl2_pspec, cmd, baddr, &blk->data[0], bsize, now, lat);
#endif /* CACHE_PANALYZER_H */
#endif /* LV2_PANALYZER_H */
#endif /* PANALYZER_H */


		
#endif /* VANALYZER_H */
		if(cmd == Read)
		{
			return lat;
		}
		else
		{
			/* FIXME: unlimited write buffers */
			return 0;
		}
	}
	else
	{
		unsigned int mem_lat; 
		/* access main memory */
#ifdef VANALYZER_H
		if(mem_access(mem, cmd, baddr, &blk->data[0], bsize) != md_fault_none) 
			fatal("memory access fault occurs!\n");
#endif /* VANALYZER_H */
		mem_lat = mem_access_latency(bsize);
#ifdef PANALYZER_H

#ifdef LV2_PANALYZER_H
#ifdef IO_PANALYZER_H
      aio_panalyzer(aio_pspec, cmd, baddr, &blk->data[0], bsize, sim_cycle, mem_lat);
	  dio_panalyzer(dio_pspec, cmd, baddr, &blk->data[0], bsize, sim_cycle, mem_lat);
#endif /* IO_PANALYZER_H */
#endif /* LV2_PANALYZER_H */
		
#ifdef LV1_PANALYZER_H
#ifdef LV1_IO_PANALYZER_H
		lv1_io_arg.command = cmd;
		lv1_io_arg.address = baddr;
		lv1_io_arg.buffer = &blk->data[0];
		lv1_io_arg.pwr_frame = lv1_pwr_frame;
		lv1_io_arg.bsize = bsize;
		lv1_io_arg.now = sim_cycle;
		lv1_io_arg.lat = mem_lat;
		lv1_panalyzer(lv1_io,(int) (&lv1_io_arg));
#endif				
#ifdef PA_TRACE_WINDOW_H
	    lv1_pa_trace_arg.cache_miss = 1;
#endif
#endif	
#endif
		if (cmd == Read)
		{
			return mem_lat;
		}
		else
		{
			/* FIXME: unlimited write buffers */
			return 0;
		}
	}
}

/* l2 data cache block miss handler function */
static unsigned int			/* latency of block access */
dl2_access_fn(enum mem_cmd cmd,	md_addr_t baddr, int bsize, struct cache_blk_t *blk, tick_t now)		
{
	unsigned int mem_lat;
	/* this is a miss to the lowest level, so access main memory */
#ifdef VANALYZER_H
    if ( cmd != Write )
        if(mem_access(mem, cmd, baddr, &blk->data[0], bsize) != md_fault_none) 
            fatal("memory access fault occurs!\n"); 
#endif /* VANALYZER_H */
	mem_lat = mem_access_latency(bsize);

#ifdef PANALYZER_H
#ifdef LV1_PANALYZER_H
#ifdef LV1_IO_PANALYZER_H
		lv1_io_arg.command = cmd;
		lv1_io_arg.address = baddr;
		lv1_io_arg.buffer = &blk->data[0];
		lv1_io_arg.pwr_frame = lv1_pwr_frame;
		lv1_io_arg.bsize = bsize;
		lv1_io_arg.now = sim_cycle;
		lv1_io_arg.lat = mem_lat;
		lv1_panalyzer(lv1_io,(int) (&lv1_io_arg));
#endif				
#ifdef PA_TRACE_WINDOW_H
	    lv1_pa_trace_arg.cache_miss = 1;
#endif
#endif		
#endif

#ifdef LV2_PANALYZER_H
#ifdef IO_PANALYZER_H
   aio_panalyzer(aio_pspec, cmd, baddr, &blk->data[0], bsize, sim_cycle, mem_lat);
      dio_panalyzer(dio_pspec, cmd, baddr, &blk->data[0], bsize, sim_cycle, mem_lat);
#endif /* IO_PANALYZER_H */
#endif

	if(cmd == Read)
	{ 
		return mem_lat;
	}
	else
	{
		/* FIXME: unlimited write buffers */
		return 0;
	}
}

/* l1 inst cache l1 block miss handler function */
static unsigned int	/* latency of block access */
il1_access_fn(enum mem_cmd cmd, md_addr_t baddr, int bsize, struct cache_blk_t *blk, tick_t now)		
{
	unsigned int lat;

	if(cache_il2)
	{
		/* access next level of inst cache hierarchy */
#ifdef VANALYZER_H
		lat = cache_access(cache_il2, cmd, baddr, &blk->data[0], bsize, /* now */now, /* pudata */NULL, /* repl addr */NULL);

#ifdef PANALYZER_H
#ifdef LV1_PANALYZER_H
		lv1_panalyzer(lv1_il2,(int) cmd);
#endif	

#ifdef LV2_PANALYZER_H
#ifdef CACHE_PANALYZER_H
    	cache_panalyzer(il2_pspec, cmd, baddr, &blk->data[0], bsize, now, lat);
#endif /* CACHE_PANALYZER_H */
#endif

#endif


#else /* VANALYZER_H */
		lat = cache_access(cache_il2, cmd, baddr, NULL, bsize, /* now */now, /* pudata */NULL, /* repl addr */NULL);
#endif /* VANALYZER_H */
		if(cmd == Read)
		{
			return lat;
		}
		else
		{
			panic("writes to instruction memory not supported");
		}
	}
	else
	{
		unsigned int mem_lat;
		/* access main memory */
#ifdef VANALYZER_H
		if(mem_access(mem, cmd, baddr, &blk->data[0], bsize) != md_fault_none) 
			fatal("memory access fault occurs!\n");
#endif /* VANALYZER_H */
		mem_lat = mem_access_latency(bsize);

#ifdef PANALYZER_H
#ifdef LV1_PANALYZER_H
#ifdef LV1_IO_PANALYZER_H
		lv1_io_arg.command = cmd;
		lv1_io_arg.address = baddr;
		lv1_io_arg.buffer = &blk->data[0];
		lv1_io_arg.bsize = bsize;
		lv1_io_arg.pwr_frame = lv1_pwr_frame;
		lv1_io_arg.now = sim_cycle;
		lv1_io_arg.lat = mem_lat;

		lv1_panalyzer(lv1_io,(int) (&lv1_io_arg));
#endif				
#endif

#ifdef LV2_PANALYZER_H
#ifdef IO_PANALYZER_H
        aio_panalyzer(aio_pspec, cmd, baddr, &blk->data[0], bsize, sim_cycle, mem_lat);
		        dio_panalyzer(dio_pspec, cmd, baddr, &blk->data[0], bsize, sim_cycle, mem_lat);
#endif /* IO_PANALYZER_H */
#endif
										
#endif


		if(cmd == Read)
		{
			return mem_lat;
		}
		else
		{
			panic("writes to instruction memory not supported");
		}
	}
}

/* l2 inst cache block miss handler function */
static unsigned int	/* latency of block access */
il2_access_fn(enum mem_cmd cmd, md_addr_t baddr, int bsize, struct cache_blk_t *blk, tick_t now)
{
	unsigned int mem_lat;

	/* this is a miss to the lowest level, so access main memory */
#ifdef VANALYZER_H
    if ( cmd != Write )
        if(mem_access(mem, cmd, baddr, &blk->data[0], bsize) != md_fault_none) 
            fatal("memory access fault occurs!\n"); 
#endif /* VANALYZER */

	mem_lat = mem_access_latency(bsize);
	
#ifdef PANALYZER_H
#ifdef LV1_PANALYZER_H
#ifdef LV1_IO_PANALYZER_H
		lv1_io_arg.command = cmd;
		lv1_io_arg.address = baddr;
		lv1_io_arg.buffer = &blk->data[0];
		lv1_io_arg.bsize = bsize;
		lv1_io_arg.pwr_frame = lv1_pwr_frame;
		lv1_io_arg.now = sim_cycle;
		lv1_io_arg.lat = mem_lat;

		lv1_panalyzer(lv1_io,(int) (&lv1_io_arg));
#endif				
#endif

#ifdef LV2_PANALYZER_H
#ifdef IO_PANALYZER_H
     aio_panalyzer(aio_pspec, cmd, baddr, &blk->data[0], bsize, sim_cycle, mem_lat);
	 dio_panalyzer(dio_pspec, cmd, baddr, &blk->data[0], bsize, sim_cycle, mem_lat);
#endif /* IO_PANALYZER_H */
#endif
										
#endif
	if(cmd == Read)
	{ 
		return mem_lat;
	}
	else
	{
		panic("writes to instruction memory not supported");
	}
}


/* TLB miss handlers */
/* inst cache block miss handler function */
static unsigned int	/* latency of block access */
itlb_access_fn(enum mem_cmd cmd, md_addr_t baddr, int bsize, struct cache_blk_t *blk, tick_t now)		
{
	md_addr_t *phy_page_ptr = (md_addr_t *)blk->user_data;

	/* no real memory access, however, should have user data space attached */
	assert(phy_page_ptr); 

	/* fake translation, for now... */
	*phy_page_ptr = 0;

	/* return tlb miss latency */
	return tlb_miss_lat;
}

/* data cache block miss handler function */
static unsigned int	/* latency of block access */
dtlb_access_fn(enum mem_cmd cmd, md_addr_t baddr, int bsize, struct cache_blk_t *blk, tick_t now)		
{
	md_addr_t *phy_page_ptr = (md_addr_t *)blk->user_data;

	/* no real memory access, however, should have user data space attached */
	assert(phy_page_ptr);

	/* fake translation, for now... */
	*phy_page_ptr = 0;

	/* return tlb miss latency */
	return tlb_miss_lat;
}


/* register simulator-specific options */
void
sim_reg_options(struct opt_odb_t *odb)
{
  opt_reg_header(odb, 
"sim-outorder: This simulator implements a very detailed out-of-order issue\n"
"superscalar processor with a two-level memory system and speculative\n"
"execution support.  This simulator is a performance simulator, tracking the\n"
"latency of all pipeline operations.\n");

	/* instruction limit */
	opt_reg_uint(odb, 
"-max:inst", "maximum number of inst's to execute", &max_insts, /* default */0, /* print */TRUE, /* format */NULL);

	opt_reg_uint(odb, "-trigger:inst", "trigger instruction",
               &trigger_inst, /* default */0,
               /* print */TRUE, /* format */NULL);

	/* trace options */
	/* Ludo: added some simpler trace options for a inorder execution as on the arm */
	opt_reg_flag(odb, "-trace", "trace program execution",
		&trace_me, /* default */FALSE,
	    /* print */TRUE, /* format */NULL);
	opt_reg_string(odb, "-tracefile", "output trace to file",
	    &tracefilename, /* default */"simpanal.trace",
	    /* print */TRUE, /* format */NULL);
	
	/* use the specified pipe instead of a file, or stdio */
	opt_reg_int(odb, 
"-ptracepipe", "The pipe for the ptrace communications", &ptracepipe, /* default */0, /* !print */FALSE, /* format */NULL);
	opt_reg_flag(odb, 
"-profile", "look for areas with significant IPCs", &profile_mode, /* default */FALSE, /* print */TRUE, /* format */NULL);

	opt_reg_int(odb, 
"-fastfwd", "number of insts skipped before timing starts", &fastfwd_count, /* default */0, /* print */TRUE, /* format */NULL);
	opt_reg_string_list(odb, 
"-ptrace", "generate pipetrace, i.e., <fname|stdout|stderr> <range>", ptrace_opts, /* arr_sz */2, &ptrace_nelt, /* default */NULL, /* !print */FALSE, /* format */NULL, /* !accrue */FALSE);

	opt_reg_note(odb,
"  Pipetrace range arguments are formatted as follows:\n"
"\n"
"    {{@|#}<start>}:{{@|#|+}<end>}\n"
"\n"
"  Both ends of the range are optional, if neither are specified, the entire\n"
"  execution is traced.  Ranges that start with a `@' designate an address\n"
"  range to be traced, those that start with an `#' designate a cycle count\n"
"  range.  All other range values represent an instruction count range.  The\n"
"  second argument, if specified with a `+', indicates a value relative\n"
"  to the first argument, e.g., 1000:+100 == 1000:1100.  Program symbols may\n"
"  be used in all contexts.\n"
"\n"
"    Examples:   -ptrace FOO.trc #0:#1000\n"
"                -ptrace BAR.trc @2000:\n"
"                -ptrace BLAH.trc :1500\n"
"                -ptrace UXXE.trc :\n"
"                -ptrace FOOBAR.trc @main:+278\n");

	/* ifetch options */
	opt_reg_int(odb, 
"-fetch:ifqsize", "instruction fetch queue size (in insts)", &ruu_ifq_size, /* default */4, /* print */TRUE, /* format */NULL);

	opt_reg_int(odb, 
"-fetch:mplat", "extra branch mis-prediction latency", &ruu_branch_penalty, /* default */3, /* print */TRUE, /* format */NULL);

	opt_reg_int(odb, 
"-fetch:speed", "speed of front-end of machine relative to execution core", &fetch_speed, /* default */1, /* print */TRUE, /* format */NULL);

	/* branch predictor options */
	opt_reg_note(odb,
"  Branch predictor configuration examples for 2-level predictor:\n"
"    Configurations:   N, M, W, X\n"
"      N   # entries in first level (# of shift register(s))\n"
"      W   width of shift register(s)\n"
"      M   # entries in 2nd level (# of counters, or other FSM)\n"
"      X   (yes-1/no-0) xor history and address for 2nd level index\n"
"    Sample predictors:\n"
"      GAg     : 1, W, 2^W, 0\n"
"      GAp     : 1, W, M (M > 2^W), 0\n"
"      PAg     : N, W, 2^W, 0\n"
"      PAp     : N, W, M (M == 2^(N+W)), 0\n"
"      gshare  : 1, W, 2^W, 1\n"
"  Predictor `comb' combines a bimodal and a 2-level predictor.\n");

	opt_reg_string(odb, 
"-bpred", "branch predictor type {nottaken|taken|perfect|bimod|2lev|comb}", &pred_type, /* default */"bimod", /* print */TRUE, /* format */NULL);

	opt_reg_int_list(odb, 
"-bpred:bimod", "bimodal predictor config (<table size>)", bimod_config, bimod_nelt, &bimod_nelt, /* default */bimod_config, /* print */TRUE, /* format */NULL, /* !accrue */FALSE);

	opt_reg_int_list(odb, 
"-bpred:2lev", "2-level predictor config " "(<l1size> <l2size> <hist_size> <xor>)", twolev_config, twolev_nelt, &twolev_nelt, /* default */twolev_config, /* print */TRUE, /* format */NULL, /* !accrue */FALSE);

	opt_reg_int_list(odb, 
"-bpred:comb", "combining predictor config (<meta_table_size>)", comb_config, comb_nelt, &comb_nelt, /* default */comb_config, /* print */TRUE, /* format */NULL, /* !accrue */FALSE);

	opt_reg_int(odb, 
"-bpred:ras", "return address stack size (0 for no return stack)", &ras_size, /* default */ras_size, /* print */TRUE, /* format */NULL);

	opt_reg_int_list(odb, 
"-bpred:btb", "BTB config (<num_sets> <associativity>)", btb_config, btb_nelt, &btb_nelt, /* default */btb_config, /* print */TRUE, /* format */NULL, /* !accrue */FALSE);

	opt_reg_string(odb, 
"-bpred:spec_update", "speculative predictors update in {ID|WB} (default non-spec)", &bpred_spec_opt, /* default */NULL, /* print */TRUE, /* format */NULL);

	/* decode options */
	opt_reg_int(odb, 
"-decode:width", "instruction decode B/W (insts/cycle)", &ruu_decode_width, /* default */4, /* print */TRUE, /* format */NULL);

	/* issue options */
	opt_reg_int(odb, 
"-issue:width", "instruction issue B/W (insts/cycle)", &ruu_issue_width, /* default */4, /* print */TRUE, /* format */NULL);

	opt_reg_flag(odb, 
"-issue:inorder", "run pipeline with in-order issue", &ruu_inorder_issue, /* default */FALSE, /* print */TRUE, /* format */NULL);

	opt_reg_flag(odb, 
"-issue:wrongpath", "issue instructions down wrong execution paths", &ruu_include_spec, /* default */TRUE, /* print */TRUE, /* format */NULL);

	/* commit options */
	opt_reg_int(odb, 
"-commit:width", "instruction commit B/W (insts/cycle)", &ruu_commit_width, /* default */4, /* print */TRUE, /* format */NULL);

	/* register scheduler options */
	opt_reg_int(odb, 
"-ruu:size", "register update unit (RUU) size", &RUU_size, /* default */16, /* print */TRUE, /* format */NULL);

	/* memory scheduler options  */
	opt_reg_int(odb, 
"-lsq:size", "load/store queue (LSQ) size", &LSQ_size, /* default */8, /* print */TRUE, /* format */NULL);
	opt_reg_flag(odb, 
"-lsq:perfect", "perfect memory disambiguation", &perfect_disambig, /* default */FALSE, /* print */TRUE, NULL);


	/* cache options */
	opt_reg_string(odb, 
"-cache:dl1", "l1 data cache config, i.e., {<config>|none}", &cache_dl1_opt, "dl1:128:32:4:l", /* print */TRUE, NULL);

	opt_reg_note(odb,
"  The cache config parameter <config> has the following format:\n"
"\n"
"    <name>:<nsets>:<bsize>:<assoc>:<repl>\n"
"\n"
"    <name>   - name of the cache being defined\n"
"    <nsets>  - number of sets in the cache\n"
"    <bsize>  - block size of the cache\n"
"    <assoc>  - associativity of the cache\n"
"    <repl>   - block replacement strategy, 'l'-LRU, 'f'-FIFO, 'r'-random\n"
"\n"
"    Examples:   -cache:dl1 dl1:4096:32:1:l\n"
"                -dtlb dtlb:128:4096:32:r\n");

	opt_reg_int(odb, 
"-cache:dl1lat", "l1 data cache hit latency (in cycles)", &cache_dl1_lat, /* default */1, /* print */TRUE, /* format */NULL);

	opt_reg_string(odb, 
"-cache:dl2", "l2 data cache config, i.e., {<config>|none}", &cache_dl2_opt, "ul2:1024:64:4:l", /* print */TRUE, NULL);

	opt_reg_int(odb, 
"-cache:dl2lat", "l2 data cache hit latency (in cycles)", &cache_dl2_lat, /* default */6, /* print */TRUE, /* format */NULL);

	opt_reg_string(odb, 
"-cache:il1", "l1 inst cache config, i.e., {<config>|dl1|dl2|none}", &cache_il1_opt, "il1:512:32:1:l", /* print */TRUE, NULL);

	opt_reg_note(odb,
"  Cache levels can be unified by pointing a level of the instruction cache\n"
"  hierarchy at the data cache hiearchy using the \"dl1\" and \"dl2\" cache\n"
"  configuration arguments.  Most sensible combinations are supported, e.g.,\n"
"\n"
"    A unified l2 cache (il2 is pointed at dl2):\n"
"      -cache:il1 il1:128:64:1:l -cache:il2 dl2\n"
"      -cache:dl1 dl1:256:32:1:l -cache:dl2 ul2:1024:64:2:l\n"
"\n"
"    Or, a fully unified cache hierarchy (il1 pointed at dl1):\n"
"      -cache:il1 dl1\n"
"      -cache:dl1 ul1:256:32:1:l -cache:dl2 ul2:1024:64:2:l\n");

	opt_reg_int(odb, 
"-cache:il1lat", "l1 instruction cache hit latency (in cycles)", &cache_il1_lat, /* default */1, /* print */TRUE, /* format */NULL);

	opt_reg_string(odb, 
"-cache:il2", "l2 instruction cache config, i.e., {<config>|dl2|none}", &cache_il2_opt, "dl2", /* print */TRUE, NULL);

	opt_reg_int(odb, 
"-cache:il2lat", "l2 instruction cache hit latency (in cycles)", &cache_il2_lat, /* default */6, /* print */TRUE, /* format */NULL);

	opt_reg_flag(odb, 
"-cache:flush", "flush caches on system calls", &flush_on_syscalls, /* default */FALSE, /* print */TRUE, NULL);

	opt_reg_flag(odb, 
"-cache:icompress", "convert 64-bit inst addresses to 32-bit inst equivalents", &compress_icache_addrs, /* default */FALSE, /* print */TRUE, NULL);

/* mem options */
	opt_reg_int_list(odb, 
"-mem:lat", "memory access latency (<first_chunk> <inter_chunk>)", mem_lat, mem_nelt, &mem_nelt, mem_lat, /* print */TRUE, /* format */NULL, /* !accrue */FALSE);

	opt_reg_int(odb, 
"-mem:width", "memory access bus width (in bytes)", &mem_bus_width, /* default */8, /* print */TRUE, /* format */NULL);

	opt_reg_flag(odb, 
"-mem:pipelined", "memory accesses are fully pipelined", &mem_pipelined, /* default */TRUE, /* print */TRUE, NULL);


	/* TLB options */
	opt_reg_string(odb, 
"-tlb:itlb", "instruction TLB config, i.e., {<config>|none}", &itlb_opt, "itlb:16:4096:4:l", /* print */TRUE, NULL);

	opt_reg_string(odb, 
"-tlb:dtlb", "data TLB config, i.e., {<config>|none}", &dtlb_opt, "dtlb:32:4096:4:l", /* print */TRUE, NULL);

	opt_reg_int(odb, 
"-tlb:lat", "inst/data TLB miss latency (in cycles)", &tlb_miss_lat, /* default */30, /* print */TRUE, /* format */NULL);

	/* resource configuration */
	opt_reg_int(odb, 
"-res:ialu", "total number of integer ALU's available", &res_ialu, /* default */fu_config[FU_IALU_INDEX].quantity, /* print */TRUE, /* format */NULL);

	opt_reg_int(odb, 
"-res:imult", "total number of integer multiplier/dividers available", &res_imult, /* default */fu_config[FU_IMULT_INDEX].quantity, /* print */TRUE, /* format */NULL);

	opt_reg_int(odb, 
"-res:memport", "total number of memory system ports available (to CPU)", &res_memport, /* default */fu_config[FU_MEMPORT_INDEX].quantity, /* print */TRUE, /* format */NULL);

	opt_reg_int(odb, 
"-res:fpalu", "total number of floating point ALU's available", &res_fpalu, /* default */fu_config[FU_FPALU_INDEX].quantity, /* print */TRUE, /* format */NULL);

	opt_reg_int(odb, 
"-res:fpmult", "total number of floating point multiplier/dividers available", &res_fpmult, /* default */fu_config[FU_FPMULT_INDEX].quantity, /* print */TRUE, /* format */NULL);

	opt_reg_string_list(odb, 
"-pcstat", "profile stat(s) against text addr's (mult uses ok)", pcstat_vars, MAX_PCSTAT_VARS, &pcstat_nelt, NULL, /* !print */FALSE, /* format */NULL, /* accrue */TRUE);

	opt_reg_flag(odb, 
"-bugcompat", "operate in backward-compatible bugs mode (for testing only)", &bugcompat_mode, /* default */FALSE, /* print */TRUE, NULL);

#ifdef PANALYZER_H
#ifdef LV1_PANALYZER_H
	opt_reg_double(odb, 
"-lv1:voltage", "Supply voltage for level 1 power model", &lv1_volt, /* default */2.5, /* print */TRUE, /* format */NULL);
	opt_reg_double(odb, 
"-lv1:freq", "Operating frequency for level 1 power model", &lv1_freq, /* default */200.00, /* print */TRUE, /* format */NULL);


	opt_reg_double(odb, 
"-lv1:alu", "ALU effective Capacitance pF for level 1 power model", &lv1_alu_Ceff, /* default */1.0, /* print */TRUE, /* format */NULL);
	opt_reg_double(odb, 
"-lv1:fpu", "FPU effective Capacitance pF for level 1 power model", &lv1_fpu_Ceff, /* default */1.0, /* print */TRUE, /* format */NULL);

opt_reg_double(odb, 
"-lv1:rf", "Register file effective Capacitance pF for level 1 power model", &lv1_rf_Ceff, /* default */1.0, /* print */TRUE, /* format */NULL);

opt_reg_double(odb, 
"-lv1:mult", "Multiplier effective Capacitance pF for level 1 power model", &lv1_mult_Ceff, /* default */1.0, /* print */TRUE, /* format */NULL);

opt_reg_double(odb, 
"-lv1:bpred", "Branch Predictor effective Capacitance pF for level 1 power model", &lv1_bpred_Ceff, /* default */1.0, /* print */TRUE, /* format */NULL);

opt_reg_string(odb,
"-lv1:io", "IO power configuration {<name:direction:svolt:buswidth:blocksize>}", &lv1_io_option, "io:o:3.3:8:64:1.0:1.0", /* print */TRUE, NULL);

opt_reg_double(odb, 
"-lv1:clock", "Clock Tree effective Capacitance pF for level 1 power model", &lv1_clock_Ceff, /* default */1.0, /* print */TRUE, /* format */NULL);

opt_reg_double(odb, 
"-lv1:cache_il1:iCeff", "Level 1 Inst Cache internal effective Capacitance pF for level 1 power model", &(lv1_il1_Ceff.iCeff), /* default */1.0, /* print */TRUE, /* format */NULL);
opt_reg_double(odb, 
"-lv1:cache_il1:eCeff", "Level 1 Inst Cache external effective Capacitance pF for level 1 power model", &(lv1_il1_Ceff.eCeff), /* default */1.0, /* print */TRUE, /* format */NULL);

opt_reg_double(odb, 
"-lv1:cache_il2:iCeff", "Level 2 Inst Cache internal effective Capacitance pF for level 1 power model", &(lv1_il2_Ceff.iCeff), /* default */1.0, /* print */TRUE, /* format */NULL);
opt_reg_double(odb, 
"-lv1:cache_il2:eCeff", "Level 2 Inst Cache external effective Capacitance pF for level 1 power model", &(lv1_il2_Ceff.eCeff), /* default */1.0, /* print */TRUE, /* format */NULL);

opt_reg_double(odb, 
"-lv1:cache_dl1:iCeff", "Level 1 Data Cache internal effective Capacitance pF for level 1 power model", &(lv1_dl1_Ceff.iCeff), /* default */1.0, /* print */TRUE, /* format */NULL);
opt_reg_double(odb, 
"-lv1:cache_dl1:eCeff", "Level 1 Data Cache external effective Capacitance pF for level 1 power model", &(lv1_dl1_Ceff.eCeff), /* default */1.0, /* print */TRUE, /* format */NULL);

opt_reg_double(odb, 
"-lv1:cache_dl2:iCeff", "Level 2 Data Cache internal effective Capacitance pF for level 1 power model", &(lv1_dl2_Ceff.iCeff), /* default */1.0, /* print */TRUE, /* format */NULL);
opt_reg_double(odb, 
"-lv1:cache_dl2:eCeff", "Level 2 Data Cache external effective Capacitance pF for level 1 power model", &(lv1_dl2_Ceff.eCeff), /* default */1.0, /* print */TRUE, /* format */NULL);

opt_reg_double(odb, 
"-lv1:itlb:iCeff", "Level 1 ITLB internal effective Capacitance pF for level 1 power model", &(lv1_itlb_Ceff.iCeff), /* default */1.0, /* print */TRUE, /* format */NULL);
opt_reg_double(odb, 
"-lv1:itlb:eCeff", "Level 1 ITLB Cache external effective Capacitance pF for level 1 power model", &(lv1_itlb_Ceff.eCeff), /* default */1.0, /* print */TRUE, /* format */NULL);

opt_reg_double(odb, 
"-lv1:dtlb:iCeff", "Level 1 DTLB Cache internal effective Capacitance pF for level 1 power model", &(lv1_dtlb_Ceff.iCeff), /* default */1.0, /* print */TRUE, /* format */NULL);
opt_reg_double(odb, 
"-lv1:dtlb:eCeff", "Level 1 DTLB Cache external effective Capacitance pF for level 1 power model", &(lv1_dtlb_Ceff.eCeff), /* default */1.0, /* print */TRUE, /* format */NULL);

#endif

#ifdef LV2_PANALYZER_H
#ifdef UARCH_PANALYZER_H
  opt_reg_note(odb,
  "  The u-architecture panalyzer config parameter <config> has the following format:\n"
  "\n"
  "Analytical power model - integrate power dissipation of defined power models\n"
  "                         of u-arch functional blocks (e.g, cache, i/o etc.\n"
  "    <name>:<pmodel>:<opfreq>\n"
  "\n"
  "    <name>   - name of the uarch being defined\n"
  "    <pmodel> - power model, 'a'- Analytical, 'e'-Emperical\n"
  "    <opfreq> - operating frequency (MHz)\n"
  "\n"
  "or\n"
  "Empirical power model - ignore power models of u-arch functional blocks\n"
  "    <name>:<pmodel>:<opfreq>:<sCeff>:<iCeff>:<lCeff>\n"
  "\n"
  "    <name>   - name of the system being defined\n"
  "    <pmodel> - power model, 'a'- Analytical, 'e'-Emperical\n"
  "    <opfreq> - operating frequency (MHz)\n"
  "    <sCeff>  - external switching capacitance (pF) of uarch\n"
  "    <iCeff>  - internal switching capacitance (pF) of uarch\n"
  "    <lCeff>  - leakage capacitance (pF) of uarch\n"
  "\n"
  "    Examples:   -panalyzer:uarch uarch:a:233\n"
  "                -panalyzer:uarch uarch:e:233:sCeff:iCeff:lCeff\n");

    opt_reg_string(odb, "-panalyzer:uarch",
	         "u-architecture panalyzer conf",
			          &panalyzer_uarch_opt, "uarch:a:233",
					           /* print */TRUE, NULL);
#endif /* UARCH_PANALYZER_H */
	
#ifdef CLOCK_PANALYZER_H
	  opt_reg_note(odb,
	  "  The system clock panalyzer config parameter <config> has the following format:\n"
	  "\n"
	  "Analytical power model\n"
	  "    <name>:<pmodel>:<opfreq>:<ctstyle>:<cskew><ncbstages_opt>\n"
	  "\n"
	  "    <name>   - name of the clock being defined\n"
	  "    <pmodel> - power model, 'a'- Analytical, 'e'-Emperical\n"
	  "    <opfreq> - operating frequency (MHz)\n"
	  "    <ctstyle>- clock tree style, 'n'- normal H-tree, 'b'- balanced H-tree\n"
	  "    <cskew>  - target clock skew in (ps) - use 5% of clock period\n"
	  "    <ncbstages_opt>\n"
	  "             - optimal clock buffer stage ratio - use 3- 5\n"
	  "\n"
	  "or\n"
	  "Empirical power model\n"
	  "    <name>:<pmodel>:<opfreq>:<sCeff>:<iCeff>:<lCeff>\n"
	  "\n"
	  "    <name>   - name of the clock being defined\n"
	  "    <pmodel> - power model, 'a'- Analytical, 'e'-Emperical\n"
	  "    <opfreq> - operating frequency (MHz)\n"
	  "    <sCeff>  - clocked node capacitance (pF) of a clock tree\n"
	  "    <iCeff>  - internal switching capacitance (pF) of a clock tree buffer\n"
	  "    <lCeff>  - leakage capacitance (pF) of clock tree buffer\n"
	  "\n"
	  "    Examples:   -panalyzer:clock clock:a:233:n:100:4\n"
	  "                -panalyzer:clock clock:e:233:sCeff:iCeff:lCeff\n");
	  
	    opt_reg_string(odb, "-panalyzer:clock",
		         "system clock panalyzer conf",
				          &panalyzer_clock_opt, "clock:a:233:n:90.0:4",
						           /* print */TRUE, NULL);
#endif /* CLOCK_PANALYZER_H */

#ifdef IO_PANALYZER_H
  opt_reg_note(odb,
  "  The io panalyzer config parameter <config> has the following format:\n"
  "\n"
  "Analytical power model\n"
  "    <name>:<pmodel>:<opfreq>:<style>:<nbstages_opt>:<sCeff>\n"
  "\n"
  "    <name>   - name of the io being defined\n"
  "    <pmodel> - power model, 'a'- Analytical, 'e'-Emperical\n"
  "    <opfreq> - operating frequency (MHz)\n"
  "    <style>  - io style, 'i'- in, 'o'- out, 'b'- bidirectional\n"
  "    <iovoltage> - IO Voltage\n"
  "    <nbstages_opt>\n"
  "             - optimal buffer stage ratio - use 3- 5\n"
  "    <micro_striplength> - length of microstrip\n"
  "    <sCeff>  - external switching capacitance (pF) of an i/o pin\n"
  "\n"
  "or\n"
  "Empirical power model\n"
  "    <name>:<pmodel>:<style>:<opfreq>:<sCeff>:<iCeff>:<lCeff>\n"
  "\n"
  "    <name>   - name of the io being defined\n"
  "    <pmodel> - power model, 'a'- Analytical, 'e'-Emperical\n"
  "    <style>  - io style, 'i'- in, 'o'- out, 'b'- bidirectional\n"
  "    <opfreq> - operating frequency in (MHz)\n"
  "    <sCeff>  - external switching capacitance (pF) of an i/o pin\n"
  "    <iCeff>  - internal switching capacitance (pF) of an i/o pin\n"
  "    <lCeff>  - leakage capacitance (pF) of an i/o pin\n"
  "\n"
  "    Examples:   -panalyzer:aio aio:a:233:o:4:25.0\n"
  "                -panalyzer:dio dio:e:233:sCeff:iCeff:lCeff\n");
 //    opt_reg_string(odb, "-panalyzer:aio",
//	         "address i/o panalyzer conf",
//			          &panalyzer_aio_opt, "aio:a:o:233:5:25.0",
//					           /* print */TRUE, NULL);
 
    opt_reg_string(odb, "-panalyzer:aio",
	         "address i/o panalyzer conf",
			          &panalyzer_aio_opt, "aio:a:233:o:2.5:5:2:25.0",
					           /* print */TRUE, NULL);
//	 opt_reg_string(odb, "-panalyzer:dio",
//	           "data i/o panalyzer conf",
//			            &panalyzer_dio_opt, "dio:a:b:233:5:25.0",
//						         /* print */TRUE, NULL);

	  opt_reg_string(odb, "-panalyzer:dio",
	           "data i/o panalyzer conf",
			            &panalyzer_dio_opt, "dio:a:233:b:2.5:5:2:25.0",
						         /* print */TRUE, NULL);
#endif /* IO_PANALYZER_H */

#ifdef MEMORY_PANALYZER_H
  opt_reg_note(odb,
  "\n"
  "  register file panalyzer\n"
  "  The register file panalyzer config parameter <config> has the following format:\n"
  "\n"
  "Analytical power model\n"
  "\n"
  "    <name>:a:<opfreq>:<sCeff>\n"
  "    <name>    - name of the register file being defined\n"
  "                irf - integer, fprf - floating point register files\n"
  "    <pmodel>  - power model, 'a'- Analytical, 'e'-Emperical\n"
  "    <opfreq>  - operating frequency (MHz)\n"
  "    <sCeff>   - switching capacitance (pF) of a register file i/o pin\n"
  "\n"
  "or\n"
  "Empirical power model\n"
  "\n"
  "    <name>:e:<opfreq>:<opvolt>:<sCeff>:<iCeff>:<lCeff>\n"
  "\n"
  "    <name>    - name of the register file being defined\n"
  "                irf - integer, fprf - floating point register files\n"
  "    <pmodel>  - power model, 'a'- Analytical, 'e'-Emperical\n"
  "    <opfreq>  - operating frequency in (MHz)\n"
  "    <sCeff>   - switching capacitance (pF) of a register file i/o pin\n"
  "    <iCeff>   - internal switching capacitance of a register file i/o pin\n"
  "    <lCeff>   - leakage capacitance (pF) of register file i/o pin\n"
  "\n"
  "    Examples:  -panalyzer:irf irf:a:233:sCeff\n"
  "               -panalyzer:fprf fprf:a:233:sCeff\n");
  
    opt_reg_string(odb, "-panalyzer:irf",
	         "integer register file panalyzer conf",
			          &panalyzer_irf_opt, "irf:a:233:0.1",
					           /* print */TRUE, NULL);
	
	  opt_reg_string(odb, "-panalyzer:fprf",
	           "floating point register file panalyzer conf",
			            &panalyzer_fprf_opt, "fprf:a:233:0.1",
						         /* print */TRUE, NULL);
#endif /* MEMORY_PANALYZER_H */

#ifdef CACHE_PANALYZER_H
  opt_reg_note(odb,
  "\n"
  "  cache panalyzer for il1, il2, dl1, dl2, itlb, dtlb, and btb\n"
  "  The cache panalyzer config parameter <config> has the following format:\n"
  "\n"
  "Analytical power model\n"
  "\n"
  "    <name>:a:<opfreq>:<nbls>:<nwls>:<sCeff>\n"
  "    <name>    - name of cache being defined\n"
  "    <pmodel>  - power model, 'a'- Analytical, 'e'-Emperical\n"
  "    <opfreq>  - operating frequency (MHz)\n"
  "    <nbls>    - number of bitlines in the memory array\n"
  "    <nwls>    - number of wordlines in the memory array\n"
  "    <sCeff>   - external switching capacitance (pF) of a cache i/o pin\n"
  "\n"
  "or\n"
  "Empirical power model\n"
  "\n"
  "    <name>:e:<opfreq>:<opvolt>:<sCeff>:<iCeff>:<lCeff>\n"
  "\n"
  "    <name>    - name of cache being defined\n"
  "    <pmodel>  - power model, 'a'- Analytical, 'e'-Emperical\n"
  "    <opfreq>  - operating frequency (MHz)\n"
  "    <sCeff>   - switching capacitance (pF) of a cache i/o pin\n"
  "    <iCeff>   - internal switching capacitance of a cache i/o pin\n"
  "    <lCeff>   - leakage capacitance (pF) of a cache i/o pin\n"
  "\n"
  "    Examples:   -panalyzer:il1 il1:a:233:4:1:sCeff\n"
  "                -panalyzer:dl1 dl1:e:233:sCeff:iCeff:lCeff\n");

  opt_reg_string(odb, "-panalyzer:il1",
           "il1 panalyzer conf",
            &panalyzer_il1_opt, "il1:a:233:1:1:0.1",
	         /* print */TRUE, NULL);
  
  opt_reg_string(odb, "-panalyzer:dl1",
	         "dl1 panalyzer conf",
	          &panalyzer_dl1_opt, "dl1:a:233:1:1:0.1",
	          /* print */TRUE, NULL);
	
   opt_reg_string(odb, "-panalyzer:il2",
           "il2 panalyzer conf",
            &panalyzer_il2_opt, "il2:a:233:1:1:0.1",
	         /* print */TRUE, NULL);
	  
   opt_reg_string(odb, "-panalyzer:dl2",
         "dl2 panalyzer conf",
          &panalyzer_dl2_opt, "dl2:a:233:1:1:0.1",
           /* print */TRUE, NULL);
		
    opt_reg_string(odb, "-panalyzer:itlb",
          "itlb  panalyzer conf",
           &panalyzer_itlb_opt, "itlb:a:233:1:1:0.1",
           /* print */TRUE, NULL);
		  
    opt_reg_string(odb, "-panalyzer:dtlb",
         "dtlb  panalyzer conf",
          &panalyzer_dtlb_opt, "dtlb:a:233:1:1:0.1",
           /* print */TRUE, NULL);
		
     opt_reg_string(odb, "-panalyzer:btb",
           "btb  panalyzer conf",
            &panalyzer_btb_opt, "btb:a:233:1:1:0.1",
	         /* print */TRUE, NULL);
#endif /* CACHE_PANALYZER_H */

#ifdef MEMORY_PANALYZER_H
  opt_reg_note(odb,
  "\n"
  "  small memory structure panalyzer\n"
  "  The small memory panalyzer config parameter <config> has the following format:\n"
  "\n"
  "Analytical power model\n"
  "\n"
  "    <name>:a:<opfreq>:<sCeff>\n"
  "    <name>    - name of the small memory structure  being defined\n"
  "                bimod - bimodal branch predictor\n"
  "                lev1 - lev 1 array of 2-lev branch predictor\n"
  "                lev2 - lev 2 array of 2-lev branch predictor\n"
  "                ras - return address stack\n"
  "    <pmodel>  - power model, 'a'- Analytical, 'e'-Emperical\n"
  "    <opfreq>  - operating frequency (MHz)\n"
  "    <sCeff>   - switching capacitance (pF) of memory structure / bus signal\n"
  "\n"
  "or\n"
  "Empirical power model\n"
  "\n"
  "    <name>:e:<opfreq>:<opvolt>:<sCeff>:<iCeff>:<lCeff>\n"
  "\n"
  "    <name>    - name of the small memory structure  being defined\n"
  "                bimod - bimodal branch predictor\n"
  "                lev1 - lev 1 array of 2-lev branch predictor\n"
  "                lev2 - lev 2 array of 2-lev branch predictor\n"
  "                ras - return address stack\n"
  "    <pmodel>  - power model, 'a'- Analytical, 'e'-Emperical\n"
  "    <opfreq>  - operating frequency in (MHz)\n"
  "    <sCeff>   - switching capacitance (pF) of register file / bus signal\n"
  "    <iCeff>   - internal switching capacitance of register file /bit\n"
  "    <lCeff>   - leakage capacitance (pF) of register file / bit\n"
  "\n"
  "    Examples:  -panalyzer:ras ras:a:233:sCeff\n"
  "               -panalyzer:bimod bimod:a:233:sCeff\n");

  opt_reg_string(odb, "-panalyzer:bimod",
           "bimod panalyzer conf",
            &panalyzer_bimod_opt, "bimod:a:233:0.1",
            /* print */TRUE, NULL);

  opt_reg_string(odb, "-panalyzer:lev1",
           "bimod panalyzer conf",
            &panalyzer_lev1_opt, "lev1:a:233:0.05",
            /* print */TRUE, NULL);

  opt_reg_string(odb, "-panalyzer:lev2",
           "bimod panalyzer conf",
            &panalyzer_lev2_opt, "lev2:a:233:0.1",
           /* print */TRUE, NULL);

  opt_reg_string(odb, "-panalyzer:ras",
            "ras panalyzer conf",
             &panalyzer_ras_opt, "ras:a:233:0.1",
            /* print */TRUE, NULL);
#endif /* MEMORY_PANALYZER_H */
#endif

#ifdef LOGIC_PANALYZER_H
	  opt_reg_note(odb,
	  "  The system logic panalyzer config parameter <config> has the following format:\n"
	  "\n"
	  "Analytical power model\n"
	  "    <name>:<pmodel>:<opfreq>:<svolt>:<lsytle>:<Ngates>:<Nfunc>:<fi>:<fld>\n"
	  "\n"
	  "    <name>   - name of the clock being defined\n"
	  "    <pmodel> - power model, 'a'- Analytical, 'e'-Emperical\n"
	  "    <opfreq> - operating frequency (MHz)\n"
	  "    <svolt>  - supply voltage\n"
	  "    <lstyle>  - logic style Static, Dynamic\n"
	  "    <Ngates>  - Number of Gates\n"
	  "    <Nfunc>  - Number of functions\n"
	  "    <fi>  - number of Fan-in\n"
	  "    <fld>  - number of Fan-out\n"
	  "\n"
	  "or\n"
	  "Empirical power model\n"
	  "    <name>:<pmodel>:<opfreq>:<xCeff>:<yCeff>:<iCeff>:<lCeff>\n"
	  "\n"
	  "    <name>   - name of the clock being defined\n"
	  "    <pmodel> - power model, 'a'- Analytical, 'e'-Emperical\n"
	  "    <opfreq> - operating frequency (MHz)\n"
	  "    <xCeff>  - capacitance (pF) \n"
	  "    <yCeff>  - capacitance (pF) \n"
	  "    <iCeff>  - internal switching capacitance (pF) \n"
	  "    <lCeff>  - leakage capacitance (pF)\n"
	  "\n"
	  "    Examples:   -panalyzer:logic logic:a:233:2.5:Static:30000:4:10:10\n"
	  "                -panalyzer:logic logic:e:233:xCeff:yCeff:iCeff:lCeff\n");
	  
	    opt_reg_string(odb, "-panalyzer:logic",
		         "logic panalyzer conf",
				          &panalyzer_logic_opt, "logic:a:233:2.5:Static:30000:4:10:10",
						           /* print */TRUE, NULL);
#endif /* LOGIC_PANALYZER_H */
		
#ifdef FP_PANALYZER_H
	opt_reg_double(odb, 
"-panalyzer:alu", "ALU effective Capacitance pF for power model", &alu_Ceff, /* default */1.0, /* print */TRUE, /* format */NULL);
	opt_reg_double(odb, 
"-panalyzer:mult", "multiplier effective Capacitance pF for power model", &mult_Ceff, /* default */1.0, /* print */TRUE, /* format */NULL);
	opt_reg_double(odb, 
"-panalyzer:fpu", "floating point unit effective Capacitance pF for power model", &fpu_Ceff, /* default */1.0, /* print */TRUE, /* format */NULL);

#endif
	
#endif	

}

/* check simulator-specific option values */
void
sim_check_options(struct opt_odb_t *odb,        /* options database */
		  int argc, char **argv)        /* command line arguments */
{
	char name[128], c;
	unsigned nsets, bsize, assoc;

#ifdef PANALYZER_H
#ifdef LV2_PANALYZER_H
  char pmodel, style, ctstyle;
  fu_pmodel_mode_t pmode;
//  unsigned nsets, bsize, assoc;
//  unsigned nbstages_opt, ncbstages_opt, nbls, nwls, nrwports, nrports, nwports;
  unsigned nbstages_opt, ncbstages_opt, nbls, nwls; 
  /*, nrwports, nrports, nwports; Ludo : unused, avoiding compilerwarnings*/
  double opfreq, cskew, tdarea, tcnodeCeff;
  // double sCeff, iCeff, lCeff, cnodeCeff;
  double sCeff = 0.0, iCeff = 0.0, lCeff = 0.0, cnodeCeff;
  double iovolt;
  double length; /* microstrip length */
  
  // logic related variables
  	double lvolt;
	unsigned Ngates;
	unsigned Nfunc;
	unsigned fi,fld;  
	double xCeff,yCeff;
	fu_logic_style_t logic_style;
	char lstyle[100];


#endif
#endif

	if(fastfwd_count < 0 || fastfwd_count >= 2147483647)
		fatal("bad fast forward count: %d", fastfwd_count);

	if(ruu_ifq_size < 1 || (ruu_ifq_size & (ruu_ifq_size - 1)) != 0)
		fatal("inst fetch queue size must be positive > 0 and a power of two");

#if 0 /* zero misprediction latency is not a problem... */
	if(ruu_branch_penalty < 1)
		fatal("mis-prediction penalty must be at least 1 cycle");
#endif

	if(fetch_speed < 1)
		fatal("front-end speed must be positive and non-zero");

	if(!mystricmp(pred_type, "perfect"))
	{
		/* perfect predictor */
		pred = NULL;
		pred_perfect = TRUE;
	}
	else 
	if(!mystricmp(pred_type, "taken"))
	{
		/* static predictor, not taken */
		pred = bpred_create(BPredTaken, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	}
	else 
	if(!mystricmp(pred_type, "nottaken"))
	{
		/* static predictor, taken */
		pred = bpred_create(BPredNotTaken, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	}
	else 
	if(!mystricmp(pred_type, "bimod"))
	{
		/* bimodal predictor, bpred_create() checks BTB_SIZE */
		if(bimod_nelt != 1)
			fatal("bad bimod predictor config (<table_size>)");
		if(btb_nelt != 2)
			fatal("bad btb config (<num_sets> <associativity>)");

		/* bimodal predictor, bpred_create() checks BTB_SIZE */
		/* bimod table size, 2lev l1 size, 2lev l2 size, meta table size, history reg size, history xor address, btb sets, btb assoc, ret-addr stack size */
		pred = bpred_create(BPred2bit, bimod_config[0], 0, 0, 0, 0, 0, btb_config[0], btb_config[1], ras_size);

#ifdef PANALYZER_H
#ifdef LV2_PANALYZER_H
#ifdef MEMORY_PANALYZER_H
  /* bimod configurator */
  sscanf(panalyzer_bimod_opt, "%[^:]:%c", name, &pmodel);
  if(pmodel == 'a')
  {
	 if(sscanf(panalyzer_bimod_opt, "%[^:]:%c:%lf:%lf",
	    name, &pmodel, &opfreq, &sCeff) != 4)
	   fatal("bad bimod panalyzer analytical power model parms: <name>:<pmodel>:<opfreq>:<sCeff>\n");
	   pmode = Analytical;
	  }
	  else
	  if(pmodel == 'e')
	  {
	     if(sscanf(panalyzer_bimod_opt, "%[^:]:%c:%lf:%lf:%lf:%lf", name, &pmodel, &opfreq, &sCeff, &iCeff, &lCeff) != 6)
	        fatal("bad bimod panalyzer empirical power model parms: <name>:<pmodel>:<opfreq>:<sCeff>:<iCeff>:<lCeff>");
	    pmode = Empirical;
	  }
      else
	    fatal("invalid bimod power model mode!");
	
	 bimod_pspec = create_sbank_panalyzer(
			      name, /* memory name */
			      pmode, /* bpred power model mode */
			      opfreq * 1E6, Vdd, /* operating frequency/supply voltage */
					  
			      bimod_config[0] /* nrows */, 2 /* ncols */,/* number of rows / cols (2^n) */
	  
			      0, fetch_speed, ruu_commit_width,
																										  
			      /* switching/internal/lekage  effective capacitances */
			      sCeff * 1E-12, iCeff * 1E-12, lCeff * 1E-12);
#endif /* MEMORY_PANALYZER_H */

#ifdef CACHE_PANALYZER_H
  /* btb configurator */
   sscanf(panalyzer_btb_opt, "%[^:]:%c", name, &pmodel);
   if(pmodel == 'a')
   {
      if(sscanf(panalyzer_btb_opt, "%[^:]:%c:%lf:%u:%u:%lf",
			    name, &pmodel, &opfreq, &nbls, &nwls, &sCeff) != 6)
	      fatal("bad btb panalyzer analytical power model parms: <name>:<pmodel>:<opfreq>:<nbls>:<nwls>:<sCeff>\n");
	      pmode = Analytical;
       }
	  else
      if(pmodel == 'e')
	  {
	      if(sscanf(panalyzer_btb_opt, "%[^:]:%c:%lf:%lf:%lf:%lf", name, &pmodel, &opfreq, &sCeff, &iCeff, &lCeff) != 6)
		       fatal("bad btb panalyzer empirical power model parms: <name>:<pmodel>:<opfreq>:<sCeff>:<iCeff>:<lCeff>");
		   pmode = Empirical;
	  }
	  else
		    fatal("invalid btb power model mode!");
																					  btb_pspec = create_cache_panalyzer(
		      name, /* cache name */
		      pmode, /* cache power model mode */
		      opfreq * 1E6, Vdd, /* operating frequency/supply voltage */
		      btb_config[0] , 1 /* bsize */, btb_config[1] /* assoc */,
		      nbls, nwls, 0, fetch_speed, ruu_commit_width,
		      /* switching/internal/lekage  effective capacitances of the array */
		      sCeff * 1E-12, iCeff * 1E-12, lCeff * 1E-12);
#endif /* CACHE_PANALYZER_H */

#ifdef MEMORY_PANALYZER_H
  /* ras configurator */
    sscanf(panalyzer_ras_opt, "%[^:]:%c", name, &pmodel);
	if(pmodel == 'a')
	{
	    if(sscanf(panalyzer_ras_opt, "%[^:]:%c:%lf:%lf",
	  	  	  name, &pmodel, &opfreq, &sCeff) != 4)
		      fatal("bad ras panalyzer analytical power model parms: <name>:<pmodel>:<opfreq>:<sCeff>\n");
			   pmode = Analytical;
	}
	else
	if(pmodel == 'e')
	{
	     if(sscanf(panalyzer_ras_opt, "%[^:]:%c:%lf:%lf:%lf:%lf", name, &pmodel, &opfreq, &sCeff, &iCeff, &lCeff) != 6)
	        fatal("bad ras panalyzer empirical power model parms: <name>:<pmodel>:<opfreq>:<sCeff>:<iCeff>:<lCeff>");
	      pmode = Empirical;
	}
	else
	    fatal("invalid ras power model mode!");
																				
	ras_pspec = create_sbank_panalyzer(
		      name, /* memory name */
		      pmode, /* bpred power model mode */
		      opfreq * 1E6, Vdd, /* operating frequency/supply voltage */
								  
		      ras_size /* nrows */, sizeof(fu_address_t) * 8 /* ncols */,
														
		      0, fetch_speed, ruu_commit_width,
		      /* switching/internal/lekage  effective capacitances */
			  sCeff * 1E-12, iCeff * 1E-12, lCeff * 1E-12);
#endif /* MEMORY_PANALYZER_H */
#endif /* LV2_PANALYZER_H*/				
#endif /* PANALYZER_H*/	
	
	}
	else 
	if(!mystricmp(pred_type, "2lev"))
	{
		/* 2-level adaptive predictor, bpred_create() checks args */
		if(twolev_nelt != 4)
			fatal("bad 2-level pred config (<l1size> <l2size> <hist_size> <xor>)");
		if(btb_nelt != 2)
			fatal("bad btb config (<num_sets> <associativity>)");

		/* bimod table size, 2lev l1 size, 2lev l2 size, meta table size, history reg size, history xor address, btb sets, btb assoc, ret-addr stack size */
		pred = bpred_create(BPred2Level, 0, twolev_config[0], twolev_config[1], 0, twolev_config[2], twolev_config[3], btb_config[0], btb_config[1], ras_size);
	
#ifdef PANALYZER_H
#ifdef LV2_PANALYZER_H
#ifdef MEMORY_PANALYZER_H
  /* lev1 configurator */
    sscanf(panalyzer_lev1_opt, "%[^:]:%c", name, &pmodel);
    if(pmodel == 'a')
    {
	    if(sscanf(panalyzer_lev1_opt, "%[^:]:%c:%lf:%lf",
		    name, &pmodel, &opfreq, &sCeff) != 4)
  	 	     fatal("bad lev1 panalyzer analytical power model parms: <name>:<pmodel>:<opfreq>:<sCeff>\n");
		     pmode = Analytical;
	 }
	 else
     if(pmodel == 'e')
     {
	      if(sscanf(panalyzer_lev1_opt, "%[^:]:%c:%lf:%lf:%lf:%lf", name, &pmodel, &opfreq, &sCeff, &iCeff, &lCeff) != 6)
		        fatal("bad lev1 panalyzer empirical power model parms: <name>:<pmodel>:<opfreq>:<sCeff>:<iCeff>:<lCeff>");
		    pmode = Empirical;
	  }
	  else
		    fatal("invalid lev1 power model mode!");

	  lev1_pspec = create_sbank_panalyzer(
		      name, /* memory name */
		      pmode, /* bpred power model mode */
		      opfreq * 1E6, Vdd, /* operating frequency/supply voltage */
		      twolev_config[0] /* nrows */, twolev_config[2]/* ncols */,
		      0, fetch_speed, ruu_commit_width,
		      /* switching/internal/lekage  effective capacitances */
		      sCeff * 1E-12, iCeff * 1E-12, lCeff * 1E-12);
	  	      /* lev2 configurator */
			  sscanf(panalyzer_lev2_opt, "%[^:]:%c", name, &pmodel);
	
	 if(pmodel == 'a')
	 {
	     if(sscanf(panalyzer_lev2_opt, "%[^:]:%c:%lf:%lf",
			      name, &pmodel, &opfreq, &sCeff) != 4)
		        fatal("bad lev2 panalyzer analytical power model parms: <name>:<pmodel>:<opfreq>:<sCeff>\n");
	     pmode = Analytical;
	 }

  else
  if(pmodel == 'e')
  {
      if(sscanf(panalyzer_lev2_opt, "%[^:]:%c:%lf:%lf:%lf:%lf", name, &pmodel, &opfreq, &sCeff, &iCeff, &lCeff) != 6)
	        fatal("bad lev2 panalyzer empirical power model parms: <name>:<pmodel>:<opfreq>:<sCeff>:<iCeff>:<lCeff>");
      pmode = Empirical;
   }
   else
	    fatal("invalid lev2 power model mode!");
					
    lev2_pspec = create_sbank_panalyzer(
			      name, /* memory name */
			      pmode, /* bpred power model mode */
			      opfreq * 1E6, Vdd, /* operating frequency/supply voltage */
							  
			      twolev_config[1] /* nrows */, 2/* ncols */,
			      0, fetch_speed, ruu_commit_width,
																  
			      /* switching/internal/lekage  effective capacitances */
			      sCeff * 1E-12, iCeff * 1E-12, lCeff * 1E-12);
#endif /* MEMORY_PANALYZER_H */

#ifdef CACHE_PANALYZER_H
  /* btb configurator */
    sscanf(panalyzer_btb_opt, "%[^:]:%c", name, &pmodel);
	if(pmodel == 'a')
	{
	    if(sscanf(panalyzer_btb_opt, "%[^:]:%c:%lf:%u:%u:%lf",
		    name, &pmodel, &opfreq, &nbls, &nwls, &sCeff) != 6)
		      fatal("bad btb panalyzer analytical power model parms: <name>:<pmodel>:<opfreq>:<nbls>:<nwls>:<sCeff>\n");
		pmode = Analytical;
	}
	else
	if(pmodel == 'e')
	{
	     if(sscanf(panalyzer_btb_opt, "%[^:]:%c:%lf:%lf:%lf:%lf", name, &pmodel, &opfreq, &sCeff, &iCeff, &lCeff) != 6)
		        fatal("bad btb panalyzer empirical power model parms: <name>:<pmodel>:<opfreq>:<sCeff>:<iCeff>:<lCeff>");
		  pmode = Empirical;
	}
	else
	    fatal("invalid btb power model mode!");

	btb_pspec = create_cache_panalyzer(
			      name, /* cache name */
			      pmode, /* cache power model mode */
			      opfreq * 1E6, Vdd, /* operating frequency/supply voltage */
			      btb_config[0] , 1 /* bsize */, btb_config[1] /* assoc */,
			      nbls, nwls, 0, fetch_speed, ruu_commit_width,
			      /* switching/internal/lekage  effective capacitances of the array */
				  sCeff * 1E-12, iCeff * 1E-12, lCeff * 1E-12);
#endif /* CACHE_PANALYZER_H */

#ifdef MEMORY_PANALYZER_H
  /* ras configurator */
    sscanf(panalyzer_ras_opt, "%[^:]:%c", name, &pmodel);
	if(pmodel == 'a')
    {
		 if(sscanf(panalyzer_ras_opt, "%[^:]:%c:%lf:%lf",
		    name, &pmodel, &opfreq, &sCeff) != 4)
		      fatal("bad ras panalyzer analytical power model parms: <name>:<pmodel>:<opfreq>:<sCeff>\n");
		  pmode = Analytical;
	}
	else
	if(pmodel == 'e')
	{
	      if(sscanf(panalyzer_ras_opt, "%[^:]:%c:%lf:%lf:%lf:%lf", name, &pmodel, &opfreq, &sCeff, &iCeff, &lCeff) != 6)
		      fatal("bad ras panalyzer empirical power model parms: <name>:<pmodel>:<opfreq>:<sCeff>:<iCeff>:<lCeff>");
		   pmode = Empirical;
	}
	else
		fatal("invalid ras power model mode!");
	
	ras_pspec = create_sbank_panalyzer(
			      name, /* memory name */
			      pmode, /* bpred power model mode */
			      opfreq * 1E6, Vdd, /* operating frequency/supply voltage */
			      ras_size /* nrows */, sizeof(fu_address_t) * 8 /* ncols */,
			      0, fetch_speed, ruu_commit_width,
			      /* switching/internal/lekage  effective capacitances */
			      sCeff * 1E-12, iCeff * 1E-12, lCeff * 1E-12);
#endif /* MEMORY_PANALYZER_H */

#endif /* LV2_PANALYZER_H*/				
#endif /* PANALYZER_H*/	
	
	}
	else 
	if(!mystricmp(pred_type, "comb"))
	{
		/* combining predictor, bpred_create() checks args */
		if(twolev_nelt != 4)
			fatal("bad 2-level pred config (<l1size> <l2size> <hist_size> <xor>)");
		if(bimod_nelt != 1)
			fatal("bad bimod predictor config (<table_size>)");
		if(comb_nelt != 1)
			fatal("bad combining predictor config (<meta_table_size>)");
		if(btb_nelt != 2)
			fatal("bad btb config (<num_sets> <associativity>)");

		pred = bpred_create(BPredComb, bimod_config[0], twolev_config[0], twolev_config[1], comb_config[0], twolev_config[2], twolev_config[3], btb_config[0], btb_config[1], ras_size);

#ifdef PANALYZER_H
#ifdef LV2_PANALYZER_H
#ifdef CACHE_PANALYZER_H
  /* btb configurator */
  sscanf(panalyzer_btb_opt, "%[^:]:%c", name, &pmodel);
  if(pmodel == 'a')
  {
    if(sscanf(panalyzer_btb_opt, "%[^:]:%c:%lf:%u:%u:%lf",
		    name, &pmodel, &opfreq, &nbls, &nwls, &sCeff) != 6)
	      fatal("bad btb panalyzer analytical power model parms: <name>:<pmodel>:<opfreq>:<nbls>:<nwls>:<sCeff>\n");
     pmode = Analytical;
  }
  else
  if(pmodel == 'e')
  {
      if(sscanf(panalyzer_btb_opt, "%[^:]:%c:%lf:%lf:%lf:%lf", name, &pmodel, &opfreq, &sCeff, &iCeff, &lCeff) != 6)
	        fatal("bad btb panalyzer empirical power model parms: <name>:<pmodel>:<opfreq>:<sCeff>:<iCeff>:<lCeff>");
	  pmode = Empirical;
   }
   else
	   fatal("invalid btb power model mode!");
   btb_pspec = create_cache_panalyzer(
			      name, /* cache name */
			      pmode, /* cache power model mode */
			      opfreq * 1E6, Vdd, /* operating frequency/supply voltage */
			      btb_config[0] , 1 /* bsize */, btb_config[1] /* assoc */,
			      nbls, nwls, 0, fetch_speed, ruu_commit_width,
			      /* switching/internal/lekage  effective capacitances of the array */
			      sCeff * 1E-12, iCeff * 1E-12, lCeff * 1E-12);
#endif /* CACHE_PANALYZER_H */

#ifdef MEMORY_PANALYZER_H
  /* ras configurator */
    sscanf(panalyzer_ras_opt, "%[^:]:%c", name, &pmodel);
	if(pmodel == 'a')
	{
	   if(sscanf(panalyzer_ras_opt, "%[^:]:%c:%lf:%lf",
		    name, &pmodel, &opfreq, &sCeff) != 4)
		      fatal("bad ras panalyzer analytical power model parms: <name>:<pmodel>:<opfreq>:<sCeff>\n");
	    pmode = Analytical;
	}
	else
	if(pmodel == 'e')
	{
	     if(sscanf(panalyzer_ras_opt, "%[^:]:%c:%lf:%lf:%lf:%lf", name, &pmodel, &opfreq, &sCeff, &iCeff, &lCeff) != 6)
		        fatal("bad ras panalyzer empirical power model parms: <name>:<pmodel>:<opfreq>:<sCeff>:<iCeff>:<lCeff>");
	     pmode = Empirical;
	}
	else
		fatal("invalid ras power model mode!");
	
	ras_pspec = create_sbank_panalyzer(
				      name, /* memory name */
				      pmode, /* bpred power model mode */
				      opfreq * 1E6, Vdd, /* operating frequency/supply voltage */
				      ras_size /* nrows */, sizeof(fu_address_t) * 8 /* ncols */,
				      0, fetch_speed, ruu_commit_width,
				      /* switching/internal/lekage  effective capacitances */
				      sCeff * 1E-12, iCeff * 1E-12, lCeff * 1E-12);
#endif /* MEMORY_PANALYZER_H */

#endif /* LV2_PANALYZER_H*/				
#endif /* PANALYZER_H*/	
	
	}
	else
		fatal("cannot parse predictor type `%s'", pred_type);

	if(!bpred_spec_opt)
		bpred_spec_update = spec_CT;
	else 
	if(!mystricmp(bpred_spec_opt, "ID"))
		bpred_spec_update = spec_ID;
	else
	if (!mystricmp(bpred_spec_opt, "WB"))
		bpred_spec_update = spec_WB;
	else
		fatal("bad speculative update stage specifier, use {ID|WB}");

	if(ruu_decode_width < 1 || (ruu_decode_width & (ruu_decode_width-1)) != 0)
		fatal("issue width must be positive non-zero and a power of two");

	if(ruu_issue_width < 1 || (ruu_issue_width & (ruu_issue_width-1)) != 0)
		fatal("issue width must be positive non-zero and a power of two");

	if(ruu_commit_width < 1)
		fatal("commit width must be positive non-zero");

	if(RUU_size < 2 || (RUU_size & (RUU_size-1)) != 0)
		fatal("RUU size must be a positive number > 1 and a power of two");

	if(LSQ_size < 2 || (LSQ_size & (LSQ_size-1)) != 0)
		fatal("LSQ size must be a positive number > 1 and a power of two");

	/* use a level 1 D-cache? */
	if(!mystricmp(cache_dl1_opt, "none"))
	{
		cache_dl1 = NULL;
		/* the level 2 D-cache cannot be defined */
		if(strcmp(cache_dl2_opt, "none"))
			fatal("the l1 data cache must defined if the l2 cache is defined");
		cache_dl2 = NULL;
	}
	else /* dl1 is defined */
	{
		if(sscanf(cache_dl1_opt, "%[^:]:%d:%d:%d:%c", name, &nsets, &bsize, &assoc, &c) != 5)
			fatal("bad l1 D-cache parms: <name>:<nsets>:<bsize>:<assoc>:<repl>");
#ifdef VANALYZER_H
		cache_dl1 = cache_create(name, nsets, bsize, /* balloc */TRUE, /* usize */0, assoc, cache_char2policy(c), dl1_access_fn, /* hit lat */cache_dl1_lat);
#else /* VANALYZER_H */
		cache_dl1 = cache_create(name, nsets, bsize, /* balloc */FALSE, /* usize */0, assoc, cache_char2policy(c), dl1_access_fn, /* hit lat */cache_dl1_lat);
#endif /* VANALYZER_H */
// added by Taeho Kgil 		
		cache_dl1_ID = cache_dl1; 
// ends here		

#ifdef PANALYZER_H
#ifdef LV2_PANALYZER_H
#ifdef CACHE_PANALYZER_H
 /* dl1 configurator */
	    sscanf(panalyzer_dl1_opt, "%[^:]:%c", name, &pmodel);
	    if(pmodel == 'a')
	    {
		    if(sscanf(panalyzer_dl1_opt, "%[^:]:%c:%lf:%u:%u:%lf",
				    name, &pmodel, &opfreq, &nbls, &nwls, &sCeff) != 6)
	 		      fatal("bad dl1 panalyzer analytical power model parms: <name>:<pmodel>:<opfreq>:<nbls>:<nwls>:<sCeff>\n");
		     pmode = Analytical;
		}
		else
		if(pmodel == 'e')
		{
		    if(sscanf(panalyzer_dl1_opt, "%[^:]:%c:%lf:%lf:%lf:%lf", name, &pmodel, &opfreq, &sCeff, &iCeff, &lCeff) != 6)
		        fatal("bad dl1 panalyzer empirical power model parms: <name>:<pmodel>:<opfreq>:<sCeff>:<iCeff>:<lCeff>");
		    pmode = Empirical;
		}
		else
			fatal("invalid dl1 power model mode!");
		dl1_pspec = create_cache_panalyzer(
					      name, /* cache name */
					      pmode, /* cache power model mode */
					      opfreq * 1E6, Vdd, /* operating frequency/supply voltage */
					      cache_dl1->nsets ,  cache_dl1->bsize, cache_dl1->assoc,
					      nbls, nwls, 1, 0, 0,
					      /* switching/internal/lekage  effective capacitances of the array */
					      sCeff * 1E-12, iCeff * 1E-12, lCeff * 1E-12);
#endif /* CACHE_PANALYZER_H */
#endif
		
#endif


		/* is the level 2 D-cache defined? */
		if(!mystricmp(cache_dl2_opt, "none"))
		{
			cache_dl2 = NULL;
#ifdef PANALYZER_H
#ifdef LV2_PANALYZER_H
#ifdef CACHE_PANALYZER_H
			dl2_pspec = NULL;
#endif /* CACHE_PANALYZER_H */
#endif						
#endif	
		}
		else
		{
			if(sscanf(cache_dl2_opt, "%[^:]:%d:%d:%d:%c", name, &nsets, &bsize, &assoc, &c) != 5)
				fatal("bad l2 D-cache parms: " "<name>:<nsets>:<bsize>:<assoc>:<repl>");
#ifdef VANALYZER_H
			cache_dl2 = cache_create(name, nsets, bsize, /* balloc */TRUE, /* usize */0, assoc, cache_char2policy(c), dl2_access_fn, /* hit lat */cache_dl2_lat);
#else /* VANALYZER_H */
			cache_dl2 = cache_create(name, nsets, bsize, /* balloc */FALSE, /* usize */0, assoc, cache_char2policy(c), dl2_access_fn, /* hit lat */cache_dl2_lat);
#endif /* VANALYZER_H */

#ifdef PANALYZER_H
#ifdef LV2_PANALYZER_H
#ifdef CACHE_PANALYZER_H
		  /* dl2 configurator */
		  sscanf(panalyzer_dl2_opt, "%[^:]:%c", name, &pmodel);
		  if(pmodel == 'a')
		  {
		     if(sscanf(panalyzer_dl2_opt, "%[^:]:%c:%lf:%u:%u:%lf",
			    name, &pmodel, &opfreq, &nbls, &nwls, &sCeff) != 6)
		      fatal("bad dl2 panalyzer analytical power model parms: <name>:<pmodel>:<opfreq>:<nbls>:<nwls>:<sCeff>\n");
		      pmode = Analytical;
		   }
		   else
		   if(pmodel == 'e')
		   {
		      if(sscanf(panalyzer_dl2_opt, "%[^:]:%c:%lf:%lf:%lf:%lf", name, &pmodel, &opfreq, &sCeff, &iCeff, &lCeff) != 6)
			       fatal("bad dl2 panalyzer empirical power model parms: <name>:<pmodel>:<opfreq>:<sCeff>:<iCeff>:<lCeff>");
			   pmode = Empirical;
		   }
		   else
			   fatal("invalid dl2 power model mode!");
 		   dl2_pspec = create_cache_panalyzer(
					      name, /* cache name */
					      pmode, /* cache power model mode */
					      opfreq * 1E6, Vdd, /* operating frequency/supply voltage */
					      cache_dl2->nsets ,  cache_dl2->bsize, cache_dl2->assoc,
					      nbls, nwls, 1, 0, 0,
					      /* switching/internal/lekage  effective capacitances of the array */
					      sCeff * 1E-12, iCeff * 1E-12, lCeff * 1E-12);
#endif /* CACHE_PANALYZER_H */
			
#endif
#endif

			
		}
	}

	/* use a level 1 I-cache? */
	if(!mystricmp(cache_il1_opt, "none"))
	{
		cache_il1 = NULL;
#ifdef PANALYZER_H
#ifdef LV2_PANALYZER_H
#ifdef CACHE_PANALYZER_H
		 il1_pspec = NULL;
#endif /* CACHE_PANALYZER_H */
#endif				
#endif				
		/* the level 2 I-cache cannot be defined */
		if(strcmp(cache_il2_opt, "none"))
			fatal("the l1 inst cache must defined if the l2 cache is defined");
		cache_il2 = NULL;

#ifdef PANALYZER_H
#ifdef LV2_PANALYZER_H
#ifdef CACHE_PANALYZER_H
		 il2_pspec = NULL;
#endif /* CACHE_PANALYZER_H */
#endif				
#endif	
	
	}
	else 
	if(!mystricmp(cache_il1_opt, "dl1"))
	{
		if(!cache_dl1)
			fatal("I-cache l1 cannot access D-cache l1 as it's undefined");
		cache_il1 = cache_dl1;

#ifdef PANALYZER_H
#ifdef LV2_PANALYZER_H
#ifdef CACHE_PANALYZER_H
		 il1_pspec = dl1_pspec;
#endif /* CACHE_PANALYZER_H */
#endif				
#endif	

		/* the level 2 I-cache cannot be defined */
		if(strcmp(cache_il2_opt, "none"))
			fatal("the l1 inst cache must defined if the l2 cache is defined");
		cache_il2 = NULL;

#ifdef PANALYZER_H
#ifdef LV2_PANALYZER_H
#ifdef CACHE_PANALYZER_H
		 il2_pspec = NULL;
#endif /* CACHE_PANALYZER_H */
#endif				
#endif	

	}
	else 
	if(!mystricmp(cache_il1_opt, "dl2"))
	{
		if(!cache_dl2)
			fatal("I-cache l1 cannot access D-cache l2 as it's undefined");
		cache_il1 = cache_dl2;

#ifdef PANALYZER_H
#ifdef LV2_PANALYZER_H
#ifdef CACHE_PANALYZER_H
		 il1_pspec = dl2_pspec;
#endif /* CACHE_PANALYZER_H */
#endif				
#endif	
		/* the level 2 I-cache cannot be defined */
		if(strcmp(cache_il2_opt, "none"))
			fatal("the l1 inst cache must defined if the l2 cache is defined");
		cache_il2 = NULL;
#ifdef PANALYZER_H
#ifdef LV2_PANALYZER_H
#ifdef CACHE_PANALYZER_H
		 il2_pspec = NULL;
#endif /* CACHE_PANALYZER_H */
#endif				
#endif

	}
	else /* il1 is defined */
	{
		if(sscanf(cache_il1_opt, "%[^:]:%d:%d:%d:%c", name, &nsets, &bsize, &assoc, &c) != 5) 
			fatal("bad l1 I-cache parms: <name>:<nsets>:<bsize>:<assoc>:<repl>");
#ifdef VANALYZER_H
		cache_il1 = cache_create(name, nsets, bsize, /* balloc */TRUE, /* usize */0, assoc, cache_char2policy(c), il1_access_fn, /* hit lat */cache_il1_lat);
#else /* VANALYZER_H */
		cache_il1 = cache_create(name, nsets, bsize, /* balloc */FALSE, /* usize */0, assoc, cache_char2policy(c), il1_access_fn, /* hit lat */cache_il1_lat);
#endif /* VANALYZER_H */

// added by Taeho Kgil 		
		cache_il1_ID = cache_il1; 
// ends here		


#ifdef PANALYZER_H
#ifdef LV2_PANALYZER_H
#ifdef CACHE_PANALYZER_H
/* il1 configurator */
  sscanf(panalyzer_il1_opt, "%[^:]:%c", name, &pmodel);
  if(pmodel == 'a')
  {
	  if(sscanf(panalyzer_il1_opt, "%[^:]:%c:%lf:%u:%u:%lf",
		    name, &pmodel, &opfreq, &nbls, &nwls, &sCeff) != 6)
	        fatal("bad il1 panalyzer analytical power model parms: <name>:<pmodel>:<opfreq>:<nbls>:<nwls>:<sCeff>\n");
	   pmode = Analytical;
   }
   else
   if(pmodel == 'e')
   {
	   if(sscanf(panalyzer_il1_opt, "%[^:]:%c:%lf:%lf:%lf:%lf", name, &pmodel, &opfreq, &sCeff, &iCeff, &lCeff) != 6)
		   fatal("bad il1 panalyzer empirical power model parms: <name>:<pmodel>:<opfreq>:<sCeff>:<iCeff>:<lCeff>");
	    pmode = Empirical;
    }
    else
	    fatal("invalid il1 power model mode!");
    il1_pspec = create_cache_panalyzer(
			      name, /* cache name */
			      pmode, /* cache power model mode */
			      opfreq * 1E6, Vdd, /* operating frequency/supply voltage */
			      cache_il1->nsets ,  cache_il1->bsize, cache_il1->assoc,
			      nbls, nwls, 1, 0, 0,
			      /* switching/internal/lekage  effective capacitances of the array */
			      sCeff * 1E-12, iCeff * 1E-12, lCeff * 1E-12);
#endif /* CACHE_PANALYZER_H */
#endif
#endif

		/* is the level 2 D-cache defined? */
		if(!mystricmp(cache_il2_opt, "none")) 
		{
			cache_il2 = NULL;
#ifdef PANALYZER_H
#ifdef LV2_PANALYZER_H
#ifdef CACHE_PANALYZER_H
		 il2_pspec = NULL;
#endif /* CACHE_PANALYZER_H */
#endif				
#endif

		}
		else 
		if(!mystricmp(cache_il2_opt, "dl2"))
		{
			if(!cache_dl2)
				fatal("I-cache l2 cannot access D-cache l2 as it's undefined");
			cache_il2 = cache_dl2;

#ifdef PANALYZER_H
#ifdef LV2_PANALYZER_H
#ifdef CACHE_PANALYZER_H
			 il2_pspec = dl2_pspec;
#endif /* CACHE_PANALYZER_H */
#endif				
#endif

		}
		else
		{
			if(sscanf(cache_il2_opt, "%[^:]:%d:%d:%d:%c", name, &nsets, &bsize, &assoc, &c) != 5)
				fatal("bad l2 I-cache parms: " "<name>:<nsets>:<bsize>:<assoc>:<repl>");
#ifdef VANALYZER_H
			cache_il2 = cache_create(name, nsets, bsize, /* balloc */TRUE, /* usize */0, assoc, cache_char2policy(c), il2_access_fn, /* hit lat */cache_il2_lat);
#else /* VANALYZER_H */
			cache_il2 = cache_create(name, nsets, bsize, /* balloc */FALSE, /* usize */0, assoc, cache_char2policy(c), il2_access_fn, /* hit lat */cache_il2_lat);
#endif /* VANALYZER_H */

#ifdef PANALYZER_H
#ifdef LV2_PANALYZER_H
#ifdef CACHE_PANALYZER_H
/* il2 configurator */
   sscanf(panalyzer_il2_opt, "%[^:]:%c", name, &pmodel);
   if(pmodel == 'a')
   {
	   if(sscanf(panalyzer_il2_opt, "%[^:]:%c:%lf:%u:%u:%lf",
			    name, &pmodel, &opfreq, &nbls, &nwls, &sCeff) != 6)
			     fatal("bad il2 panalyzer analytical power model parms: <name>:<pmodel>:<opfreq>:<nbls>:<nwls>:<sCeff>\n");
	    pmode = Analytical;
    }
    else
    if(pmodel == 'e')
    {
	    if(sscanf(panalyzer_il2_opt, "%[^:]:%c:%lf:%lf:%lf:%lf", name, &pmodel, &opfreq, &sCeff, &iCeff, &lCeff) != 6)
		    fatal("bad il2 panalyzer empirical power model parms: <name>:<pmodel>:<opfreq>:<sCeff>:<iCeff>:<lCeff>");
		pmode = Empirical;
	}
	else
		fatal("invalid il2 power model mode!");
	il2_pspec = create_cache_panalyzer(
			      name, /* cache name */
			      pmode, /* cache power model mode */
			      opfreq * 1E6, Vdd, /* operating frequency/supply voltage */
			      cache_il2->nsets ,  cache_il2->bsize, cache_il2->assoc,
			      nbls, nwls, 1, 0, 0,
			      /* switching/internal/lekage  effective capacitances of the array */
			      sCeff * 1E-12, iCeff * 1E-12, lCeff * 1E-12);
#endif /* CACHE_PANALYZER_H */
#endif
#endif

		}
	}

	/* use an I-TLB? */
	if(!mystricmp(itlb_opt, "none"))
	{
		itlb = NULL;
#ifdef PANALYZER_H
#ifdef LV2_PANALYZER_H
#ifdef CACHE_PANALYZER_H
		itlb_pspec = NULL;
#endif /* CACHE_PANALYZER_H */
#endif
#endif	
	}
	else
	{
		if(sscanf(itlb_opt, "%[^:]:%d:%d:%d:%c", name, &nsets, &bsize, &assoc, &c) != 5) 
			fatal("bad TLB parms: <name>:<nsets>:<page_size>:<assoc>:<repl>");
		itlb = cache_create(name, nsets, bsize, /* balloc */FALSE, /* usize */sizeof(md_addr_t), assoc, cache_char2policy(c), itlb_access_fn, /* hit latency */1);

#ifdef PANALYZER_H
#ifdef LV2_PANALYZER_H
#ifdef CACHE_PANALYZER_H
	  /* itlb configurator */
	    sscanf(panalyzer_itlb_opt, "%[^:]:%c", name, &pmodel);
  	    if(pmodel == 'a')
		{
			if(sscanf(panalyzer_itlb_opt, "%[^:]:%c:%lf:%u:%u:%lf",
				name, &pmodel, &opfreq, &nbls, &nwls, &sCeff) != 6)
				fatal("bad itlb panalyzer analytical power model parms: <name>:<pmodel>:<opfreq>:<nbls>:<nwls>:<sCeff>\n");
			pmode = Analytical;
		}
		else
		if(pmodel == 'e')
		{
			if(sscanf(panalyzer_itlb_opt, "%[^:]:%c:%lf:%lf:%lf:%lf", name, &pmodel, &opfreq, &sCeff, &iCeff, &lCeff) != 6)
				fatal("bad itlb panalyzer empirical power model parms: <name>:<pmodel>:<opfreq>:<sCeff>:<iCeff>:<lCeff>");
		    pmode = Empirical;
		}
		else
			fatal("invalid itlb power model mode!");
		itlb_pspec = create_cache_panalyzer(
					      name, /* cache name */
					      pmode, /* cache power model mode */
					      opfreq * 1E6, Vdd, /* operating frequency/supply voltage */
					      itlb->nsets ,  itlb->bsize, itlb->assoc,
					      nbls, nwls, 1, 0, 0,
					      /* switching/internal/lekage  effective capacitances of the array */
					      sCeff * 1E-12, iCeff * 1E-12, lCeff * 1E-12);
#endif /* CACHE_PANALYZER_H */
#endif
#endif	
	
	}

	/* use a D-TLB? */
	if(!mystricmp(dtlb_opt, "none"))
	{
		dtlb = NULL;
#ifdef PANALYZER_H
#ifdef LV2_PANALYZER_H
#ifdef CACHE_PANALYZER_H
		dtlb_pspec = NULL;
#endif /* CACHE_PANALYZER_H */
#endif
#endif	
	}
	else
	{
		if(sscanf(dtlb_opt, "%[^:]:%d:%d:%d:%c", name, &nsets, &bsize, &assoc, &c) != 5) 
			fatal("bad TLB parms: <name>:<nsets>:<page_size>:<assoc>:<repl>");
		dtlb = cache_create(name, nsets, bsize, /* balloc */FALSE, /* usize */sizeof(md_addr_t), assoc, cache_char2policy(c), dtlb_access_fn, /* hit latency */1);
	
#ifdef PANALYZER_H
#ifdef LV2_PANALYZER_H
#ifdef CACHE_PANALYZER_H
  /* dtlb configurator */
    sscanf(panalyzer_dtlb_opt, "%[^:]:%c", name, &pmodel);
	if(pmodel == 'a')
	{
		if(sscanf(panalyzer_dtlb_opt, "%[^:]:%c:%lf:%u:%u:%lf",
			name, &pmodel, &opfreq, &nbls, &nwls, &sCeff) != 6)
			fatal("bad dtlb panalyzer analytical power model parms: <name>:<pmodel>:<opfreq>:<nbls>:<nwls>:<sCeff>\n");
		 pmode = Analytical;
	}
	else
	if(pmodel == 'e')
	{
		if(sscanf(panalyzer_dtlb_opt, "%[^:]:%c:%lf:%lf:%lf:%lf", name, &pmodel, &opfreq, &sCeff, &iCeff, &lCeff) != 6)
		    fatal("bad dtlb panalyzer empirical power model parms: <name>:<pmodel>:<opfreq>:<sCeff>:<iCeff>:<lCeff>");
		pmode = Empirical;
	}
	else
		fatal("invalid dtlb power model mode!");
    dtlb_pspec = create_cache_panalyzer(
				      name, /* cache name */
				      pmode, /* cache power model mode */
				      opfreq * 1E6, Vdd, /* operating frequency/supply voltage */
				      dtlb->nsets ,  dtlb->bsize, dtlb->assoc,
				      nbls, nwls, 1, 0, 0,
				      /* switching/internal/lekage  effective capacitances of the array */
				      sCeff * 1E-12, iCeff * 1E-12, lCeff * 1E-12);
#endif /* CACHE_PANALYZER_H */
#endif
#endif	
	}

	if(cache_dl1_lat < 1)
		fatal("l1 data cache latency must be greater than zero");

	if(cache_dl2_lat < 1)
		fatal("l2 data cache latency must be greater than zero");

	if(cache_il1_lat < 1)
		fatal("l1 instruction cache latency must be greater than zero");

	if(cache_il2_lat < 1)
		fatal("l2 instruction cache latency must be greater than zero");

	if(mem_nelt != 2)
		fatal("bad memory access latency (<first_chunk> <inter_chunk>)");

	if(mem_lat[0] < 1 || mem_lat[1] < 1)
		fatal("all memory access latencies must be greater than zero");

	if(mem_bus_width < 1 || (mem_bus_width & (mem_bus_width-1)) != 0)
		fatal("memory bus width must be positive non-zero and a power of two");

	if(tlb_miss_lat < 1)
		fatal("TLB miss latency must be greater than zero");

	if(res_ialu < 1)
		fatal("number of integer ALU's must be greater than zero");
	if(res_ialu > MAX_INSTS_PER_CLASS)
		fatal("number of integer ALU's must be <= MAX_INSTS_PER_CLASS");
	fu_config[FU_IALU_INDEX].quantity = res_ialu;

	if(res_imult < 1)
		fatal("number of integer multiplier/dividers must be greater than zero");
	if(res_imult > MAX_INSTS_PER_CLASS)
		fatal("number of integer mult/div's must be <= MAX_INSTS_PER_CLASS");
	fu_config[FU_IMULT_INDEX].quantity = res_imult;

	if(res_memport < 1)
		fatal("number of memory system ports must be greater than zero");
	if (res_memport > MAX_INSTS_PER_CLASS)
		fatal("number of memory system ports must be <= MAX_INSTS_PER_CLASS");
	fu_config[FU_MEMPORT_INDEX].quantity = res_memport;

	if(res_fpalu < 1)
		fatal("number of floating point ALU's must be greater than zero");
	if(res_fpalu > MAX_INSTS_PER_CLASS)
		fatal("number of floating point ALU's must be <= MAX_INSTS_PER_CLASS");
	fu_config[FU_FPALU_INDEX].quantity = res_fpalu;

	if(res_fpmult < 1)
		fatal("number of floating point multiplier/dividers must be > zero");
	if(res_fpmult > MAX_INSTS_PER_CLASS)
		fatal("number of FP mult/div's must be <= MAX_INSTS_PER_CLASS");
	fu_config[FU_FPMULT_INDEX].quantity = res_fpmult;

#ifdef PANALYZER_H
#ifdef LV1_PANALYZER_H
	sim_lv1_panalyzer_check_options(
		&lv1_volt,
		&lv1_freq,
		&lv1_alu_Ceff,
		&lv1_fpu_Ceff,
		&lv1_mult_Ceff,
		&lv1_rf_Ceff,
		&lv1_bpred_Ceff,
		&lv1_clock_Ceff,
		lv1_io_option,
		mem_lat[0],
		mem_lat[1],
		&lv1_il1_Ceff,
		&lv1_il2_Ceff,
		&lv1_dl1_Ceff,
		&lv1_dl2_Ceff,
		&lv1_itlb_Ceff,
		&lv1_dtlb_Ceff);
#endif	

#ifdef LV2_PANALYZER_H
#ifdef IO_PANALYZER_H
  /* aio configurator */
  sscanf(panalyzer_aio_opt, "%[^:]:%c", name, &pmodel);
  if(pmodel == 'a')
  {
	  if(sscanf(panalyzer_aio_opt, "%[^:]:%c:%lf:%c:%lf:%u:%lf:%lf",
		  name, &pmodel, &opfreq, &style, &iovolt, &nbstages_opt, &length, &sCeff) != 8)
	      fatal("bad io panalyzer analytical power model parms: <name>:<pmodel>:<opfreq>:<iovoltage>:<style>:<nbstages_opt>:<strip_length>:<sCeff>");
	   pmode = Analytical;
   }
   else
   if(pmodel == 'e')
   {
	   if(sscanf(panalyzer_aio_opt, "%[^:]:%c:%c:%lf:%lf:%lf:%lf",
		   name, &pmodel, &style, &opfreq, &sCeff, &iCeff, &lCeff) != 7)
	  	   fatal("bad io panalyzer empirical power model parms: <name>:<pmodel>:<style>:<opfreq>:<sCeff>:<iCeff>:<lCeff>");
	    pmode = Empirical;
	}
	else
		fatal("invalid aio power model mode!");
	 aio_pspec = create_io_panalyzer(
			      name, /* io name */
			      pmode, /* io power model mode */
			      opfreq * 1E6, /* operating frequency in MHz */
					iovolt,													  
			      /* io specific parameters */
			      ((style == 'o') ? odirBuffer: (style == 'i') ? idirBuffer : bidirBuffer),
			      /* io style */
				  nbstages_opt, /* optimial number of io buffer stages */
				  length, /* micro strip length */
				  mem_bus_width, /* io bus width */
				  mem_lat[0], mem_lat[1], /* io access/cycle time in cycles */
				  sizeof(fu_address_t), /* io size in bytes */
				  /* switching/internal/lekage  effective capacitances */
				  sCeff * 1E-12, iCeff * 1E-12, lCeff * 1E-12);
	 /* dio configurator */
	sscanf(panalyzer_dio_opt, "%[^:]:%c", name, &pmodel);
	if(pmodel == 'a') {
		if(sscanf(panalyzer_dio_opt, "%[^:]:%c:%lf:%c:%lf:%u:%lf:%lf",
			   name, &pmodel, &opfreq, &style, &iovolt, &nbstages_opt, &length, &sCeff) != 8)
			fatal("bad io panalyzer analytical power model parms: <name>:<pmodel>:<opfreq>:<style>:<iovoltage>:<nbstages_opt>:<strip_length>:<yCeff>");
		   pmode = Analytical;
	}
	else if(pmodel == 'e')
    {
	   if(sscanf(panalyzer_dio_opt, "%[^:]:%c:%c:%lf:%lf:%lf:%lf",
		   name, &pmodel, &style, &opfreq, &sCeff, &iCeff, &lCeff) != 7)
		   fatal("bad io panalyzer empirical power model parms: <name>:<pmodel>:<style>:<opfreq>:<sCeff>:<iCeff>:<lCeff>");
		pmode = Empirical;
	 }
 	else
		fatal("invalid dio power model mode!");
    dio_pspec = create_io_panalyzer(
			    name, /* io name */
			    pmode, /* io power model mode */
			    opfreq * 1E6, /* operating frequency in MHz */
				iovolt,
			    /* io specific parameters */
			    ((style == 'o') ? odirBuffer: (style == 'i') ? idirBuffer : bidirBuffer),
			    /* io style */
			    nbstages_opt, /* optimial number of io buffer stages */
				length, /* micro strip length */
			    mem_bus_width, /* io bus width */
			    mem_lat[0], mem_lat[1], /* io access/cycle time in cycles */
			    sizeof(word_t), /* io size in bytes */
			    /* switching/internal/lekage  effective capacitances */
			    sCeff * 1E-12, iCeff * 1E-12, lCeff * 1E-12);
#endif /* IO_PANALYZER_H */

#ifdef MEMORY_PANALYZER_H
  /* integer register file configurator */
    sscanf(panalyzer_irf_opt, "%[^:]:%c", name, &pmodel);
	if(pmodel == 'a')
	{
		if(sscanf(panalyzer_irf_opt, "%[^:]:%c:%lf:%lf", name, &pmodel, &opfreq, &sCeff) != 4)
			fatal("bad integer register file panalyzer analytical power model parms: <name>:<pmodel>:<opfreq>:<sCeff>\n");
		pmode = Analytical;
	}
	else
	if(pmodel == 'e')
	{
		if(sscanf(panalyzer_irf_opt, "%[^:]:%c:%lf:%lf:%lf:%lf", name, &pmodel, &opfreq, &sCeff, &iCeff, &lCeff) != 6)
			fatal("bad rf panalyzer empirical power model parms: <name>:<pmodel>:<opfreq>:<sCeff>:<iCeff>:<lCeff>");
		pmode = Empirical;
	}
	else
		 fatal("invalid integer register file power model mode!");

	irf_pspec = create_sbank_panalyzer(
				      name, /* memory name */
				      pmode, /* bpred power model mode */
				      opfreq * 1E6, Vdd, /* operating frequency/supply voltage */
				      /* number of rows / cols (2^n) */
				      MD_NUM_IREGS + MD_NUM_PIREGS /* nrows */,
				      sizeof(md_gpr_t) * 8 / MD_NUM_IREGS /* ncols */,
				      0, ruu_issue_width * 2, ruu_commit_width,
				      /* switching/internal/lekage  effective capacitances */
				      sCeff * 1E-12, iCeff * 1E-12, lCeff * 1E-12);
				     /* floating point register file configurator */
					  sscanf(panalyzer_fprf_opt, "%[^:]:%c", name, &pmodel);
    if(pmodel == 'a')
	{
		if(sscanf(panalyzer_fprf_opt, "%[^:]:%c:%lf:%lf", name, &pmodel, &opfreq, &sCeff) != 4)
			fatal("bad floating point register file panalyzer analytical power model parms: <name>:<pmodel>:<opfreq>:<sCeff>\n");
		 pmode = Analytical;
	}

  else
  if(pmodel == 'e')
  {
	  if(sscanf(panalyzer_fprf_opt, "%[^:]:%c:%lf:%lf:%lf:%lf", name, &pmodel, &opfreq, &sCeff, &iCeff, &lCeff) != 6)
		  fatal("bad floating point register file panalyzer empirical power model parms: <name>:<pmodel>:<opfreq>:<sCeff>:<iCeff>:<lCeff>");
	   pmode = Empirical;
   }
   else
	   fatal("invalid floating point register file power model mode!");
   fprf_pspec = create_sbank_panalyzer(
				      name, /* memory name */
				      pmode, /* bpred power model mode */
				      opfreq * 1E6, Vdd, /* operating frequency/supply voltage */
				      /* number of rows / cols (2^n) */
				      MD_NUM_FREGS + MD_NUM_PFREGS /* nrows */,
				      sizeof(md_fpr_t) * 8 / MD_NUM_FREGS /* ncols */,
				      0, ruu_issue_width * 2, ruu_commit_width,
				      /* switching/internal/lekage  effective capacitances */
				      sCeff * 1E-12, iCeff * 1E-12, lCeff * 1E-12);
#endif /* MEMORY_PANALYZER_H */

#ifdef CLOCK_PANALYZER_H
  /* clock panalyzer configurator */
    sscanf(panalyzer_clock_opt, "%[^:]:%c", name, &pmodel);
	if(pmodel == 'a')
	{
		if(sscanf(panalyzer_clock_opt, "%[^:]:%c:%lf:%c:%lf:%u",
			name, &pmodel, &opfreq, &ctstyle, &cskew, &ncbstages_opt) != 6)
			fatal("bad clock panalyzer analytical power model parms: <name>:<pmodel>:<opfreq>:<ctstyle>:<cskew>:<ncbstages_opt>");
		pmode = Analytical;
	}
	else
	if(pmodel == 'e')
	{
		if(sscanf(panalyzer_clock_opt, "%[^:]:%c:%lf:%lf:%lf:%lf",
			name, &pmodel, &opfreq, &sCeff, &iCeff, &lCeff) != 6)
			fatal("bad clock panalyzer empirical power model parms: <name>:<pmodel>:<opfreq>:<sCeff>:iCeff>:<lCeff>");
		pmode = Empirical;
	}
	else
		fatal("invalid clock power model mode!");
	/* include area and clocked node capacitance integration routines */
	tdarea =  irf_pspec->dimension->area;
	tdarea += fprf_pspec->dimension->area;
	if(bimod_pspec)
		tdarea
			+= bimod_pspec->dimension->area;
	if(lev1_pspec)
		tdarea
			+= lev1_pspec->dimension->area;
	if(lev2_pspec)
		tdarea
			+= lev2_pspec->dimension->area;
	if(ras_pspec)
		tdarea
			+= ras_pspec->dimension->area;
	if(il1_pspec && (il1_pspec != dl1_pspec && il1_pspec != dl2_pspec))
		tdarea
			+= il1_pspec->dimension->area;
	if(dl1_pspec)
		tdarea
			+= dl1_pspec->dimension->area;
	if(il2_pspec && (il2_pspec != dl1_pspec && il2_pspec != dl2_pspec))
		tdarea
		    += il1_pspec->dimension->area;
	if(dl2_pspec)
		tdarea
	       += dl1_pspec->dimension->area;
	if(itlb_pspec)
		tdarea
		   += itlb_pspec->dimension->area;
	if(dtlb_pspec)
	    tdarea
		    += dtlb_pspec->dimension->area;
	if(btb_pspec)
		tdarea
		    += btb_pspec->dimension->area;
    tcnodeCeff
		     =  irf_pspec->Ceffs->cnodeCeff;
	fprintf(stderr,"irf node = %lf\n",1E12*irf_pspec->Ceffs->cnodeCeff);
	tcnodeCeff
	    += fprf_pspec->Ceffs->cnodeCeff;
	fprintf(stderr,"frf node = %lf\n",1E12*fprf_pspec->Ceffs->cnodeCeff);
    if(bimod_pspec) {
	   tcnodeCeff
		       += bimod_pspec->Ceffs->cnodeCeff;
	   fprintf(stderr,"bimod node = %lf\n",1E12*bimod_pspec->Ceffs->cnodeCeff);
	}
	if(lev1_pspec) {
	   tcnodeCeff
		   += lev1_pspec->Ceffs->cnodeCeff;
	   fprintf(stderr,"lev1 node = %lf\n",1E12*lev1_pspec->Ceffs->cnodeCeff);
	}
    if(lev2_pspec) {
	   tcnodeCeff
	       += lev2_pspec->Ceffs->cnodeCeff;
	   fprintf(stderr,"lev2 node = %lf\n",1E12*lev2_pspec->Ceffs->cnodeCeff);
	}
    if(ras_pspec) {
		tcnodeCeff
		   += ras_pspec->Ceffs->cnodeCeff;
	    fprintf(stderr,"ras node = %lf\n",1E12*ras_pspec->Ceffs->cnodeCeff);
	}
    if(il1_pspec && (il1_pspec != dl1_pspec && il1_pspec != dl2_pspec)) {
	    tcnodeCeff
		    += (il1_pspec->t_Ceffs->cnodeCeff + il1_pspec->d_Ceffs->cnodeCeff);
	    fprintf(stderr,"il1 node = %lf\n",1E12*(il1_pspec->t_Ceffs->cnodeCeff + il1_pspec->d_Ceffs->cnodeCeff));
	}
  
   if(dl1_pspec) {
	    tcnodeCeff
		    += (dl1_pspec->t_Ceffs->cnodeCeff + dl1_pspec->d_Ceffs->cnodeCeff);
	    fprintf(stderr,"dl1 node = %lf\n",1E12*(dl1_pspec->t_Ceffs->cnodeCeff + dl1_pspec->d_Ceffs->cnodeCeff));
	}
				  
	if(il2_pspec && (il2_pspec != dl1_pspec && il2_pspec != dl2_pspec)) {
		tcnodeCeff
			+= (il2_pspec->t_Ceffs->cnodeCeff + il2_pspec->d_Ceffs->cnodeCeff);
		fprintf(stderr,"il2 node = %lf\n",1E12*(il2_pspec->t_Ceffs->cnodeCeff + il2_pspec->d_Ceffs->cnodeCeff));
	}
	if(dl2_pspec) {
		tcnodeCeff
			+= (dl2_pspec->t_Ceffs->cnodeCeff + dl2_pspec->d_Ceffs->cnodeCeff);
		fprintf(stderr,"dl2 node = %lf\n",1E12*(dl2_pspec->t_Ceffs->cnodeCeff + dl2_pspec->d_Ceffs->cnodeCeff));
	}
	if(itlb_pspec) {
		tcnodeCeff
		    += (itlb_pspec->t_Ceffs->cnodeCeff + itlb_pspec->d_Ceffs->cnodeCeff);
	    fprintf(stderr,"itlb node = %lf\n",1E12*(itlb_pspec->t_Ceffs->cnodeCeff + itlb_pspec->d_Ceffs->cnodeCeff));
	}
	if(dtlb_pspec) {
	    tcnodeCeff
		    += (dtlb_pspec->t_Ceffs->cnodeCeff + dtlb_pspec->d_Ceffs->cnodeCeff);
		fprintf(stderr,"dtlb node = %lf\n",1E12*(dtlb_pspec->t_Ceffs->cnodeCeff + dtlb_pspec->d_Ceffs->cnodeCeff));
	}
	if(btb_pspec) {
		tcnodeCeff
			+= (btb_pspec->t_Ceffs->cnodeCeff + btb_pspec->d_Ceffs->cnodeCeff);
		fprintf(stderr,"btb node = %lf\n",1E12*(btb_pspec->t_Ceffs->cnodeCeff + btb_pspec->d_Ceffs->cnodeCeff));
	}
	//  tcnodeCeff = 0.0;
   clock_pspec = create_clock_panalyzer(
   		  name, /* clock tree name */
	      pmode, /* io power model mode */
		  opfreq * 1E6, /* operating frequency in MHz */
		  Vdd, /* operating voltage in V */
		  /* clock specific parameters */
		  tdarea, /* tdarea : 2cm x 2cm total die area in um^2 */
		  tcnodeCeff, /* tcnodeCeff : total clocked node capacitance in F */
		
		  /* clock tree style */
		  ((ctstyle == 'n') ? Htree : balHtree),
		  cskew * 1E-12, /* in ps */
		  ncbstages_opt, /* optimial number of io buffer stages */
		
		  /* switching/internal/lekage  effective capacitances */
		 sCeff * 1E-12, iCeff * 1E-12, lCeff * 1E-12);
#endif /* CLOCK_PANALYZER_H */

#ifdef LV2_PANALYZER_H
#ifdef LOGIC_PANALYZER_H
  /* logic panalyzer configurator */
//  {
    sscanf(panalyzer_logic_opt, "%[^:]:%c", name, &pmodel);
	fprintf(stderr,"before logic panal creation\n"); 
	if(pmodel == 'a')
	{
		fprintf(stderr,"Analytical\n"); 
		if(sscanf(panalyzer_logic_opt, "%[^:]:%c:%lf:%lf:%[^:]:%d:%d:%d:%d",
			name, &pmodel, &opfreq, &lvolt, lstyle, &Ngates, &Nfunc,&fi,&fld) != 9)
			fatal("bad logic panalyzer analytical power model parms: <name>:<pmodel>:<opfreq>:<svolt>:<lstyle>:<Ngates>:<Nfunc>:<fi>:<fld>");
		pmode = Analytical;
		if(strcmp(lstyle,"Static") == 0) 
		{
			logic_style = Static;
		}
		else 
			logic_style = Dynamic;
	}
	else
	if(pmodel == 'e')
	{
		if(sscanf(panalyzer_clock_opt, "%[^:]:%c:%lf:%lf:%lf:%lf:%lf",
			name, &pmodel, &opfreq, &xCeff, &yCeff, &iCeff, &lCeff) != 7)
			fatal("bad logic panalyzer empirical power model parms: <name>:<pmodel>:<opfreq>:<xCeff>:<yCeff>:<iCeff>:<lCeff>");
		pmode = Empirical;
	}
	else
		fatal("invalid logic power model mode!");
	
	
//	fprintf(stderr,"after scanning \n"); 
	logic_pspec = create_logic_panalyzer(
	//create_logic_panalyzer(
	name, /* memory name */
	pmode, /* power model mode */
	/* memory operating parameters: operating frequency/supply voltage */
	opfreq, lvolt, 
	logic_style, /* logic tree style */
	Ngates, Nfunc, 
	fi, fld, 
	/* x/y switching/internal/lekage  effective capacitances */
	xCeff, yCeff, 
	iCeff, lCeff);
//	fprintf(stderr,"logic_pspec =  %x\n",logic_pspec); 
//  }
#endif

#ifdef FP_PANALYZER_H
	alu_pspec = create_alu_panalyzer("alu",(int) (opfreq*1E6),Vdd,alu_Ceff*1E-12);
	mult_pspec = create_mult_panalyzer("mult",(int) (opfreq*1E6),Vdd,mult_Ceff*1E-12);
	fpu_pspec = create_fpu_panalyzer("fpu",(int) (opfreq*1E6),Vdd,fpu_Ceff*1E-12);
#endif
#endif
#ifdef UARCH_PANALYZER_H
  /* uarch configurator */
    sscanf(panalyzer_uarch_opt, "%[^:]:%c", name, &pmodel);
	if(pmodel == 'a')
	{
		if(sscanf(panalyzer_uarch_opt, "%[^:]:%c:%lf", name, &pmodel, &opfreq) != 3)
			fatal("bad uarch panalyzer analytical power model parms: <name>:<pmodel>:<opfreq>");
		pmode = Analytical;
	}
	else
	if(pmodel == 'e')
	{
		if(sscanf(panalyzer_uarch_opt, "%[^:]:%c:%lf:%lf:%lf:%lf",
			name, &pmodel, &opfreq, &sCeff, &iCeff, &lCeff) != 6)
		fatal("bad uarch panalyzer empirical power model parms: <name>:<pmodel>:<opfreq>:<sCeff>:<iCeff>:<lCeff>");
	    pmode = Empirical;
	}
	else
		fatal("invalid uarch power model mode!");
	uarch_pspec = create_uarch_panalyzer(
					      name, /* uarch name */ pmode, /* power model mode */
					      opfreq * 1E6, Vdd, /* operating frequency/supply voltage */
					      aio_pspec, dio_pspec, /* io power models */
					      irf_pspec, fprf_pspec, /* integer/floating point power models */
					      bimod_pspec, lev1_pspec, lev2_pspec, ras_pspec,
					      il1_pspec, dl1_pspec, /* L1 caches */
					      il2_pspec, dl2_pspec, /* L2 caches */
					      itlb_pspec, dtlb_pspec, /* TLBs */
					      btb_pspec,
						  logic_pspec,
					      clock_pspec, /* clock power model */
#ifdef FP_PANALYZER_H
						  alu_pspec,
						  mult_pspec,
						  fpu_pspec,
#endif						  
						  
					      sCeff * 1E-12, iCeff * 1E-12, lCeff * 1E-12);
#endif /* UARCH_PANALYZER_H */
#endif

			
#endif	
}

/* print simulator-specific configuration information */
void
sim_aux_config(FILE *stream) /* output stream */
{
  /* nada */
}

/* register the pipetrace statistics */
/* Things with a NT at the start are not trace in gpv */
void 
ptrace_reg_stats(struct stat_sdb_t *sdb)
{
	stat_reg_counter(sdb, 
"NTsim_num_insn", "total number of instructions committed", &sim_num_insn, sim_num_insn, NULL);
	stat_reg_counter(sdb, 
"NTsim_cycle", "total simulation time in cycles", &sim_cycle, /* initial value */0, /* format */NULL);
	stat_reg_formula(sdb, 
"sim_IPC", "instructions per cycle", "NTsim_num_insn / NTsim_cycle", /* format */NULL);
}

/* register simulator-specific statistics */
void
sim_reg_stats(struct stat_sdb_t *sdb)   /* stats database */
{
	int i;
	stat_reg_counter(sdb, 	"sim_num_insn", "total number of instructions committed", &sim_num_insn, sim_num_insn, NULL);
	stat_reg_counter(sdb, 	"sim_num_uops", "total number of UOPs executed", &sim_num_uops, sim_num_uops, NULL);
	stat_reg_formula(sdb, 	"sim_avg_flowlen", "uops per instruction", "sim_num_uops / sim_num_insn", NULL);
	stat_reg_counter(sdb, 	"sim_num_refs", "total number of loads and stores committed", &sim_num_refs, 0, NULL);
	stat_reg_counter(sdb, 	"sim_num_loads", "total number of loads committed", &sim_num_loads, 0, NULL);
	stat_reg_formula(sdb, 	"sim_num_stores", "total number of stores committed", "sim_num_refs - sim_num_loads", NULL);
	stat_reg_counter(sdb, 	"sim_num_branches", "total number of branches committed", &sim_num_branches, /* initial value */0, /* format */NULL);
	stat_reg_int(sdb, 		"sim_elapsed_time", "total simulation time in seconds", &sim_elapsed_time, 0, NULL);
	stat_reg_formula(sdb,	"sim_inst_rate", "simulation speed (in insts/sec)", "sim_num_insn / sim_elapsed_time", NULL);

	stat_reg_counter(sdb, 	"sim_total_insn", "total number of instructions executed", &sim_total_insn, 0, NULL);
	stat_reg_counter(sdb, 	"sim_total_refs", "total number of loads and stores executed", &sim_total_refs, 0, NULL);
	stat_reg_counter(sdb, 	"sim_total_loads", "total number of loads executed", &sim_total_loads, 0, NULL);
	stat_reg_formula(sdb, 	"sim_total_stores", "total number of stores executed", "sim_total_refs - sim_total_loads", NULL);
	stat_reg_counter(sdb, 	"sim_total_branches", "total number of branches executed", &sim_total_branches, /* initial value */0, /* format */NULL);

	/* register performance stats */
	stat_reg_counter(sdb, 	"sim_cycle", "total simulation time in cycles", &sim_cycle, /* initial value */0, /* format */NULL);
	stat_reg_formula(sdb, 	"sim_IPC", "instructions per cycle", "sim_num_insn / sim_cycle", /* format */NULL);
	stat_reg_formula(sdb, 	"sim_CPI", "cycles per instruction", "sim_cycle / sim_num_insn", /* format */NULL);
	stat_reg_formula(sdb, 	"sim_exec_BW", "total instructions (mis-spec + committed) per cycle", "sim_total_insn / sim_cycle", /* format */NULL);
	stat_reg_formula(sdb, 	"sim_IPB", "instruction per branch", "sim_num_insn / sim_num_branches", /* format */NULL);

	/* occupancy stats */
	stat_reg_counter(sdb, 	"IFQ_count", "cumulative IFQ occupancy", &IFQ_count, /* initial value */0, /* format */NULL);
	stat_reg_counter(sdb, 	"IFQ_fcount", "cumulative IFQ full count", &IFQ_fcount, /* initial value */0, /* format */NULL);
	stat_reg_formula(sdb, 	"ifq_occupancy", "avg IFQ occupancy (insn's)", "IFQ_count / sim_cycle", /* format */NULL);
	stat_reg_formula(sdb, 	"ifq_rate", "avg IFQ dispatch rate (insn/cycle)", "sim_total_insn / sim_cycle", /* format */NULL);
	stat_reg_formula(sdb, 	"ifq_latency", "avg IFQ occupant latency (cycle's)", "ifq_occupancy / ifq_rate", /* format */NULL);
	stat_reg_formula(sdb, 	"ifq_full", "fraction of time (cycle's) IFQ was full", "IFQ_fcount / sim_cycle", /* format */NULL);

	stat_reg_counter(sdb, 	"RUU_count", "cumulative RUU occupancy", &RUU_count, /* initial value */0, /* format */NULL);
	stat_reg_counter(sdb, 	"RUU_fcount", "cumulative RUU full count", &RUU_fcount, /* initial value */0, /* format */NULL);
	stat_reg_formula(sdb, 	"ruu_occupancy", "avg RUU occupancy (insn's)", "RUU_count / sim_cycle", /* format */NULL);
	stat_reg_formula(sdb, 	"ruu_rate", "avg RUU dispatch rate (insn/cycle)", "sim_total_insn / sim_cycle", /* format */NULL);
	stat_reg_formula(sdb, 	"ruu_latency", "avg RUU occupant latency (cycle's)", "ruu_occupancy / ruu_rate", /* format */NULL);
	stat_reg_formula(sdb, 	"ruu_full", "fraction of time (cycle's) RUU was full", "RUU_fcount / sim_cycle", /* format */NULL);

	stat_reg_counter(sdb, 	"LSQ_count", "cumulative LSQ occupancy", &LSQ_count, /* initial value */0, /* format */NULL);
	stat_reg_counter(sdb, 	"LSQ_fcount", "cumulative LSQ full count", &LSQ_fcount, /* initial value */0, /* format */NULL);
	stat_reg_formula(sdb, 	"lsq_occupancy", "avg LSQ occupancy (insn's)", "LSQ_count / sim_cycle", /* format */NULL);
	stat_reg_formula(sdb, 	"lsq_rate", "avg LSQ dispatch rate (insn/cycle)", "sim_total_insn / sim_cycle", /* format */NULL);
	stat_reg_formula(sdb, 	"lsq_latency", "avg LSQ occupant latency (cycle's)", "lsq_occupancy / lsq_rate", /* format */NULL);
	stat_reg_formula(sdb, 	"lsq_full", "fraction of time (cycle's) LSQ was full", "LSQ_fcount / sim_cycle", /* format */NULL);

	stat_reg_counter(sdb, 	"sim_slip", "total number of slip cycles", &sim_slip, 0, NULL); /* register baseline stats */
	stat_reg_formula(sdb, 	"avg_sim_slip", "the average slip between issue and retirement", "sim_slip / sim_num_insn", NULL);

	/* register predictor stats */
	if(pred)
		bpred_reg_stats(pred, sdb);

	/* register cache stats */	
	// added by Taeho Kgil
	stat_reg_counter(sdb, 	"concurrent_misses", "total number of concurrent misses", &concurrent_misses, 0 , NULL);
	stat_reg_counter(sdb, 	"prefetch_hits", "total number of prefetch hits", &prefetch_hit, 0 , NULL);
	// ends here
	
	if(cache_il1 && (cache_il1 != cache_dl1 && cache_il1 != cache_dl2))
		cache_reg_stats(cache_il1, sdb);
	if(cache_il2 && (cache_il2 != cache_dl1 && cache_il2 != cache_dl2))
		cache_reg_stats(cache_il2, sdb);
	if(cache_dl1) 
		cache_reg_stats(cache_dl1, sdb);
	if(cache_dl2)
		cache_reg_stats(cache_dl2, sdb);
	if(itlb)
		cache_reg_stats(itlb, sdb);
	if(dtlb)
		cache_reg_stats(dtlb, sdb);
	/* debug variable(s) */
	stat_reg_counter(sdb, 	"sim_invalid_addrs", "total non-speculative bogus addresses seen (debug var)", &sim_invalid_addrs, /* initial value */0, /* format */NULL);

	for(i = 0; i < pcstat_nelt; i++)
	{
		char buf[512], buf1[512];
		struct stat_stat_t *stat;

		/* track the named statistical variable by text address */

		/* find it... */
		stat = stat_find_stat(sdb, pcstat_vars[i]);
		if(!stat)
			fatal("cannot locate any statistic named `%s'", pcstat_vars[i]);

		/* stat must be an integral type */
		if(stat->sc != sc_int && stat->sc != sc_uint && stat->sc != sc_counter)
			fatal("`-pcstat' statistical variable `%s' is not an integral type", stat->name);

		/* register this stat */
		pcstat_stats[i] = stat;
		pcstat_lastvals[i] = STATVAL(stat);

		/* declare the sparce text distribution */
		sprintf(buf, "%s_by_pc", stat->name);
		sprintf(buf1, "%s (by text address)", stat->desc);
		/* initial value, print format, format, print fn */
		pcstat_sdists[i] = stat_reg_sdist(sdb, buf, buf1, 0, (PF_COUNT|PF_PDF), "0x%lx %lu %.2f", NULL);
	}
	/* add profiling stats */
	if(profile_mode)
	{
		stat_reg_float(sdb,	"MAX_IPC", "The Maximum IPC", &max_ipc_value, /* initial value */0, /* format */NULL);
		stat_reg_int(sdb, 	"MAX_IPC", "the first cycle of the maximum IPC", &max_ipc_time, /* initial value */0, /* format */NULL);
		stat_reg_float(sdb, "MIN_IPC", "The Minimum IPC", &min_ipc_value, /* initial value */0, /* format */NULL);
		stat_reg_int(sdb, 	"MIN_IPC", "the first cycle of the minimum IPC", &min_ipc_time, /* initial value */0, /* format */NULL);
		stat_reg_float(sdb, "MAXM_IPC", "Maximum IPC(insn>10000000)", &maxm_ipc_value, /* initial value */0, /* format */NULL);
		stat_reg_int(sdb, 	"MAXM_IPC", "the cycle (insn>10000000)of the maximum IPC", &maxm_ipc_time, /* initial value */0, /* format */NULL);
		stat_reg_float(sdb, "MINM_IPC", "Minimum IPC(insn>10000000)", &minm_ipc_value, /* initial value */0, /* format */NULL);
		stat_reg_int(sdb, 	"MINM_IPC", "the cycle (insn>10000000)of the minimum IPC", &minm_ipc_time, /* initial value */0, /* format */NULL);


		stat_reg_float(sdb, "range1", "Greatest IPC delta ", &range_val[0], /* initial value */0, /* format */NULL);
		stat_reg_int(sdb, 	"range1", "start 10000 cycle range with greatest IPC delta ", &range[0], /* initial value */0, /* format */NULL);
		stat_reg_float(sdb, "range2", "2nd greatest IPC delta ", &range_val[1], /* initial value */0, /* format */NULL);
		stat_reg_int(sdb, 	"range2", "start 10000 cycle range with 2nd greatest IPC delta ", &range[1], /* initial value */0, /* format */NULL);
		stat_reg_float(sdb, "range3", "3rd greatest IPC delta ", &range_val[2], /* initial value */0, /* format */NULL);
		stat_reg_int(sdb, 	"range3", "start 10000 cycle range with 3rd greatest IPC delta ", &range[2], /* initial value */0, /* format */NULL);
		stat_reg_float(sdb, "range4", "4th greatest IPC delta ", &range_val[3], /* initial value */0, /* format */NULL);
		stat_reg_int(sdb, 	"range4", "start 10000 cycle range with 4th greatest IPC delta ", &range[3], /* initial value */0, /* format */NULL);
		stat_reg_float(sdb, "range5", "5th greatest IPC delta ", &range_val[4], /* initial value */0, /* format */NULL);
		stat_reg_int(sdb, 	"range5", "start 10000 cycle range  with 5th greatest IPC delta ", &range[4], /* initial value */0, /* format */NULL);
	}
	ld_reg_stats(sdb);
	mem_reg_stats(mem, sdb);

  /* microarchitecture stats */
  md_reg_stats(sdb);

#ifdef PANALYZER_H
#ifdef LV1_PANALYZER_H
  lv1_panalyzer_reg_stats(sdb);
#endif

#ifdef LV2_PANALYZER_H
#ifdef IO_PANALYZER_H
  if (aio_pspec)
	  panalyzer_reg_stats(aio_pspec->name, aio_pspec->pdissipation, sdb);
  if (dio_pspec)
      panalyzer_reg_stats(dio_pspec->name, dio_pspec->pdissipation, sdb);
#endif
#ifdef MEMORY_PANALYZER_H
  if (irf_pspec)
	  panalyzer_reg_stats(irf_pspec->name, irf_pspec->pdissipation, sdb);
  if (fprf_pspec)
      panalyzer_reg_stats(fprf_pspec->name, fprf_pspec->pdissipation, sdb);
#endif
#ifdef CACHE_PANALYZER_H
  if (il1_pspec)
	  panalyzer_reg_stats(il1_pspec->name, il1_pspec->pdissipation, sdb);
  if (dl1_pspec)
      panalyzer_reg_stats(dl1_pspec->name, dl1_pspec->pdissipation, sdb);
  if (cache_il2
      && (cache_il2 != cache_dl1 && cache_il2 != cache_dl2))
	  panalyzer_reg_stats(il2_pspec->name, il2_pspec->pdissipation, sdb);
  if (dl2_pspec)
      panalyzer_reg_stats(dl2_pspec->name, dl2_pspec->pdissipation, sdb);
  if (itlb_pspec)
      panalyzer_reg_stats(itlb_pspec->name, itlb_pspec->pdissipation, sdb);
  if (dtlb_pspec)
      panalyzer_reg_stats(dtlb_pspec->name, dtlb_pspec->pdissipation, sdb);
#endif
#ifdef CACHE_PANALYZER_H
  if (btb_pspec)
	  panalyzer_reg_stats(btb_pspec->name, btb_pspec->pdissipation, sdb);
#endif
#ifdef MEMORY_PANALYZER_H
  if (bimod_pspec)
	  panalyzer_reg_stats(bimod_pspec->name, bimod_pspec->pdissipation, sdb);
  if (lev1_pspec)
	  panalyzer_reg_stats(lev1_pspec->name, lev1_pspec->pdissipation, sdb);
  if (lev2_pspec)
	  panalyzer_reg_stats(lev2_pspec->name, lev2_pspec->pdissipation, sdb);
  if (ras_pspec)
      panalyzer_reg_stats(ras_pspec->name, ras_pspec->pdissipation, sdb);
#endif
#ifdef LOGIC_PANALYZER_H
  if (logic_pspec)
	  panalyzer_reg_stats(logic_pspec->name, logic_pspec->pdissipation, sdb);
#endif
#ifdef CLOCK_PANALYZER_H
  if (clock_pspec)
	  panalyzer_reg_stats(clock_pspec->name, clock_pspec->pdissipation, sdb);
#endif

#ifdef FP_PANALYZER_H
	if( alu_pspec)
	{
		stat_reg_int(sdb, "alu access", "number of times alu is accessed", 
			&(alu_pspec->alu_access), 0, NULL);	
		stat_reg_double(sdb, "alu max power", "Maximum power for alu", 
			&(alu_pspec->max_power), 0, NULL);
		stat_reg_double(sdb, "alu total power", "Total power for alu", 
			&(alu_pspec->alu_total_power), 0, NULL);
		stat_reg_double(sdb, "alu avg power", "Avg power for alu", 
			&(alu_pspec->avg_power), 0, NULL);
	}
	if( mult_pspec)
	{
		stat_reg_int(sdb, "mult access", "number of times mult is accessed", 
			&(mult_pspec->mult_access), 0, NULL);	
		stat_reg_double(sdb, "mult max power", "Maximum power for mult", 
			&(mult_pspec->max_power), 0, NULL);
		stat_reg_double(sdb, "mult total power", "Total power for mult", 
			&(mult_pspec->mult_total_power), 0, NULL);
		stat_reg_double(sdb, "mult avg power", "Avg power for mult", 
			&(mult_pspec->avg_power), 0, NULL);
	}
	if( fpu_pspec)
	{
		stat_reg_int(sdb, "fpu access", "number of times fpu is accessed", 
			&(fpu_pspec->fpu_access), 0, NULL);	
		stat_reg_double(sdb, "fpu max power", "Maximum power for fpu", 
			&(fpu_pspec->max_power), 0, NULL);
		stat_reg_double(sdb, "fpu total power", "Total power for fpu", 
			&(fpu_pspec->fpu_total_power), 0, NULL);
		stat_reg_double(sdb, "fpu avg power", "Avg power for fpu", 
			&(fpu_pspec->avg_power), 0, NULL);
	}

#endif	

#ifdef UARCH_PANALYZER_H  
  panalyzer_reg_stats(uarch_pspec->name, uarch_pspec->pdissipation, sdb);
#endif
  
#endif
  
#endif
	
}

/* forward declarations */
static void ruu_init(void);
static void lsq_init(void);
static void rslink_init(int nlinks);
static void eventq_init(void);
static void readyq_init(void);
static void cv_init(void);
static void tracer_init(void);
static void fetch_init(void);

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

#ifdef PANALYZER_H
#ifdef LV1_PANALYZER_H
	lv1_pwr_frame = init_pwr_frame();
#endif
#endif
	/* */
}

/* default register state accessor, used by DLite */
static char * /* err str, NULL for no err */
/* registers to access, access type, reg bank to probe, register number, input, output */
simoo_reg_obj(struct regs_t *regs, int is_write, enum md_reg_type rt, int reg, struct eval_value_t *val);	

/* default memory state accessor, used by DLite */
static char * /* err str, NULL for no err */
/* memory space to access, access type, address to access, input/output buffer, size of access */
simoo_mem_obj(struct mem_t *mem, int is_write, md_addr_t addr, char *p,	int nbytes);			

/* default machine state accessor, used by DLite */
static char * /* err str, NULL for no err */
/* output stream */ /* optional command string */ /* registers to access */ /* memory space to access */
simoo_mstate_obj(FILE *stream,	char *cmd, struct regs_t *regs, struct mem_t *mem);		

/* total RS links allocated at program start */
#define MAX_RS_LINKS                    4096

/* load program into simulated state */
void
/* program to load, program arguments, program environment */
sim_load_prog(char *fname, int argc, char **argv, char **envp)		
{
	/* load program text and data, set up environment, memory, and regs */
	ld_load_prog(fname, argc, argv, envp, &regs, mem, TRUE);

	/* initialize here, so symbols can be loaded */
	if (ptrace_nelt == 2)
	{
		/* generate a pipeline trace */
		ptrace_open(/* fname */ptrace_opts[0], /* range */ptrace_opts[1]);
	}
	else 
	if (ptrace_nelt == 0)
	{
		/* no pipetracing */;
	}
	else
		fatal("bad pipetrace args, use: <fname|stdout|stderr> <range>");

	/* finish initialization of the simulation engine */
	fu_pool = res_create_pool("fu-pool", fu_config, N_ELT(fu_config));
	rslink_init(MAX_RS_LINKS);
	tracer_init();
	fetch_init();
	cv_init();
	eventq_init();
	readyq_init();
	ruu_init();
	lsq_init();

	/* initialize the DLite debugger */
	dlite_init(simoo_reg_obj, simoo_mem_obj, simoo_mstate_obj);
}

/* dump simulator-specific auxiliary simulator statistics */
void
sim_aux_stats(FILE *stream)             /* output stream */
{
	/* nada */
#ifdef PANALYZER_H
#ifdef LV1_PANALYZER_H		
#ifdef PA_TRACE_WINDOW_H
	FILE *fw;
	int i;
#endif
#endif
#endif
	
#ifdef PANALYZER_H
#ifdef LV1_PANALYZER_H		
	sim_lv1_panalyzer_stats(stream,sim_cycle); 
	
#ifdef PA_TRACE_WINDOW_H
	fw = fopen("pa_window","w+");
	lv1_pa_trace_arg.fp = fw;
	lv1_pa_trace_arg.window = pa_trace_window;
#ifdef PA_TRANS_COUNT	
	lv1_pa_trace_arg.aio_ham_window = pa_aio_ham_window;
	lv1_pa_trace_arg.dio_ham_window = pa_dio_ham_window;
#endif
	if(fw == NULL)
		fatal("Could not create PA window trace file\n");
	lv1_panalyzer(lv1_pa_stats,(int) (&lv1_pa_trace_arg));
	fclose(fw);
#endif
#endif
#endif		
}

/* un-initialize the simulator */
void
sim_uninit(void)
{
	if(ptrace_nelt > 0)
		ptrace_close();
	if(trace_me)
	   fclose(tracefile);
}


/*
 * processor core definitions and declarations
 */

/* inst tag type, used to tag an operation instance in the RUU */
typedef unsigned int INST_TAG_TYPE;

/* inst sequence type, used to order instructions in the ready list, if this rolls over the ready list order temporarily will get messed up, 
 * but execution will continue and complete correctly */
typedef unsigned int INST_SEQ_TYPE;


/* total input dependencies possible */
#define MAX_IDEPS               4

/* total output dependencies possible */
#define MAX_ODEPS               3

/* a register update unit (RUU) station, this record is contained in the processors RUU, which serves as a collection of ordered reservations stations.  
 * The reservation stations capture register results and await the time when all operands are ready, at which time the instruction is issued to the functional units; 
 * the RUU is an order circular queue, in which instructions are inserted in fetch (program) order, results are stored in the RUU buffers, 
 * and later when an RUU entry is the oldest entry in the machines, it and its instruction's value is retired to the architectural register file in program order, 
 * NOTE: the RUU and LSQ share the same structure, this is useful because loads and stores are split into two operations: 
 * an effective address add and a load/store, the add is inserted into the RUU and the load/store inserted into the LSQ, allowing the add to wake up the load/store 
 * when effective address computation has finished */
struct RUU_station {
  /* inst info */
  md_inst_t IR; /* instruction bits */
  enum md_opcode op; /* decoded instruction opcode */
  md_addr_t PC, next_PC, pred_PC; /* inst PC, next PC, predicted PC */
  int in_LSQ; /* non-zero if op is in LSQ */
  int ea_comp; /* non-zero if op is an addr comp */
  int recover_inst; /* start of mis-speculation? */
  int stack_recover_idx; /* non-speculative TOS for RSB pred */
  struct bpred_update_t dir_update;	/* bpred direction update info */
  int spec_mode; /* non-zero if issued in spec_mode */
  md_addr_t addr; /* effective address for ld/st's */
  INST_TAG_TYPE tag; /* RUU slot tag, increment to squash operation */
  INST_SEQ_TYPE seq; /* instruction sequence, used to sort the ready list and tag inst */
  unsigned int ptrace_seq; /* pipetrace sequence number */
  int slip;
  /* instruction status */
  int queued; /* operands ready and queued */
  int issued; /* operation is/was executing */
  int completed; /* operation has completed execution */
  
  /* output operand dependency list, these lists are used to limit the number of associative searches into the RUU when instructions complete and need to wake up dependent insts */
  int onames[MAX_ODEPS]; /* output logical names (NA=unused) */
  struct RS_link *odep_list[MAX_ODEPS];	/* chains to consuming operations */

  /* input dependent links, the output chains rooted above use these fields to mark input operands as ready, when all these fields have been set non-zero, 
   * the RUU operation has all of its register operands, it may commence execution as soon as all of its memory operands are known to be read 
   * (see lsq_refresh() for details on enforcing memory dependencies) */
  int idep_ready[MAX_IDEPS]; /* input operand ready? */
};

/* non-zero if all register operands are ready, update with MAX_IDEPS */
#define OPERANDS_READY(RS) ((RS)->idep_ready[0] && (RS)->idep_ready[1] && (RS)->idep_ready[2] && (RS)->idep_ready[3])

/* register update unit, combination of reservation stations and reorder buffer device, organized as a circular queue */
static struct RUU_station *RUU;	/* register update unit */
static int RUU_head, RUU_tail; /* RUU head and tail pointers */
static int RUU_num;	/* num entries currently in RUU */

/* allocate and initialize register update unit (RUU) */
static void
ruu_init(void)
{
	RUU = calloc(RUU_size, sizeof(struct RUU_station));
	if(!RUU)
		fatal("out of virtual memory");

	RUU_num = 0;
	RUU_head = RUU_tail = 0;
	RUU_count = 0;
	RUU_fcount = 0;
}

/* dump the contents of the RUU */
static void
/* ptr to RUU station, entry index, output stream, print header? */
ruu_dumpent(struct RUU_station *rs, int index, FILE *stream, int header)				
{
	if(!stream)
		stream = stderr;

	if(header)
		fprintf(stream,	"idx: %2d: opcode: %s, inst: `", index, MD_OP_NAME(rs->op));
	else
		fprintf(stream,	"       opcode: %s, inst: `", MD_OP_NAME(rs->op));
	md_print_uop(rs->op, rs->IR, rs->PC, stream);
	fprintf(stream, 	"'\n");
	myfprintf(stream, 	"         PC: 0x%08p, NPC: 0x%08p (pred_PC: 0x%08p)\n", rs->PC, rs->next_PC, rs->pred_PC);
	fprintf(stream, 	"         in_LSQ: %s, ea_comp: %s, recover_inst: %s\n", rs->in_LSQ ? "t" : "f", rs->ea_comp ? "t" : "f", rs->recover_inst ? "t" : "f");
	myfprintf(stream, 	"         spec_mode: %s, addr: 0x%08p, tag: 0x%08x\n", rs->spec_mode ? "t" : "f", rs->addr, rs->tag);
	fprintf(stream, 	"         seq: 0x%08x, ptrace_seq: 0x%08x\n", rs->seq, rs->ptrace_seq);
	fprintf(stream, 	"         queued: %s, issued: %s, completed: %s\n", rs->queued ? "t" : "f", rs->issued ? "t" : "f", rs->completed ? "t" : "f");
	fprintf(stream, 	"         operands ready: %s\n", OPERANDS_READY(rs) ? "t" : "f");
}

/* dump the contents of the RUU */
static void
ruu_dump(FILE *stream)				/* output stream */
{
	int num, head;
	struct RUU_station *rs;

	if(!stream)
		stream = stderr;

	fprintf(stream, "** RUU state **\n");
	fprintf(stream, "RUU_head: %d, RUU_tail: %d\n", RUU_head, RUU_tail);
	fprintf(stream, "RUU_num: %d\n", RUU_num);

	num = RUU_num;
	head = RUU_head;
	while(num)
	{
		rs = &RUU[head];
		ruu_dumpent(rs, rs - RUU, stream, /* header */TRUE);
		head = (head + 1) % RUU_size;
		num--;
	}
}

/*
 * load/store queue (LSQ): holds loads and stores in program order, indicating
 * status of load/store access:
 *
 *   - issued: address computation complete, memory access in progress
 *   - completed: memory access has completed, stored value available
 *   - squashed: memory access was squashed, ignore this entry
 *
 * loads may execute when:
 *   1) register operands are ready, and
 *   2) memory operands are ready (no earlier unresolved store)
 *
 * loads are serviced by:
 *   1) previous store at same address in LSQ (hit latency), or
 *   2) data cache (hit latency + miss latency)
 *
 * stores may execute when:
 *   1) register operands are ready
 *
 * stores are serviced by:
 *   1) depositing store value into the load/store queue
 *   2) writing store value to the store buffer (plus tag check) at commit
 *   3) writing store buffer entry to data cache when cache is free
 *
 * NOTE: the load/store queue can bypass a store value to a load in the same
 *   cycle the store executes (using a bypass network), thus stores complete
 *   in effective zero time after their effective address is known
 */
static struct RUU_station *LSQ; /* load/store queue */
static int LSQ_head, LSQ_tail; /* LSQ head and tail pointers */
static int LSQ_num; /* num entries currently in LSQ */

/*
 * input dependencies for stores in the LSQ:
 *   idep #0 - operand input (value that is store'd)
 *   idep #1+ - effective address input (address of store operation)
*/
#define STORE_OP_READY(RS)		((RS)->idep_ready[0])
#define STORE_ADDR_READY(RS) 	((RS)->idep_ready[1] && (RS)->idep_ready[2] && (RS)->idep_ready[3])

/* allocate and initialize the load/store queue (LSQ) */
static void
lsq_init(void)
{
	LSQ = calloc(LSQ_size, sizeof(struct RUU_station));
	if(!LSQ)
		fatal("out of virtual memory");

	LSQ_num = 0;
	LSQ_head = LSQ_tail = 0;
	LSQ_count = 0;
	LSQ_fcount = 0;
}

/* dump the contents of the RUU */
static void
lsq_dump(FILE *stream) /* output stream */
{
	int num, head;
	struct RUU_station *rs;

	if(!stream)
		stream = stderr;

	fprintf(stream, "** LSQ state **\n");
	fprintf(stream, "LSQ_head: %d, LSQ_tail: %d\n", LSQ_head, LSQ_tail);
	fprintf(stream, "LSQ_num: %d\n", LSQ_num);

	num = LSQ_num;
	head = LSQ_head;
	while(num)
	{
		rs = &LSQ[head];
		ruu_dumpent(rs, rs - LSQ, stream, /* header */TRUE);
		head = (head + 1) % LSQ_size;
		num--;
	}
}


/*
 * RS_LINK defs and decls
 */

/* a reservation station link: this structure links elements of a RUU reservation station list; used for ready instruction queue, event queue, and output dependency lists; 
 * each RS_LINK node contains a pointer to the RUU entry it references along with an instance tag, 
 * the RS_LINK is only valid if the instruction instance tag matches the instruction RUU entry instance tag; 
 * this strategy allows entries in the RUU can be squashed and reused without updating the lists that point to it, 
 * which significantly improves the performance of (all to frequent) squash events */
struct RS_link {
	struct RS_link *next;	/* next entry in list */
	struct RUU_station *rs; /* referenced RUU resv station */
	INST_TAG_TYPE tag; /* inst instance sequence number */
	union {
		tick_t when; /* time stamp of entry (for eventq) */
		INST_SEQ_TYPE seq; /* inst sequence */
		int opnum; /* input/output operand number */
	} x;
};

/* RS link free list, grab RS_LINKs from here, when needed */
static struct RS_link *rslink_free_list;

/* NULL value for an RS link */
#define RSLINK_NULL_DATA		{ NULL, NULL, 0 }
static struct RS_link RSLINK_NULL = RSLINK_NULL_DATA;

/* create and initialize an RS link */
#define RSLINK_INIT(RSL, RS)	((RSL).next = NULL, (RSL).rs = (RS), (RSL).tag = (RS)->tag)

/* non-zero if RS link is NULL */
#define RSLINK_IS_NULL(LINK)    ((LINK)->rs == NULL)

/* non-zero if RS link is to a valid (non-squashed) entry */
#define RSLINK_VALID(LINK)      ((LINK)->tag == (LINK)->rs->tag)

/* extra RUU reservation station pointer */
#define RSLINK_RS(LINK)         ((LINK)->rs)

/* get a new RS link record */
#define RSLINK_NEW(DST, RS)								\
{ 														\
 	struct RS_link *n_link;								\
	if(!rslink_free_list)								\
		panic("out of rs links");						\
	n_link = rslink_free_list;							\
	rslink_free_list = rslink_free_list->next;			\
	n_link->next = NULL;								\
	n_link->rs = (RS); n_link->tag = n_link->rs->tag;	\
	(DST) = n_link;										\
}

/* free an RS link record */
#define RSLINK_FREE(LINK)								\
{  														\
	struct RS_link *f_link = (LINK);					\
	f_link->rs = NULL; f_link->tag = 0;					\
	f_link->next = rslink_free_list;					\
	rslink_free_list = f_link;							\
}

/* FIXME: could this be faster!!! */
/* free an RS link list */
#define RSLINK_FREE_LIST(LINK)							\
{  														\
	struct RS_link *fl_link, *fl_link_next;				\
	for (fl_link=(LINK); fl_link; fl_link=fl_link_next)	\
	{													\
		fl_link_next = fl_link->next;					\
		RSLINK_FREE(fl_link);							\
	}													\
}

/* initialize the free RS_LINK pool */
static void
rslink_init(int nlinks) /* total number of RS_LINK available */
{
	int i;
	struct RS_link *link;

	rslink_free_list = NULL;
	for (i=0; i<nlinks; i++)
	{
		link = calloc(1, sizeof(struct RS_link));
		if(!link)
			fatal("out of virtual memory");
		link->next = rslink_free_list;
		rslink_free_list = link;
	}
}

/* service all functional unit release events, this function is called once per cycle, and it used to step the BUSY timers attached to each functional unit in the function unit resource pool,
 * as long as a functional unit's BUSY count is > 0, it cannot be issued an operation */
static void
ruu_release_fu(void)
{
	int i;

	/* walk all resource units, decrement busy counts by one */
	for(i = 0; i < fu_pool->num_resources; i++)
	{
		/* resource is released when BUSY hits zero */
		if(fu_pool->resources[i].busy > 0)
			fu_pool->resources[i].busy--;
	}
}


/* the execution unit event queue implementation follows, the event queue indicates which instruction will complete next, the writeback handler * drains this queue */

/* pending event queue, sorted from soonest to latest event (in time), NOTE: RS_LINK nodes are used for the event queue list so that it need not be updated during squash events */
static struct RS_link *event_queue;

/* initialize the event queue structures */
static void
eventq_init(void)
{
	event_queue = NULL;
}

/* dump the contents of the event queue */
static void
eventq_dump(FILE *stream) /* output stream */
{
	struct RS_link *ev;

	if(!stream)
		stream = stderr;

	fprintf(stream, "** event queue state **\n");

	for(ev = event_queue; ev != NULL; ev = ev->next)
	{
		/* is event still valid? */
		if (RSLINK_VALID(ev))
		{
			struct RUU_station *rs = RSLINK_RS(ev);

			fprintf(stream, "idx: %2d: @ %.0f\n", (int)(rs - (rs->in_LSQ ? LSQ : RUU)), (double)ev->x.when);
			ruu_dumpent(rs, rs - (rs->in_LSQ ? LSQ : RUU), stream, /* !header */FALSE);
		}
	}
}

/* insert an event for RS into the event queue, event queue is sorted from earliest to latest event, event and associated side-effects will be apparent at the start of cycle WHEN */
static void
eventq_queue_event(struct RUU_station *rs, tick_t when)
{
	struct RS_link *prev, *ev, *new_ev;

	if(rs->completed)
		panic("event completed");

	if(when <= sim_cycle)
		panic("event occurred in the past");

	/* get a free event record */
	RSLINK_NEW(new_ev, rs);
	new_ev->x.when = when;

	/* locate insertion point */
	for(prev=NULL, ev=event_queue; ev && ev->x.when < when; prev=ev, ev=ev->next);

	if(prev)
	{
		/* insert middle or end */
		new_ev->next = prev->next;
		prev->next = new_ev;
	}
	else
	{
		/* insert at beginning */
		new_ev->next = event_queue;
		event_queue = new_ev;
	}
}

/* return the next event that has already occurred, returns NULL when no remaining events or all remaining events are in the future */
static struct RUU_station *
eventq_next_event(void)
{
	struct RS_link *ev;

	if(event_queue && event_queue->x.when <= sim_cycle)
	{
		/* unlink and return first event on priority list */
		ev = event_queue;
		event_queue = event_queue->next;

		/* event still valid? */
		if (RSLINK_VALID(ev))
		{
			struct RUU_station *rs = RSLINK_RS(ev);

			/* reclaim event record */
			RSLINK_FREE(ev);

			/* event is valid, return resv station */
			return rs;
		}
		else
		{
			/* reclaim event record */
			RSLINK_FREE(ev);

			/* receiving inst was squashed, return next event */
			return eventq_next_event();
		}
		}
		else
		{
			/* no event or no event is ready */
			return NULL;
		}
}


/* the ready instruction queue implementation follows, the ready instruction queue indicates which instruction have all of there register dependencies satisfied, instruction will issue when 
 * 1) all memory dependencies for the instruction have been satisfied (see lsq_refresh() for details on how * this is accomplished) and 
 * 2) resources are available; ready queue is fully constructed each cycle before any operation is issued from it -- this ensures that instruction issue priorities are properly observed; 
 * NOTE: RS_LINK nodes are used for the event queue list so that it need not be updated during squash events */

/* the ready instruction queue */
static struct RS_link *ready_queue;

/* initialize the event queue structures */
static void
readyq_init(void)
{
	ready_queue = NULL;
}

/* dump the contents of the ready queue */
static void
readyq_dump(FILE *stream) /* output stream */
{
	struct RS_link *link;

	if(!stream)
		stream = stderr;

	fprintf(stream, "** ready queue state **\n");

	for(link = ready_queue; link != NULL; link = link->next)
	{
		/* is entry still valid? */
		if (RSLINK_VALID(link))
		{
			struct RUU_station *rs = RSLINK_RS(link);

			ruu_dumpent(rs, rs - (rs->in_LSQ ? LSQ : RUU), stream, /* header */TRUE);
		}
	}
}

/* insert ready node into the ready list using ready instruction scheduling policy; currently the following scheduling policy is enforced: 
 * memory and long latency operands, and branch instructions first then all other instructions, 
 * oldest instructions first this policy works well because branches pass through the machine quicker which works to reduce branch misprediction latencies, 
 * and very long latency instructions (such loads and multiplies) get priority since they are very likely on the program's critical path */
static void
readyq_enqueue(struct RUU_station *rs) /* RS to enqueue */
{
	struct RS_link *prev, *node, *new_node;

	/* node is now queued */
	if(rs->queued)
		panic("node is already queued");
	rs->queued = TRUE;

	/* get a free ready list node */
	RSLINK_NEW(new_node, rs);
	new_node->x.seq = rs->seq;

	/* locate insertion point */
	if(rs->in_LSQ || MD_OP_FLAGS(rs->op) & (F_LONGLAT|F_CTRL))
	{
		/* insert loads/stores and long latency ops at the head of the queue */
		prev = NULL;
		node = ready_queue;
	}
	else
	{
		/* otherwise insert in program order (earliest seq first) */
		for (prev = NULL, node = ready_queue; node && node->x.seq < rs->seq; prev=node, node=node->next);
	}

	if(prev)
	{
		/* insert middle or end */
		new_node->next = prev->next;
		prev->next = new_node;
	}
	else
	{
		/* insert at beginning */
		new_node->next = ready_queue;
		ready_queue = new_node;
	}
}


/* the create vector maps a logical register to a creator in the RUU (and specific output operand) or the architected register file (if RS_link is NULL) */

/* an entry in the create vector */
struct CV_link {
	struct RUU_station *rs;	/* creator's reservation station */
	int odep_num; /* specific output operand */
};

/* a NULL create vector entry */
static struct CV_link CVLINK_NULL = { NULL, 0 };

/* get a new create vector link */
#define CVLINK_INIT(CV, RS,ONUM)	((CV).rs = (RS), (CV).odep_num = (ONUM))

/* size of the create vector (one entry per architected register) */
#define CV_BMAP_SZ					(BITMAP_SIZE(MD_TOTAL_REGS))

/* the create vector, NOTE: speculative copy on write storage provided for fast recovery during wrong path execute (see tracer_recover() for details on this process */
static BITMAP_TYPE(MD_TOTAL_REGS, use_spec_cv);
static struct CV_link create_vector[MD_TOTAL_REGS];
static struct CV_link spec_create_vector[MD_TOTAL_REGS];

/* these arrays shadow the create vector an indicate when a register was last created */
static tick_t create_vector_rt[MD_TOTAL_REGS];
static tick_t spec_create_vector_rt[MD_TOTAL_REGS];

/* read a create vector entry */
#define CREATE_VECTOR(N) 		(BITMAP_SET_P(use_spec_cv, CV_BMAP_SZ, (N)) ? spec_create_vector[N] : create_vector[N])

/* read a create vector timestamp entry */
#define CREATE_VECTOR_RT(N)		(BITMAP_SET_P(use_spec_cv, CV_BMAP_SZ, (N)) ? spec_create_vector_rt[N] : create_vector_rt[N])

/* set a create vector entry */
#define SET_CREATE_VECTOR(N, L)	(spec_mode ? (BITMAP_SET(use_spec_cv, CV_BMAP_SZ, (N)), spec_create_vector[N] = (L)) : (create_vector[N] = (L)))

/* initialize the create vector */
static void
cv_init(void)
{
	int i;

	/* initially all registers are valid in the architected register file, i.e., the create vector entry is CVLINK_NULL */
	for(i = 0; i < MD_TOTAL_REGS; i++)
	{
		create_vector[i] = CVLINK_NULL;
		create_vector_rt[i] = 0;
		spec_create_vector[i] = CVLINK_NULL;
		spec_create_vector_rt[i] = 0;
	}

	/* all create vector entries are non-speculative */
	BITMAP_CLEAR_MAP(use_spec_cv, CV_BMAP_SZ);
}

/* dump the contents of the create vector */
static void
cv_dump(FILE *stream)				/* output stream */
{
	int i;
	struct CV_link ent;

	if(!stream)
		stream = stderr;

	fprintf(stream, "** create vector state **\n");

	for(i = 0; i < MD_TOTAL_REGS; i++)
	{
		ent = CREATE_VECTOR(i);
		if(!ent.rs)
			fprintf(stream, "[cv%02d]: from architected reg file\n", i);
		else
			fprintf(stream, "[cv%02d]: from %s, idx: %d\n", i, (ent.rs->in_LSQ ? "LSQ" : "RUU"), (int)(ent.rs - (ent.rs->in_LSQ ? LSQ : RUU)));
	}
}


/*
 *  RUU_COMMIT() - instruction retirement pipeline stage
 */

/* this function commits the results of the oldest completed entries from the RUU and LSQ to the architected reg file, 
 * stores in the LSQ will commit their store data to the data cache at this point as well */
static void
ruu_commit(void)
{
	int i, lat, events, committed = 0, done;
	static counter_t sim_ret_insn = 0;

	/* all values must be retired to the architected reg file in program order */
	done = FALSE;
	while(!done && committed < ruu_commit_width)
	{
		/* default commit events */
		events = 0;
		done = TRUE;

#ifdef PANALYZER_H
#ifdef LV2_PANALYZER_H
#ifdef LOGIC_PANALYZER_H
        // logic_panalyzer(commit_pspec, sim_cycle);
        logic_panalyzer(logic_pspec, sim_cycle);
#endif /* LOGIC_PANALYZER_H */
#endif          
#endif /* PANALYZER_H */

		/* load/stores must retire load/store queue entry as well */
		if(LSQ_num > 0 && (RUU_num == 0 || LSQ[LSQ_head].seq < RUU[RUU_head].seq))
		{
			/* load/store, retire head of LSQ as well */
			if(LSQ_num <= 0 || !LSQ[LSQ_head].in_LSQ)
				panic("RUU out of sync with LSQ");

			/* load/store operation must be complete */
			if(!LSQ[LSQ_head].completed)
			{
				/* load/store operation is not yet complete */
				break;
			}

			if((MD_OP_FLAGS(LSQ[LSQ_head].op) & (F_MEM|F_STORE)) == (F_MEM|F_STORE))
			{
				struct res_template *fu;

				/* stores must retire their store value to the cache at commit, try to get a store port (functional unit allocation) */
				fu = res_get(fu_pool, MD_OP_FUCLASS(LSQ[LSQ_head].op));
				if(fu)
				{
					/* reserve the functional unit */
					if(fu->master->busy)
						panic("functional unit already in use");

					/* schedule functional unit release event */
					fu->master->busy = fu->issuelat;

					/* go to the data cache */
					if(cache_dl1)
					{
#ifdef VANALYZER_H
						byte_t *p;
						p = (byte_t *)malloc(sizeof(md_inst_t));
						if(mem_access(mem, Read, (LSQ[LSQ_head].addr&~3), p, 4) != md_fault_none) 
							fatal("memory access fault occurs!\n");
						/* commit store value to D-cache */
						lat = cache_access(cache_dl1, Write, (LSQ[LSQ_head].addr&~3), p, 4, sim_cycle, NULL, NULL);

#ifdef PANALYZER_H
#ifdef LV1_PANALYZER_H
				lv1_panalyzer(lv1_dl1,(int) Write);

#endif		
#ifdef LV2_PANALYZER_H
#ifdef CACHE_PANALYZER_H
              cache_panalyzer(dl1_pspec, Write, (LSQ[LSQ_head].addr&~3), NULL, sizeof(md_inst_t), sim_cycle, lat);
#endif /* CACHE_PANALYZER_H */
#endif
						
#endif


						if(lat == cache_dl1_lat)
						{
						}
						free(p);
#else
						/* commit store value to D-cache */
						lat = cache_access(cache_dl1, Write, (LSQ[LSQ_head].addr&~3), NULL, 4, sim_cycle, NULL, NULL);
#endif
						if(lat > cache_dl1_lat)
							events |= PEV_CACHEMISS;
					}

					/* all loads and stores must to access D-TLB */
					if(dtlb)
					{
						/* access the D-TLB */
						lat = cache_access(dtlb, Read, (LSQ[LSQ_head].addr & ~3), NULL, 4, sim_cycle, NULL, NULL);

#ifdef PANALYZER_H
#ifdef LV1_PANALYZER_H
				lv1_panalyzer(lv1_dtlb,(int) Read);
#endif
#ifdef LV2_PANALYZER_H
#ifdef CACHE_PANALYZER_H
				 cache_panalyzer(dtlb_pspec, Read, (LSQ[LSQ_head].addr&~3), NULL, sizeof(md_inst_t), sim_cycle, lat);
#endif /* CACHE_PANALYZER_H */
#endif
#endif				 


						if(lat > 1)
							events |= PEV_TLBMISS;
					}
				}
				else
				{
					/* no store ports left, cannot continue to commit insts */
					break;
				}
			}

			/* invalidate load/store operation instance */
			LSQ[LSQ_head].tag++;
			sim_slip += (sim_cycle - LSQ[LSQ_head].slip);

			/* print retirement trace if in verbose mode */
			if(verbose && sim_num_insn >= trigger_inst)
			{
				sim_ret_insn++;
				myfprintf(stderr, "%10n @ 0x%08p: ", sim_ret_insn, LSQ[LSQ_head].PC);
				md_print_uop(LSQ[LSQ_head].op, LSQ[LSQ_head].IR, LSQ[LSQ_head].PC, stderr);
				if(MD_OP_FLAGS(LSQ[LSQ_head].op) & F_MEM)
					myfprintf(stderr, "  mem: 0x%08p", LSQ[LSQ_head].addr);
				fprintf(stderr, "\n");
				/* fflush(stderr); */
			}

			/* indicate to pipeline trace that this instruction retired */
			ptrace_newstage(LSQ[LSQ_head].ptrace_seq, PST_COMMIT, events, 0,0);
			ptrace_endinst(LSQ[LSQ_head].ptrace_seq);

			for(i = 0; i < MAX_ODEPS; i++)
			{
				if(LSQ[LSQ_head].odep_list[i])
					panic("retired instruction has odeps\n");
			}

			/* commit head of LSQ as well */
			LSQ_head = (LSQ_head + 1) % LSQ_size;
			LSQ_num--;

			/* one more instruction committed to architected state */
			committed++;
			done = FALSE;
		}

		/* retire an instruction from the RUU, if possible */ 
		if(RUU_num > 0 && (LSQ_num == 0 || RUU[RUU_head].seq < LSQ[LSQ_head].seq))
		{
			struct RUU_station *rs = &(RUU[RUU_head]);

			if(!rs->completed)
			{
				/* at least RUU entry must be complete */
				break;
			}


			if(pred && bpred_spec_update == spec_CT && (MD_OP_FLAGS(rs->op) & F_CTRL))
			{
				
				/* branch address, actual target address, taken?, pred taken?, correct pred?, opcode, dir predictor update pointer */
				bpred_update(pred, rs->PC, rs->next_PC, rs->next_PC != (rs->PC + sizeof(md_inst_t)), rs->pred_PC != (rs->PC + sizeof(md_inst_t)), rs->pred_PC == rs->next_PC, rs->op, &rs->dir_update);

#ifdef PANALYZER_H
#ifdef LV1_PANALYZER_H
				lv1_panalyzer(lv1_bpred,(int) 1 /* update */);
#endif								
#endif

			}

			/* invalidate RUU operation instance */
			RUU[RUU_head].tag++;
			sim_slip += (sim_cycle - RUU[RUU_head].slip);

			/* print retirement trace if in verbose mode */
			if(verbose && sim_num_insn >= trigger_inst)
			{
				sim_ret_insn++;
				myfprintf(stderr, "(%n) %10n {%d} @ 0x%08p: ", sim_cycle, sim_ret_insn, RUU[RUU_head].seq, RUU[RUU_head].PC);
				md_print_uop(RUU[RUU_head].op, RUU[RUU_head].IR, RUU[RUU_head].PC, stderr);
				if(MD_OP_FLAGS(RUU[RUU_head].op) & F_MEM)
					myfprintf(stderr, "  mem: 0x%08p", RUU[RUU_head].addr);
				fprintf(stderr, "\n");
				/* fflush(stderr); */
			}

			/* indicate to pipeline trace that this instruction retired */
			ptrace_newstage(RUU[RUU_head].ptrace_seq, PST_COMMIT, events,0,0);
			ptrace_endinst(RUU[RUU_head].ptrace_seq);

			for(i = 0; i < MAX_ODEPS; i++)
			{
				if(rs->odep_list[i])
					panic ("retired instruction has odeps\n");
			}

			/* commit head entry of RUU */
			RUU_head = (RUU_head + 1) % RUU_size;
			RUU_num--;

			/* one more instruction committed to architected state */
			committed++;
			done = FALSE;
		}
	}
}


/* RUU_RECOVER() - squash mispredicted microarchitecture state */

/* recover processor microarchitecture state back to point of the mis-predicted branch at RUU[BRANCH_INDEX] */
static void
ruu_recover(int branch_index) /* index of mis-pred branch */
{
	int i, RUU_index = RUU_tail, LSQ_index = LSQ_tail;
	int RUU_prev_tail = RUU_tail, LSQ_prev_tail = LSQ_tail;

	/* recover from the tail of the RUU towards the head until the branch index is reached, this direction ensures that the LSQ can be synchronized with the RUU */

	/* go to first element to squash */
	RUU_index = (RUU_index + (RUU_size-1)) % RUU_size;
	LSQ_index = (LSQ_index + (LSQ_size-1)) % LSQ_size;

	/* traverse to older insts until the mispredicted branch is encountered */
	while (RUU_index != branch_index)
	{
		/* the RUU should not drain since the mispredicted branch will remain */
		if(!RUU_num)
			panic("empty RUU");

		/* should meet up with the tail first */
		if(RUU_index == RUU_head)
			panic("RUU head and tail broken");

		/* is this operation an effective addr calc for a load or store? */
		if(/* RUU[RUU_index].ea_comp && */ LSQ_num > 0 && (LSQ[LSQ_index].seq > RUU[RUU_index].seq))
		{
			/* should be at least one load or store in the LSQ */
			if(!LSQ_num)
				panic("RUU and LSQ out of sync");

			/* recover any resources consumed by the load or store operation */
			for(i = 0; i < MAX_ODEPS; i++)
			{
				RSLINK_FREE_LIST(LSQ[LSQ_index].odep_list[i]);
				/* blow away the consuming op list */
				LSQ[LSQ_index].odep_list[i] = NULL;
			}

			/* squash this LSQ entry */
			LSQ[LSQ_index].tag++;

			/* indicate in pipetrace that this instruction was squashed */
			ptrace_endinst(LSQ[LSQ_index].ptrace_seq);

			/* go to next earlier LSQ slot */
			LSQ_prev_tail = LSQ_index;
			LSQ_index = (LSQ_index + (LSQ_size-1)) % LSQ_size;
			LSQ_num--;

			/* try to clear another load */
			continue;
		}

		/* recover any resources used by this RUU operation */
		for(i = 0; i < MAX_ODEPS; i++)
		{
			RSLINK_FREE_LIST(RUU[RUU_index].odep_list[i]);
			/* blow away the consuming op list */
			RUU[RUU_index].odep_list[i] = NULL;
		}

		/* squash this RUU entry */
		RUU[RUU_index].tag++;

		/* indicate in pipetrace that this instruction was squashed */
		ptrace_endinst(RUU[RUU_index].ptrace_seq);

		/* go to next earlier slot in the RUU */
		RUU_prev_tail = RUU_index;
		RUU_index = (RUU_index + (RUU_size-1)) % RUU_size;
		RUU_num--;
	}

	/* reset head/tail pointers to point to the mis-predicted branch */
	RUU_tail = RUU_prev_tail;
	LSQ_tail = LSQ_prev_tail;

	/* revert create vector back to last precise create vector state, NOTE: this is accomplished by resetting all the copied-on-write bits in the USE_SPEC_CV bit vector */
	BITMAP_CLEAR_MAP(use_spec_cv, CV_BMAP_SZ);

	/* FIXME: could reset functional units at squash time */
}


/*  RUU_WRITEBACK() - instruction result writeback pipeline stage */

/* forward declarations */
static void tracer_recover(void);

/* writeback completed operation results from the functional units to RUU, at this point, 
 * the output dependency chains of completing instructions are also walked to determine if any dependent instruction now has all of its register operands, 
 * if so the (nearly) ready instruction is inserted into the ready instruction queue */
static void
ruu_writeback(void)
{
	int i;
	struct RUU_station *rs;

	/* service all completed events */
	while ((rs = eventq_next_event()))
	{
		/* RS has completed execution and (possibly) produced a result */
		if (!OPERANDS_READY(rs) || rs->queued || !rs->issued || rs->completed)
			panic("inst completed and !ready, !issued, or completed");

#ifdef PANALYZER_H
#ifdef LV2_PANALYZER_H
#ifdef MEMORY_PANALYZER_H
      if (MD_OP_FLAGS(rs->op) != F_FCOMP)
		  sbank_panalyzer(irf_pspec, NULL, sim_cycle);
	  else
		  sbank_panalyzer(fprf_pspec, NULL, sim_cycle);
#endif
#endif
#endif
								
		/* operation has completed */
		rs->completed = TRUE;

		/* does this operation reveal a mis-predicted branch? */
		if(rs->recover_inst)
		{
			if(rs->in_LSQ)
				panic("mis-predicted load or store?!?!?");

			/* recover processor state and reinit fetch to correct path */
			ruu_recover(rs - RUU);
			tracer_recover();
			bpred_recover(pred, rs->PC, rs->stack_recover_idx);

			/* stall fetch until I-fetch and I-decode recover */
			ruu_fetch_issue_delay = MAX(ruu_branch_penalty,ruu_fetch_issue_delay);

			/* continue writeback of the branch/control instruction */
		}

		/* if we speculatively update branch-predictor, do it here */
		if(pred && bpred_spec_update == spec_WB && !rs->in_LSQ && (MD_OP_FLAGS(rs->op) & F_CTRL)) 
		{
			/* branch address, actual target address, taken?, pred taken?, correct pred?, opcode, dir predictor update pointer */
			bpred_update(pred, rs->PC, rs->next_PC, rs->next_PC != (rs->PC + sizeof(md_inst_t)), rs->pred_PC != (rs->PC + sizeof(md_inst_t)), rs->pred_PC == rs->next_PC, rs->op, &rs->dir_update);

#ifdef PANALYZER_H
#ifdef LV1_PANALYZER_H
				lv1_panalyzer(lv1_bpred,(int) 1 /* update */);
#endif								
#endif

		}

		/* entered writeback stage, indicate in pipe trace */
		ptrace_newstage(rs->ptrace_seq, PST_WRITEBACK,
		rs->recover_inst ? PEV_MPDETECT : 0,0,0);

		/* broadcast results to consuming operations, this is more efficiently accomplished by walking the output dependency chains of the completed instruction */
		for(i = 0; i < MAX_ODEPS; i++)
		{
			if(rs->onames[i] != NA)
			{
				struct CV_link link;
				struct RS_link *olink, *olink_next;

				if(rs->spec_mode)
				{
					/* update the speculative create vector, future operations get value from later creator or architected reg file */
					link = spec_create_vector[rs->onames[i]];
					if(/* !NULL */link.rs && /* refs RS */(link.rs == rs && link.odep_num == i))
					{
						/* the result can now be read from a physical register,
						indicate this as so */
						spec_create_vector[rs->onames[i]] = CVLINK_NULL;
						spec_create_vector_rt[rs->onames[i]] = sim_cycle;
					}
					/* else, creator invalidated or there is another creator */
				}
				else
				{
					/* update the non-speculative create vector, future operations get value from later creator or architected reg file */
					link = create_vector[rs->onames[i]];
					if(/* !NULL */link.rs && /* refs RS */(link.rs == rs && link.odep_num == i))
					{
						/* the result can now be read from a physical register, indicate this as so */
						create_vector[rs->onames[i]] = CVLINK_NULL;
						create_vector_rt[rs->onames[i]] = sim_cycle;
					}
					/* else, creator invalidated or there is another creator */
				}

				/* walk output list, queue up ready operations */
				for(olink = rs->odep_list[i]; olink; olink = olink_next)
				{
					if(RSLINK_VALID(olink))
					{
						if(olink->rs->idep_ready[olink->x.opnum])
							panic("output dependence already satisfied");

						/* input is now ready */
						olink->rs->idep_ready[olink->x.opnum] = TRUE;

						/* are all the register operands of target ready? */
						if(OPERANDS_READY(olink->rs))
						{
							/* yes! enqueue instruction as ready, NOTE: stores complete at dispatch, so no need to enqueue them */
							if(!olink->rs->in_LSQ || ((MD_OP_FLAGS(olink->rs->op)&(F_MEM|F_STORE)) == (F_MEM|F_STORE)))
								readyq_enqueue(olink->rs);
							/* else, ld op, issued when no mem conflict */
						}
					}

					/* grab link to next element prior to free */
					olink_next = olink->next;

					/* free dependence link element */
					RSLINK_FREE(olink);
				}
				/* blow away the consuming op list */
				rs->odep_list[i] = NULL;
			} /* if not NA output */
		} /* for all outputs */
	} /* for all writeback events */
}


/* LSQ_REFRESH() - memory access dependence checker/scheduler */

/* this function locates ready instructions whose memory dependencies have been satisfied, 
 * this is accomplished by walking the LSQ for loads, looking for blocking memory dependency condition (e.g., earlier store with an unknown address) */
#define MAX_STD_UNKNOWNS		256
static void
lsq_refresh(void)
{
	int i, j, index, n_std_unknowns;
	md_addr_t std_unknowns[MAX_STD_UNKNOWNS];

	/* scan entire queue for ready loads: scan from oldest instruction (head) until we reach the tail or an unresolved store, after which no other instruction will become ready */
	for(i = 0, index = LSQ_head, n_std_unknowns = 0; i < LSQ_num; i++, index=(index + 1) % LSQ_size)
	{
		/* terminate search for ready loads after first unresolved store, as no later load could be resolved in its presence */
			/* store? */ 
		if((MD_OP_FLAGS(LSQ[index].op) & (F_MEM|F_STORE)) == (F_MEM|F_STORE))
		{
			if(!STORE_ADDR_READY(&LSQ[index]) && !perfect_disambig)
			{
				/* FIXME: a later STD + STD known could hide the STA unknown */
				/* sta unknown, blocks all later loads, stop search */
				break;
			}
			else 
			if(!OPERANDS_READY(&LSQ[index]))
			{
				/* sta known, but std unknown, may block a later store, record this address for later referral, 
				 * we use an array here because for most simulations the number of entries to search will be very small */
				if (n_std_unknowns == MAX_STD_UNKNOWNS)
					fatal("STD unknown array overflow, increase MAX_STD_UNKNOWNS");
				std_unknowns[n_std_unknowns++] = LSQ[index].addr;
			}
			else /* STORE_ADDR_READY() && OPERANDS_READY() */
			{
				/* a later STD known hides an earlier STD unknown */
				for(j = 0; j < n_std_unknowns; j++)
				{
					if(std_unknowns[j] == /* STA/STD known */LSQ[index].addr)
						std_unknowns[j] = /* bogus addr */0;
				}
			}
		}
			/* load? */ 														/* queued? */ 			/* waiting? */ 		/* completed? */ 			/* regs ready? */
		if(((MD_OP_FLAGS(LSQ[index].op) & (F_MEM|F_LOAD)) == (F_MEM|F_LOAD)) && !LSQ[index].queued && !LSQ[index].issued && !LSQ[index].completed && OPERANDS_READY(&LSQ[index]))
		{
			/* no STA unknown conflict (because we got to this check), check for a STD unknown conflict */
			for(j = 0; j < n_std_unknowns; j++)
			{
				/* found a relevant STD unknown? */
				if(std_unknowns[j] == LSQ[index].addr)
				break;
			}
			if(j == n_std_unknowns)
			{
				/* no STA or STD unknown conflicts, put load on ready queue */
				readyq_enqueue(&LSQ[index]);
			}
		}
	}
}


/*  RUU_ISSUE() - issue instructions to functional units */

/* attempt to issue all operations in the ready queue; insts in the ready instruction queue have all register dependencies satisfied, this function must then 
 * 1) ensure the instructions memory dependencies have been satisfied (see lsq_refresh() for details on this process) and 
 * 2) a function unit is available in this cycle to commence execution of the operation; 
 * if all goes well, the function unit is allocated, a writeback event is scheduled, and the instruction begins execution */
static void
ruu_issue(void)
{
  int i, load_lat =0, d_longerlat = 0, tlb_lat, n_issued;
	 
	struct RS_link *node, *next_node;
	struct res_template *fu;

	/* FIXME: could be a little more efficient when scanning the ready queue */

	/* copy and then blow away the ready list, NOTE: the ready list is always totally reclaimed each cycle, 
	 * and instructions that are not issue are explicitly reinserted into the ready instruction queue, 
	 * this management strategy ensures that the ready instruction queue is always properly sorted */
	node = ready_queue;
	ready_queue = NULL;

	/* visit all ready instructions (i.e., insts whose register input dependencies have been satisfied, stop issue when no more instructions are available or issue bandwidth is exhausted */
	for(n_issued = 0; node && n_issued < ruu_issue_width; node = next_node)
	{
		next_node = node->next;

		/* still valid? */
		if(RSLINK_VALID(node))
		{
			struct RUU_station *rs = RSLINK_RS(node);

			/* issue operation, both reg and mem deps have been satisfied */
			if(!OPERANDS_READY(rs) || !rs->queued || rs->issued || rs->completed)
				panic("issued inst !ready, issued, or completed");

			/* node is now un-queued */
			rs->queued = FALSE;

#ifdef PANALYZER_H
#ifdef LV2_PANALYZER_H
#ifdef MEMORY_PANALYZER_H
     	 if (MD_OP_FLAGS(rs->op) != F_FCOMP)
			  sbank_panalyzer(irf_pspec, NULL, sim_cycle);
		 else
			 sbank_panalyzer(fprf_pspec, NULL, sim_cycle);
#endif
#endif
#endif
			if(rs->in_LSQ && ((MD_OP_FLAGS(rs->op) & (F_MEM|F_STORE)) == (F_MEM|F_STORE)))
			{
				/* stores complete in effectively zero time, result is written into the load/store queue, the actual store into the memory system occurs when the instruction is retired 
				 * (see ruu_commit()) */
				rs->issued = TRUE;
				rs->completed = TRUE;
				if(rs->onames[0] || rs->onames[1])
					panic("store creates result");

				if(rs->recover_inst)
					panic("mis-predicted store");

				/* entered execute stage, indicate in pipe trace */
				ptrace_newstage(rs->ptrace_seq, PST_WRITEBACK, 0,0,0);

				/* one more inst issued */
				n_issued++;
			}
			else
			{
				/* issue the instruction to a functional unit */
				if(MD_OP_FUCLASS(rs->op) != NA)
				{
					fu = res_get(fu_pool, MD_OP_FUCLASS(rs->op));
					if(fu)
					{
						/* got one! issue inst to functional unit */
						rs->issued = TRUE;

						/* reserve the functional unit */
						if(fu->master->busy)
							panic("functional unit already in use");

						/* schedule functional unit release event */
						fu->master->busy = fu->issuelat;

						/* schedule a result writeback event */
						if(rs->in_LSQ && ((MD_OP_FLAGS(rs->op) & (F_MEM|F_LOAD)) == (F_MEM|F_LOAD)))
						{
							int events = 0;

							/* for loads, determine cache access latency: first scan LSQ to see if a store forward is possible, if not, access the data cache */
							load_lat = 0;
							i = (rs - LSQ);
							if(i != LSQ_head)
							{
								for( ; ; )
								{
									/* go to next earlier LSQ entry */
									i = (i + (LSQ_size-1)) % LSQ_size;

									/* FIXME: not dealing with partials! */
									if((MD_OP_FLAGS(LSQ[i].op) & F_STORE) && (LSQ[i].addr == rs->addr))
									{
										/* hit in the LSQ */
										load_lat = 1;
										break;
									}

									/* scan finished? */
									if(i == LSQ_head)
										break;
								}
							}

							/* was the value store forwared from the LSQ? */
							if(!load_lat)
							{
								int valid_addr = MD_VALID_ADDR(rs->addr);

								if(!spec_mode && !valid_addr)
									sim_invalid_addrs++;

								/* no! go to the data cache if addr is valid */
								if(cache_dl1 && valid_addr)
								{ 
#ifdef VANALYZER_H
									byte_t *p;

									p = (byte_t *)malloc(sizeof(md_inst_t)); 
									/* access the cache if non-faulting */
									load_lat = cache_access(cache_dl1, Read, (rs->addr & ~3), p, 4, sim_cycle, NULL, NULL);

#ifdef PANALYZER_H
#ifdef LV1_PANALYZER_H
									lv1_panalyzer(lv1_dl1,(int) Read);
	
#endif		

#ifdef LV2_PANALYZER_H
#ifdef CACHE_PANALYZER_H
                                  cache_panalyzer(dl1_pspec, Read, (rs->addr & ~3), NULL, 4, sim_cycle, load_lat);
#endif /* CACHE_PANALYZER_H */
#endif							
#endif


									if(load_lat == cache_dl1_lat)
									{
									}
									free(p);
#else /* VANALYZER_H */
									/* access the cache if non-faulting */
									load_lat = cache_access(cache_dl1, Read, (rs->addr & ~3), NULL, 4, sim_cycle, NULL, NULL);
#endif
									if(load_lat > cache_dl1_lat)
										events |= PEV_CACHEMISS;
								}
								else
								{
									/* no caches defined, just use op latency */
									load_lat = fu->oplat;
								}
							}

							/* all loads and stores must to access D-TLB */
							if(dtlb && MD_VALID_ADDR(rs->addr))
							{
								/* access the D-DLB, NOTE: this code will initiate speculative TLB misses */
								tlb_lat = cache_access(dtlb, Read, (rs->addr & ~3), NULL, 4, sim_cycle, NULL, NULL);
#ifdef PANALYZER_H
#ifdef LV1_PANALYZER_H
									lv1_panalyzer(lv1_dtlb,(int) Read);
#endif
#ifdef LV2_PANALYZER_H
#ifdef CACHE_PANALYZER_H
	                                cache_panalyzer(dtlb_pspec, Read, (rs->addr &~3), NULL, 4, sim_cycle, tlb_lat);
#endif /* CACHE_PANALYZER_H */
#endif
#endif


								if(tlb_lat > 1)
									events |= PEV_TLBMISS;

								/* D-cache/D-TLB accesses occur in parallel */
								d_longerlat=(tlb_lat > load_lat ) ? PEV_TLBMISS : PEV_CACHEMISS;
								load_lat = MAX(tlb_lat, load_lat);
							}

							/* use computed cache access latency */
							eventq_queue_event(rs, sim_cycle + load_lat);

							/* entered execute stage, indicate in pipe trace */
							ptrace_newstage(rs->ptrace_seq, PST_EXECUTE, ((rs->ea_comp ? PEV_AGEN : 0) | events),load_lat, d_longerlat);
						}
						else /* !load && !store */
						{
#ifdef PANALYZER_H
#ifdef LV1_PANALYZER_H														
							/* */
							/*if(MD_OP_FUCLASS(rs->op) != 1) {
							fprintf(stderr,"fu class %d",MD_OP_FUCLASS(rs->op));
							md_print_uop(rs->op, rs->IR, rs->PC, stderr);
							getchar();	
							}*/
							switch (MD_OP_FUCLASS(rs->op)) 
							{
								case 1:
								/* integer add */
									lv1_panalyzer(lv1_alu,0);
									break;
								case 2:
								/* integer multiplier */
									lv1_panalyzer(lv1_mult,0);
									break;
								case 3:
									break;
								case 4:
								case 7:
								case 8:
									lv1_panalyzer(lv1_fpu,0);	
									break; 
								default:
									break;	
							}
						
#endif
						
#ifdef FP_PANALYZER_H														
							/* */
							/*if(MD_OP_FUCLASS(rs->op) != 1) {
							fprintf(stderr,"fu class %d",MD_OP_FUCLASS(rs->op));
							md_print_uop(rs->op, rs->IR, rs->PC, stderr);
							getchar();	
							}*/
							switch (MD_OP_FUCLASS(rs->op)) 
							{
								case 1:
								/* integer add */
									alu_panalyzer(alu_pspec,sim_cycle);
									break;
								case 2:
								/* integer multiplier */
									mult_panalyzer(mult_pspec,sim_cycle);
									break;
								case 3:
									break;
								case 4:
								case 7:
								case 8:
									fpu_panalyzer(fpu_pspec,sim_cycle);
									break; 
								default:
									break;	
							}
						
#endif

#endif														
							
							/* use deterministic functional unit latency */
							switch (MD_OPLAT (rs->op))
							  {
							  case -1:
							    eventq_queue_event(rs, sim_cycle + fu->oplat);
							    break;
							  default:
							    eventq_queue_event(rs, sim_cycle + MD_OPLAT (rs->op));
							  }

							/* entered execute stage, indicate in pipe trace */
							ptrace_newstage(rs->ptrace_seq, PST_EXECUTE, 
							rs->ea_comp ? PEV_AGEN : 0, 0, 0);
						} 
						/* one more inst issued */
						n_issued++;
					}
					else /* no functional unit */
					{
						/* insufficient functional unit resources, put operation back onto the ready list, we'll try to issue it again next cycle */
						readyq_enqueue(rs);
					}
				}
				else /* does not require a functional unit! */
				{
					/* FIXME: need better solution for these */
					/* the instruction does not need a functional unit */
					rs->issued = TRUE;

					/* schedule a result event */
					eventq_queue_event(rs, sim_cycle + 1);

					/* entered execute stage, indicate in pipe trace */
					ptrace_newstage(rs->ptrace_seq, PST_EXECUTE,
					rs->ea_comp ? PEV_AGEN : 0, 0, 0);

					/* one more inst issued */
					n_issued++;
				}
			} /* !store */

		}
		/* else, RUU entry was squashed */

		/* reclaim ready list entry, NOTE: this is done whether or not the instruction issued, since the instruction was once again reinserted into the ready queue if it did not issue, 
		 * this ensures that the ready queue is always properly sorted */
		RSLINK_FREE(node);
	}

	/* put any instruction not issued back into the ready queue, go through normal channels to ensure instruction stay ordered correctly */
	for ( ; node; node = next_node)
	{
		next_node = node->next;

		/* still valid? */
		if(RSLINK_VALID(node))
		{
			struct RUU_station *rs = RSLINK_RS(node);

			/* node is now un-queued */
			rs->queued = FALSE;

			/* not issued, put operation back onto the ready list, we'll try to issue it again next cycle */
			readyq_enqueue(rs);
		}
		/* else, RUU entry was squashed */

		/* reclaim ready list entry, NOTE: this is done whether or not the instruction issued, since the instruction was once again reinserted into the ready queue if it did not issue, 
		 * this ensures that the ready queue is always properly sorted */
		RSLINK_FREE(node);
	}
}


/* routines for generating on-the-fly instruction traces with support for control and data misspeculation modeling */

/* integer register file */
#define R_BMAP_SZ       (BITMAP_SIZE(MD_NUM_IREGS))
static BITMAP_TYPE(MD_NUM_IREGS, use_spec_R);
static md_gpr_t spec_regs_R;

/* floating point register file */
#define F_BMAP_SZ       (BITMAP_SIZE(MD_NUM_FREGS))
static BITMAP_TYPE(MD_NUM_FREGS, use_spec_F);
static md_fpr_t spec_regs_F;

/* miscellaneous registers */
#define C_BMAP_SZ       (BITMAP_SIZE(MD_NUM_CREGS))
static BITMAP_TYPE(MD_NUM_FREGS, use_spec_C);
static md_ctrl_t spec_regs_C;

/* dump speculative register state */
static void
rspec_dump(FILE *stream)			/* output stream */
{
	int i;

	if(!stream)
		stream = stderr;

	fprintf(stream, "** speculative register contents **\n");

	fprintf(stream, "spec_mode: %s\n", spec_mode ? "t" : "f");

	/* dump speculative integer regs */
	for(i = 0; i < MD_NUM_IREGS; i++)
	{
		if(BITMAP_SET_P(use_spec_R, R_BMAP_SZ, i))
		{
			md_print_ireg(spec_regs_R, i, stream);
			fprintf(stream, "\n");
		}
	}

	/* dump speculative FP regs */
	for(i = 0; i < MD_NUM_FREGS; i++)
	{
		if(BITMAP_SET_P(use_spec_F, F_BMAP_SZ, i))
		{
			md_print_fpreg(spec_regs_F, i, stream);
			fprintf(stream, "\n");
		}
	}

	/* dump speculative CTRL regs */
	for(i = 0; i < MD_NUM_CREGS; i++)
	{
		if (BITMAP_SET_P(use_spec_C, C_BMAP_SZ, i))
		{
			md_print_creg(spec_regs_C, i, stream);
			fprintf(stream, "\n");
		}
	}
}


/* speculative memory hash table size, NOTE: this must be a power-of-two */
#define STORE_HASH_SIZE		32

/* speculative memory hash table definition, accesses go through this hash table when accessing memory in speculative mode, 
 * the hash table flush the table when recovering from mispredicted branches */
struct spec_mem_ent {
	struct spec_mem_ent *next; /* ptr to next hash table bucket */
	md_addr_t addr;	/* virtual address of spec state */
	unsigned int data[2]; /* spec buffer, up to 8 bytes */
};

/* speculative memory hash table */
static struct spec_mem_ent *store_htable[STORE_HASH_SIZE];

/* speculative memory hash table bucket free list */
static struct spec_mem_ent *bucket_free_list = NULL;


/* program counter */
static md_addr_t pred_PC;
static md_addr_t recover_PC;

/* fetch unit next fetch address */
static md_addr_t fetch_regs_PC;
static md_addr_t fetch_pred_PC;

/* IFETCH -> DISPATCH instruction queue definition */
struct fetch_rec {
	md_inst_t IR; /* inst register */
	md_addr_t regs_PC, pred_PC;	/* current PC, predicted next PC */
	struct bpred_update_t dir_update; /* bpred direction update info */
	int stack_recover_idx; /* branch predictor RSB index */
	unsigned int ptrace_seq; /* print trace sequence id */
};

static struct fetch_rec *fetch_data; /* IFETCH -> DISPATCH inst queue */
static int fetch_num; /* num entries in IF -> DIS queue */
static int fetch_tail, fetch_head;	/* head and tail pointers of queue */

/* recover instruction trace generator state to precise state state immediately before the first mis-predicted branch; 
 * this is accomplished by resetting all register value copied-on-write bitmasks are reset, and the speculative memory hash table is cleared */
static void
tracer_recover(void)
{
	int i;
	struct spec_mem_ent *ent, *ent_next;

	/* better be in mis-speculative trace generation mode */
	if(!spec_mode)
		panic("cannot recover unless in speculative mode");

	if (verbose && sim_num_insn >= trigger_inst)
	  fprintf(stderr, "** exiting spec mode...\n");

	/* reset to non-speculative trace generation mode */
	spec_mode = FALSE;
	in_flow = FALSE;
	/* ADDED */
	jmp_flow = FALSE;

	/* reset copied-on-write register bitmasks back to non-speculative state */
	BITMAP_CLEAR_MAP(use_spec_R, R_BMAP_SZ);
	BITMAP_CLEAR_MAP(use_spec_F, F_BMAP_SZ);
	BITMAP_CLEAR_MAP(use_spec_C, C_BMAP_SZ);

	/* reset memory state back to non-speculative state */
	/* FIXME: could version stamps be used here?!?!? */
	for(i = 0; i < STORE_HASH_SIZE; i++)
	{
		/* release all hash table buckets */
		for(ent = store_htable[i]; ent; ent = ent_next)
		{
			ent_next = ent->next;
			ent->next = bucket_free_list;
			bucket_free_list = ent;
		}
		store_htable[i] = NULL;
	}

	/* if pipetracing, indicate squash of instructions in the inst fetch queue */
	if(ptrace_active)
	{
		while (fetch_num != 0)
		{
			/* squash the next instruction from the IFETCH -> DISPATCH queue */
			ptrace_endinst(fetch_data[fetch_head].ptrace_seq);

			/* consume instruction from IFETCH -> DISPATCH queue */
			fetch_head = (fetch_head+1) & (ruu_ifq_size - 1);
			fetch_num--;
		}
	}

	/* reset IFETCH state */
	fetch_num = 0;
	fetch_tail = fetch_head = 0;
	fetch_pred_PC = fetch_regs_PC = recover_PC;
}

/* initialize the speculative instruction state generator state */
static void
tracer_init(void)
{
	int i;

	/* initially in non-speculative mode */
	spec_mode = FALSE;

	/* register state is from non-speculative state buffers */
	BITMAP_CLEAR_MAP(use_spec_R, R_BMAP_SZ);
	BITMAP_CLEAR_MAP(use_spec_F, F_BMAP_SZ);
	BITMAP_CLEAR_MAP(use_spec_C, C_BMAP_SZ);

	/* memory state is from non-speculative memory pages */
	for(i = 0; i < STORE_HASH_SIZE; i++)
		store_htable[i] = NULL;
}


/* speculative memory hash table address hash function */
#define HASH_ADDR(ADDR)		((((ADDR) >> 24)^((ADDR) >> 16)^((ADDR) >> 8)^(ADDR)) & (STORE_HASH_SIZE-1))

/* this functional provides a layer of mis-speculated state over the non-speculative memory state, 
 * when in mis-speculation trace generation mode, the simulator will call this function to access memory, instead of the non-speculative memory access interfaces defined in memory.h; 
 * when storage is written, an entry is allocated in the speculative memory hash table, future reads and writes while in mis-speculative trace generation mode will access this buffer 
 * instead of non-speculative memory state; when the trace generator transitions back to non-speculative trace generation mode, tracer_recover() clears this table, returns any access fault */

/*
struct mem_t *mem,	: memory space to access 
enum mem_cmd cmd,	: Read or Write access cmd 
md_addr_t addr,		: virtual address of access 
void *p,			: input/output buffer 
int nbytes			: number of bytes to access 
*/
static enum md_fault_type
spec_mem_access(struct mem_t *mem, enum mem_cmd cmd, md_addr_t addr, void *p, int nbytes)			
{
		int i, index;
		struct spec_mem_ent *ent, *prev;

		/* FIXME: partially overlapping writes are not combined... */
		/* FIXME: partially overlapping reads are not handled correctly... */

		/* check alignments, even speculative this test should always pass */
		if((nbytes & (nbytes-1)) != 0 || (addr & (nbytes-1)) != 0)
		{
			/* no can do, return zero result */
			for(i = 0; i < nbytes; i++)
			 ((char *)p)[i] = 0;

			return md_fault_none;
		}

		/* check permissions */
		if(!((addr >= ld_text_base && addr < (ld_text_base+ld_text_size) && cmd == Read) || MD_VALID_ADDR(addr)))
		{
			/* no can do, return zero result */
			for(i = 0; i < nbytes; i++)
				((char *)p)[i] = 0;

			return md_fault_none;
		}

		/* has this memory state been copied on mis-speculative write? */
		index = HASH_ADDR(addr);
		for(prev = NULL, ent = store_htable[index]; ent; prev = ent,ent=ent->next)
		{
			if(ent->addr == addr)
			{
				/* reorder chains to speed access into hash table */
				if(prev != NULL)
				{
					/* not at head of list, relink the hash table entry at front */
					prev->next = ent->next;
					ent->next = store_htable[index];
					store_htable[index] = ent;
				}
				break;
			}
		}

		/* no, if it is a write, allocate a hash table entry to hold the data */
		if(!ent && cmd == Write)
		{
			/* try to get an entry from the free list, if available */
			if(!bucket_free_list)
			{
				/* otherwise, call calloc() to get the needed storage */
				bucket_free_list = calloc(1, sizeof(struct spec_mem_ent));
				if(!bucket_free_list)
					fatal("out of virtual memory");
			}
			ent = bucket_free_list;
			bucket_free_list = bucket_free_list->next;

			if(!bugcompat_mode)
			{
				/* insert into hash table */
				ent->next = store_htable[index];
				store_htable[index] = ent;
				ent->addr = addr;
				ent->data[0] = 0; ent->data[1] = 0;
			}
		}

		/* handle the read or write to speculative or non-speculative storage */
		switch (nbytes)
		{
			case 1:
			if(cmd == Read)
			{
				if(ent)
				{
					/* read from mis-speculated state buffer */
					*((byte_t *)p) = *((byte_t *)(&ent->data[0]));
				}
				else
				{
					/* read from non-speculative memory state, don't allocate memory pages with speculative loads */
					*((byte_t *)p) = MEM_READ_BYTE(mem, addr);
				}
			}
			else
			{
				/* always write into mis-speculated state buffer */
				*((byte_t *)(&ent->data[0])) = *((byte_t *)p);
			}
			break;
			case 2:
			if(cmd == Read)
			{
				if(ent)
				{
					/* read from mis-speculated state buffer */
					*((half_t *)p) = *((half_t *)(&ent->data[0]));
				}
				else
				{
					/* read from non-speculative memory state, don't allocate memory pages with speculative loads */
					*((half_t *)p) = MEM_READ_HALF(mem, addr);
				}
				}
				else
				{
					/* always write into mis-speculated state buffer */
					*((half_t *)&ent->data[0]) = *((half_t *)p);
				}
				break;
			case 4:
			if(cmd == Read)
			{
				if(ent)
				{
					/* read from mis-speculated state buffer */
					*((word_t *)p) = *((word_t *)&ent->data[0]);
				}
				else
				{
					/* read from non-speculative memory state, don't allocate memory pages with speculative loads */
					*((word_t *)p) = MEM_READ_WORD(mem, addr);
				}
			}
			else
			{
				/* always write into mis-speculated state buffer */
				*((word_t *)&ent->data[0]) = *((word_t *)p);
			}
			break;
			case 8:
			if(cmd == Read)
			{
				if(ent)
				{
					/* read from mis-speculated state buffer */
					*((word_t *)p) = *((word_t *)&ent->data[0]);
					*(((word_t *)p)+1) = *((word_t *)&ent->data[1]);
				}
				else
				{
					/* read from non-speculative memory state, don't allocate memory pages with speculative loads */
					*((word_t *)p) = MEM_READ_WORD(mem, addr);
					*(((word_t *)p)+1) =
					MEM_READ_WORD(mem, addr + sizeof(word_t));
				}
			}
			else
			{
				/* always write into mis-speculated state buffer */
				*((word_t *)&ent->data[0]) = *((word_t *)p);
				*((word_t *)&ent->data[1]) = *(((word_t *)p)+1);
			}
			break;
			default:
					panic("access size not supported in mis-speculative mode");
		}
  return md_fault_none;
}

static inline enum md_fault_type
sim_mem_access(struct mem_t *mem,		/* memory space to access */
	       enum mem_cmd cmd,		/* Read or Write access cmd */
	       md_addr_t addr,			/* virtual address of access */
	       void *p,				/* input/output buffer */
	       int nbytes)			/* number of bytes to access */
{
  if (spec_mode)
    return spec_mem_access(mem, cmd, addr, p, nbytes);
  else
    return mem_access(mem, cmd, addr, p, nbytes);
}

/* dump speculative memory state */
static void
mspec_dump(FILE *stream)			/* output stream */
{
	int i;
	struct spec_mem_ent *ent;

	if(!stream)
		stream = stderr;

	fprintf(stream, "** speculative memory contents **\n");
	fprintf(stream, "spec_mode: %s\n", spec_mode ? "t" : "f");

	for(i = 0; i < STORE_HASH_SIZE; i++)
	{
		/* dump contents of all hash table buckets */
		for(ent=store_htable[i]; ent; ent=ent->next)
		{
			myfprintf(stream, "[0x%08p]: %12.0f/0x%08x:%08x\n", ent->addr, (double)(*((double *)ent->data)), *((unsigned int *)&ent->data[0]), *(((unsigned int *)&ent->data[0]) + 1));
		}
	}
}

/* default memory state accessor, used by DLite */
/*
struct mem_t *mem,	: memory space to access 
int is_write,		: access type
md_addr_t addr,		: address to access
char *p,			: input/output buffer
int nbytes			: size of access
*/
static char *					/* err str, NULL for no err */
simoo_mem_obj(struct mem_t *mem, int is_write, md_addr_t addr, char *p, int nbytes)	
{
	enum mem_cmd cmd;

	if(!is_write)
		cmd = Read;
	else
		cmd = Write;

	/* else, no error, access memory */
	sim_mem_access(mem, cmd, addr, p, nbytes);

	/* no error */
	return NULL;
}


/* RUU_DISPATCH() - decode instructions and allocate RUU and LSQ resources */

/* link RS onto the output chain number of whichever operation will next create the architected register value IDEP_NAME */
/*
struct RUU_station *rs,	: rs station to link 
int idep_num,			: input dependence number
int idep_name			: input register name
*/
static INLINE void
ruu_link_idep(struct RUU_station *rs, int idep_num, int idep_name)
{
	struct CV_link head;
	struct RS_link *link;

	/* any dependence? */
	if(idep_name == NA)
	{
		/* no input dependence for this input slot, mark operand as ready */
		rs->idep_ready[idep_num] = TRUE;
		return;
	}

	/* locate creator of operand */
	head = CREATE_VECTOR(idep_name);

	/* any creator? */
	if(!head.rs)
	{
		/* no active creator, use value available in architected reg file, indicate the operand is ready for use */
		rs->idep_ready[idep_num] = TRUE;

		return;
	}
	/* else, creator operation will make this value sometime in the future */

	/* indicate value will be created sometime in the future, i.e., operand is not yet ready for use */
	rs->idep_ready[idep_num] = FALSE;

	/* link onto creator's output list of dependant operand */
	RSLINK_NEW(link, rs); link->x.opnum = idep_num;
	link->next = head.rs->odep_list[head.odep_num];
	head.rs->odep_list[head.odep_num] = link;
}

/* make RS the creator of architected register ODEP_NAME */
/*
struct RUU_station *rs,	: creating RUU station 
int odep_num,			: output operand number 
int odep_name			: output register name 
*/
static INLINE void
ruu_install_odep(struct RUU_station *rs, int odep_num, int odep_name)
{
	struct CV_link cv;

	/* any dependence? */
	if(odep_name == NA)
	{
		/* no value created */
		rs->onames[odep_num] = NA;
		return;
	}
	/* else, create a RS_NULL terminated output chain in create vector */

	/* record output name, used to update create vector at completion */
	rs->onames[odep_num] = odep_name;

	/* initialize output chain to empty list */
	rs->odep_list[odep_num] = NULL;

	/* indicate this operation is latest creator of ODEP_NAME */
	CVLINK_INIT(cv, rs, odep_num);
	SET_CREATE_VECTOR(odep_name, cv);
}


/* configure the instruction decode engine */

#define DNA			(0)

#if defined(TARGET_PISA)

/* general register dependence decoders */
#define DGPR(N)			((N))
#define DGPR_D(N)		((N) &~1)

/* floating point register dependence decoders */
#define DFPR_L(N)		(((N)+32)&~1)
#define DFPR_F(N)		(((N)+32)&~1)
#define DFPR_D(N)		(((N)+32)&~1)

/* miscellaneous register dependence decoders */
#define DHI				(0+32+32)
#define DLO				(1+32+32)
#define DFCC			(2+32+32)
#define DTMP			(3+32+32)

#elif defined(TARGET_ALPHA)

/* general register dependence decoders, $r31 maps to DNA (0) */
#define DGPR(N)			(31 - (N)) /* was: (((N) == 31) ? DNA : (N)) */

/* floating point register dependence decoders */
#define DFPR(N)			(((N) == 31) ? DNA : ((N)+32))

/* miscellaneous register dependence decoders */
#define DFPCR			(0+32+32)
#define DUNIQ			(1+32+32)
#define DTMP			(2+32+32)

#elif defined(TARGET_ARM)

/* general register dependence decoders */
#define DGPR(N)			((N)+1)

/* floating point register dependence decoders */
#define DFPR(N)			(((N)+18+1))

/* miscellaneous register dependence decoders */
#define DPSR			(0+26+1)
#define DFPSR			(1+26+1)

#else
#error No ISA target defined...
#endif


/* configure the execution engine */

/* next program counter */
#define SET_NPC(EXPR)		(regs.regs_NPC = (EXPR))

/* target program counter */
#undef  SET_TPC
#define SET_TPC(EXPR)		(target_PC = (EXPR))

/* current program counter */
#define CPC					(regs.regs_PC)

/* general purpose register accessors, NOTE: speculative copy on write storage provided for fast recovery during wrong path execute (see tracer_recover() for details on this process */
#define GPR(N)				(BITMAP_SET_P(use_spec_R, R_BMAP_SZ, (N)) ? spec_regs_R[N] : regs.regs_R[N])
#define SET_GPR(N,EXPR)		((void)(((N) == 15) ? setPC++ : 0),	spec_mode ? ((spec_regs_R[N] = (EXPR)), BITMAP_SET(use_spec_R, R_BMAP_SZ, (N)), spec_regs_R[N]) : (regs.regs_R[N] = (EXPR)))

#if defined(TARGET_PISA)

/* floating point register accessors, NOTE: speculative copy on write storage provided for fast recovery during wrong path execute (see tracer_recover() for details on this process */
#define FPR_L(N)			(BITMAP_SET_P(use_spec_F, F_BMAP_SZ, ((N)&~1)) ? spec_regs_F.l[(N)] : regs.regs_F.l[(N)])
#define SET_FPR_L(N,EXPR)	(spec_mode ? ((spec_regs_F.l[(N)] = (EXPR)), BITMAP_SET(use_spec_F,F_BMAP_SZ,((N)&~1)), spec_regs_F.l[(N)]) : (regs.regs_F.l[(N)] = (EXPR)))
#define FPR_F(N)			(BITMAP_SET_P(use_spec_F, F_BMAP_SZ, ((N)&~1)) ? spec_regs_F.f[(N)] : regs.regs_F.f[(N)])
#define SET_FPR_F(N,EXPR)	(spec_mode ? ((spec_regs_F.f[(N)] = (EXPR)), BITMAP_SET(use_spec_F,F_BMAP_SZ,((N)&~1)), spec_regs_F.f[(N)]) : (regs.regs_F.f[(N)] = (EXPR)))
#define FPR_D(N)			(BITMAP_SET_P(use_spec_F, F_BMAP_SZ, ((N)&~1)) ? spec_regs_F.d[(N) >> 1] : regs.regs_F.d[(N) >> 1])
#define SET_FPR_D(N,EXPR)	(spec_mode ? ((spec_regs_F.d[(N) >> 1] = (EXPR)), BITMAP_SET(use_spec_F,F_BMAP_SZ,((N)&~1)), spec_regs_F.d[(N) >> 1]) : (regs.regs_F.d[(N) >> 1] = (EXPR)))

/* miscellanous register accessors, NOTE: speculative copy on write storage provided for fast recovery during wrong path execute (see tracer_recover() for details on this process */
#define HI					(BITMAP_SET_P(use_spec_C, C_BMAP_SZ, /*hi*/0) ? spec_regs_C.hi : regs.regs_C.hi)
#define SET_HI(EXPR)		(spec_mode ? ((spec_regs_C.hi = (EXPR)), BITMAP_SET(use_spec_C, C_BMAP_SZ,/*hi*/0), spec_regs_C.hi) : (regs.regs_C.hi = (EXPR)))
#define LO					(BITMAP_SET_P(use_spec_C, C_BMAP_SZ, /*lo*/1) ? spec_regs_C.lo : regs.regs_C.lo)
#define SET_LO(EXPR)		(spec_mode ? ((spec_regs_C.lo = (EXPR)), BITMAP_SET(use_spec_C, C_BMAP_SZ,/*lo*/1), spec_regs_C.lo) : (regs.regs_C.lo = (EXPR)))
#define FCC					(BITMAP_SET_P(use_spec_C, C_BMAP_SZ,/*fcc*/2) ? spec_regs_C.fcc	: regs.regs_C.fcc)
#define SET_FCC(EXPR)		(spec_mode ? ((spec_regs_C.fcc = (EXPR)), BITMAP_SET(use_spec_C,C_BMAP_SZ,/*fcc*/1), spec_regs_C.fcc) : (regs.regs_C.fcc = (EXPR)))

#elif defined(TARGET_ALPHA)

/* floating point register accessors, NOTE: speculative copy on write storage provided for fast recovery during wrong path execute (see tracer_recover() for details on this process */
#define FPR_Q(N)			(BITMAP_SET_P(use_spec_F, F_BMAP_SZ, (N)) ? spec_regs_F.q[(N)] : regs.regs_F.q[(N)]) 
#define SET_FPR_Q(N,EXPR)	(spec_mode ? ((spec_regs_F.q[(N)] = (EXPR)), BITMAP_SET(use_spec_F,F_BMAP_SZ, (N)), spec_regs_F.q[(N)]) : (regs.regs_F.q[(N)] = (EXPR)))
#define FPR(N)				(BITMAP_SET_P(use_spec_F, F_BMAP_SZ, (N)) ? spec_regs_F.d[(N)] : regs.regs_F.d[(N)])
#define SET_FPR(N,EXPR)		(spec_mode ? ((spec_regs_F.d[(N)] = (EXPR)), BITMAP_SET(use_spec_F,F_BMAP_SZ, (N)), spec_regs_F.d[(N)]) : (regs.regs_F.d[(N)] = (EXPR)))

/* miscellanous register accessors, NOTE: speculative copy on write storage provided for fast recovery during wrong path execute (see tracer_recover() for details on this process */
#define FPCR				(BITMAP_SET_P(use_spec_C, C_BMAP_SZ,/*fpcr*/0) ? spec_regs_C.fpcr : regs.regs_C.fpcr)
#define SET_FPCR(EXPR)		(spec_mode ? ((spec_regs_C.fpcr = (EXPR)), BITMAP_SET(use_spec_C,C_BMAP_SZ,/*fpcr*/0), spec_regs_C.fpcr) : (regs.regs_C.fpcr = (EXPR)))
#define UNIQ				(BITMAP_SET_P(use_spec_C, C_BMAP_SZ,/*uniq*/1) ? spec_regs_C.uniq : regs.regs_C.uniq)
#define SET_UNIQ(EXPR)		(spec_mode ? ((spec_regs_C.uniq = (EXPR)), BITMAP_SET(use_spec_C,C_BMAP_SZ,/*uniq*/1), spec_regs_C.uniq) : (regs.regs_C.uniq = (EXPR)))
#define FCC					(BITMAP_SET_P(use_spec_C, C_BMAP_SZ,/*fcc*/2) ? spec_regs_C.fcc : regs.regs_C.fcc)
#define SET_FCC(EXPR)		(spec_mode ? ((spec_regs_C.fcc = (EXPR)), BITMAP_SET(use_spec_C,C_BMAP_SZ,/*fcc*/1), spec_regs_C.fcc) : (regs.regs_C.fcc = (EXPR)))
#elif defined(TARGET_ARM)

/* processor status register */
#define PSR					(BITMAP_SET_P(use_spec_C, C_BMAP_SZ,/*cpsr*/0) ? spec_regs_C.cpsr : regs.regs_C.cpsr)
#define SET_PSR(EXPR)		(spec_mode	? ((spec_regs_C.cpsr = (EXPR)),	BITMAP_SET(use_spec_C,C_BMAP_SZ,/*cpsr*/0), spec_regs_C.cpsr) : (regs.regs_C.cpsr = (EXPR)))

#define PSR_N				(BITMAP_SET_P(use_spec_C, C_BMAP_SZ,/*cpsr*/0) ? _PSR_N(spec_regs_C.cpsr) : _PSR_N(regs.regs_C.cpsr))
#define SET_PSR_N(EXPR)		(spec_mode ? ((_SET_PSR_N(spec_regs_C.cpsr, (EXPR))), BITMAP_SET(use_spec_C,C_BMAP_SZ,/*cpsr*/0), _PSR_N(spec_regs_C.cpsr))	: (_SET_PSR_N(regs.regs_C.cpsr, (EXPR))))

#define PSR_C				(BITMAP_SET_P(use_spec_C, C_BMAP_SZ,/*cpsr*/0) ? _PSR_C(spec_regs_C.cpsr) : _PSR_C(regs.regs_C.cpsr))
#define SET_PSR_C(EXPR)		(spec_mode	? ((_SET_PSR_C(spec_regs_C.cpsr, (EXPR))), BITMAP_SET(use_spec_C,C_BMAP_SZ,/*cpsr*/0), _PSR_C(spec_regs_C.cpsr)) : (_SET_PSR_C(regs.regs_C.cpsr, (EXPR))))

#define PSR_Z				(BITMAP_SET_P(use_spec_C, C_BMAP_SZ,/*cpsr*/0) ? _PSR_Z(spec_regs_C.cpsr) : _PSR_Z(regs.regs_C.cpsr))
#define SET_PSR_Z(EXPR)		(spec_mode	? ((_SET_PSR_Z(spec_regs_C.cpsr, (EXPR))), BITMAP_SET(use_spec_C,C_BMAP_SZ,/*cpsr*/0), _PSR_Z(spec_regs_C.cpsr)) : (_SET_PSR_Z(regs.regs_C.cpsr, (EXPR))))

#define PSR_V				(BITMAP_SET_P(use_spec_C, C_BMAP_SZ,/*cpsr*/0) ? _PSR_V(spec_regs_C.cpsr) : _PSR_V(regs.regs_C.cpsr))
#define SET_PSR_V(EXPR)		(spec_mode	? ((_SET_PSR_V(spec_regs_C.cpsr, (EXPR))), BITMAP_SET(use_spec_C,C_BMAP_SZ,/*cpsr*/0), _PSR_V(spec_regs_C.cpsr)) : (_SET_PSR_V(regs.regs_C.cpsr, (EXPR))))

#define FPSR				(BITMAP_SET_P(use_spec_C, C_BMAP_SZ,/*fpsr*/1) ? spec_regs_C.fpsr : regs.regs_C.fpsr)
#define SET_FPSR(EXPR)		(spec_mode ? ((spec_regs_C.fpsr = (EXPR)),	BITMAP_SET(use_spec_C,C_BMAP_SZ,/*fpsr*/1), spec_regs_C.fpsr) : (regs.regs_C.fpsr = (EXPR)))

/* floating point conversions */
union x { float f; word_t w; };

#ifdef _ALPHA_
word_t DTOW( float f)
{ union x fw; fw.f = f; return fw.w; }
float WTOD( word_t w)
{ union x fw; fw.w = w; return fw.f; }
#else
#define DTOW(D)				({ union x fw; fw.f = (float)(D); fw.w; })
#define WTOD(W)				({ union x fw; fw.w = (W); (double)fw.f; })
#endif

#define QSWP(Q)				((((Q) << 32) & ULL(0xffffffff00000000)) | (((Q) >> 32) & ULL(0xffffffff)))

/* floating point registers, L->word, F->single-prec, D->double-prec */
#define FPR_Q(N)			(BITMAP_SET_P(use_spec_F, F_BMAP_SZ, (N)) ? QSWP(spec_regs_F.q[(N)]) : QSWP(regs.regs_F.q[(N)]))
#define SET_FPR_Q(N,EXPR)	(spec_mode ? ((spec_regs_F.q[(N)] = QSWP(EXPR)), BITMAP_SET(use_spec_F,F_BMAP_SZ, (N)), spec_regs_F.q[(N)]) : (regs.regs_F.q[(N)] = QSWP(EXPR)))

#define FPR_W(N)			(BITMAP_SET_P(use_spec_F, F_BMAP_SZ, (N)) ? DTOW(spec_regs_F.d[(N)]) : DTOW(regs.regs_F.d[(N)]))
#define SET_FPR_W(N,EXPR)	(spec_mode	? ((spec_regs_F.d[(N)] = WTOD(EXPR)), BITMAP_SET(use_spec_F,F_BMAP_SZ, (N)), DTOW(spec_regs_F.d[(N)])) : (regs.regs_F.d[(N)] = WTOD(EXPR)))

#define FPR(N)				(BITMAP_SET_P(use_spec_F, F_BMAP_SZ, (N)) ? spec_regs_F.d[(N)] : regs.regs_F.d[(N)])
#define SET_FPR(N,EXPR)		(spec_mode	? ((spec_regs_F.d[(N)] = (EXPR)), BITMAP_SET(use_spec_F,F_BMAP_SZ, (N)), spec_regs_F.d[(N)]) : (regs.regs_F.d[(N)] = (EXPR)))

#else
#error No ISA target defined...
#endif

/* precise architected memory state accessor macros, NOTE: speculative copy on write storage provided for fast recovery during wrong path execute 
 * (see tracer_recover() for details on this process */
 #define __READ_SPECMEM(SRC, SRC_V, FAULT)				\
   (addr = (SRC),							\
   ((FAULT) = sim_mem_access(mem, Read, addr, &SRC_V, sizeof(SRC_V))),	\
    SRC_V)

#define READ_BYTE(SRC, FAULT) 	__READ_SPECMEM((SRC), temp_byte, (FAULT))
#define READ_HALF(SRC, FAULT) 	MD_SWAPH(__READ_SPECMEM((SRC), temp_half, (FAULT)))
#define READ_WORD(SRC, FAULT) 	MD_SWAPW(__READ_SPECMEM((SRC), temp_word, (FAULT)))
#ifdef HOST_HAS_QWORD
#define READ_QWORD(SRC, FAULT)	MD_SWAPQ(__READ_SPECMEM((SRC), temp_qword, (FAULT)))
#endif /* HOST_HAS_QWORD */


#define __WRITE_SPECMEM(SRC, DST, DST_V, FAULT)				\
  (DST_V = (SRC), addr = (DST),						\
   ((FAULT) = sim_mem_access(mem, Write, addr, &DST_V, sizeof(DST_V))))

#define WRITE_BYTE(SRC, DST, FAULT)		__WRITE_SPECMEM((SRC), (DST), temp_byte, (FAULT))
#define WRITE_HALF(SRC, DST, FAULT)		__WRITE_SPECMEM(MD_SWAPH(SRC), (DST), temp_half, (FAULT))
#define WRITE_WORD(SRC, DST, FAULT)		__WRITE_SPECMEM(MD_SWAPW(SRC), (DST), temp_word, (FAULT))
#ifdef HOST_HAS_QWORD
#define WRITE_QWORD(SRC, DST, FAULT) 	__WRITE_SPECMEM(MD_SWAPQ(SRC), (DST), temp_qword, (FAULT))
#endif /* HOST_HAS_QWORD */

/* system call handler macro */
#define SYSCALL(INST) (/* only execute system calls in non-speculative mode */ (spec_mode ? panic("speculative syscall") : (void) 0), sys_syscall(&regs, mem_access, mem, INST, TRUE))

/* default register state accessor, used by DLite */
/*
struct regs_t *xregs		: registers to access 
int is_write				: access type 
enum md_reg_type rt			: reg bank to probe 
int reg						: register number 
struct eval_value_t *val	: input, output 
*/
static char * /* err str, NULL for no err */
simoo_reg_obj(struct regs_t *xregs, int is_write, enum md_reg_type rt, int reg, struct eval_value_t *val)
{
	int setPC = 0;

	switch(rt)
	{
		case rt_gpr:
		if(reg < 0 || reg >= MD_NUM_IREGS)
			return "register number out of range";

		if(!is_write)
		{
			val->type = et_uint;
			val->value.as_uint = GPR(reg);
		}
		else
			SET_GPR(reg, eval_as_uint(*val));
		break;

		case rt_lpr:
		if(reg < 0 || reg >= MD_NUM_FREGS)
			return "register number out of range";

		/* FIXME: this is not portable... */
		abort();
		break;

		case rt_fpr:
		/* FIXME: this is not portable... */
		abort();
		break;

		case rt_dpr:
		/* FIXME: this is not portable... */
		abort();
		break;

		/* FIXME: this is not portable... */

		case rt_PC:
		if(!is_write)
		{
			val->type = et_addr;
			val->value.as_addr = regs.regs_PC;
		}
		else
			regs.regs_PC = eval_as_addr(*val);
		break;

		case rt_NPC:
		if(!is_write)
		{
			val->type = et_addr;
			val->value.as_addr = regs.regs_NPC;
		}
		else
			regs.regs_NPC = eval_as_addr(*val);
		break;

		default:
			panic("bogus register bank");
	}

	/* no error */
	return NULL;
}

/* the last operation that ruu_dispatch() attempted to dispatch, for implementing in-order issue */
static struct RS_link last_op = RSLINK_NULL_DATA;

/* dispatch instructions from the IFETCH -> DISPATCH queue: 
 * instructions are first decoded, then they allocated RUU (and LSQ for load/stores) resources and input and output dependence chains are updated accordingly */

static void
ruu_dispatch(void)
{
	int i;
	int n_dispatched; /* total insts dispatched */
	static md_inst_t inst; /* actual instruction bits */
	enum md_opcode op; /* decoded opcode enum */
	int in1, in2, in3, in4; /* input register names */
	int out1, out2, out3; /* output register names */
	md_addr_t target_PC; /* actual next/target PC address */
	// md_addr_t addr;	/* effective address, if load/store */
	md_addr_t addr = 0;	/* effective address, if load/store */
	struct RUU_station *rs;	/* RUU station being allocated */
	struct RUU_station *lsq; /* LSQ station for ld/st's */
	static struct bpred_update_t *dir_update_ptr;
	/* branch predictor dir update ptr */
	static int stack_recover_idx; /* bpred retstack recovery index */
	static unsigned int pseq; /* pipetrace sequence number */
// 	int is_write; /* store? */
	int is_write = FALSE; /* store? */
	int made_check; /* used to ensure DLite entry */
	int br_taken, br_pred_taken; /* if br, taken?  predicted taken? */
	int fetch_redirected = FALSE;
	byte_t temp_byte = 0; /* temp variable for spec mem access */
	half_t temp_half = 0; /* " ditto " */
	word_t temp_word = 0; /* " ditto " */
#ifdef HOST_HAS_QWORD
	qword_t temp_qword = 0; /* " ditto " */
#endif /* HOST_HAS_QWORD */
	enum md_fault_type fault;

	int setPC, cond_valid;

	made_check = FALSE;
	n_dispatched = 0;
			/* instruction decode B/W left? */ 				/* RUU and LSQ not full? */ 		/* insts still available from fetch unit? */ /* on an acceptable trace path */
	while(n_dispatched < (ruu_decode_width * fetch_speed) && RUU_num < RUU_size && LSQ_num < LSQ_size && fetch_num != 0 && (ruu_include_spec || !spec_mode))
	{
#ifdef PANALYZER_H
#ifdef LV2_PANALYZER_H
#ifdef LOGIC_PANALYZER_H
		     //   logic_panalyzer(dispatch_pspec, sim_cycle);
		        logic_panalyzer(logic_pspec, sim_cycle);
#endif /* LOGIC_PANALYZER_H */
#endif
#endif
		
		
		/* if issuing in-order, block until last op issues if inorder issue */
		if(ruu_inorder_issue && (last_op.rs && RSLINK_VALID(&last_op) && !OPERANDS_READY(last_op.rs)))
		{
			/* stall until last operation is ready to issue */
			break;
		}

		if(!in_flow)
		{
			/* keep an instruction count */

			if(!spec_mode)
				sim_num_insn++;

			/* get the next instruction from the IFETCH -> DISPATCH queue */
			inst = fetch_data[fetch_head].IR;
			regs.regs_PC = fetch_data[fetch_head].regs_PC;
			pred_PC = fetch_data[fetch_head].pred_PC;
			dir_update_ptr = &(fetch_data[fetch_head].dir_update);
			stack_recover_idx = fetch_data[fetch_head].stack_recover_idx;
			pseq = fetch_data[fetch_head].ptrace_seq;

			/* decode the instruction */
			MD_SET_OPCODE(op, inst);
			if(!spec_mode && op == NA)
				panic("bogus instruction");

		    if (trace_me && !spec_mode)
		    {
			   if(!(MD_OP_FLAGS(op) & F_TRAP) || RUU_num == 0)
			   {
				    int teller;
				    for(teller =0; teller <16; teller++)
				     fprintf(tracefile,"%x,",regs.regs_R[teller]);
				     fprintf(tracefile,"\n"); 
				     fprintf(tracefile,"%08x ",regs.regs_PC);
				     md_print_insn(inst, regs.regs_PC,tracefile);
				     fprintf(tracefile,"\n"); 
			   }
		    }
		#if 0 /* Disabled this since I'm not sure if this is the behaviour most people want */
		      /* keep an instruction count, Ludo: I noticed that instruction count was differs from real execution because TRAP's were counted multiple times as long as the pipeline is not empty */
		    if (!spec_mode && !(MD_OP_FLAGS(op) & F_TRAP && RUU_num != 0))
			   sim_num_insn++;
		#endif
		


			/* compute default next PC */
			regs.regs_NPC = regs.regs_PC + sizeof(md_inst_t);

			/* drain RUU for TRAPs and system calls */
			if(MD_OP_FLAGS(op) & F_TRAP)
			{
				if(RUU_num != 0)
					break;

				/* else, syscall is only instruction in the machine, at this point we should not be in (mis-)speculative mode */
				if(spec_mode)
					panic("drained and speculative");
			}

			/* one more complete instruction issued */
			n_dispatched++;
			/* whenever dispatched register file accessed */
#ifdef PANALYZER_H
#ifdef LV1_PANALYZER_H
			lv1_panalyzer(lv1_rf,0);
#endif						
#endif									

			if(MD_OP_FLAGS(op) & F_CISC)
			{
				/* get instruction flow */
				/* DAN MODIFIED */
				ptrace_newstage(pseq, PST_DISPATCH, 0,0,0);
				ptrace_endinst(pseq);  /* this baby is done */

				nflow = md_get_flow(op, inst, flowtab);
				if(nflow > 0)
				{
					in_flow = TRUE;
					jmp_flow = FALSE;
					flow_index = 0;
				}
				else
					fatal("could not locate CISC flow");
				sim_num_uops += nflow;
			}
			else
				sim_num_uops++;
		}
		if(in_flow)
		{
			op = flowtab[flow_index].op;
			if(!spec_mode && op == NA)
				panic("bogus instruction");
			inst = flowtab[flow_index++].inst;
			if(flow_index == nflow)
				in_flow = FALSE;

			pseq = ptrace_seq++;
			ptrace_newuop(pseq,inst,op,regs.regs_PC,0);

			/* DAN             need to add sequence numbers here!!*/
			/*                 Also, find where the ptrace is updated in this stage */
		}

		/* maintain $r0 semantics (in spec and non-spec space) */
#ifndef TARGET_ARM
		regs.regs_R[MD_REG_ZERO] = 0; spec_regs_R[MD_REG_ZERO] = 0;
#endif /* !TARGET_ARM */
#ifdef TARGET_ALPHA
		regs.regs_F.d[MD_REG_ZERO] = 0.0; spec_regs_F.d[MD_REG_ZERO] = 0.0;
#endif /* TARGET_ALPHA */

		/* default effective address (none) and access */
		addr = 0; is_write = FALSE;

		/* set default fault - none */
		fault = md_fault_none;


		/* more decoding and execution */
		setPC = 0;
		if(!spec_mode)
			regs.regs_R[MD_REG_PC] = regs.regs_PC;
		cond_valid = (((COND) == COND_AL) || md_cond_ok(inst, PSR));

		switch(op)
		{
#define DEFINST(OP,MSK,NAME,OPFORM,RES,CLASS,O1,O2,O3,I1,I2,I3,I4)		\
			case OP:													\
			/* compute output/input dependencies to out1-2 and in1-3 */	\
			out1 = O1; out2 = O2; out3 = O3;							\
			in1 = I1; in2 = I2; in3 = I3; in4 = I4;						\
			/* execute the instruction */								\
			SYMCAT(OP,_IMPL);											\
			break;
#define DEFUOP(OP,NAME,OPFORM,RES,CLASS,O1,O2,O3,I1,I2,I3,I4)			\
			case OP:													\
			/* compute output/input dependencies to out1-2 and in1-3 */	\
			out1 = O1; out2 = O2; out3 = O3;							\
			in1 = I1; in2 = I2; in3 = I3; in4 = I4;						\
			/* execute the instruction */								\
			SYMCAT(OP,_IMPL);											\
			break;
#define DEFLINK(OP,MSK,NAME,MASK,SHIFT)									\
			case OP:													\
			/* could speculatively decode a bogus inst, convert to NOP*/\
			op = MD_NOP_OP;												\
			/* compute output/input dependencies to out1-2 and in1-3 */	\
			out1 = NA; out2 = NA;	out3 = NA;							\
			in1 = NA; in2 = NA; in3 = NA; in4 = NA;						\
			/* no EXPR */												\
			break;
#define CONNECT(OP)	/* nada... */
		/* the following macro wraps the instruction fault declaration macro with a test to see if the trace generator is in non-speculative mode, 
		 * if so the instruction fault is declared, otherwise, the error is shunted because instruction faults need to be masked on the mis-speculated instruction paths */
#define DECLARE_FAULT(FAULT)											\
			{															\
			if(!spec_mode)												\
				fault = (FAULT);										\
			/* else, spec fault, ignore it, always terminate exec... */	\
			break;														\
			}
#include "machine.def"
			default:
			/* can speculatively decode a bogus inst, convert to a NOP */
			op = MD_NOP_OP;
			/* compute output/input dependencies to out1-2 and in1-3 */	
			out1 = NA; out2 = NA; out3 = NA;
			in1 = NA; in2 = NA; in3 = NA; in4 = NA;
			/* no EXPR */
		}
		/* operation sets next PC */

		if(fault != md_fault_none)
			fatal("non-speculative fault (%d) detected @ 0x%08p", fault, regs.regs_PC);

		/* NOP'ify all predicate-false instructions */
		if(!cond_valid && (op != BR && op != BRL))
		{
			op = ADD;
			out1 = DGPR(0); out2 = NA; out3 = NA;
			in1 = DGPR(0); in2 = NA; in3 = NA; in4 = NA;
		}


		/* FIXME: remove this crystal ball! */
		if(!spec_mode && setPC != 0/* regs.regs_R[MD_REG_PC] != regs.regs_PC */)
		{
			regs.regs_NPC = regs.regs_R[MD_REG_PC];
			jmp_flow = TRUE;
		}

		/* print retirement trace if in verbose mode */
		if(verbose && sim_num_insn >= trigger_inst)
		{
			myfprintf(stderr, "%s %10n [xor: 0x%08x] {%d} @ 0x%08p: ",
				  spec_mode ? "--" : "++",
				  sim_num_insn, md_xor_regs(&regs), inst_seq+1, regs.regs_PC);

			if(op == MD_NOP_OP) 
				myfprintf(stderr, " <pred-false>");
			else
				md_print_uop(op, inst, regs.regs_PC, stderr);
			fprintf(stderr, "\n");
			/* fflush(stderr); */
		}

		/* update memory access stats */
		if(MD_OP_FLAGS(op) & F_MEM)
		{
			sim_total_refs++;
			if(!spec_mode)
				sim_num_refs++;

			if(MD_OP_FLAGS(op) & F_STORE)
				is_write = TRUE;
			else
			{
				sim_total_loads++;
				if(!spec_mode)
					sim_num_loads++;
			}
		}

    if (trace_me && !spec_mode && MD_OP_FLAGS(op) & F_MEM)
    {
	      fprintf(tracefile,"%s: 0x%08x %08x\n",(MD_OP_FLAGS(op) & F_STORE)?"STORE":"READ ",addr, GPR(RD));
	}
	

		br_taken = (regs.regs_NPC != (regs.regs_PC + sizeof(md_inst_t)));
		br_pred_taken = (pred_PC != (regs.regs_PC + sizeof(md_inst_t)));

		if(!in_flow && (jmp_flow ||(pred_PC != regs.regs_NPC && (pred_perfect || op == MOV)) || ((MD_OP_FLAGS(op) & (F_CTRL|F_DIRJMP)) == (F_CTRL|F_DIRJMP) && target_PC != pred_PC && br_pred_taken)))

		{
			/* Either 
			 * 1) we're simulating perfect prediction and are in a mis-predict state and need to patch up, or 
			 * 2) We're not simulating perfect prediction, we've predicted the branch taken, but our predicted target doesn't match the computed target (i.e., mis-fetch).  
			 * Just update the PC values and do a fetch squash.  This is just like calling fetch_squash() except we pre-anticipate the updates to the fetch values at the end of this function.
			 * If case #2, also charge a mispredict penalty for redirecting fetch */
			fetch_pred_PC = fetch_regs_PC = regs.regs_NPC;
			/* was: if (pred_perfect) */
			if(pred_perfect)
				pred_PC = regs.regs_NPC;

			fetch_head = (ruu_ifq_size-1);
			fetch_num = 1;
			fetch_tail = 0;

			if(!pred_perfect)
			{
				ruu_fetch_issue_delay = ruu_branch_penalty;
				if(verbose && sim_num_insn >= trigger_inst)
					myfprintf(stderr, "MF: ruu_fetch_issue_delay = %d @ (%n)...\n",
				ruu_fetch_issue_delay, sim_cycle);
			}

			fetch_redirected = TRUE;
			jmp_flow = FALSE;
		}

		/* is this a NOP */
		if(op != MD_NOP_OP)
		{
			/* for load/stores:
				idep #0     - store operand (value that is store'ed)
				idep #1, #2 - eff addr computation inputs (addr of access)

				resulting RUU/LSQ operation pair:
				RUU (effective address computation operation):
				idep #0, #1 - eff addr computation inputs (addr of access)
				LSQ (memory access operation):
				idep #0     - operand input (value that is store'd)
				idep #1     - eff addr computation result (from RUU op)

				effective address computation is transfered via the reserved name DTMP
			*/

			rs = lsq = NULL;
			if(!(MD_OP_FLAGS(op) & F_MEM))
			{
				/* ALU operation */

				/* fill in RUU reservation station */
				rs = &RUU[RUU_tail];
				rs->slip = sim_cycle - 1;
				rs->IR = inst;
				rs->op = op;
				rs->PC = regs.regs_PC;
				rs->next_PC = regs.regs_NPC; rs->pred_PC = pred_PC;
				rs->in_LSQ = FALSE;
				rs->ea_comp = (op == AGEN || op == AGEN_U || op == AGENI || op == AGENI_U);
				rs->recover_inst = FALSE;
				rs->dir_update = *dir_update_ptr;
				rs->stack_recover_idx = stack_recover_idx;
				rs->spec_mode = spec_mode;
				rs->addr = 0;
				/* rs->tag is already set */
				rs->seq = ++inst_seq;
				rs->queued = rs->issued = rs->completed = FALSE;
				rs->ptrace_seq = pseq; 

				/* link onto producing operation */
				ruu_link_idep(rs, /* idep_ready[] index */0, in1);
				ruu_link_idep(rs, /* idep_ready[] index */1, in2);
				ruu_link_idep(rs, /* idep_ready[] index */2, in3);
				ruu_link_idep(rs, /* idep_ready[] index */3, in4);

				/* install output after inputs to prevent self reference */
				ruu_install_odep(rs, /* odep_list[] index */0, out1);
				ruu_install_odep(rs, /* odep_list[] index */1, out2);
				ruu_install_odep(rs, /* odep_list[] index */2, out3);

				/* install operation in the RUU */
				RUU_tail = (RUU_tail + 1) % RUU_size;
				RUU_num++;

				/* issue op if all its reg operands are ready (no mem input) */
				if(OPERANDS_READY(rs))
				{
					/* put operation on ready list, ruu_issue() issue it later */
					readyq_enqueue(rs);
					/* issue may continue */
					RSLINK_INIT(last_op, rs);
					/* was: last_op = RSLINK_NULL; */
				}
				else
				{
					/* could not issue this inst, stall issue until we can */
					RSLINK_INIT(last_op, rs);
				}
			}
			else
			{
				/* MEM operation */

				/* fill in LSQ reservation station */
				rs = lsq = &LSQ[LSQ_tail];
				lsq->slip = sim_cycle - 1;
				lsq->IR = inst;
				lsq->op = op;
				lsq->PC = regs.regs_PC;
				lsq->next_PC = regs.regs_NPC; lsq->pred_PC = pred_PC;
				lsq->in_LSQ = TRUE;
				lsq->ea_comp = FALSE;
				lsq->recover_inst = FALSE;
				lsq->dir_update.pdir1 = lsq->dir_update.pdir2 = NULL;
				lsq->dir_update.pmeta = NULL;
				lsq->stack_recover_idx = 0;
				lsq->spec_mode = spec_mode;
				lsq->addr = addr;
				/*
				if(!spec_mode && addr < 0x1000)
					panic("bad addr: 0x%p", addr); 
				lsq->tag is already set */
				lsq->seq = ++inst_seq;
				lsq->queued = lsq->issued = lsq->completed = FALSE;
				lsq->ptrace_seq = pseq;

				/* pipetrace this uop */

				/* link onto producing operation */
				ruu_link_idep(lsq, /* idep_ready[] index */0, in1);
				ruu_link_idep(lsq, /* idep_ready[] index */1, in2);
				ruu_link_idep(lsq, /* idep_ready[] index */2, in3);
				ruu_link_idep(lsq, /* idep_ready[] index */3, in4);

				/* install output after inputs to prevent self reference */
				ruu_install_odep(lsq, /* odep_list[] index */0, out1);
				ruu_install_odep(lsq, /* odep_list[] index */1, out2);
				ruu_install_odep(lsq, /* odep_list[] index */2, out3);
				/* install operation in the RUU and LSQ BUG? */
				LSQ_tail = (LSQ_tail + 1) % LSQ_size;
				LSQ_num++;

				/* issue may continue when the load/store is issued */
				/* was: RSLINK_INIT(last_op, lsq); */

				/* issue stores only, loads are issued by lsq_refresh() */
				if(((MD_OP_FLAGS(op) & (F_MEM|F_STORE)) == (F_MEM|F_STORE)) && OPERANDS_READY(lsq))
				{
					/* panic("store immediately ready"); */
					/* put operation on ready list, ruu_issue() issue it later */
					readyq_enqueue(lsq);
				}
			}
		}
		else
		{
			/* this is a NOP, no need to update RUU/LSQ state */
			rs = NULL;
			lsq = NULL;
		}

		/* one more instruction executed, speculative or otherwise */
		sim_total_insn++;
		if(MD_OP_FLAGS(op) & F_CTRL)
			sim_total_branches++;

		if(!in_flow && !spec_mode && (op == BR || op == BRL))
		{
			/* if this is a branching instruction update BTB, i.e., only non-speculative state is committed into the BTB */
			if(MD_OP_FLAGS(op) & F_CTRL)
			{
				sim_num_branches++;
				if(pred && bpred_spec_update == spec_ID)
				{
					bpred_update(pred, regs.regs_PC, regs.regs_NPC, regs.regs_NPC != (regs.regs_PC + sizeof(md_inst_t)), pred_PC != (regs.regs_PC + sizeof(md_inst_t)), pred_PC == regs.regs_NPC, op, &rs->dir_update);

#ifdef PANALYZER_H
#ifdef LV1_PANALYZER_H
				lv1_panalyzer(lv1_bpred,(int) 1 /* update */);
#endif								
#endif

				}
			}

			/* is the trace generator trasitioning into mis-speculation mode? */
			if(pred_PC != regs.regs_NPC && !fetch_redirected)
			{
			                /* entering mis-speculation mode, indicate this and save PC */
			  	        if (verbose && sim_num_insn >= trigger_inst)
					  fprintf(stderr, "** entering spec mode...\n");
					spec_mode = TRUE;
					in_flow = FALSE;
					jmp_flow = FALSE;
					rs->recover_inst = TRUE;
					recover_PC = regs.regs_NPC;
			}
		}

		/* entered decode/allocate stage, indicate in pipe trace */
		/*DAN        YO! Here it is.  Fix it.*/

		ptrace_newstage(pseq, PST_DISPATCH, (pred_PC != regs.regs_NPC) ? PEV_MPOCCURED : 0, 0, 0);
		if(op == MD_NOP_OP)
		{
			/* end of the line */
			ptrace_endinst(pseq);
		}

		/* update any stats tracked by PC */
		for(i = 0; i < pcstat_nelt; i++)
		{
			counter_t newval;
			int delta;

			/* check if any tracked stats changed */
			newval = STATVAL(pcstat_stats[i]);
			delta = newval - pcstat_lastvals[i];
			if(delta != 0)
			{
				stat_add_samples(pcstat_sdists[i], regs.regs_PC, delta);
				pcstat_lastvals[i] = newval;
			}
		}

		/* consume instruction from IFETCH -> DISPATCH queue */
		if(!in_flow)
		{
			fetch_head = (fetch_head+1) & (ruu_ifq_size - 1);
			fetch_num--;
		}

		/* check for DLite debugger entry condition */
		made_check = TRUE;
		if(dlite_check_break(pred_PC, is_write ? ACCESS_WRITE : ACCESS_READ, addr, sim_num_insn, sim_cycle))
			dlite_main(regs.regs_PC, pred_PC, sim_cycle, &regs, mem);
	}

	/* need to enter DLite at least once per cycle */
	if(!made_check)
	{
		if(dlite_check_break(/* no next PC */0, is_write ? ACCESS_WRITE : ACCESS_READ, addr, sim_num_insn, sim_cycle))
			dlite_main(regs.regs_PC, /* no next PC */0, sim_cycle, &regs, mem);
	}
}


/* RUU_FETCH() - instruction fetch pipeline stage(s) */

/* initialize the instruction fetch pipeline stage */
static void
fetch_init(void)
{
	/* allocate the IFETCH -> DISPATCH instruction queue */
	fetch_data = (struct fetch_rec *)calloc(ruu_ifq_size, sizeof(struct fetch_rec));
	if(!fetch_data)
		fatal("out of virtual memory");

	fetch_num = 0;
	fetch_tail = fetch_head = 0;
	IFQ_count = 0;
	IFQ_fcount = 0;
}

/* dump contents of fetch stage registers and fetch queue */
void
fetch_dump(FILE *stream)			/* output stream */
{
	int num, head;

	if(!stream)
		stream = stderr;

	fprintf(stream, "** fetch stage state **\n");

	fprintf(stream, "spec_mode: %s\n", spec_mode ? "t" : "f");
	myfprintf(stream, "pred_PC: 0x%08p, recover_PC: 0x%08p\n",
	pred_PC, recover_PC);
	myfprintf(stream, "fetch_regs_PC: 0x%08p, fetch_pred_PC: 0x%08p\n",
	fetch_regs_PC, fetch_pred_PC);
	fprintf(stream, "\n");

	fprintf(stream, "** fetch queue contents **\n");
	fprintf(stream, "fetch_num: %d\n", fetch_num);
	fprintf(stream, "fetch_head: %d, fetch_tail: %d\n",
	fetch_head, fetch_tail);

	num = fetch_num;
	head = fetch_head;
	while(num)
	{
		fprintf(stream, "idx: %2d: inst: `", head);
		md_print_insn(fetch_data[head].IR, fetch_data[head].regs_PC, stream);
		fprintf(stream, "'\n");
		myfprintf(stream, "         regs_PC: 0x%08p, pred_PC: 0x%08p\n",
		fetch_data[head].regs_PC, fetch_data[head].pred_PC);
		head = (head + 1) & (ruu_ifq_size - 1);
		num--;
	}
}

static int last_inst_missed = FALSE;
static int last_inst_tmissed = FALSE;

/* fetch up as many instruction as one branch prediction and one cache line acess will support without overflowing the IFETCH -> DISPATCH QUEUE */
static void
ruu_fetch(void)
{
	// int i, lat, tlb_lat, done = FALSE;
        int i_longerlat = 0, latency_storage = 0;
	int i, lat=cache_il1_lat, tlb_lat, done = FALSE;
	md_inst_t inst;
	int stack_recover_idx;
	int branch_cnt;

#ifdef PANALYZER_H
#ifdef LV2_PANALYZER_H
#ifdef CACHE_PANALYZER_H
    fu_address_t astart;
	// buffer_t *dstreambuf;
	buffer_t *dstreambuf = NULL;/* Ludo: initialized pointer to avoid troubles later */

	/* cache access start decoded address while fetching multiple instructions */
	astart = fetch_pred_PC;
	/* fetched instruction stream while fetching multiple instructions */
	//    if(cache_il1)
	//    if(!(dstreambuf = (buffer_t *)calloc(cache_il1->bsize, sizeof(buffer_t))))
	//      fatal("out of virtual memory!");
#endif /* CACHE_PANALYZER_H */
#endif
#endif

	/* fetch up to as many instruction as the DISPATCH stage can decode */ /* fetch until IFETCH -> DISPATCH queue fills */ /* and no IFETCH blocking condition encountered */
	for(i = 0, branch_cnt = 0; i < (ruu_decode_width * fetch_speed) && fetch_num < ruu_ifq_size && !done; i++)
	{
		/* fetch an instruction at the next predicted fetch address */
		fetch_regs_PC = fetch_pred_PC;

		/* is this a bogus text address? (can happen on mis-spec path) */
		if(ld_text_base <= fetch_regs_PC && fetch_regs_PC < (ld_text_base+ld_text_size) && !(fetch_regs_PC & (sizeof(md_inst_t)-1)))
		{
			/* read instruction from memory */
			MD_FETCH_INST(inst, mem, fetch_regs_PC);

			/* address is within program text, read instruction from memory */
			lat = cache_il1_lat;
			if(cache_il1)
			{
				/* access the I-cache */
#ifdef VANALYZER_H
				byte_t *p;
				p = (byte_t *)malloc(sizeof(md_inst_t));
				lat = cache_access(cache_il1, Read, IACOMPRESS(fetch_regs_PC), p, ISCOMPRESS(sizeof(md_inst_t)), sim_cycle, NULL, NULL);
				
#ifdef PANALYZER_H
#ifdef LV1_PANALYZER_H
				lv1_panalyzer(lv1_il1,(int) Read);
	
#endif
	
#ifdef LV2_PANALYZER_H
	//#ifdef CACHE_PANALYZER_H
	//              for(j = 0; j < sizeof(md_inst_t); j++)
	//                  *(dstreambuf+sizeof(md_inst_t)*i + j) = *(p + j);
	//#endif /* CACHE_PANALYZER_H */
#ifdef CACHE_PANALYZER_H
	  if(cache_il1){
		        cache_panalyzer(il1_pspec, Read, IACOMPRESS(fetch_regs_PC), NULL, sizeof(md_inst_t), sim_cycle, lat);
		  //        free(dstreambuf);
		              }
#endif /* CACHE_PANALYZER_H */

#endif
#endif
								
				if (lat == cache_il1_lat)
				{
				}
				free(p);
#else
				lat = cache_access(cache_il1, Read, IACOMPRESS(fetch_regs_PC), NULL, ISCOMPRESS(sizeof(md_inst_t)), sim_cycle, NULL, NULL);
#endif
				if (lat > cache_il1_lat)
					last_inst_missed = TRUE;
			}

			if(itlb)
			{
				/* access the I-TLB, NOTE: this code will initiate speculative TLB misses */
				tlb_lat = cache_access(itlb, Read, IACOMPRESS(fetch_regs_PC), NULL, ISCOMPRESS(sizeof(md_inst_t)), sim_cycle, NULL, NULL);

#ifdef PANALYZER_H
#ifdef LV1_PANALYZER_H
				lv1_panalyzer(lv1_itlb,(int) Read);
#endif
#ifdef LV2_PANALYZER_H
#ifdef CACHE_PANALYZER_H
	            cache_panalyzer(itlb_pspec, Read, IACOMPRESS(fetch_regs_PC), NULL, 4, sim_cycle, tlb_lat);
#endif /* CACHE_PANALYZER_H */
#endif
#endif

				if(tlb_lat > 1) 
					last_inst_tmissed = TRUE;

				/* I-cache/I-TLB accesses occur in parallel */ 
				i_longerlat = (tlb_lat > lat ) ? PEV_TLBMISS : PEV_CACHEMISS; 
				lat = MAX(tlb_lat, lat);
			}

			/* I-cache/I-TLB miss? assumes I-cache hit >= I-TLB hit */
			if(lat != cache_il1_lat)
			{
				/* I-cache miss, block fetch until it is resolved */
				ruu_fetch_issue_delay += lat - 1;
				if(verbose && sim_num_insn >= trigger_inst) 
					myfprintf(stderr, "IF: ruu_fetch_issue_delay = %d @ (%n)...\n", ruu_fetch_issue_delay, sim_cycle); latency_storage=ruu_fetch_issue_delay;
				break;
			}
			/* else, I-cache/I-TLB hit */
		}
		else
		{
			/* fetch PC is bogus, send a NOP down the pipeline */
			inst = MD_NOP_INST;
		}

		/* have a valid inst, here */

		/* possibly use the BTB target */
		if(pred)
		{
			enum md_opcode op;

			/* pre-decode instruction, used for bpred stats recording */
			MD_SET_OPCODE(op, inst);

			/* get the next predicted fetch address; only use branch predictor result for branches (assumes pre-decode bits); 
			 * NOTE: returned value may be 1 if bpred can only predict a direction */
			if(MD_OP_FLAGS(op) & F_CTRL)
			{
#ifdef PANALYZER_H
#ifdef LV2_PANALYZER_H             
				buffer_t *bus;
				fu_address_t baddr = fetch_regs_PC;
#endif
#endif /* PANALYZER_H */
	
				/* branch address, target address FIXME: not computed, opcode, call?, return?, updt, RSB index */
				fetch_pred_PC = bpred_lookup(pred, fetch_regs_PC, 0, op, MD_IS_CALL(op), MD_IS_RETURN(op), &(fetch_data[fetch_tail].dir_update), &stack_recover_idx);

#ifdef PANALYZER_H
#ifdef LV1_PANALYZER_H
				lv1_panalyzer(lv1_bpred,(int) 0 /* lookup */);
#endif		
#ifdef LV2_PANALYZER_H
#ifdef MEMORY_PANALYZER_H
				if(pred->class == BPred2bit)
				{
					fu_address_t bimodaddr;
					char bimods;
					bimodaddr = (((baddr >> 19) ^ (baddr >> MD_BR_SHIFT))
						& (pred->dirpred.bimod->config.bimod.size-1));
					bimods = pred->dirpred.bimod->config.bimod.table[bimodaddr];
					bus = create_buffer_t(&bimods, bimod_pspec->bsize);
					sbank_panalyzer(
						bimod_pspec, /* memory pspec*/
						bus, /* out bus: Y = f(x) */
						sim_cycle /* current simulation time */);
						free(bus);
				 }
				 if(pred->class == BPred2Level)
				 {
					 int l1index, l2index;
					 char twolevs;
					 /* traverse 2-level tables */
					 l1index = (baddr >> MD_BR_SHIFT)
						 & (pred->dirpred.twolev->config.two.l1size - 1);
					 l2index = pred->dirpred.twolev->config.two.shiftregs[l1index];
					 if (pred->dirpred.twolev->config.two.xor)
					 {
/* this L2 index computation is more "compatible" to McFarling's verison of it, i.e., if the PC xor address component is only
part of the index, take the lower order address bits for the
other part of the index, rather than the higher order ones */
			           l2index = (((l2index ^ (baddr >> MD_BR_SHIFT))
			               & ((1 << pred->dirpred.twolev->config.two.shift_width) - 1))
			               | ((baddr >> MD_BR_SHIFT)
			               << pred->dirpred.twolev->config.two.shift_width));
    				}
	    			else
			        {
						l2index =
							l2index | ((baddr >> MD_BR_SHIFT) << pred->dirpred.twolev->config.two.shift_width);
					}
					l2index = l2index & (pred->dirpred.twolev->config.two.l2size - 1);
					/* get a pointer to prediction state information */
					twolevs = pred->dirpred.twolev->config.two.l2table[l2index];
					bus = create_buffer_t(&l2index, lev1_pspec->bsize);
					sbank_panalyzer(
						lev1_pspec, /* memory pspec*/
						bus, /* out bus */
						sim_cycle /* current simulation time */);
					free(bus);
					bus = create_buffer_t(&twolevs, lev2_pspec->bsize);
					sbank_panalyzer(
						lev2_pspec, /* memory pspec*/
						bus, /* out bus */
						sim_cycle /* current simulation time */);
					free(bus);
				}
																								/* RAS panalyzer */
																								/* if this is a return, pop return-address stack */
				if (MD_IS_RETURN(op) && pred->retstack.size) {
			         fu_address_t target = pred->retstack.stack[pred->retstack.tos].target;
					 pred->retstack.tos = (pred->retstack.tos + pred->retstack.size - 1)
						 % pred->retstack.size;
					 pred->retstack_pops++;
 			         bus = create_buffer_t(&target, ras_pspec->bsize);
					 sbank_panalyzer(
						 ras_pspec, /* memory pspec*/
						 bus, /* out bus */
						 sim_cycle /* current simulation time */);
						 free(bus);
				  }
				  else
				  {
#ifdef CACHE_PANALYZER_H
				   /* btb is allocated */
					  if(pred->btb.btb_data)
					  {
						  bus = create_buffer_t(&fetch_pred_PC, sizeof(fu_address_t));
					      cache_panalyzer(btb_pspec, Read, fetch_regs_PC, bus, sizeof(md_addr_t), sim_cycle, 1);
					     free(bus);
					   }
#endif /* CACHE_PANALYZER_H */
			        }
#endif /* MEMORY_PANALYZER_H */
#endif
									
#endif
			
			}
			else
				fetch_pred_PC = 0;

			/* valid address returned from branch predictor? */
			if(!fetch_pred_PC)
			{
				/* no predicted taken target, attempt not taken target */
				fetch_pred_PC = fetch_regs_PC + sizeof(md_inst_t);
			}
			else
			{
				/* go with target, NOTE: discontinuous fetch, so terminate */
				branch_cnt++;
				if (branch_cnt >= fetch_speed)
					done = TRUE;
			}
		}
		else
		{
			/* no predictor, just default to predict not taken, and continue fetching instructions linearly */
			fetch_pred_PC = fetch_regs_PC + sizeof(md_inst_t);
		}

		/* commit this instruction to the IFETCH -> DISPATCH queue */
		fetch_data[fetch_tail].IR = inst;
		fetch_data[fetch_tail].regs_PC = fetch_regs_PC;
		fetch_data[fetch_tail].pred_PC = fetch_pred_PC;
		fetch_data[fetch_tail].stack_recover_idx = stack_recover_idx;
		fetch_data[fetch_tail].ptrace_seq = ptrace_seq++;

		/* for pipe trace */
		ptrace_newinst(fetch_data[fetch_tail].ptrace_seq, inst, fetch_data[fetch_tail].regs_PC, 0);
		ptrace_newstage(fetch_data[fetch_tail].ptrace_seq, PST_IFETCH, ((last_inst_missed ? PEV_CACHEMISS : 0) | (last_inst_tmissed ? PEV_TLBMISS : 0)), latency_storage, i_longerlat);
		last_inst_missed = FALSE;
		last_inst_tmissed = FALSE;
		latency_storage = 0;  //GPV: reset this   
		i_longerlat=0;

		/* adjust instruction fetch queue */
		fetch_tail = (fetch_tail + 1) & (ruu_ifq_size - 1);
		fetch_num++;
	}
}

/* default machine state accessor, used by DLite */
/*
FILE *stream,			: output stream 
char *cmd,				: optional command string 
struct regs_t *regs,	: registers to access
struct mem_t *mem		: memory space to access
*/
static char *					/* err str, NULL for no err */
simoo_mstate_obj(FILE *stream, char *cmd, struct regs_t *regs, struct mem_t *mem)
{
	if(!cmd || !strcmp(cmd, "help"))
		fprintf(stream,
		"mstate commands:\n"
		"\n"
		"    mstate help   - show all machine-specific commands (this list)\n"
		"    mstate stats  - dump all statistical variables\n"
		"    mstate res    - dump current functional unit resource states\n"
		"    mstate ruu    - dump contents of the register update unit\n"
		"    mstate lsq    - dump contents of the load/store queue\n"
		"    mstate eventq - dump contents of event queue\n"
		"    mstate readyq - dump contents of ready instruction queue\n"
		"    mstate cv     - dump contents of the register create vector\n"
		"    mstate rspec  - dump contents of speculative regs\n"
		"    mstate mspec  - dump contents of speculative memory\n"
		"    mstate fetch  - dump contents of fetch stage registers and fetch queue\n"
		"\n");
	else 
	if(!strcmp(cmd, "stats"))
	{
		/* just dump intermediate stats */
		sim_print_stats(stream);
	}
	else 
	if(!strcmp(cmd, "res"))
	{
		/* dump resource state */
		res_dump(fu_pool, stream);
	}
	else 
	if(!strcmp(cmd, "ruu"))
	{
		/* dump RUU contents */
		ruu_dump(stream);
	}
	else 
	if (!strcmp(cmd, "lsq"))
	{
		/* dump LSQ contents */
		lsq_dump(stream);
	}
	else 
	if(!strcmp(cmd, "eventq"))
	{
		/* dump event queue contents */
		eventq_dump(stream);
	}
	else 
	if(!strcmp(cmd, "readyq"))
	{
		/* dump event queue contents */
		readyq_dump(stream);
	}
	else 
	if(!strcmp(cmd, "cv"))
	{
		/* dump event queue contents */
		cv_dump(stream);
	}
	else 
	if(!strcmp(cmd, "rspec"))
	{
		/* dump event queue contents */
		rspec_dump(stream);
	}
	else 
	if(!strcmp(cmd, "mspec"))
	{
		/* dump event queue contents */
		mspec_dump(stream);
	}
	else 
	if(!strcmp(cmd, "fetch"))
	{
		/* dump event queue contents */
		fetch_dump(stream);
	}
	else
		return "unknown mstate command";

	/* no error */
	return NULL;
}

void
profile_ipc()
{
	float local_IPC;
	float local_range;
	int i,j;

	/* wait till I have something to look at :-) */
	if((sim_num_insn !=0) && (sim_cycle !=0))
	{
		//typecast to float otherwise you get zero :-)
		local_IPC = (float) ((int)(sim_num_insn)) / ((int)sim_cycle);
		/* if first cycle set this as the IPCs to beat */
		if((sim_num_insn > 1) && (sim_num_insn < ruu_commit_width))
		{
			max_ipc_time=sim_cycle;
			min_ipc_time=sim_cycle;
			max_ipc_value=local_IPC;
			min_ipc_value=local_IPC;
			range_max=local_IPC;
			range_min=local_IPC;
			for(i = 0; i < 5; i++)
			{
				range_val[i]=0;
				range[i]=0;
			}
		}
		/* after 10 million instructions set maxm and minm values */
		if(sim_num_insn == 10000000)
		{
			maxm_ipc_time=sim_cycle;
			minm_ipc_time=sim_cycle;
			maxm_ipc_value=local_IPC;
			minm_ipc_value=local_IPC;
		}

		if(local_IPC > max_ipc_value)
		{
			max_ipc_time=sim_cycle;
			max_ipc_value=local_IPC;
		}
		if(local_IPC < min_ipc_value)
		{
			min_ipc_time=sim_cycle;
			min_ipc_value=local_IPC;
		}
		if((local_IPC > maxm_ipc_value)&&((int)sim_num_insn > 10000000))
		{
			maxm_ipc_time=sim_cycle;
			maxm_ipc_value=local_IPC;
		}
		if((local_IPC < minm_ipc_value)&&((int)sim_num_insn > 10000000))
		{
			minm_ipc_time=sim_cycle;
			minm_ipc_value=local_IPC;
		}

		//every 10000 simulation cycles check the range      
		if((((int)sim_cycle) % 10000) == 0)
		{
			local_range= range_max - range_min;
			for(i = 0; i < 5; i++)
			{
				//if the value is greater then one of the ranges shift all
				//lower ones down and put that one in
				if(range_val[i] < local_range)
				{
					for(j = 5 - i; j > i + 1; j--)
					{
						range[j]=range[j-1];
						range_val[j]=range_val[j-1];
					}
					range[i]=sim_cycle;
					range_val[i]=local_range;
					break;
				}
			}
			range_min=local_IPC;
			range_max=local_IPC;
		}
		else
		{
			if(local_IPC > range_max)
			{
				range_max=local_IPC;
			}
			if(local_IPC < range_min)
			{
				range_min=local_IPC;
			}
		}
	}
} /* end profiling option */

/* start simulation, program loaded, processor precise state initialized */
void
sim_main(void)
{
	int setPC;

	/* ignore any floating point exceptions, they may occur on mis-speculated execution paths */
	signal(SIGFPE, SIG_IGN);

  if(trace_me)
    tracefile = fopen(tracefilename,"w");

	/* set up program entry state */
	regs.regs_PC = ld_prog_entry;
	regs.regs_NPC = regs.regs_PC + sizeof(md_inst_t);

	/* make a database for the pipetrace statistics */
	ptrace_sdb = stat_new();
	ptrace_reg_stats(ptrace_sdb);

	/* check for DLite debugger entry condition */
	if(dlite_check_break(regs.regs_PC, /* no access */0, /* addr */0, 0, 0)) 
		dlite_main(regs.regs_PC, regs.regs_PC + sizeof(md_inst_t), sim_cycle, &regs, mem);

	/* fast forward simulator loop, performs functional simulation for FASTFWD_COUNT insts, then turns on performance (timing) simulation */
	if(fastfwd_count > 0)
	{
		int icount;
		md_inst_t inst; /* actual instruction bits */
		enum md_opcode op; /* decoded opcode enum */
		md_addr_t target_PC; /* actual next/target PC address */
		md_addr_t addr;	/* effective address, if load/store */
		int is_write; /* store? */
		byte_t temp_byte = 0; /* temp variable for spec mem access */
		half_t temp_half = 0; /* " ditto " */
		word_t temp_word = 0; /* " ditto " */
#ifdef HOST_HAS_QWORD
		qword_t temp_qword = 0; /* " ditto " */
#endif /* HOST_HAS_QWORD */
		enum md_fault_type fault;

		fprintf(stderr, "sim: ** fast forwarding %d insts **\n", fastfwd_count);
		getchar();
		for(icount = 0; icount < fastfwd_count; icount++)
		{
		/* maintain $r0 semantics */
#ifndef TARGET_ARM
			regs.regs_R[MD_REG_ZERO] = 0;
#endif /* !TARGET_ARM */
#ifdef TARGET_ALPHA
			regs.regs_F.d[MD_REG_ZERO] = 0.0;
#endif /* TARGET_ALPHA */

			/* keep an instruction count */
			sim_num_insn++;

			/* get the next instruction to execute */
			MD_FETCH_INST(inst, mem, regs.regs_PC);

			/* set default reference address */
			addr = 0; is_write = FALSE;

			/* set default fault - none */
			fault = md_fault_none;

			/* decode the instruction */
			MD_SET_OPCODE(op, inst);

     if(trace_me)
     {
	       fprintf(tracefile,"%08x ",regs.regs_PC);
	       md_print_insn(inst, regs.regs_PC,tracefile);
	       fprintf(tracefile,"\n"); 
	  }
	

#ifdef TARGET_ARM
			setPC = 0;
			regs.regs_R[MD_REG_PC] = regs.regs_PC;
#endif /* TARGET_ARM */

			/* execute the instruction */
			switch(op)
			{
#define DEFINST(OP,MSK,NAME,OPFORM,RES,FLAGS,O1,O2,O3,I1,I2,I3,I4)	\
				case OP:											\
				SYMCAT(OP,_IMPL);									\
				break;
#define DEFLINK(OP,MSK,NAME,MASK,SHIFT)								\
				case OP:											\
				panic("attempted to execute a linking opcode");
#define CONNECT(OP)
#undef DECLARE_FAULT
#define DECLARE_FAULT(FAULT)										\
				{ fault = (FAULT); break; }
#include "machine.def"
				default:
				panic("attempted to execute a bogus opcode");
			}

			if(fault != md_fault_none)
				fatal("fault (%d) detected @ 0x%08p", fault, regs.regs_PC);

#ifdef TARGET_ARM
			if(setPC != 0/* regs.regs_R[MD_REG_PC] != regs.regs_PC */)
				regs.regs_NPC = regs.regs_R[MD_REG_PC];
#endif /* TARGET_ARM */

			if(verbose && sim_num_insn >= trigger_inst)
			{
				myfprintf(stderr, "%10n [xor: 0x%08x] @ 0x%08p: ",
				sim_num_insn, md_xor_regs(&regs), regs.regs_PC);
				md_print_insn(inst, regs.regs_PC, stderr);
				fprintf(stderr, "\n");
			}

			/* update memory access stats */
			if (MD_OP_FLAGS(op) & F_MEM)
			{
				if (MD_OP_FLAGS(op) & F_STORE)
					is_write = TRUE;
        	 //if(trace_me)
      	 //fprintf(tracefile,"%s: 0x%08x %08x\n",is_write?"STORE":"READ ",addr, GPR(RD));
			}

			/* check for DLite debugger entry condition */
			if(dlite_check_break(regs.regs_NPC, is_write ? ACCESS_WRITE : ACCESS_READ, addr, sim_num_insn, sim_num_insn)) 
				dlite_main(regs.regs_PC, regs.regs_NPC, sim_num_insn, &regs, mem);

			/* go to the next instruction */
			regs.regs_PC = regs.regs_NPC;
			regs.regs_NPC += sizeof(md_inst_t);
		}
	}

	fprintf(stderr, "sim: ** starting performance simulation **\n");
	/* set up timing simulation entry state */
	fetch_regs_PC = regs.regs_PC - sizeof(md_inst_t);
	fetch_pred_PC = regs.regs_PC;
	regs.regs_PC = regs.regs_PC - sizeof(md_inst_t);

	/* main simulator loop, NOTE: the pipe stages are traverse in reverse order to eliminate this/next state synchronization and relaxation problems */
	for( ; ; )
	{
		/* RUU/LSQ sanity checks */
		if(((RUU_head + RUU_num) % RUU_size) != RUU_tail)
			panic("RUU_head/RUU_tail wedged");
		if(((LSQ_head + LSQ_num) % LSQ_size) != LSQ_tail)
			panic("LSQ_head/LSQ_tail wedged");

		/* check if pipetracing is still active */
		ptrace_check_active(regs.regs_PC, sim_num_insn, sim_cycle);

		/* indicate new cycle in pipetrace */
		ptrace_newcycle(sim_cycle);

		/* commit entries from RUU/LSQ to architected register file */
		ruu_commit();

		/* service function unit release events */
		ruu_release_fu();

		/* ==> may have ready queue entries carried over from previous cycles */

		/* service result completions, also readies dependent operations */
		/* ==> inserts operations into ready queue --> register deps resolved */
		ruu_writeback();

		if(!bugcompat_mode)
		{
			/* try to locate memory operations that are ready to execute */
			/* ==> inserts operations into ready queue --> mem deps resolved */
			lsq_refresh();

			/* issue operations ready to execute from a previous cycle */
			/* <== drains ready queue <-- ready operations commence execution */
			ruu_issue();
		}

		/* decode and dispatch new operations */
		/* ==> insert ops w/ no deps or all regs ready --> reg deps resolved */
		ruu_dispatch();

		if (bugcompat_mode)
		{
			/* try to locate memory operations that are ready to execute */
			/* ==> inserts operations into ready queue --> mem deps resolved */
			lsq_refresh();

			/* issue operations ready to execute from a previous cycle */
			/* <== drains ready queue <-- ready operations commence execution */
			ruu_issue();
		}

		/* call instruction fetch unit if it is not blocked */
		if(!ruu_fetch_issue_delay)
			ruu_fetch();
		else 
		if(ruu_fetch_issue_delay == 9999)
		{
			ruu_fetch();
			ruu_dispatch();
			ruu_fetch();
			ruu_fetch_issue_delay = 0;
		}
		else
		{
			ruu_fetch_issue_delay--;
			if(verbose && sim_num_insn >= trigger_inst)
				myfprintf(stderr, "** fetch delayed in cycle (%n)...\n", sim_cycle);
		} 

		/* update buffer occupancy stats */
		IFQ_count += fetch_num;
		IFQ_fcount += ((fetch_num == ruu_ifq_size) ? 1 : 0);
		RUU_count += RUU_num;
		RUU_fcount += ((RUU_num == RUU_size) ? 1 : 0);
		LSQ_count += LSQ_num;
		LSQ_fcount += ((LSQ_num == LSQ_size) ? 1 : 0);
		/*print out the cycle ptrace stats */
		if(ptrace_active)
			ptrace_print_stats(ptrace_sdb,ptrace_outfd);		

		if(profile_mode)
			profile_ipc();

		/* go to next cycle */
		sim_cycle++;
	// added by Taeho Kgil	
		if((icache_miss_flag == 1) && (dcache_miss_flag == 1))
		 {
			 concurrent_misses++;
		 }
		  icache_miss_flag = 0;
		  dcache_miss_flag = 0;
	// ends here
		  
		/* update power in cylce*/		
#ifdef PANALYZER_H
#ifdef LV1_PANALYZER_H
		lv1_uarch_arg.now = sim_cycle;
		lv1_uarch_arg.pwr_frame = lv1_pwr_frame;
		lv1_panalyzer(lv1_uarch,(int) (&lv1_uarch_arg));
		
#ifdef LV1_CLOCK_PANALYZER_H		
		lv1_panalyzer(lv1_clock,0);
#endif
		
#ifdef PA_TRACE_WINDOW_H
		lv1_pa_trace_arg.now = sim_cycle;
		lv1_pa_trace_arg.window = pa_trace_window;
#ifdef PA_TRANS_COUNT	
		lv1_pa_trace_arg.aio_ham_window = pa_aio_ham_window;
		lv1_pa_trace_arg.dio_ham_window = pa_dio_ham_window;
#endif	
		lv1_pa_trace_arg.miss_window = pa_trace_miss_window;
								
		lv1_panalyzer(lv1_pa_trace,(int) (&lv1_pa_trace_arg));
#endif	

#ifdef PA_TRACE_WINDOW_H			
		/* clear l1 cache event*/
		lv1_pa_trace_arg.cache_miss = 0;
#endif
		
#endif
		
#ifdef LV1_PANALYZER_H
	    lv1_panalyzer(lv1_uarch_clear,0);
#endif

#ifdef LV2_PANALYZER_H
#ifdef CLOCK_PANALYZER_H
        clock_panalyzer(clock_pspec, sim_cycle);
#endif /* CLOCK_PANALYZER_H */
#ifdef UARCH_PANALYZER_H
		        uarch_panalyzer(uarch_pspec, sim_cycle);
#endif /* UARCH_PANALYZER_H */
#endif
			
#endif		
		
		/* finish early? */
		if (max_insts && sim_num_insn >= max_insts)
		return;
	}
}

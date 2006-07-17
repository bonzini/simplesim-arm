/*
* uarch_panalyzer.h - contains top level panalyzer. This integrate all the
* power dissipation from functional units (e.g., il1 cache, aio).
*
* This file is a part of the PowerAnalyzer tool suite written by
* Nam Sung Kim as a part of the PowerAnalyzer Project.
*  
* The tool suite is currently maintained by Nam Sung Kim.
* 
* Copyright (C) 2001 by Nam Sung Kim
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
*/

#ifndef UARCH_PANALYZER_H
#define UARCH_PANALYZER_H
#include "../host.h"
#include "../stats.h"
#include "panalyzer.h"
#include "clock_panalyzer.h"
#include "io_panalyzer.h"
#include "memory_panalyzer.h"
#include "cache_panalyzer.h"
#include "logic_panalyzer.h"

#ifdef FP_PANALYZER_H
#include "alu_panalyzer.h"
#include "mult_panalyzer.h"
#include "fpu_panalyzer.h"
#endif


/* uarch power specification type: 
 * contain all the info for analyzing io pdissipation */
typedef struct _fu_arch_pspec_t {
	char *name;	/* name */
	fu_pmodel_mode_t pmodel; /* power model mode */
	double opfreq, svolt; /* operating frequency/supply voltage */

	fu_io_pspec_t *aio, *dio;
	fu_sbank_pspec_t *irf, *fprf;
	fu_sbank_pspec_t *bimod, *lev1, *lev2, *ras;
	fu_cache_pspec_t *il1, *dl1, *il2, *dl2, *itlb, *dtlb;
	fu_cache_pspec_t *btb;
	fu_clock_pspec_t *clock;
	fu_clock_pspec_t *logic;
	
#ifdef FP_PANALYZER_H 	
	fu_alu_pspec_t *alu;
	fu_mult_pspec_t *mult; 
	fu_fpu_pspec_t *fpu; 
#endif


	fu_Ceffs_t *Ceffs;	/* effective pdissipation capacitances */
	fu_pdissipation_t *pdissipation; /* pdissipation statistics */	
	fu_pdissipation_t pmwindow[MaxPMWindows]; /* power monitoring window */
} fu_uarch_pspec_t;

/* uarch: micro architecture */
fu_uarch_pspec_t *
create_uarch_panalyzer(
	char *name,
	fu_pmodel_mode_t pmodel, /* power model mode */
	double opfreq, double svolt, /* operating frequency/supply voltage */
	fu_io_pspec_t *aio, fu_io_pspec_t *dio,
	fu_sbank_pspec_t *irf, fu_sbank_pspec_t *fprf,
	fu_sbank_pspec_t *bimod, fu_sbank_pspec_t *lev1, fu_sbank_pspec_t *lev2, 
	fu_sbank_pspec_t *ras,
	fu_cache_pspec_t *il1, fu_cache_pspec_t *dl1,
	fu_cache_pspec_t *il2, fu_cache_pspec_t *dl2,
	fu_cache_pspec_t *itlb, fu_cache_pspec_t *dtlb,
	fu_cache_pspec_t *btb,
	fu_logic_pspec_t *logic,
	fu_clock_pspec_t *clock,

#ifdef FP_PANALYZER_H 	
	fu_alu_pspec_t *alu,
	fu_mult_pspec_t *mult, 
	fu_fpu_pspec_t *fpu, 
#endif

	/* effective capacitance for empirical power model */
	double sCeff, double iCeff, double lCeff);

/* top level panalyzer: 
 * this routine integrate all the power dissipation from the functional units 
 * at each simulation cycle */
void 
uarch_panalyzer(
	fu_uarch_pspec_t *pspec,
	tick_t now);
#endif /* UARCH_PANALYZER_H */

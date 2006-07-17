/*
* lv1_uarch_panalyzer.h - contains top level panalyzer. This integrate all the
* power dissipation from functional units (e.g., il1 cache, aio).
*
* This file is a part of the PowerAnalyzer tool suite written by
* Nam Sung Kim as a part of the PowerAnalyzer Project.
*  
* The tool suite is currently maintained by Nam Sung Kim.
* 
* Copyright (C) 2001 by Nam Sung Kim
*
* Revised by Taeho Kgil
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

// #include "lv1_panalyzer.h"
/* uarch power specification type: 
 * contain all the info for analyzing io pdissipation */
typedef struct _fu_lv1_arch_pspec_t {
	char *name;	/* name */
	double opfreq, volt; /* operating frequency/supply voltage */
	double max_power;
	double uarch_power;
	double uarch_total_power;
#ifdef PA_TRANS_COUNT	
	int aio_trans;
	int dio_trans;
#endif	
	fu_lv1_io_pspec_t *aio, *dio;
	fu_lv1_rf_pspec_t *rf;
	fu_lv1_alu_pspec_t *alu;
	fu_lv1_fpu_pspec_t *fpu;
	fu_lv1_mult_pspec_t *mult;
	fu_lv1_bpred_pspec_t *bpred;
	fu_lv1_cache_pspec_t *il1, *dl1, *il2, *dl2, *itlb, *dtlb;
	fu_lv1_clock_pspec_t *clock;

} fu_lv1_uarch_pspec_t;

/* uarch: micro architecture */
fu_lv1_uarch_pspec_t *
create_lv1_uarch_panalyzer(
	char *name,
//	fu_lv1_Ceff_list_t *Ceff_list, /* netlist for Ceff, read from option */
	double opfreq, double volt, /* operating frequency/supply voltage */
//	fu_io_pspec_t *aio, fu_io_pspec_t *dio,
/*	fu_sbank_pspec_t *irf, fu_sbank_pspec_t *fprf,
	fu_sbank_pspec_t *bimod, fu_sbank_pspec_t *lev1, fu_sbank_pspec_t *lev2,  
	fu_sbank_pspec_t *ras,
	fu_cache_pspec_t *il1, fu_cache_pspec_t *dl1,
	fu_cache_pspec_t *il2, fu_cache_pspec_t *dl2,
	fu_cache_pspec_t *itlb, fu_cache_pspec_t *dtlb,
	fu_cache_pspec_t *btb, */
	fu_lv1_alu_pspec_t *alu,
	fu_lv1_fpu_pspec_t *fpu,
	fu_lv1_bpred_pspec_t *bpred,
	fu_lv1_mult_pspec_t *mult,
	fu_lv1_rf_pspec_t *rf,
	fu_lv1_cache_pspec_t *il1,
	fu_lv1_cache_pspec_t *dl1,
	fu_lv1_cache_pspec_t *il2,
	fu_lv1_cache_pspec_t *dl2,
	fu_lv1_cache_pspec_t *itlb,
	fu_lv1_cache_pspec_t *dtlb
//	fu_clock_pspec_t *clock
	);

/* top level panalyzer: 
 * this routine integrate all the power dissipation from the functional units 
 * at each simulation cycle */
void 
lv1_uarch_panalyzer(
	fu_lv1_uarch_pspec_t *pspec,
	tick_t now,
	void * user_data
	);
void 
lv1_uarch_clear_panalyzer(
	fu_lv1_uarch_pspec_t *pspec
	);

#endif /* UARCH_PANALYZER_H */

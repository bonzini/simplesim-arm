/*
* uarch_panalyzer.c - contains top level panalyzer. This integrate all the
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


#include <stdio.h>
#include <stdlib.h>
#include "../host.h"
#include "../misc.h"
#include "technology.h"
#include "panalyzer.h"
#include "clock_panalyzer.h"
#include "io_panalyzer.h"
#include "memory_panalyzer.h"
#include "cache_panalyzer.h"
#include "logic_panalyzer.h"

#ifdef FP_PANALYZER_H
#include "alu_panalyzer.h"
#include "mult_panalyzer.h"
#endif

#include "uarch_panalyzer.h"

#define MonitoringWindow 128

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
	double sCeff, double iCeff, double lCeff)
{
	fu_uarch_pspec_t *pspec;
	int i;

	if(!(pspec = (fu_uarch_pspec_t *)malloc(sizeof(fu_uarch_pspec_t))))
		fatal("lack of virtual memory");
	/* default data structure */
	pspec->name = strdup(name);
	pspec->pmodel = pmodel; 
	pspec->opfreq = opfreq; 
	pspec->svolt = Vdd; /* io supply voltage */

	/* create effective capacitance data structure */
//	if(!(pspec->Ceffs = (fu_Ceffs_t *)malloc(sizeof(fu_Ceffs_t))))
   /* Ludo: changed malloc to calloc to avoid compiler warning */
   if(!(pspec->Ceffs = (fu_Ceffs_t *)calloc(1,sizeof(fu_Ceffs_t))))
		fatal("lack of virtual memory");

	/* analytical model */
	if(pmodel == Analytical) 
	{
		pspec->aio = aio; pspec->dio = dio;
		pspec->irf = irf; pspec->fprf = fprf;
		pspec->bimod = bimod; pspec->lev1 = lev1; pspec->lev2 = lev2; 
		pspec->ras = ras;
		pspec->il1 = il1;
		pspec->dl1 = dl1;
		pspec->il2 = il2;
		pspec->dl2 = dl2;
		pspec->itlb = itlb;
		pspec->dtlb = dtlb;
		pspec->btb = btb;
		pspec->logic = logic;
		pspec->logic->pdissipation = logic->pdissipation;

#ifdef FP_PANALYZER_H 	
		pspec->alu = alu;
		pspec->mult = mult;
		pspec->fpu = fpu;
#endif
		
		pspec->clock = clock;
	}
	/* empirical model */
	else 
		*(pspec->Ceffs) = assign_fu_Ceffs(sCeff, iCeff, lCeff, 0.);

	/* create a power statistics */
	if(!(pspec->pdissipation = (fu_pdissipation_t *)malloc(sizeof(fu_pdissipation_t))))
		fatal("lack of virtual memory");

	/* initialize a power statistics */
	initialize_fu_pdissipation(pspec->pdissipation);

	/* initialize power monitoring window */
	for(i = 0; i < MaxPMWindows; i++) 
	{
		initialize_fu_pmwindow(i, pspec->pmwindow);
		(pspec->pmwindow + i)->leakage = estimate_pdissipation(pspec->Ceffs->lCeff, pspec->opfreq, pspec->svolt);
	}

	return pspec;
}

/* top level panalyzer: 
 * this routine integrate all the power dissipation from the functional units 
 * at each simulation cycle */
void 
uarch_panalyzer(
	fu_uarch_pspec_t *pspec,
	tick_t now)
{
	unsigned pmwindex; 
	double pdissipation;
	fu_pdissipation_t *pmwindow;

	pmwindow = pspec->pmwindow;
	pmwindex = now % MaxPMWindows;
	pdissipation = 0.0;
	
	/* initialize the current power monitor windows entry */
	initialize_fu_pmwindow(pmwindex, pmwindow);
	if(pspec->pmodel == Analytical) 
	{
		/* integrate fu pdissiotion of each fu block */
		integrate_fu_pdissipation(pmwindex, 
						pspec->aio->pmwindow, pspec->aio->pdissipation);
		integrate_fu_pdissipation(pmwindex, 
						pspec->dio->pmwindow, pspec->dio->pdissipation);
		integrate_fu_pdissipation(pmwindex, 
						pspec->irf->pmwindow, pspec->irf->pdissipation);
		integrate_fu_pdissipation(pmwindex, 
						pspec->fprf->pmwindow, pspec->fprf->pdissipation);
		if(pspec->bimod)
		integrate_fu_pdissipation(pmwindex, 
						pspec->bimod->pmwindow, pspec->bimod->pdissipation);
		if(pspec->lev1)
		integrate_fu_pdissipation(pmwindex, 
						pspec->lev1->pmwindow, pspec->lev1->pdissipation);
		if(pspec->lev2)
		integrate_fu_pdissipation(pmwindex, 
						pspec->lev2->pmwindow, pspec->lev2->pdissipation);
		if(pspec->ras)
		integrate_fu_pdissipation(pmwindex, 
						pspec->ras->pmwindow, pspec->ras->pdissipation);
		if(pspec->il1 && (pspec->il1 != pspec->dl1 && pspec->il1 != pspec->dl2))
		integrate_fu_pdissipation(pmwindex, 
						pspec->il1->pmwindow, pspec->il1->pdissipation);
		if(pspec->il2 && (pspec->il2 != pspec->dl1 && pspec->il2 != pspec->dl2))
		integrate_fu_pdissipation(pmwindex, 
						pspec->il2->pmwindow, pspec->il2->pdissipation);
		if(pspec->dl1)
		integrate_fu_pdissipation(pmwindex, 
						pspec->dl1->pmwindow, pspec->dl1->pdissipation);
		if(pspec->dl2)
		integrate_fu_pdissipation(pmwindex, 
						pspec->dl1->pmwindow, pspec->dl2->pdissipation);
		if(pspec->itlb)
		integrate_fu_pdissipation(pmwindex, 
						pspec->itlb->pmwindow, pspec->itlb->pdissipation);
		if(pspec->dtlb)
		integrate_fu_pdissipation(pmwindex, 
						pspec->dtlb->pmwindow, pspec->dtlb->pdissipation);
		if(pspec->btb)
		integrate_fu_pdissipation(pmwindex, 
						pspec->btb->pmwindow, pspec->btb->pdissipation);
		if(pspec->logic)
		integrate_fu_pdissipation(pmwindex, 
						pspec->logic->pmwindow, pspec->logic->pdissipation);

		integrate_fu_pdissipation(pmwindex, 
						pspec->clock->pmwindow, pspec->clock->pdissipation);

		/* integrate all the swiching power */
		(pmwindow + pmwindex)->switching
			= obtain_switching(pmwindex, pspec->aio->pmwindow);
		(pmwindow + pmwindex)->switching 
			+= obtain_switching(pmwindex, pspec->dio->pmwindow);
		(pmwindow + pmwindex)->switching 
			+= obtain_switching(pmwindex, pspec->irf->pmwindow);
		(pmwindow + pmwindex)->switching 
			+= obtain_switching(pmwindex, pspec->fprf->pmwindow);
		if(pspec->bimod)
		(pmwindow + pmwindex)->switching 
			+= obtain_switching(pmwindex, pspec->bimod->pmwindow);
		if(pspec->lev1)
		(pmwindow + pmwindex)->switching 
			+= obtain_switching(pmwindex, pspec->lev1->pmwindow);
		if(pspec->lev2)
		(pmwindow + pmwindex)->switching 
			+= obtain_switching(pmwindex, pspec->lev2->pmwindow);
		if(pspec->ras)
		(pmwindow + pmwindex)->switching 
			+= obtain_switching(pmwindex, pspec->ras->pmwindow);
		if(pspec->il1 && (pspec->il1 != pspec->dl1 && pspec->il1 != pspec->dl2))
		(pmwindow + pmwindex)->switching 
			+= obtain_switching(pmwindex, pspec->il1->pmwindow);
		if(pspec->il2 && (pspec->il2 != pspec->dl1 && pspec->il2 != pspec->dl2))
		(pmwindow + pmwindex)->switching 
			+= obtain_switching(pmwindex, pspec->il2->pmwindow);
		if(pspec->dl1)
		(pmwindow + pmwindex)->switching 
			+= obtain_switching(pmwindex, pspec->dl1->pmwindow);
		if(pspec->dl2)
		(pmwindow + pmwindex)->switching 
			+= obtain_switching(pmwindex, pspec->dl2->pmwindow);
		if(pspec->itlb)
		(pmwindow + pmwindex)->switching 
			+= obtain_switching(pmwindex, pspec->itlb->pmwindow);
		if(pspec->dtlb)
		(pmwindow + pmwindex)->switching 
			+= obtain_switching(pmwindex, pspec->dtlb->pmwindow);
		if(pspec->btb)
		(pmwindow + pmwindex)->switching 
			+= obtain_switching(pmwindex, pspec->btb->pmwindow);
//		if(pspec->logic)
//		(pmwindow + pmwindex)->switching 
//			+= obtain_switching(pmwindex, pspec->logic->pmwindow);

		(pmwindow + pmwindex)->switching
			+= obtain_switching(pmwindex, pspec->clock->pmwindow);

		/* integrate all the internal power */
		(pmwindow + pmwindex)->internal
			= obtain_internal(pmwindex, pspec->aio->pmwindow);
		(pmwindow + pmwindex)->internal 
			+= obtain_internal(pmwindex, pspec->dio->pmwindow);
		(pmwindow + pmwindex)->internal 
			+= obtain_internal(pmwindex, pspec->irf->pmwindow);
		(pmwindow + pmwindex)->internal 
			+= obtain_internal(pmwindex, pspec->fprf->pmwindow);
		if(pspec->bimod)
		(pmwindow + pmwindex)->internal 
			+= obtain_internal(pmwindex, pspec->bimod->pmwindow);
		if(pspec->lev1)
		(pmwindow + pmwindex)->internal 
			+= obtain_internal(pmwindex, pspec->lev1->pmwindow);
		if(pspec->lev2)
		(pmwindow + pmwindex)->internal 
			+= obtain_internal(pmwindex, pspec->lev2->pmwindow);
		if(pspec->ras)
		(pmwindow + pmwindex)->internal 
			+= obtain_internal(pmwindex, pspec->ras->pmwindow);
		if(pspec->il1 && (pspec->il1 != pspec->dl1 && pspec->il1 != pspec->dl2))
		(pmwindow + pmwindex)->internal 
			+= obtain_internal(pmwindex, pspec->il1->pmwindow);
		if(pspec->il2 && (pspec->il2 != pspec->dl1 && pspec->il2 != pspec->dl2))
		(pmwindow + pmwindex)->internal 
			+= obtain_internal(pmwindex, pspec->il2->pmwindow);
		if(pspec->dl1)
		(pmwindow + pmwindex)->internal 
			+= obtain_internal(pmwindex, pspec->dl1->pmwindow);
		if(pspec->dl2)
		(pmwindow + pmwindex)->internal 
			+= obtain_internal(pmwindex, pspec->dl2->pmwindow);
		if(pspec->itlb)
		(pmwindow + pmwindex)->internal 
			+= obtain_internal(pmwindex, pspec->itlb->pmwindow);
		if(pspec->dtlb)
		(pmwindow + pmwindex)->internal 
			+= obtain_internal(pmwindex, pspec->dtlb->pmwindow);
		if(pspec->btb)
		(pmwindow + pmwindex)->internal 
			+= obtain_internal(pmwindex, pspec->btb->pmwindow);
		if(pspec->logic)
		(pmwindow + pmwindex)->internal 
			+= obtain_internal(pmwindex, pspec->logic->pmwindow);

#ifdef FP_PANALYZER_H 	
		if(pspec->alu) {
		(pmwindow + pmwindex)->internal 
			+= pspec->alu->alu_power;
			pspec->alu->alu_power = 0;
		}
		if(pspec->mult) {
		(pmwindow + pmwindex)->internal 
			+= pspec->mult->pipe_mult_power[now % 3];
			pspec->mult->pipe_mult_power[now % 3] = 0;
		}
		
		if(pspec->fpu) {
		(pmwindow + pmwindex)->internal 
			+= pspec->fpu->pipe_fpu_power[now % 3];
			pspec->fpu->pipe_fpu_power[now % 3] = 0;
		}

#endif

		(pmwindow + pmwindex)->internal
			+= obtain_internal(pmwindex, pspec->clock->pmwindow);

		/* integrate all the leakage power */
		(pmwindow + pmwindex)->leakage
			= obtain_leakage(pmwindex, pspec->aio->pmwindow);
		(pmwindow + pmwindex)->leakage 
			+= obtain_leakage(pmwindex, pspec->dio->pmwindow);
		(pmwindow + pmwindex)->leakage 
			+= obtain_leakage(pmwindex, pspec->irf->pmwindow);
		(pmwindow + pmwindex)->leakage 
			+= obtain_leakage(pmwindex, pspec->fprf->pmwindow);
		if(pspec->bimod)
		(pmwindow + pmwindex)->leakage 
			+= obtain_leakage(pmwindex, pspec->bimod->pmwindow);
		if(pspec->lev1)
		(pmwindow + pmwindex)->leakage 
			+= obtain_leakage(pmwindex, pspec->lev1->pmwindow);
		if(pspec->lev2)
		(pmwindow + pmwindex)->leakage 
			+= obtain_leakage(pmwindex, pspec->lev2->pmwindow);
		if(pspec->ras)
		(pmwindow + pmwindex)->leakage 
			+= obtain_leakage(pmwindex, pspec->ras->pmwindow);
		if(pspec->il1 && (pspec->il1 != pspec->dl1 && pspec->il1 != pspec->dl2))
		(pmwindow + pmwindex)->leakage 
			+= obtain_leakage(pmwindex, pspec->il1->pmwindow);
		if(pspec->il2 && (pspec->il2 != pspec->dl1 && pspec->il2 != pspec->dl2))
		(pmwindow + pmwindex)->leakage 
			+= obtain_leakage(pmwindex, pspec->il2->pmwindow);
		if(pspec->dl1)
		(pmwindow + pmwindex)->leakage 
			+= obtain_leakage(pmwindex, pspec->dl1->pmwindow);
		if(pspec->dl2)
		(pmwindow + pmwindex)->leakage 
			+= obtain_leakage(pmwindex, pspec->dl2->pmwindow);
		if(pspec->itlb)
		(pmwindow + pmwindex)->leakage 
			+= obtain_leakage(pmwindex, pspec->itlb->pmwindow);
		if(pspec->dtlb)
		(pmwindow + pmwindex)->leakage 
			+= obtain_leakage(pmwindex, pspec->dtlb->pmwindow);
		if(pspec->btb)
		(pmwindow + pmwindex)->leakage 
			+= obtain_leakage(pmwindex, pspec->btb->pmwindow);
//		if(pspec->logic)
//		(pmwindow + pmwindex)->leakage 
//			+= obtain_leakage(pmwindex, pspec->logic->pmwindow);

		(pmwindow + pmwindex)->leakage
			+= obtain_leakage(pmwindex, pspec->clock->pmwindow);
	} 
	else 
	{
		/* analyzer pdissipation  */
		(pmwindow + pmwindex)->switching
			= estimate_pdissipation(pspec->Ceffs->sCeff, pspec->opfreq, pspec->svolt);
		(pmwindow + pmwindex)->internal 
			= estimate_pdissipation(pspec->Ceffs->iCeff, pspec->opfreq, pspec->svolt);
		(pmwindow + pmwindex)->leakage 
			= estimate_pdissipation(pspec->Ceffs->lCeff, pspec->opfreq, pspec->svolt);
	}

	/* obtain total pdissipation of the current cycle */
	(pmwindow + pmwindex)->pdissipation
		= summate_fu_pdissipation(pmwindex, pmwindow);

	/* integrate total pdissipation */
	integrate_fu_pdissipation(pmwindex, pmwindow, pspec->pdissipation);
}

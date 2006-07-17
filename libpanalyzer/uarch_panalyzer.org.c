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
#include "panalyzer.h"
#include "clock_panalyzer.h"
#include "io_panalyzer.h"
#include "cache_panalyzer.h"
#include "cache_panalyzer.h"
#include "memory_panalyzer.h"
#include "uarch_panalyzer.h"
#define MonitoringWindow 100

/* uarch: micro architecture */
fu_uarch_pspec_t *
create_uarch_panalyzer(
	char *name,
	fu_pmodel_mode_t pmodel, /* power model mode */
	double opfreq, double svolt, /* operating frequency/supply voltage */
	fu_clock_pspec_t *clock_pspec,
	fu_io_pspec_t *aio_pspec,
	fu_io_pspec_t *dio_pspec,

	/* effective capacitance for empirical power model */
	double sCeff, double iCeff, double lCeff)
{
	int i;
	fu_uarch_pspec_t *pspec;

	if(!(pspec = (fu_uarch_pspec_t *)malloc(sizeof(fu_uarch_pspec_t))))
		fatal("lack of virtual memory");
	/* default data structure */
	pspec->name = strdup(name);
	pspec->pmodel = pmodel; 
	pspec->opfreq = opfreq; 
	pspec->svolt = Vdd; /* io supply voltage */

	/* create effective capacitance data structure */
	if(!(pspec->Ceffs = (fu_Ceffs_t *)malloc(sizeof(fu_Ceffs_t))))
		fatal("lack of virtual memory");

	/* analytical model */
	if(pmodel == Analytical) 
	{
		pspec->clock = clock_pspec;
		pspec->aio = aio_pspec;
		pspec->dio = dio_pspec;
	}
	/* empirical model */
	else 
		*(pspec->Ceffs) = assign_fu_Ceffs(sCeff, iCeff, lCeff);

	/* create a power statistics */
	if(!(pspec->pdissipation = (fu_pdissipation_t *)malloc(sizeof(fu_pdissipation_t))))
		fatal("lack of virtual memory");

	/* initialize a power statistics */
	initialize_fu_pdissipation(pspec->pdissipation);

	/* initialize power monitoring window */
	for(i = 0; i < MaxPMWindows; i++) 
	{
		initialize_fu_pmwindow(i, pspec->pmwindow);
		if(pmodel == Empirical) 
			(pspec->pmwindow + i)->leakage = estimate_pdissipation(pspec->Ceffs->lCeff, pspec->opfreq, pspec->svolt);
		else
			(pspec->pmwindow + i)->leakage = 0.0;
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
	if(pspec->pmodel == Analytical) {
		/* integrate all the swiching power */
		(pmwindow + pmwindex)->xswitching
			= obtain_xswitching(pmwindex, pspec->clock->pmwindow);
		(pmwindow + pmwindex)->xswitching
			+= obtain_xswitching(pmwindex, pspec->aio->pmwindow);
		(pmwindow + pmwindex)->xswitching 
			+= obtain_xswitching(pmwindex, pspec->dio->pmwindow);

		/* integrate all the internal power */
		(pmwindow + pmwindex)->internal
			= obtain_internal(pmwindex, pspec->clock->pmwindow);
		(pmwindow + pmwindex)->internal
			+= obtain_internal(pmwindex, pspec->aio->pmwindow);
		(pmwindow + pmwindex)->internal 
			+= obtain_internal(pmwindex, pspec->dio->pmwindow);
		(pmwindow + pmwindex)->internal 
			+= obtain_internal(pmwindex, pspec->irf->pmwindow);
		(pmwindow + pmwindex)->internal 
			+= obtain_internal(pmwindex, pspec->frf->pmwindow);
		if(pspec->il1 && (pspec->il1 != pspec->dl1 && pspec->il1 != pspec->dl2))
		(pmwindow + pmwindex)->internal 
			+= obtain_internal(pmwindex, pspec->il1->pmwindow);
		if(pspec->il2 && (pspec->il2 != pspec->dl1 && pspec->il2 != pspec->dl2))
		(pmwindow + pmwindex)->internal += obtain_internal(pmwindex, pspec->il2->pmwindow);
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

		/* integrate all the leakage power */
		(pmwindow + pmwindex)->leakage
			= obtain_leakage(pmwindex, pspec->clock->pmwindow);
		(pmwindow + pmwindex)->leakage
			+= obtain_leakage(pmwindex, pspec->aio->pmwindow);
		(pmwindow + pmwindex)->leakage 
			+= obtain_leakage(pmwindex, pspec->dio->pmwindow);
		(pmwindow + pmwindex)->leakage 
			+= obtain_leakage(pmwindex, pspec->irf->pmwindow);
		(pmwindow + pmwindex)->leakage 
			+= obtain_leakage(pmwindex, pspec->frf->pmwindow);
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
	} 
	else 
	{
		/* analyzer pdissipation  */
		(pmwindow + pmwindex)->xswitching
			= estimate_pdissipation(pspec->Ceffs->xCeff, pspec->opfreq, pspec->svolt);
		(pmwindow + pmwindex)->yswitching
			= estimate_pdissipation(pspec->Ceffs->yCeff, pspec->opfreq, pspec->svolt);
		(pmwindow + pmwindex)->internal 
			= estimate_pdissipation(pspec->Ceffs->iCeff, pspec->opfreq, pspec->svolt);
		(pmwindow + pmwindex)->leakage 
			= estimate_pdissipation(pspec->Ceffs->lCeff, pspec->opfreq, pspec->svolt);
	}

	/* obtain total pdissipation of the current cycle */
	(pmwindow + pmwindex)->pdissipation
		= summate_fu_pdissipation(pmwindex, pmwindow);
	integrate_fu_pdissipation(pmwindex, pmwindow, pspec->pdissipation);
}

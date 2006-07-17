/*
* lv1_uarch_panalyzer.c - contains top level panalyzer. This integrate all the
* power dissipation of a level 1 power model from functional units (e.g., il1 cache, aio).
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


#include <stdio.h>
#include <stdlib.h>
#include "../host.h"
#include "../misc.h"
#include "technology.h"
#include "lv1_opts.h"
#include "lv1_panalyzer.h"
#include "lv1_clock_panalyzer.h"
#include "lv1_io_panalyzer.h"
//#include "memory_panalyzer.h"
#include "lv1_cache_panalyzer.h"
#include "lv1_bpred_panalyzer.h"
#include "lv1_alu_panalyzer.h"
#include "lv1_mult_panalyzer.h"
#include "lv1_fpu_panalyzer.h"
#include "lv1_rf_panalyzer.h"
#include "lv1_uarch_panalyzer.h"
   
#define MonitoringWindow 128

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
	)
{
	fu_lv1_uarch_pspec_t *pspec;
	int i;

	if(!(pspec = (fu_lv1_uarch_pspec_t *)malloc(sizeof(fu_lv1_uarch_pspec_t))))
		fatal("lack of virtual memory");
	/* default data structure */
	pspec->name = strdup(name);
	pspec->opfreq = opfreq; 
	pspec->volt = volt; /* io supply voltage */
	pspec->max_power = 0;
	pspec->uarch_power = 0;
	pspec->uarch_total_power = 0;
//	pspec->aio = aio; pspec->dio = dio;
	pspec->rf = rf;
	pspec->alu = alu; 
	pspec->bpred = bpred; 
	pspec->fpu = fpu; 
	pspec->mult = mult; 
	pspec->il1 = il1;
	pspec->dl1 = dl1;
	pspec->il2 = il2;
	pspec->dl2 = dl2;
	pspec->itlb = itlb;
	pspec->dtlb = dtlb;
//	pspec->clock = clock;
	

	return pspec;
}

/* top level panalyzer: 
 * this routine integrate all the power dissipation from the functional units 
 * at each simulation cycle */
void 
lv1_uarch_panalyzer(
	fu_lv1_uarch_pspec_t *pspec,
	tick_t now,
	void * user_data
	)
{
	// double pdissipation = 0.0;
	
	double dynamic_power; 
	/* integrate_fu_pdissipation(pmwindex, 
					pspec->aio->pmwindow, pspec->aio->pdissipation);
	integrate_fu_pdissipation(pmwindex, 
	 				pspec->dio->pmwindow, pspec->dio->pdissipation); */
	/* integrate uarch power */	
	fu_lv1_pwr_frame_t * pwr_frame = (fu_lv1_pwr_frame_t * ) user_data;
	fu_lv1_pwr_frame_t * root_frame = (fu_lv1_pwr_frame_t * ) user_data;
	

	dynamic_power = pspec->rf->rf_power + 
	pspec->alu->alu_power + 	
	pspec->bpred->bpred_power + 	
	pspec->fpu->fpu_power + 	
	pspec->mult->mult_power + 	
	pspec->il1->cache_power + 	
	pspec->dl1->cache_power + 	
	pspec->il2->cache_power + 	
	pspec->dl2->cache_power + 	
	pspec->itlb->cache_power + 	
	pspec->dtlb->cache_power; // + 	

	if(pwr_frame->next != NULL)
	{
		pwr_frame = pwr_frame->next; 
		/* evaluate frame */
		if((now - pwr_frame->frame_start) < pwr_frame->frame_size)
		 {
			 int offset = now - pwr_frame->frame_start - 1;
			 if(now < 0 )
				 fatal("Insanity checking %d\n",now);
			 dynamic_power += pwr_frame->frame_contents[offset].pdissipation;  
#ifdef PA_TRANS_COUNT
			 pspec->aio_trans = pwr_frame->frame_contents[offset].aio_trans;
			 pspec->dio_trans = pwr_frame->frame_contents[offset].dio_trans;
#endif			 			 
		 }
		 else
		 {
			 delete_pwr_frame(pwr_frame,root_frame);
		 }
	}
	
	pspec->uarch_total_power += 
		dynamic_power; 	

	pspec->uarch_power = 
		dynamic_power;
		
	/* clear power that has been evaluated */
/*	pspec->rf->rf_power= 0;	 	
	pspec->alu->alu_power= 0;	 	
	pspec->fpu->fpu_power= 0;	 	
	pspec->mult->mult_power= 0;	 	
	pspec->bpred->bpred_power= 0;	 	
	pspec->il1->cache_power= 0;	 	
	pspec->dl1->cache_power= 0;	 	
	pspec->il2->cache_power= 0;	 	
	pspec->dl2->cache_power= 0;	 	
	pspec->itlb->cache_power= 0;	 	
	pspec->dtlb->cache_power= 0;
	pspec->uarch_power = 0;	 	 */
//	pspec->clock->clock_power= 0;	 	

	pspec->max_power = (dynamic_power > pspec->max_power) ? dynamic_power : pspec->max_power; 
}
void 
lv1_uarch_clear_panalyzer(
	fu_lv1_uarch_pspec_t *pspec
	)
{
	pspec->rf->rf_power= 0;	 	
	pspec->alu->alu_power= 0;	 	
	pspec->fpu->fpu_power= 0;	 	
	pspec->mult->mult_power= 0;	 	
	pspec->bpred->bpred_power= 0;	 	
	pspec->il1->cache_power= 0;	 	
	pspec->dl1->cache_power= 0;	 	
	pspec->il2->cache_power= 0;	 	
	pspec->dl2->cache_power= 0;	 	
	pspec->itlb->cache_power= 0;	 	
	pspec->dtlb->cache_power= 0;
	pspec->uarch_power = 0;	 
#ifdef PA_TRANS_COUNT
	pspec->aio_trans = 0;
	pspec->dio_trans = 0;
#endif			
}


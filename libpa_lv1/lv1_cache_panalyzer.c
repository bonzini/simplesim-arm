/*
* cache_panalyzer.c - cache structure power analysis tools for 
* il1, il2, dl1, dl2, itlb, dtlb.
*
* This file is a part of the PowerAnalyzer tool suite written by
* Nam Sung Kim as a part of the PowerAnalyzer Project.
*  
* The tool suite is currently maintained by Nam Sung Kim.
* 
* Copyright (C) 2001 by Nam Sung Kim
*  
* Revised by Taeho Kgil
* Level 1 Power model analysis 
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

#include <string.h>
#include <math.h>
#include "../misc.h"
#include "../stats.h"

#include "lv1_opts.h"
#include "lv1_panalyzer.h"
/* #include "lv1_memory_panalyzer.h" */
#include "lv1_cache_panalyzer.h"

/* create level 1 power model cache panalyzer database
 * return an allocated location pointer.
 * caution: please deallocate the cache space  */
fu_lv1_cache_pspec_t *
create_cache_panalyzer(
	char *name, /* cache name */
	int freq,double volt,
	double iCeff,double eCeff)
{
	fu_lv1_cache_pspec_t *pspec;

	fprintf(stderr, "\n****************************************************\n");
	fprintf(stderr, "\t\t level 1 power model cache panalyzer configuration\n");
	fprintf(stderr,   "****************************************************\n");
	if(!(pspec = (fu_lv1_cache_pspec_t *)malloc(sizeof(fu_lv1_cache_pspec_t))))
		fatal("lack of virtual memory");

	pspec->name = strdup(name);
    pspec->iCeff = iCeff;
    pspec->eCeff = eCeff;
	pspec->volt = volt;
	pspec->freq = freq;
	pspec->cache_access = 0;
	pspec->cache_total_power = 0;
	pspec->cache_power = 0;
	pspec->max_power = 0;
	

	fprintf(stderr,   "**********************************************\n");
	fprintf(stderr, "Effective CAP : %lf\n", iCeff + eCeff);

	return pspec;
}

/* level 1 power model cache panalyzer */
void 
lv1_cache_panalyzer(
	fu_lv1_cache_pspec_t *pspec, 
	fu_mcommand_t mcommand  /* cache command: Read/Write */
	)
{
	double dynamic_power;
	if(mcommand == Read)
		dynamic_power = 0.5 * pspec->freq * pspec->iCeff * ((pspec->volt) * (pspec->volt));   
	if(mcommand == Write)
		dynamic_power = 0.5 * pspec->freq * (pspec->iCeff + pspec->eCeff) * ((pspec->volt) * (pspec->volt));   
	// power estimated from access
	pspec->cache_access++;
	pspec->cache_total_power += dynamic_power; 
	pspec->cache_power += dynamic_power; 
	pspec->max_power = 
		(pspec->max_power > dynamic_power) ? pspec->max_power:dynamic_power;     
}

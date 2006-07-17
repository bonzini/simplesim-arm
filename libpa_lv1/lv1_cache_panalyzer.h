/*
* cache_panalyzer.h - cache structure power analysis tools for 
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

#ifndef CACHE_PANALYZER_H 
#define CACHE_PANALYZER_H

#include "../cache.h"
#include "../stats.h"
//#include "lv1_panalyzer.h"
/* #include "lv1_memory_panalyzer.h" */

/* number of virtual address */
#define NVA	40
/* number of physicalo address */
#define NPA 32

/* cache power specification type:
 * contain all the info for analyzing cache pdissipation */
typedef struct _fu_lv1_cache_pspec_t {
	char *name; /* name */
	double freq, volt; /* operating frequency/supply voltage */
	double iCeff,eCeff;
	unsigned long cache_access;
	double cache_total_power;
	double cache_power;
	double max_power;
} fu_lv1_cache_pspec_t;

/* create level 1 power model cache panalyzer database
 * return an allocated location pointer.
 * caution: please deallocate the cache space  */
fu_lv1_cache_pspec_t *
create_cache_panalyzer(
	char *name, /* cache name */
	int freq,double volt,
	double iCeff,double eCeff);

/* level 1 power model cache panalyzer */
void 
lv1_cache_panalyzer(
	fu_lv1_cache_pspec_t *pspec, 
	fu_mcommand_t mcommand  /* cache command: Read/Write */
	);
#endif /* CACHE_PANALYZER_H */

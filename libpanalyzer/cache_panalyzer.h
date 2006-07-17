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
#include "panalyzer.h"
#include "memory_panalyzer.h"

/* number of virtual address */
#define NVA	40
/* number of physicalo address */
#define NPA 32

/* cache power specification type:
 * contain all the info for analyzing cache pdissipation */
typedef struct _fu_cache_pspec_t {
	char *name; /* name */
	fu_pmodel_mode_t pmodel; /* power model mode */
	double opfreq, svolt; /* operating frequency/supply voltage */
	fu_dimension_t *dimension;

	unsigned nsets, bsize, assoc;
	unsigned nbls, nwls;
	unsigned nacycles; /* cache access time in cycles */
	unsigned ntcols, ndcols;
	fu_sbank_pspec_t *t_pspec, *d_pspec; /* tag / data array pspecs */

	fu_Ceffs_t *t_Ceffs, *d_Ceffs;  /* effective pdissipation capacitances */
	fu_pdissipation_t *pdissipation; /* pdissipation statistics */
	fu_pdissipation_t pmwindow[MaxPMWindows]; /* power monitoring window */
} fu_cache_pspec_t;

/* create cache panalyzer database
 * return an allocated location pointer.
 * caution: please deallocate the cache space  */
fu_cache_pspec_t *
create_cache_panalyzer(
	char *name, /* cache name */
	fu_pmodel_mode_t pmodel, /* cache power model mode */
	double opfreq, double svolt, /* operating frequency/supply voltage */

	/*  n: number of
	 * nsets: sets, bsize: line or block size, assoc: associativity
	 * nbls : bitlines in a cache array 
	 * nwls : wordlines in a cache array 
	 * nrwports: read/write. nrports: read. nwports: write ports */
	unsigned nsets, unsigned bsize, unsigned assoc, 
	unsigned nbls, unsigned nwls, 
	unsigned nrwports, unsigned nrports, unsigned nwports, 

	/* switching/internal/lekage  effective capacitances of the array */
	double sCeff, double iCeff, double lCeff);

/* cache panalyzer */
void 
cache_panalyzer(
	fu_cache_pspec_t *pspec, 
	fu_mcommand_t mcommand,  /* cache command: Read/Write */
	fu_address_t address, /* cache access starting address */
	buffer_t *dbus, /* commnication buffer between sim-outorder / panalyzer */
	unsigned bsize, /* transaction size */
	tick_t now, /* current cycle */
	unsigned lat /* access latency from sim-outorder */);
#endif /* CACHE_PANALYZER_H */

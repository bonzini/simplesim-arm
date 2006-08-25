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
#include "technology.h"
#include "panalyzer.h"
#include "memory_panalyzer.h"
#include "cache_panalyzer.h"

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
	double sCeff, double iCeff, double lCeff)
{
	fu_cache_pspec_t *pspec;
	unsigned ntcols, ndcols, ntlbits; 
	char tname[20], dname[20];
	int i;

	fprintf(stderr, "\n**********************************************\n");
	fprintf(stderr, "\t\tcache panalyzer configuration\n");
	fprintf(stderr,   "**********************************************\n");
	if(!(pspec = (fu_cache_pspec_t *)malloc(sizeof(fu_cache_pspec_t))))
		fatal("lack of virtual memory");

	pspec->name = strdup(name);
	pspec->pmodel = pmodel;
	pspec->opfreq = opfreq;
    pspec->svolt = svolt;

    if(!(pspec->dimension = (fu_dimension_t *)malloc(sizeof(fu_dimension_t))))
		fatal("lack of virtual memory!");


	/* cache parameters */
    pspec->nsets = nsets;
    pspec->bsize = bsize;
    pspec->assoc = assoc;
    pspec->nbls = nbls;
    pspec->nwls = nwls;
	fprintf(stderr, "number of sets in cache: %u\n", nsets);
	if(strcmp(name, "itlb") || !strcmp(name, "dtlb") || !strcmp(name, "btb"))
	fprintf(stderr, "block size of cache: %u (bytes)\n", bsize);
	fprintf(stderr, "number of assoc in cache: %u\n", assoc);
	fprintf(stderr, "number of word-line segments (nwls) in cache: %u\n", nwls);
	fprintf(stderr, "number of bit-line segments (nbls) in cache: %u\n", nbls);
	fprintf(stderr, "number of sub-arrays in cache: %u\n", assoc * nbls * nwls);

	/* obtain number of bits in one tag entry line */
	pspec->ntcols = ntcols = NVA - (log_base2(bsize) + log_base2(nsets));

	/** create tag array **/
	strcpy(tname, name);
	strcat(tname, "_tag sub-array");
	pspec->t_pspec = create_sbank_panalyzer(
	tname, pmodel, opfreq, svolt,
	nsets / nbls, ntcols,
	nrwports, nrports, nwports,
	estimate_trCeff(1.), 0., 0.);

	if(!strcmp(name, "itlb") || !strcmp(name, "dtlb") || !strcmp(name, "btb"))
		pspec->ndcols = ndcols = NVA;
	else
		pspec->ndcols = ndcols = bsize * 8;

	/** create data array **/
	strcpy(dname, name);
	strcat(dname, "_data sub-array");
	pspec->d_pspec = create_sbank_panalyzer(
	dname, /* memory name */
	pmodel, opfreq, svolt,
	nsets / nbls, ndcols,
	nrwports, nrports, nwports,
	sCeff, 0., 0.);

	/* create effective capacitance data structure for tag array */
	if(!(pspec->t_Ceffs = (fu_Ceffs_t *)malloc(sizeof(fu_Ceffs_t))))
		fatal("lack of virtual memory");
	/* create effective capacitance data structure for data array  */
	if(!(pspec->d_Ceffs = (fu_Ceffs_t *)malloc(sizeof(fu_Ceffs_t))))
		fatal("lack of virtual memory");

	/* analytical model */
	if(pmodel == Analytical) 
	{
		pspec->t_Ceffs->sCeff 
				= pspec->t_pspec->Ceffs->sCeff;
		pspec->t_Ceffs->iCeff 
				= pspec->t_pspec->Ceffs->iCeff * (double)(nbls * assoc);
		pspec->t_Ceffs->lCeff 
				= pspec->t_pspec->Ceffs->lCeff * (double)(nbls * assoc);
		pspec->t_Ceffs->cnodeCeff 
				= pspec->t_pspec->Ceffs->cnodeCeff * (double)(nbls * assoc);
		
		pspec->d_Ceffs->sCeff 
				= pspec->d_pspec->Ceffs->sCeff;
		pspec->d_Ceffs->iCeff 
				= pspec->d_pspec->Ceffs->iCeff * (double)(nbls * assoc);
		pspec->d_Ceffs->lCeff 
				= pspec->d_pspec->Ceffs->lCeff * (double)(nbls * assoc);
		pspec->d_Ceffs->cnodeCeff 
				= pspec->d_pspec->Ceffs->cnodeCeff * (double)(nbls * assoc);

		pspec->dimension->width 
				= pspec->t_pspec->dimension->width * (double)assoc
				+ pspec->d_pspec->dimension->width * (double)assoc; 
		pspec->dimension->height 
				= pspec->t_pspec->dimension->height * (double)assoc
				+ pspec->d_pspec->dimension->height * (double)assoc; 

		/* estimate total area */
		pspec->dimension->area 
				= pspec->dimension->width * pspec->dimension->height; 
	}
	else 
	{
		unsigned ntbits;
		ntbits = nsets * assoc * ntcols;
		pspec->t_Ceffs->sCeff = sCeff;
		pspec->t_Ceffs->iCeff = iCeff * (double)ntbits;
		pspec->t_Ceffs->lCeff = iCeff * (double)ntbits;

		ntbits = nsets * assoc * (bsize * 8);
		pspec->d_Ceffs->sCeff = sCeff; 
		pspec->d_Ceffs->iCeff = iCeff * (double)ntbits;
		pspec->d_Ceffs->lCeff = iCeff * (double)ntbits;
	}
	fprintf(stderr,   "**********************************************\n");
	fprintf(stderr, "total memory area: %3.4e (um^2)\n", pspec->dimension->area);
	fprintf(stderr, "total external switching capacitance: %3.4e (pF)\n", 
					(pspec->t_Ceffs->sCeff + pspec->d_Ceffs->sCeff )* 1E12);
	fprintf(stderr, "total internal switching capacitance: %3.4e (pF)\n", 
					(pspec->t_Ceffs->iCeff + pspec->d_Ceffs->iCeff )* 1E12);
	fprintf(stderr, "total effective leakage capacitance: %3.4e (pF)\n", 
					(pspec->t_Ceffs->lCeff + pspec->d_Ceffs->lCeff )* 1E12);
	fprintf(stderr, "total effective clocked capacitance: %3.4e (pF)\n", 
					(pspec->t_Ceffs->cnodeCeff + pspec->d_Ceffs->cnodeCeff )* 1E12);

    /* initialize a power statistics db structure */
	if(!(pspec->pdissipation = (fu_pdissipation_t *)malloc(sizeof(fu_pdissipation_t)))) 
		fatal("lack of virtual memory");
	initialize_fu_pdissipation(pspec->pdissipation); 

    /* initialize a power monitor window */
	for(i = 0; i < MaxPMWindows; i++) {
		initialize_fu_pmwindow(i, pspec->pmwindow); 
		/* put leakage power numbers in all the power window entry
		 * b/c leakage is access independent and swithing independent */
		(pspec->pmwindow + i)->leakage 
		= estimate_pdissipation(pspec->t_Ceffs->lCeff, pspec->opfreq, pspec->svolt)
		+ estimate_pdissipation(pspec->d_Ceffs->lCeff, pspec->opfreq, pspec->svolt);
	}

	return pspec;
}

/* cache panalyzer */
void 
cache_panalyzer(
	fu_cache_pspec_t *pspec, 
	fu_mcommand_t mcommand,  /* cache command: Read/Write */
	fu_address_t address, /* cache access starting address */
	buffer_t *dbus, /* commnication buffer between sim-outorder / panalyzer */
	unsigned bsize, /* transaction size */
	tick_t now, /* current cycle */
	unsigned lat /* access latency from sim-outorder */)
{
	int i;
	fu_pdissipation_t *pmwindow;
	/* xbus: address in bus, t_ybus: tag out bus, d_ybus: data out bus */
	buffer_t *tbus;
	fu_address_t taddress; /* set address */
	unsigned pmwindex;
	double switching, internal;
//    static tick_t past;
    static tick_t past = -1;

	pmwindow = pspec->pmwindow;
	
	/* tag array */
	/* address generation */
	taddress = address >> (log_base2(pspec->bsize) + log_base2(pspec->nsets));
	tbus = create_buffer_t(&taddress, pspec->t_pspec->bsize);
	sbank_panalyzer(pspec->t_pspec, tbus, now);
	free(tbus);

	/* data array */
	sbank_panalyzer(pspec->d_pspec, dbus, now);

	pmwindex = now % MaxPMWindows;
	switching 
		= obtain_switching(pmwindex, pspec->t_pspec->pmwindow)
		+ obtain_switching(pmwindex, pspec->d_pspec->pmwindow);

	internal
		= obtain_internal(pmwindex, pspec->t_pspec->pmwindow)
		+ obtain_internal(pmwindex, pspec->d_pspec->pmwindow);
	internal = internal * (double)(pspec->nbls * pspec->nwls * pspec->assoc);

	pmwindex = (now + lat) % MaxPMWindows;

	if(past != now)
	{
		initialize_fu_pmwindow(pmwindex, pmwindow);
		past = now;
	}

	/* data transition dependent pdissipation */
	(pmwindow + pmwindex)->switching += switching;

	/* transition independent pdissipation */
	(pmwindow + pmwindex)->internal += internal;

	(pmwindow + pmwindex)->pdissipation
		= summate_fu_pdissipation(pmwindex, pmwindow);
}

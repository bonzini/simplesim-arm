/*
* io_panalyzer.c - I/O power analysis tools for address/data bus.
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

#include <stdlib.h>
#include <string.h>
#include "../host.h"
#include "../machine.h"
#include "../stats.h"
#include "../misc.h"
#include "technology.h"
#include "panalyzer.h"
#include "circuit_analyzer.h"
#include "io_panalyzer.h"
#include "microstrip.h"

/* create io panalyzer database
 * return an allocated location pointer.
 * caution: please deallocate the memory space  */
fu_io_pspec_t *
create_io_panalyzer(
	char *name, /* io name */
	fu_pmodel_mode_t pmodel, /* io power model mode */
	double opfreq, /* operating frequency */ 
	double iovolt,
	/* io specific parameters */
	fu_io_style_t style, /* io style */
	unsigned nbstages_opt, /* optimial number of io buffer stages */
	double strip_length, /* length of microstrip */
	unsigned buswidth, /* bus width */
	unsigned nacycles, unsigned nctcycles, /* io access/cycle time in cycles */
	
	unsigned bsize, /* bus size in bytes */
	/* switching/internal/lekage  effective capacitances */
	double sCeff, double iCeff, double lCeff)
{
	int i;
	fu_io_pspec_t *pspec;

	fprintf(stderr, "\n**********************************************\n");
	fprintf(stderr, "\t\tio panalyzer configuration\n");
	fprintf(stderr,   "**********************************************\n");

	if(!(pspec = (fu_io_pspec_t *)malloc(sizeof(fu_io_pspec_t))))
		fatal("lack of virtual memory");

	fprintf(stderr, "name: %s\n", name);
	/* default data structure */
	pspec->name = strdup(name);
	pspec->pmodel = pmodel; 
	pspec->opfreq = opfreq; 
	pspec->svolt = iovolt; /* io supply voltage */

	if(pmodel == Analytical)
	fprintf(stderr, "pmodel type: analytical power model\n");
	else
	fprintf(stderr, "pmodel type: empirical power model\n");
	
	/* io specific parameters */
	pspec->style = style; 
	pspec->nbstages_opt = nbstages_opt; 
	pspec->buswidth = buswidth; 
	pspec->nacycles = nacycles; 
	pspec->nctcycles = nctcycles; 
	pspec->bsize = bsize;

	/* address bus size in bytes */
	//fprintf(stderr, "bus size:%u\t", bsize);
	/* create bus buffer */
	if(!(pspec->bus = (buffer_t *)calloc(bsize,sizeof(buffer_t))))
	// if(!(pspec->bus = (buffer_t *)malloc(bsize * sizeof(buffer_t))))
		fatal("lack of virtual memory");

	/* create effective capacitance data structure */
	if(!(pspec->Ceffs = (fu_Ceffs_t *)malloc(sizeof(fu_Ceffs_t))))
		fatal("lack of virtual memory");

	/* analytical power model */
	if(pmodel == Analytical) 
	{
		double btr_opt;
		double microCeff;
		microstrip_t micro_spec;
		micro_spec.dielectric = MICROSTRIP_ER;  
		micro_spec.width = MICROSTRIP_H;  
		micro_spec.thickness = MICROSTRIP_T;  
		micro_spec.height = MICROSTRIP_W;  
		micro_spec.length = strip_length;  
		microCeff = estimate_microstrip_capacitance(&micro_spec);
	fprintf(stderr, "strip_length =  %lf\n", strip_length);
	fprintf(stderr, "microCeff %3.4e\n", microCeff);
//	getchar();
		btr_opt = estimate_btr_opt(nbstages_opt, sCeff + microCeff);
	fprintf(stderr, "optimal buffer transistor ratio: %3.4e\n", btr_opt);
		pspec->Ceffs->sCeff = sCeff + microCeff /* (packaging cap) */;
		pspec->Ceffs->iCeff = estimate_ibufchainiCeff(btr_opt, sCeff + microCeff);
		pspec->Ceffs->lCeff 
				= estimate_ibufchainlCeff(nbstages_opt, btr_opt, pspec->opfreq, pspec->svolt) * (double)(bsize * 8); 
	}
	/* empirical model */
	else 
		*(pspec->Ceffs) = assign_fu_Ceffs(sCeff, iCeff, lCeff * (double)(bsize * 8), 0.);

	fprintf(stderr, "external switching capacitance: %3.4e (pF)\n", pspec->Ceffs->sCeff * 1E12);
	fprintf(stderr, "internal switching capacitance: %3.4e (pF)\n", pspec->Ceffs->iCeff * 1E12);
	fprintf(stderr, "effective leakage capacitance: %3.4e (pF)\n", pspec->Ceffs->lCeff * 1E12);

	/* create a power statistics db */
	
//	if(!(pspec->pdissipation = (fu_pdissipation_t *)malloc(sizeof(fu_pdissipation_t))))
	if(!(pspec->pdissipation = (fu_pdissipation_t *)calloc(1,sizeof(fu_pdissipation_t))))
		fatal("lack of virtual memory");
	/* initialize a power statistics db structure */
	initialize_fu_pdissipation(pspec->pdissipation); 

	/* initialize a power monitor window */
	for(i = 0; i < MaxPMWindows; i++) 
	{
		initialize_fu_pmwindow(i, pspec->pmwindow); 
		/* put leakage power numbers in all the power window entry 
		 * b/c leakage is access independent and swithing independent */
		(pspec->pmwindow + i)->leakage = estimate_pdissipation(pspec->Ceffs->lCeff, pspec->opfreq, pspec->svolt);
		(pspec->pmwindow + i)->pdissipation = (pspec->pmwindow + i)->leakage;
	}
	
	return pspec;
}

/* analyze address io pdissipation */
void 
aio_panalyzer(
	fu_io_pspec_t *pspec, 
	fu_mcommand_t mcommand, /* io command: Read/Write */
	fu_address_t address, /* io access starting address */
	buffer_t *buffer, /* commnication buffer between sim-outorder / panalyzer */
	unsigned bsize, /* io transaction size */
	tick_t now,	/* current cycle */
	unsigned lat /* access latency from sim-outorder */)
{
	int i;
	tick_t tstart; /* transaction start sim_cycle */
	fu_pdissipation_t *pmwindow; /* power monitoring windows */
	buffer_t *bus; /* bus */
	unsigned pmwindex;
   	unsigned tsize, chunks, plat;
	unsigned nswitchings;

	pmwindow = pspec->pmwindow;
	/* number of fetching data chunks */
	chunks = (bsize + (pspec->buswidth - 1)) / pspec->buswidth;
	/* pure memory access latency */
	plat = (/* first chunk latency */pspec->nacycles +
				(/* remainder chunk latency */pspec->nctcycles * (chunks - 1)));

	/* transaction start time */
	/* lat is pure memory access latency + memory_ready time */
	/* Ludo: added some casts here, since all variables are unsigned! This solves some segmentation faults. */
	if(((int)lat - (int)plat - (int)pspec->nctcycles) > 0)
	// if((lat - plat - pspec->nctcycles) > 0)
		tstart = now + lat - plat - pspec->nctcycles;
	else
		tstart = now;

	/* modeling high impedence state */
	if(pspec->style == odirBuffer) {
		/* transaction size */
//		tsize = bsize / pspec->buswidth; 
       /* Ludo: transaction size, why is this not the same as chunks above? */
       tsize = bsize / pspec->buswidth; /* the number of transfers that have to be done over the bus */
	   /* measure each address transaction power */
		for(i = 0; i < tsize; i++) {
			/* address generation from io_buffer */
			address = address + i * pspec->buswidth;
			bus = create_buffer_t(&address, pspec->bsize);
			/* obtain number of transitions */
			nswitchings = obtain_nswitchings(bus, pspec->bus, pspec->bsize);
			copy_buffer_t(bus, pspec->bus, pspec->bsize);
			free(bus);

			/* obtain an appropriate transaction cycle 
			 * and power monitor window index */
			if(i == 0)
				pmwindex = tstart % MaxPMWindows;
			else
				pmwindex = (tstart + (pspec->nacycles + (i - 1) * pspec->nctcycles)) % MaxPMWindows;

			/* analyze pdissipation  */
			(pmwindow + pmwindex)->switching 
				= (double)nswitchings * estimate_pdissipation(pspec->Ceffs->sCeff, pspec->opfreq, pspec->svolt);
			(pmwindow + pmwindex)->internal 
				= (double)nswitchings * estimate_pdissipation(pspec->Ceffs->iCeff, pspec->opfreq, pspec->svolt);
			/* obtain total pdissipation of the current cycle */
			(pmwindow + pmwindex)->pdissipation
				= summate_fu_pdissipation(pmwindex, pmwindow);
		}
	}
	else
		fatal("other type than output type io is not supported yet!");
}

/* analyze data io pdissipation */
void 
dio_panalyzer(
	fu_io_pspec_t *pspec, 
	fu_mcommand_t mcommand, /* io command: Read/Write */
	fu_address_t address, /* io access starting address */
	buffer_t *buffer, /* commnication buffer between sim-outorder / panalyzer */
	unsigned bsize, /* io transaction size */
	tick_t now,	/* current cycle */
	unsigned lat /* access latency from sim-outorder */)
{
	int i;
	tick_t tstart; /* transaction start sim_cycle */
	fu_pdissipation_t *pmwindow;
	buffer_t *bus;
	unsigned pmwindex, tsize, chunks, plat;
	unsigned nswitchings;
	
	pmwindow = pspec->pmwindow;
	/* number of fetching data chunks */
	chunks = (bsize + (pspec->buswidth - 1)) / pspec->buswidth;
	/* pure memory access latency */
	plat = (/* first chunk latency */pspec->nacycles +
				(/* remainder chunk latency */pspec->nctcycles * (chunks - 1)));
	
	/* transaction start time */
	/* lat is pure memory access latency + memory_ready time */
	if((lat - plat - pspec->nctcycles) > 0)
		tstart = now + lat - plat - pspec->nctcycles;
	else
		tstart = now;
	
	if(pspec->style == odirBuffer || pspec->style == bidirBuffer) {
		/* transaction size */
		tsize = bsize / pspec->buswidth;
		for(i = 0; i < tsize; i++) {
			bus = buffer + i * pspec->bsize;
			/* obtain number of transitions */
			nswitchings = obtain_nswitchings(bus, pspec->bus, pspec->bsize);
			copy_buffer_t(bus, pspec->bus, pspec->bsize);

			/* obtain an appropriate transaction cycle 
			 * and power monitor window index */
			/* write operation */
			if(mcommand == Write ) {
			if(i == 0)
				pmwindex = tstart % MaxPMWindows;
			else
				pmwindex 
				= (tstart + (pspec->nacycles + (i - 1) * pspec->nctcycles)) % MaxPMWindows;
			}
			/* read operation */
			else {
				pmwindex 
				= (tstart + (pspec->nacycles + i * pspec->nctcycles)) % MaxPMWindows;
			}
			/* analyze pdissipation  */
			(pmwindow + pmwindex)->switching 
				= (double)nswitchings * estimate_pdissipation(pspec->Ceffs->sCeff, pspec->opfreq, pspec->svolt);
			(pmwindow + pmwindex)->internal 
				= (double)nswitchings * estimate_pdissipation(pspec->Ceffs->iCeff, pspec->opfreq, pspec->svolt);

			/* obtain total pdissipation */
			(pmwindow + pmwindex)->pdissipation
				= summate_fu_pdissipation(pmwindex, pmwindow);
		}
	}
	else
		fatal("dio should be either odir or bidirBuffer type!");
}

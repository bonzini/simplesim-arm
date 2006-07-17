/*
* lv1_io_panalyzer.c - Level 1 I/O power analysis tools for address/data bus.
*
* This file is a part of the PowerAnalyzer tool suite written by
* Nam Sung Kim as a part of the PowerAnalyzer Project.
*  
* The tool suite is currently maintained by Nam Sung Kim.
* 
* Copyright (C) 2001 by Nam Sung Kim
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

#include <stdlib.h>
#include <string.h>
#include "../host.h"
#include "../machine.h"
#include "../stats.h"
#include "../misc.h"
#include "technology.h"
#include "lv1_opts.h"
#include "lv1_panalyzer.h"
#include "lv1_io_panalyzer.h"

/* create io panalyzer database
 * return an allocated location pointer.
 * caution: please deallocate the memory space  */
fu_lv1_io_pspec_t *
lv1_create_io_panalyzer(
	char *name, /* io name */
	double opfreq, /* operating frequency */ 
	
	double svolt, /* IO voltage supply */
	/* io specific parameters */
	fu_io_style_t style,
	unsigned buswidth, /* bus width */
 	unsigned nacycles, unsigned nctcycles, /* io access/cycle time in cycles */
	
	unsigned bsize, /* bus size in bytes */
	/* internal / external  effective capacitances */
	double iCeff, double eCeff)
{
	int i;
	fu_lv1_io_pspec_t *pspec;

	fprintf(stderr, "\n**********************************************\n");
	fprintf(stderr, "\t\tLevel 1 io panalyzer configuration\n");
	fprintf(stderr,   "**********************************************\n");

	if(!(pspec = (fu_lv1_io_pspec_t *)malloc(sizeof(fu_lv1_io_pspec_t))))
		fatal("lack of virtual memory");

	fprintf(stderr, "name: %s\n", name);
	/* default data structure */
	pspec->name = strdup(name);
	pspec->opfreq = opfreq; 
	pspec->svolt = svolt; /* io supply voltage */
	pspec->style = style;
	pspec->io_access = 0;
	
	/* io specific parameters */
	pspec->buswidth = buswidth; 
	pspec->nacycles = nacycles; 
	pspec->nctcycles = nctcycles; 
	pspec->bsize = bsize;


	/* initialize power stats */
	pspec->aio_total_power = 0;
	pspec->dio_total_power = 0;
	pspec->max_aio_power = 0;
	pspec->max_dio_power = 0;
	pspec->aio_power = 0;
	pspec->dio_power = 0;
	
	/* address bus size in bytes */
	//fprintf(stderr, "bus size:%u\t", bsize);
	/* create bus buffer */
	if(!(pspec->bus = (buffer_t *)malloc(bsize * sizeof(buffer_t))))
		fatal("lack of virtual memory");

	/* create effective capacitance data structure */
	if(!(pspec->Ceffs = (fu_lv1_Ceffs_t *)malloc(sizeof(fu_lv1_Ceffs_t))))
		fatal("lack of virtual memory");

	pspec->Ceffs->eCeff = eCeff ; 
	pspec->Ceffs->iCeff = iCeff;

	fprintf(stderr, "external switching capacitance: %3.4e (pF)\n", pspec->Ceffs->eCeff * 1E12);
	fprintf(stderr, "internal switching capacitance: %3.4e (pF)\n", pspec->Ceffs->iCeff * 1E12);

	/* create a power statistics db */
	if(!(pspec->pdissipation = (fu_lv1_pdissipation_t *)malloc(sizeof(fu_lv1_pdissipation_t))))
		fatal("lack of virtual memory");
	/* initialize a power statistics db structure */
	initialize_fu_lv1_pdissipation(pspec->pdissipation); 

	/* initialize a power monitor window */
	for(i = 0; i < MaxPMWindows; i++) 
	{
		initialize_fu_lv1_pmwindow(i, pspec->pmwindow); 
	}
	
	return pspec;
}

/* analyze address io pdissipation */
void 
lv1_aio_panalyzer(
	fu_lv1_io_pspec_t *pspec, 
	fu_mcommand_t mcommand, /* io command: Read/Write */
	fu_address_t address, /* io access starting address */
	buffer_t *buffer, /* commnication buffer between sim-outorder / panalyzer */
	fu_lv1_pwr_frame_t * pwr_frame,  /* power frame used to reconstruct memory transaction */ 
	unsigned bsize, /* io transaction size */
	tick_t now,	/* current cycle */
	unsigned lat /* access latency from sim-outorder */)
{
	int i;
	tick_t tstart; /* transaction start sim_cycle */
	fu_lv1_pdissipation_t *io_pmwindow; /* power monitoring windows */
	buffer_t *bus; /* bus */
	unsigned pmwindex;
   	unsigned tsize, chunks, plat;
	unsigned nswitchings;

	io_pmwindow = pspec->pmwindow;
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


	/* modeling high impedence state */
	if(pspec->style == odirBuffer) {
		/* transaction size */
		tsize = bsize / pspec->buswidth; 
		/* measure each address transaction power */

	 memset(pwr_frame->frame_contents,0,
         sizeof(fu_lv1_pdissipation_t) * pwr_frame->frame_size);/* clear power frame */
		 
		for(i = 0; i < tsize; i++) {

			pspec->io_access++;

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
				pmwindex = 0;
			else
				pmwindex = (pspec->nacycles + (i - 1) * pspec->nctcycles);

			/* analyze pdissipation  */
		//	(io_pmwindow + pmwindex)->external 
		//		= (double)nswitchings * estimate_pdissipation(pspec->Ceffs->eCeff, pspec->opfreq, pspec->svolt);
		//	(io_pmwindow + pmwindex)->internal 
		//		= (double)nswitchings * estimate_pdissipation(pspec->Ceffs->iCeff, pspec->opfreq, pspec->svolt);

			pspec->aio_power =  (double) (nswitchings * 
				estimate_pdissipation(pspec->Ceffs->eCeff, pspec->opfreq, pspec->svolt)) + 
				(double) (nswitchings * 
				estimate_pdissipation(pspec->Ceffs->iCeff, pspec->opfreq, pspec->svolt));	
			pspec->aio_total_power +=  pspec->aio_power;	
			pspec->max_aio_power =  (pspec->max_aio_power > pspec->aio_power) ? 
			pspec->max_aio_power : pspec->aio_power;	

#ifdef PA_TRANS_COUNT
			pwr_frame->frame_contents[pmwindex].aio_trans = nswitchings; 
#endif

			/* capture power frame for uarch peak power */
			pwr_frame->frame_contents[pmwindex].internal = (double) (nswitchings * 
				estimate_pdissipation(pspec->Ceffs->iCeff, pspec->opfreq, pspec->svolt));
			pwr_frame->frame_contents[pmwindex].external = (double) (nswitchings * 
				estimate_pdissipation(pspec->Ceffs->eCeff, pspec->opfreq, pspec->svolt));
			pwr_frame->frame_contents[pmwindex].pdissipation = 
				pwr_frame->frame_contents[pmwindex].external + 
				pwr_frame->frame_contents[pmwindex].internal;
		 
		}
	}
	else
		fatal("other type than output type io is not supported yet!");
}

/* analyze data io pdissipation */
void 
lv1_dio_panalyzer(
	fu_lv1_io_pspec_t *pspec, 
	fu_mcommand_t mcommand, /* io command: Read/Write */
	fu_address_t address, /* io access starting address */
	buffer_t *buffer, /* commnication buffer between sim-outorder / panalyzer */
	fu_lv1_pwr_frame_t * pwr_frame,  /* power frame used to reconstruct memory transaction */ 
	unsigned bsize, /* io transaction size */
	tick_t now,	/* current cycle */
	unsigned lat /* access latency from sim-outorder */)
{
	int i;
	tick_t tstart; /* transaction start sim_cycle */
	fu_lv1_pdissipation_t *io_pmwindow;
	buffer_t *bus;
	unsigned pmwindex, tsize, chunks, plat;
	unsigned nswitchings;
	
	io_pmwindow = pspec->pmwindow;
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
	//	 memset(pwr_frame->frame_contents,0,
      //   sizeof(fu_lv1_pdissipation_t) * pwr_frame->frame_size);/* clear power frame */

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
				pmwindex = 0;
			else
				pmwindex 
				= pspec->nacycles + (i - 1) * pspec->nctcycles;
			}
			/* read operation */
			else {
				pmwindex 
				= pspec->nacycles + (i - 1) * pspec->nctcycles;
			}
			/* analyze pdissipation  */
/*			(io_pmwindow + pmwindex)->external 
				= (double)nswitchings * estimate_pdissipation(pspec->Ceffs->eCeff, pspec->opfreq, pspec->svolt);
			(io_pmwindow + pmwindex)->internal 
				= (double)nswitchings * estimate_pdissipation(pspec->Ceffs->iCeff, pspec->opfreq, pspec->svolt);
*/
			/* power dissipated from DRAM for Reads */
			if(mcommand == Write) { 
			pspec->dio_power =  (double) (nswitchings * 
				estimate_pdissipation(pspec->Ceffs->eCeff, pspec->opfreq, pspec->svolt)) + 
				(double) (nswitchings * 
				estimate_pdissipation(pspec->Ceffs->iCeff, pspec->opfreq, pspec->svolt));	
			pspec->dio_total_power +=  pspec->dio_power;	
			pspec->max_dio_power =  (pspec->max_dio_power > pspec->dio_power) ? 
			  pspec->max_dio_power : pspec->dio_power;	

#ifdef PA_TRANS_COUNT
			pwr_frame->frame_contents[pmwindex].dio_trans = nswitchings; 
#endif

			/* capture power frame for uarch peak power */
			pwr_frame->frame_contents[pmwindex].internal += (double) (nswitchings * 
				estimate_pdissipation(pspec->Ceffs->iCeff, pspec->opfreq, pspec->svolt));
			pwr_frame->frame_contents[pmwindex].external += (double) (nswitchings * 
				estimate_pdissipation(pspec->Ceffs->eCeff, pspec->opfreq, pspec->svolt)); 
			pwr_frame->frame_contents[pmwindex].pdissipation += 
				pwr_frame->frame_contents[pmwindex].external + 
				pwr_frame->frame_contents[pmwindex].internal;
			}
		}
	}
	else
		fatal("dio should be either odir or bidirBuffer type!");
}

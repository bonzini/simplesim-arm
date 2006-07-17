/*
* lv1_io_panalyzer.h - Level 1 I/O power analysis tools for address/data bus.
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

#ifndef IO_PANALYZER_H 
#define IO_PANALYZER_H

#include "../stats.h"
#include "technology.h"

/* io power specification type: 
 * contain all the info for analyzing io pdissipation */
typedef struct _fu_lv1_io_pspec_t {
	char *name;	/* name */
	double opfreq, svolt; /* operating frequency/supply voltage */
	
	double aio_total_power;
	double dio_total_power;

#ifdef PA_TRANS_COUNT
	int aio_tran;
	int dio_tran;
#endif	
	
	double max_aio_power;
	double max_dio_power;

	double aio_power;
	double dio_power;
	
	int io_access; 

	fu_io_style_t style; /* io style */

	unsigned buswidth; /* io bus width */
	unsigned nacycles, nctcycles; /* io access/cycle time in cycles */

    buffer_t *bus; /* bus */
	unsigned bsize; /* bus size in bytes */
	
	fu_lv1_Ceffs_t *Ceffs;	/* effective pdissipation capacitances */
	fu_lv1_pdissipation_t *pdissipation; /* pdissipation statistics */	
	fu_lv1_pdissipation_t pmwindow[MaxPMWindows]; /* power monitoring window */
} fu_lv1_io_pspec_t;

/* create io panalyzer database
 * return an allocated location pointer.
 * caution: please deallocate the memory space  */
fu_lv1_io_pspec_t *
lv1_create_io_panalyzer(
	char *name, /* io name */
	double opfreq, /* operating frequency */ 
	double svolt, /* IO voltage supply */

	/* io specific parameters */
	fu_io_style_t style, /* io style */
	
	unsigned buswidth, /* bus width */
	unsigned nacycles, unsigned nctcycles, /* io access/cycle time in cycles */
	
	unsigned bsize, /* bus size in bytes */
	/* internal / exteral  effective capacitances */
	double iCeff, double eCeff);

/* analyze aio pdissipation */
void 
lv1_aio_panalyzer(
	fu_lv1_io_pspec_t *pspec, 
	fu_mcommand_t mcommand, /* io command: Read/Write */
	fu_address_t address, /* io access starting address */
	buffer_t *buffer, /* commnication buffer between sim-outorder / panalyzer */
	fu_lv1_pwr_frame_t * pwr_frame, /* power frame used to reconstruct memory transaction */
	unsigned bsize, /* io transaction size */
	tick_t now,	/* current cycle */
	unsigned lat /* access latency from sim-outorder */);

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
	unsigned lat /* access latency from sim-outorder */);
#endif /* IO_PANALYZER_H */

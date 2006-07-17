/*
* io_panalyzer.h - I/O power analysis tools for address/data bus.
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

#ifndef IO_PANALYZER_H 
#define IO_PANALYZER_H

#include "../stats.h"
#include "technology.h"
#include "panalyzer.h"

/* io style */
typedef enum _fu_io_style_t {
	idirBuffer, /* in direction */
	odirBuffer, /* out direction */
	bidirBuffer /* in/out direction */
} fu_io_style_t;

/* io power specification type: 
 * contain all the info for analyzing io pdissipation */
typedef struct _fu_io_pspec_t {
	char *name;	/* name */
	fu_pmodel_mode_t pmodel; /* power model mode */
	double opfreq, svolt; /* operating frequency/supply voltage */

	fu_io_style_t style; /* io style */
	unsigned nbstages_opt; /* optimial number of io buffer stages */
	unsigned buswidth; /* io bus width */
	unsigned nacycles, nctcycles; /* io access/cycle time in cycles */

    buffer_t *bus; /* bus */
	unsigned bsize; /* bus size in bytes */

	fu_Ceffs_t *Ceffs;	/* effective pdissipation capacitances */
	fu_pdissipation_t *pdissipation; /* pdissipation statistics */	
	fu_pdissipation_t pmwindow[MaxPMWindows]; /* power monitoring window */
} fu_io_pspec_t;

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
	double sCeff, double iCeff, double lCeff);

/* analyze aio pdissipation */
void 
aio_panalyzer(
	fu_io_pspec_t *pspec, 
	fu_mcommand_t mcommand, /* io command: Read/Write */
	fu_address_t address, /* io access starting address */
	buffer_t *buffer, /* commnication buffer between sim-outorder / panalyzer */
	unsigned bsize, /* io transaction size */
	tick_t now,	/* current cycle */
	unsigned lat /* access latency from sim-outorder */);

/* analyze data io pdissipation */
void 
dio_panalyzer(
	fu_io_pspec_t *pspec, 
	fu_mcommand_t mcommand, /* io command: Read/Write */
	fu_address_t address, /* io access starting address */
	buffer_t *buffer, /* commnication buffer between sim-outorder / panalyzer */
	unsigned bsize, /* io transaction size */
	tick_t now,	/* current cycle */
	unsigned lat /* access latency from sim-outorder */);
#endif /* IO_PANALYZER_H */

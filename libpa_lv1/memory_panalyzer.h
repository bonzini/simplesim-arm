/*
* memory_panalyzer.h - basic memory power analyzer data types, strcutres and 
* thier manipulation functions. 
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

#ifndef MEMORY_PANALYZER_H
#define MEMORY_PANALYZER_H

#include "panalyzer.h"

#define _6TWIDTH 1.8 /* um */
#define _6THEIGHT 3.6 /* um */

typedef struct _fu_sbank_pspec_t {
	char *name; /* name */
	fu_pmodel_mode_t pmodel; /* power model mode */
	double opfreq, svolt; /* operating frequency/supply voltage */
    fu_dimension_t *dimension;

	/* sbank specification */
	unsigned nrows, ncols; /* array size */
	unsigned nrwports, nrports, nwports; /* number of ports spec. */

	/* nrows: number of entries / ncols: number of columns in bits */
	buffer_t *bus; /* output bus */
	unsigned bsize; /* output size in bytes */

	/* effective pdissipation capacitances */
	fu_Ceffs_t *Ceffs;
	fu_pdissipation_t *pdissipation; /* pdissipation statistics */
	fu_pdissipation_t pmwindow[MaxPMWindows]; /* power monitoring window */
} fu_sbank_pspec_t;

/* this is for small memory array */
/* create memory panalyzer database
 * return an allocated location pointer.
 * caution: please deallocate the memory space  */
fu_sbank_pspec_t *
create_sbank_panalyzer(
	char *name, /* memory name */
	fu_pmodel_mode_t pmodel, /* bpred power model mode */
	/* memory operating parameters: operating frequency/supply voltage */
	double opfreq, double svolt, 

	/* memory size parameters: number of rows / cols (2^n) */
	unsigned nrows, unsigned ncols, 
	unsigned nrwports, unsigned nrports, unsigned nwports,

	/* switching/internal/lekage  effective capacitances */
	double sCeff, double iCeff, double lCeff);

/* sbank panalyzer: estimate pdissipatino of the structure with the given
 * in/out bus value and access statistics */
void 
sbank_panalyzer(
	fu_sbank_pspec_t *pspec, /* memory pspec*/
	buffer_t *buffer, /* out bus */
	tick_t now /* current simulation time */);

/* estimate sbankCeff */
fu_Ceffs_t
estimate_sbankCeff(
	fu_sbank_pspec_t *pspec,
	double sCeff);
#endif /* MEMORY_PANALYZER_H */

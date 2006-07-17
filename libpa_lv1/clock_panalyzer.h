/*
* clock_panalyzer.h - basic clock power analyzer data types, strcutres and 
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

#ifndef CLOCK_PANALYZER_H
#define CLOCK_PANALYZER_H
#include "panalyzer.h"

/* clock clocktree style - this should be done by configurator - future work! */
typedef enum _fu_clocktree_style_t{
    Htree,
    balHtree
} fu_clocktree_style_t;

typedef struct _fu_clock_pspec_t {
	char *name; /* name */
	fu_pmodel_mode_t pmodel; /* power model mode */
	double opfreq, svolt; /* operating frequency/supply voltage */
	fu_dimension_t *dimension;	

	fu_clocktree_style_t ctree_style; /* clock tree style */
	double cskew; /* clock skew */
	double tcnodeCeff; /* total clocked node capacitnace */
	double ncbstages_opt; /* number of clock buffer stages */

	/* effective pdissipation capacitances */
	fu_Ceffs_t *Ceffs;
	fu_pdissipation_t *pdissipation; /* pdissipation statistics */
	fu_pdissipation_t pmwindow[MaxPMWindows]; /* power monitoring window */
} fu_clock_pspec_t;

fu_clock_pspec_t *
create_clock_panalyzer(
	char *name, /* memory name */
	fu_pmodel_mode_t pmodel, /* clock tree power model mode */
	/* memory operating parameters: operating frequency/supply voltage */
	double opfreq, double svolt, 
	double tdarea, /* total die area in um^2 */
	double tcnodeCeff, /* total clocked node capacitnace */
	fu_clocktree_style_t ctree_style, /* clock tree style */
	double cskew, /* clock skew */
	unsigned ncbstages_opt, /* optimial number of clock buffer stages */

	/* switching/internal/lekage  effective capacitances */
	double sCeff, double iCeff, double lCeff);

/* clock panalyzer: estimate pdissipatino of the structure */
void 
clock_panalyzer(
	fu_clock_pspec_t *pspec, /* memory pspec*/
	tick_t now /* current simulation time */);

/* estimate total clock distribution tree wire */
double 
estimate_tcdtwireCeff(
	fu_clock_pspec_t *pspec);

/* estimate clock distribution tree branch wire length */
double 
estimate_lbr(
	int N, /* Nth branch */
	double ledge /* edge length */);

/* estimate clock distribution tree and buffer capacitance */
fu_Ceffs_t 
estimate_tcdtCeff(
	fu_clock_pspec_t *pspec);

#endif /* CLOCK_PANALYZER_H */


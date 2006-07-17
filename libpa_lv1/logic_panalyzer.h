/*
* logic_panalyzer.h - basic random logic power analyzer data types, 
* strcutres and thier manipulation functions. 
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

#ifndef LOGIC_PANALYZER_H
#define LOGIC_PANALYZER_H
#include "panalyzer.h"

/* clock clocktree style - this should be done by configurator - future work! */
typedef enum _fu_logic_style_t{
    Static,
    Dynamic,
} fu_logic_style_t;

typedef struct _fu_logic_pspec_t {
	char *name; /* name */
	fu_pmodel_mode_t pmodel; /* power model mode */
	double opfreq, svolt; /* operating frequency/supply voltage */
	fu_dimension_t *dimension;	

	fu_logic_style_t logic_style; /* logic tree style */

	/* effective pdissipation capacitances */
	fu_Ceffs_t *Ceffs;
	fu_pdissipation_t *pdissipation; /* pdissipation statistics */
	fu_pdissipation_t pmwindow[MaxPMWindows]; /* power monitoring window */
} fu_logic_pspec_t;

double
estimate_nandCeff(
	double k, /* transistor sizing ratio */
	double fi /* average fanin */);

#endif /* LOGIC_PANALYZER_H */


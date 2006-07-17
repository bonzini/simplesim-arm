/*
* panalyzer.c - basic power analyzer data types, strcutres and 
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

#include <stdio.h>
#include <string.h>
#include <math.h>
#include "../host.h"
#include "../misc.h"
#include "technology.h"
#include "panalyzer.h"

__inline__
/* create/converte to buffer_t type data structure for various data types.
 * return an allocated location pointer.
 * caution: please deallocate the memory space  */
buffer_t *
create_buffer_t(
	void *val, 	/* target conversion value */
	int bsize 	/* conversion value data type size in bytes */)
{
	int i;
	buffer_t *p, *vp;

	if(!(p = (buffer_t *)calloc(bsize, sizeof(buffer_t))))
		fatal("out of virtual memory!");
	vp = val;
	for(i = 0; i < bsize; i++)
		*(p+i) = *(vp+(bsize-1-i));

	return p;
}

__inline__
/* copy buffer_t type data structure */
void
copy_buffer_t(
	buffer_t *ibus, /* the source bus */ 
	buffer_t *jbus, /* the destination bus */
	unsigned bsize /* size of the bus in bytes */)
{
	int i;

	for(i = 0; i < bsize; i++)
		*(ibus + i) = *(jbus + i);
}

__inline__
/* return number of switching for the give previous bus and the current bus */
unsigned
obtain_nswitchings(
	buffer_t *ibus, /* the current bus */ 
	buffer_t *jbus, /* the previous bus */
	unsigned bsize /* size of the bus in bytes */)
{
	unsigned i;
	unsigned nswitchings;
	buffer_t tbus;

	nswitchings = 0;
	for(i = 0; i < bsize; i++) {
		tbus = *(ibus + i) ^ *(jbus + i);
		while(tbus) {
			if(tbus & 0x01)
				nswitchings++;
			tbus >>= 1;
		}
	}
	return nswitchings;
}

__inline__
/* obtain x switching power from fu_pdissipation_t data structure */
double
obtain_switching(
	unsigned pmwindex, /* power monitoring access index */
	fu_pdissipation_t *pmwindow /* power monitoring window */)
{
	double switching;

	switching = (pmwindow + pmwindex)->switching;
	(pmwindow + pmwindex)->switching = 0.0;

	return switching;
}

__inline__
/* obtain internal power from fu_pdissipation_t data structure */
double
obtain_internal(
	unsigned pmwindex, /* power monitoring access index */
	fu_pdissipation_t *pmwindow /* power monitoring window */)
{
	double internal;

	internal = (pmwindow + pmwindex)->internal;
	(pmwindow + pmwindex)->internal = 0.0;

	return internal;
}

__inline__
/* obtain leakage power from fu_pdissipation_t data structure */
double
obtain_leakage(
	unsigned pmwindex, /* power monitoring access index */
	fu_pdissipation_t *pmwindow /* power monitoring window */)
{
	double leakage;

	leakage = (pmwindow + pmwindex)->leakage;
	//(pmwindow + pmwindex)->leakage = 0.0;
	return leakage;
}

__inline__
/* initialize pdissipation stat */
void 
initialize_fu_pdissipation(
	fu_pdissipation_t *pdissipation /* pdissipation stat ptr */)
{
	pdissipation->switching = 0.0;
	pdissipation->internal = 0.0;
	pdissipation->leakage = 0.0;
	pdissipation->pdissipation = 0.0;
	pdissipation->peak = 0.0;
}

__inline__
/* initialize pmwindow 
 * we dont touch the leakage power and peak power in pmwindows */
void 
initialize_fu_pmwindow(
	unsigned pmwindex, /* fu power monitoring window index */
	fu_pdissipation_t *pmwindow /* fu power monitoring window */)
{
	(pmwindow+pmwindex)->switching = 0.0;
	(pmwindow+pmwindex)->internal = 0.0;
	(pmwindow+pmwindex)->pdissipation = 0.0;
}

__inline__
/* integrate the current power dissipation of fu from power monitoring windows 
 * to pdissipation stat db. */ 
double 
integrate_fu_pdissipation(
	unsigned pmwindex,	/* fu power monitoring window index */
	fu_pdissipation_t *pmwindow, /* fu power monitoring window */
	fu_pdissipation_t *pdissipation	/* fu power dissipation statistics */)
{
	/* for each funtional unit */
	pdissipation->switching += (pmwindow + pmwindex)->switching;
	pdissipation->internal += (pmwindow + pmwindex)->internal;
	pdissipation->leakage += (pmwindow + pmwindex)->leakage;
	pdissipation->pdissipation += (pmwindow + pmwindex)->pdissipation;
	/* is this a peak power value? */
	if((pmwindow + pmwindex)->pdissipation >  pdissipation->peak)
	pdissipation->peak =  (pmwindow + pmwindex)->pdissipation;

	return (pmwindow + pmwindex)->pdissipation;
}

__inline__
/* summate switching + internal + leakage pdissipation of the current cycle.
 * return the summated pdissipation */
double 
summate_fu_pdissipation(
	unsigned pmwindex,				/* fu power monitoring window index */
	fu_pdissipation_t *pmwindow	/* fu power monitoring window */)
{
	double pdissipation;

	pdissipation 
		= (pmwindow + pmwindex)->switching
		+ (pmwindow + pmwindex)->internal
		+ (pmwindow + pmwindex)->leakage;

	return pdissipation;
}

__inline__
fu_Ceffs_t
assign_fu_Ceffs(
	double sCeff,
	double iCeff,
	double lCeff,
	double cnodeCeff)
{
	fu_Ceffs_t tCeffs, *Ceffs = &tCeffs;

	Ceffs->sCeff = sCeff;
	Ceffs->iCeff = iCeff;
	Ceffs->lCeff = lCeff;
	Ceffs->cnodeCeff = cnodeCeff;

	return tCeffs;
}
__inline__
/* return pdissipation 
 * caution: this function must incorporate with ntransitions */
double 
estimate_pdissipation(
	double Ceff, /* effective capacitance */
	double opfreq, /* operating frequency */
	double svolt /* supply voltage */) 
{
	return (Ceff * opfreq * (svolt * svolt));
}
/* power analyzer stats register */
void
panalyzer_reg_stats(
	char *name, /* fu name */
	fu_pdissipation_t *pdissipation, /* fu pdissipation  statistics db ptr */
	struct stat_sdb_t *sdb /* stats db ptr */)
{
	char buf[512], buf1[512], buf2[512];

	/* get a name for this cache */
	if (!name || !name[0])
		name = "<unknown>";

	sprintf(buf, "%s.switching", name);
	sprintf(buf1, "%s total in switching power dissipation", name);
	stat_reg_double(sdb, buf, buf1, &pdissipation->switching, 0.0, "%12.4f");
	sprintf(buf, "%s.avgswitching", name);
	sprintf(buf1, "%s.switching / sim_cycle", name);
	sprintf(buf2, "%s avg in switching power dissipation", name);
	stat_reg_formula(sdb, buf, buf2, buf1, "%12.4f");

	sprintf(buf, "%s.internal", name);
	sprintf(buf1, "%s total internal power dissipation", name);
	stat_reg_double(sdb, buf, buf1, &pdissipation->internal, 0.0, "%12.4f");
	sprintf(buf, "%s.avginternal", name);
	sprintf(buf1, "%s.internal / sim_cycle", name);
	sprintf(buf2, "%s avg internal power dissipation", name);
	stat_reg_formula(sdb, buf, buf2, buf1, "%12.4f");

	sprintf(buf, "%s.leakage", name);
	sprintf(buf1, "%s total leakage power dissipation", name);
	stat_reg_double(sdb, buf, buf1, &pdissipation->leakage, 0.0, "%12.4f");
	sprintf(buf, "%s.avgleakage", name);
	sprintf(buf1, "%s.leakage / sim_cycle", name);
	sprintf(buf2, "%s avg leakage power dissipation", name);
	stat_reg_formula(sdb, buf, buf2, buf1, "%12.4f");

	sprintf(buf, "%s.pdissipation", name);
	sprintf(buf1, "%s total power dissipation", name);
	stat_reg_double(sdb, buf, buf1, &pdissipation->pdissipation, 0.0, "%12.4f");
	sprintf(buf, "%s.avgpdissipation", name);
	sprintf(buf1, "%s.pdissipation / sim_cycle", name);
	sprintf(buf2, "%s avg power dissipation", name);
	stat_reg_formula(sdb, buf, buf2, buf1, "%12.4f");

	sprintf(buf, "%s.peak", name);
	sprintf(buf1, "%s peak power dissipation", name);
	stat_reg_double(sdb, buf, buf1, &pdissipation->peak, 0.0, "%12.4f");
}

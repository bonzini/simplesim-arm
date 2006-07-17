/*
* circuit_analyzer.c - circuits and transistor capacitance, leakage
* analysis functions.
* Some estimation functions are obtained from CACTI.
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
#include <math.h>
#include "technology.h"
#include "circuit_analyzer.h"

__inline__
/* estimate leakage current: return in A */
double 
estimate_ilekage( 
	double weff	/* in um */)
{
	/* leakage estimation for DSM transistor at 50C */
	return (10.0 * weff * pow(10.0, -Vth / 95E-3) * 1E-6);
}

__inline__
/* estimate effective leakage capacitance from leakage current. 
 * return in F */
double 
estimate_lCeff(
	double weff, /* in um */
	double opfreq, /* in Hz */
	double opvolt /* in V  */)
{
	double ileakage;

	ileakage = estimate_ilekage(weff);

	/* VxI = P = E/T = Exfreq 
	 * E = VxI/f = 0.5xCxV^2 */
	return (ileakage / (opfreq * opvolt) * 2.);
}

__inline__
/* estimate optimal transistor stage ratio for cascade inverter buffer */
/* b: buffer */
double 
estimate_btr_opt(
	unsigned nbstages_opt,	/* optimial number of buffer stages */ 
	double sCeff)
{
	double btr_opt, ginmininvCeff;

	ginmininvCeff = estimate_gininvCeff(1.);
	btr_opt = pow((sCeff / ginmininvCeff), (1.0 / (double)nbstages_opt));
	return btr_opt;
}

__inline__
/* estimate inverter buffer chain capacitance */
double 
estimate_ibufchainiCeff(
	double btr_opt,	/* optimial buffer stages ratio */ 
	double sCeff)
{
	double btrCeff;

	btrCeff = sCeff * ( 1. / (1. - 1. / btr_opt));

	return btrCeff;
}

/* estimate leakage effective capacitance of the cascaded inverter buffers 
 * return in F */
double 
estimate_ibufchainlCeff(
	unsigned nbstages_opt,/* optimial number of buffer stages */
	double btr_opt, /* optimal buffer transistor stage ratio */
	double opfreq,
	double opvolt)
{
	int i;
	double lCeff, sweff;
	
	sweff = 0.0;
	/* summate all the Weff in the inverter buffer chain */
	/* beta + 1 is the total width of minimum inverter transistors */
	for(i = 0; i < nbstages_opt; i++)
		sweff += pow(btr_opt, i) * ((beta + 1.0) * Weff) / 2.0;
	lCeff = estimate_lCeff(sweff, opfreq,  opvolt);

	return lCeff;
}

/* returns gate capacitance in Farads */
double 
gatecap(
	double weff,		/* gate width in um (length is Leff) */
	double Lpoly		/* poly wire length going to gate in lambda */)
{
	return weff * Leff * Cgate + Lpoly * Leff * Cpolywire;
}

/* returns gate capacitance in Farads */
double 
gatecappass(
	double weff, 		/* gate width in um (length is Leff) */
	double Lpoly	/* poly wire length going to gate in lambda */) 
{
	return weff * Leff * Cgatepass + Lpoly * Leff * Cpolywire;
}

/* Routine for calculating drain capacitances.  The draincap routine 
 * folds transistors larger than 10um */

/* returns drain cap in Farads */
double 
draincap(
	double weff, 	/* um */
	int nchannel, 	/* whether n or p-channel (boolean) */
	int stack 		/* number of transistors in series that are on */)  
{
	double Cdiffside, Cdiffarea, Coverlap, Cdrain;

	Cdiffside = (nchannel) ? Cndiffside : Cpdiffside;
	Cdiffarea = (nchannel) ? Cndiffarea : Cpdiffarea;
	Coverlap 
		= (nchannel) ? (Cndiffovlp + Cnoxideovlp) : (Cpdiffovlp + Cpoxideovlp);

	/* calculate directly-connected (non-stacked) capacitance */
	/* then add in capacitance due to stacking */
	if(weff > 10.0) {
		Cdrain	
			= 3.0 * Leff * weff / 2.0 * Cdiffarea 
			+ 6.0 * Leff * Cdiffside 
			+ weff * Coverlap;
		Cdrain += (double)(stack-1)*(Leff*weff*Cdiffarea+4.0*Leff*Cdiffside+2.0*weff*Coverlap);
	} 
	else {
		Cdrain 
			= 3.0 * Leff * weff * Cdiffarea + (6.0 * Leff + weff) * Cdiffside 
			+ weff * Coverlap;
		Cdrain += (double)(stack-1)*(Leff*weff*Cdiffarea+2.0*Leff*Cdiffside+2.0*weff*Coverlap);
	}

	return Cdrain;
}

/* The following routines estimate the effective resistance of an
   on transistor as described in the tech report.  The first routine
   gives the "switching" resistance, and the second gives the 
   "full-on" resistance */

/* returns resistance in ohms */
double 
transresswitch( 
	double weff,	/* um */ 
	int nchannel,	/* whether n or p-channel (boolean) */
	int stack		/* number of transistors in series */)
{
	double restrans;

	restrans = (nchannel) ? (Rnchannelstatic): (Rpchannelstatic);

	/* calculate resistance of stack - assume all but switching trans 
	 * have 0.8X the resistance since they are on throughout switching */
	return (1.0 + ((double)(stack - 1) * 0.8)) * restrans / weff;	
}

/* returns resistance in ohms */
double 
transreson(
	double weff, 	/* um */
	int nchannel, 	/* whether n or p-channel (boolean) */
	int stack		/* number of transistors in series */)  
{
	double restrans;

	restrans = (nchannel) ? Rnchannelon : Rpchannelon;

	/* calculate resistance of stack.  Unlike transres, we don't 
	 * multiply the stacked transistors by 0.8 */
	return (double)stack * restrans / weff;
}


/* This routine operates in reverse: given a resistance, it finds
 * the transistor weff that would have this R.  It is used in the
 * data wordline to estimate the wordline driver size. */

/* returns weff in um */
double 
restowidth(
	double res, 	/* resistance in ohms */
	int nchannel 	/* whether N-channel or P-channel */)  
{
	double restrans;

	restrans = (nchannel) ? Rnchannelon : Rpchannelon;

	return restrans/res;
}

double 
horowitz(
	double inputramptime,	/* input rise time */
	double tf,				/* time constant of gate */
	double vs1,
	double vs2,				/* threshold voltages */
	int rise				/* whether INPUT rise or fall (boolean) */)
{
    double a, b, td;

    a = inputramptime / tf;

    if (rise == RISE) {
       b = 0.5;
       td 	= tf * sqrt(log(vs1) * log(vs1) + 2.0 * a * b * (1.0-vs1)) 
			+ tf *(log(vs1) - log(vs2));
    } 
	else {
       b = 0.4;
       td 	= tf * sqrt(log(1.0 - vs1) * log(1.0 - vs1) + 2.0 * a * b * vs1) 
			+ tf * (log(1.0 - vs1) - log(1.0 - vs2));
    }

    return td;
}

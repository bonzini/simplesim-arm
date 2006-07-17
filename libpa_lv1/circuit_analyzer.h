/*
* circuit_analyzer.h - circuits and transistor capacitance, leakage
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

#ifndef CIRCUIT_ANALYZER_H
#define CIRCUIT_ANALYZER_H

/* estimate leakage current 
 * return in A */
/* i: current */
double 
estimate_ilekage(
	double weff	/* in um */);

/* estimate effective leakage capacitance 
 * return in F */
double 
estimate_lCeff(
	double weff, /* in um */
	double opfreq, /* in Hz */
	double opvolt /* in V  */);

/* estimate optimal transistor stage ratio for cascade inverter buffer */
/* b: buffer */
double 
estimate_btr_opt(
	unsigned nbstages_opt,	/* optimial number of buffer stages */ 
	double sCeff);

double 
estimate_ibufchainiCeff(
	double btr_opt,	/* optimial buffer stages ratio */ 
	double sCeff);

/* estimate leakage effective capacitance of the cascaded inverter buffers 
 * return in F */
double 
estimate_ibufchainlCeff(
	unsigned nbstages_opt,/* optimial number of buffer stages */
	double btr_opt, /* optimal buffer transistor stage ratio */
	double opfreq,
	double opvolt);


/* Used to communicate with the horowitz model */
#define RISE 1
#define FALL 0
#define NCH  1
#define PCH  0

/* returns gate capacitance in Farads */
double gatecap(
	double weff, 		/* gate Weff in um (length is Leff) */
	double wirelength	/* poly wire length going to gate in lambda */);

/* returns gate capacitance in Farads */
double gatecappass(
	double weff, 		/* gate Weff in um (length is Leff) */
	double wirelength	/* poly wire length going to gate in lambda */); 

/* returns drain cap in Farads */
double draincap(
	double weff, 	/* um */
	int nchannel, 	/* whether n or p-channel (boolean) */
	int stack 		/* number of transistors in series that are on */); 

/* returns resistance in ohms */
double transresswitch( 
	double weff,	/* um */ 
	int nchannel,	/* whether n or p-channel (boolean) */
	int stack		/* number of transistors in series */);

/* returns resistance in ohms */
double transreson(
	double weff, 	/* um */
	int nchannel, 	/* whether n or p-channel (boolean) */
	int stack		/* number of transistors in series */);

/* returns weff in um */
double restowidth(
	double res, 	/* resistance in ohms */
	int nchannel 	/* whether N-channel or P-channel */); 

double horowitz(
	double inputramptime,	/* input rise time */
	double tf,				/* time constant of gate */
	double vs1,
	double vs2,				/* threshold voltages */
	int rise				/* whether INPUT rise or fall (boolean) */);
#endif /* CIRCUIT_ANALYZER_H */

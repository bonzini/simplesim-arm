/*
* technology.h - 0.18 um technology process parameters.  
* The process parameters are obtained from MOSIS web site (http://www.mosis.org) 
* and a part of data are  obtained from CACTI.
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

#ifndef TECHNOLOGY_H
#define TECHNOLOGY_H
/* from MOSIS parametric test result for TSMC 0.18um technology */
/* Ld		: LINT in BSIM model */
/* Wd		: WINT in BSIM model */
/* Cja0		: CJ in BSIM model */
/* Cjp0		: CJSW in BSIM model */
/* Vbi		: PB in BSIM model */

#define tech_180nm

#ifdef tech_180nm
#define sgwireC 	(414.0E-18) /* F/um */
#define gwireC 		(440.0E-18) /* F/um */
#define sgwireR 	(96.0E-3)   /* Ohm/um */
#define gwireR 		(20.0E-3)   /* Ohm/um */
#define nVth		(0.3) 			/* minimum transistor NMOS Vth */
#define pVth		(0.34)			/* minimum transistor PMOS Vth */
#define Vth			(nVth / 2. + pVth / 2.) 
#define Ltech		(0.18)				/* min technology channel width in um */
#define Ld			(0.007631769) 		/* channel length overlap in um  */
#define Leff		(Ltech - 2. * Ld)	/* min effective channel legth */
#define Wtech       (0.27)				/* min technology channel width in um */
#define Wd			(0.001798714) 		/* channel width overlap in um */
#define Weff        (Wtech - 2. *Wd) 	/* min effective channel width in um */
#define eox			(3.9 * 8.85E-18)	/* permiability of Si in F/um */
#define tox			(4.1E-3)			/* SiO2 thinkness in um */
#define pmobility	(84.78)				/* p-type mobility */
#define nmobility	(409.16)			/* n-type mobility */
#define beta		(nmobility / pmobility)
#define Cox			(eox / tox)
#define nIdss		(546E-6) 	/* NMOS saturation current A/um */
#define pIdss		(256E-6) 	/* PMOS saturation current A/um */
#define nRc			(11.3)		/* n-active contact resistance */		
#define pRc			(11.8)		/* p-active contact resistance */		
#define Vdd			(1.8)			/* nominal supply voltage */
#define Vio			(3.3)			/* I/O circuitry supply voltage */
#define nVbi		(0.7420424)		/* built in potential in V */ 
#define pVbi		(0.8637545)		
#define nCja0		(0.9868856E-15) /* junction capacitance in F/um^2 */
#define pCja0		(1.181916E-15)
#define nCjp0		(0.2132647E-15) /* side wall capacitance in F/um */
#define pCjp0		(0.1716535E-15)
#define RKp			(0.82) /* Rent constant for microprocessor */
#define Rp			(0.45) /* Rent exponent for microprocessor */
#define packageCeff	(3.548E-12)	/* PAD plate from ARTISAN TSMC 0.18um  */
#define interspace		(3.0 * Ltech)
#define interwidth		(2.0 * Ltech)
#define interpitch		(interspace + interwidth)
#define NCH  1
#define PCH  0
#define NMOS  1
#define PMOS  0

double
estimate_trCeff(
	double k /* transistor sizing ratio */);

double 
estimate_sgwireCeff(
	double length);

double 
estimate_gwireCeff(
	double length);

double 
estimate_sgwireReff(
	double length); 

double 
estimate_gwireReff(
	double length);

double
estimate_gininvCeff(
	double k /* transistor sizing ratio */);

double
estimate_ginnandCeff(
	double k, /* transistor sizing ratio */
	double fi /* average fanin */);

double
estimate_ginnorCeff(
	double k /* transistor sizing ratio */);

double
estimate_sdCeff(
	int CH, /* channel type */
	double k /* transistor sizing ratio */);

double
estimate_sdinvCeff(
	double k /* transistor sizing ratio */);

double
estimate_sdnandCeff(
	double k, /* transistor sizing ratio */
	double fi /* average fanin*/);

double
estimate_sdnorCeff(
	double k, /* transistor sizing ratio */
	double fi /* average fanin */);

double
estimate_gm(
	int CH /* channel type*/);

double
estimate_trReff(
	int CH, /* channel type*/
	double k /* transistor sizing ratio */);

double
estimate_cReff(
	double k /* transistor sizing ratio */);

double
estimate_trinvReff(
	double k /* transistor sizing ratio */);

double
estimate_goutinvReff(
	double k /* transistor sizing ratio */);

#ifdef tech_130nm
#define sgwireC 	343.0E-18 /* aF/um */
#define gwireC 		370.0E-18 /* aF/um */
#define sgwireR 	168.0E-3  /* Ohm/um */
#define gwireR 		35.0E-3   /* Ohm/um */
#endif

#ifdef tech_100nm
#define sgwireC 	314.0E-18 /* aF/um */
#define gwireC 		335.0E-18 /* aF/um */
#define sgwireR 	260.0E-3  /* Ohm/um */
#define gwireR 		54.0E-3   /* Ohm/um */
#endif

#ifdef tech_70nm
#define sgwireC 	307.0E-18 /* aF/um */
#define gwireC 		312.0E-18 /* aF/um */
#define sgwireR 	340.0E-3  /* Ohm/um */
#define gwireR 		82.0E-3   /* Ohm/um */
#endif

#ifdef tech_50nm
#define sgwireC 	296.0E-18 /* aF/um */
#define gwireC 		296.0E-18 /* aF/um */
#define sgwireR 	600.0E-3  /* Ohm/um */
#define gwireR 		150.0E-3  /* Ohm/um */
#endif

/* CMOS 0.18um model parameters  - directly from Appendix II of tech report */
#define Cndiffarea    	0.137e-15 	/* F/um2 at 1.5V */
#define Cpdiffarea    	0.343e-15 	/* F/um2 at 1.5V */
#define Cndiffside    	0.275e-15 	/* F/um at 1.5V */
#define Cpdiffside    	0.275e-15 	/* F/um at 1.5V */
#define Cndiffovlp    	0.138e-15 	/* F/um at 1.5V */
#define Cpdiffovlp    	0.138e-15 	/* F/um at 1.5V */
#define Cnoxideovlp   	0.263e-15 	/* F/um assuming 25% Miller effect */
#define Cpoxideovlp   	0.338e-15 	/* F/um assuming 25% Miller effect */
#define Cgate         	1.95e-15	/* F/um2 */
#define Cgatepass     	1.45e-15	/* F/um2 */
#define Cpolywire	  	0.25e-15	/* F/um */
#define Cmetal 			275e-18		/* */

#define Rnchannelstatic	25800		/* ohms*um of channel width */
#define Rpchannelstatic	61200  		/* ohms*um of channel width */
#define Rnchannelon		 8751
#define Rpchannelon		20160
#define Rmetal			48e-3
#endif

/* panalyzer constant definition for 0.18um process */

/* SPICE model parameters : 
 * Vbi		PB 
 * Cja		CJ
 * Cjp		CJSW */
#endif /* TECHNOLOGY_H */

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

#ifndef INIT_PANALYZER_H
#define INIT_PANALYZER_H

#include "../host.h"
#include "../machine.h"

/* power monitoring windows */
#define MaxPMWindows 512
/* activity factor */
#define afactor 0.2

/* simplescalar data type alias and interfaces */
typedef enum mem_cmd fu_mcommand_t;
typedef md_addr_t fu_address_t;
typedef byte_t buffer_t; 

/* custom data types */
/* fu: funtional unit */
/* power model mode type */
typedef enum _fu_pmodel_mode_t {
	Analytical,
	Empirical
} fu_pmodel_mode_t;

/* dimension type */
typedef struct _fu_dimension_t{
	double height;
	double width;
	double area;
} fu_dimension_t;

/* effective capacitances type for power dissipation components */
typedef struct _fu_Ceffs_t {
	double sCeff;    	/* external switching effective capacitance */
	double iCeff;    	/* internal switching effective capacitance */
	double lCeff;    	/* leakage effective capacitance */
	double cnodeCeff;	/* clocked node capacitance */
} fu_Ceffs_t;

/* power dissipation type */
typedef struct _fu_pdissipation_t {
	double switching; 	/* switching power dissipation */
	double internal; 	/* internal switching power dissipation */
	double leakage; 	/* leakage power dissipation */
	double pdissipation;/* switching + internal + leakage */
	double peak; 		/* peak power dissipation */
} fu_pdissipation_t;

/* create/converte to buffer_t type data structure for various data types.
 * return an allocated location pointer.
 * caution: please deallocate the memory space  */
buffer_t *
create_buffer_t(
	void *val, 	/* target conversion value */
	int bsize 	/* conversion value data type size in bytes */);

/* copy buffer_t type data structure */
void
copy_buffer_t(
	buffer_t *ibus, /* the source bus */ 
	buffer_t *jbus, /* the destination bus */
	unsigned bsize /* size of the bus in bytes */);

/* return number of switching for the give previous bus and the current bus */
unsigned
obtain_nswitchings(
	buffer_t *ibus, /* the current bus */ 
	buffer_t *jbus, /* the previous bus */
	unsigned bsize /* size of the bus in bytes */);

/* obtain in switching power from fu_pdissipation_t data structure */
double
obtain_switching(
	unsigned pmwindex, /* power monitoring access index */
	fu_pdissipation_t *pmwindow /* power monitoring window */);

/* obtain internal power from fu_pdissipation_t data structure */
double
obtain_internal(
	unsigned pmwindex, /* power monitoring access index */
	fu_pdissipation_t *pmwindow /* power monitoring window */);

/* obtain leakage power from fu_pdissipation_t data structure */
double
obtain_leakage(
	unsigned pmwindex, /* power monitoring access index */
	fu_pdissipation_t *pmwindow);
	/* power monitoring window */

/* initialize pdissipation stat */
void 
initialize_fu_pdissipation(
	fu_pdissipation_t *pdissipation /* pdissipation stat ptr */);

/* initialize pmwindow */
void 
initialize_fu_pmwindow(
	unsigned pmwindex, /* fu power monitoring window index */
	fu_pdissipation_t *pmwindow /* fu power monitoring window */);

/* integrate the current power dissipation of fu from power monitoring windows 
 * to pdissipation stat db. */ 
double 
integrate_fu_pdissipation(
	unsigned pmwindex,	/* fu power monitoring window index */
	fu_pdissipation_t *pmwindow, 	/* fu power monitoring window */
	fu_pdissipation_t *pdissipation	/* fu power dissipation statistics */);

/* summate x/y switching + internal + leakage pdissipation of the current cycle.
 * return the summated pdissipation */
double 
summate_fu_pdissipation(
	unsigned pmwindex,	/* fu power monitoring window index */
	fu_pdissipation_t *pmwindow	/* fu power monitoring window */);

/* return pdissipation 
 * caution: this function must incorporate with ntransitions */
double 
estimate_pdissipation(
	double Ceff, /* effective capacitance */
	double opfreq, /* operating frequency */
	double svolt /* supply voltage */);

/* assign effective capacitance values to fu_Ceffs_t data structure */
fu_Ceffs_t
assign_fu_Ceffs(
	double sCeff,
	double iCeff,
	double lCeff,
	double cnodeCeff);

/* power analyzer stats register */
void
panalyzer_reg_stats(
	char *name, /* fu name */
	fu_pdissipation_t *pdissipation, /* fu pdissipation  statistics db ptr */
	 struct stat_sdb_t *sdb /* stats db ptr */);
#endif

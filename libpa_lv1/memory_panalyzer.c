#include <stdio.h>
#include <math.h>
#include "technology.h"
#include "panalyzer.h"
#include "circuit_analyzer.h"
#include "memory_panalyzer.h"
	
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
	double sCeff, double iCeff, double lCeff)
{
	fu_sbank_pspec_t *pspec;
	unsigned ntbits /* number of total bits */, bsize /* bus size */;
	int i /* loop index */;

	fprintf(stderr, "\n**********************************************\n");
	fprintf(stderr, "\t\tmemory panalyzer configuration\n");
	fprintf(stderr,   "**********************************************\n");

	/* create pspec data structure */
	if(!(pspec = (fu_sbank_pspec_t *)malloc(sizeof(fu_sbank_pspec_t))))
		fatal("lack of virtual memory!");
	fprintf(stderr, "name: %s\n", name);
	pspec->name = strdup(name);

	/* memory operating parameters */
	pspec->pmodel = pmodel;
	pspec->opfreq = opfreq;
    pspec->svolt = svolt;

	if(pmodel == Analytical)
	fprintf(stderr, "pmodel type: analytical power model\n");
	else
	fprintf(stderr, "pmodel type: empirical power model\n");

	if(!(pspec->dimension = (fu_dimension_t *)malloc(sizeof(fu_dimension_t))))
		        fatal("lack of virtual memory!");

	/* memory size parameters */
	pspec->nrows = nrows;
	pspec->ncols = ncols;
	fprintf(stderr, "number of rows in array: %u\n", nrows);
	fprintf(stderr, "number of cols in array: %u\n", ncols);

	/* memory ports parameters */
	pspec->nrwports = nrwports;
	pspec->nrports = nrports;
	pspec->nwports = nwports;

	/* number of total bits */
	ntbits = nrows * ncols;

	/* output bus size in bytes */
	bsize = (unsigned)ceil((double)ncols / 8.);	

	/* create data bus buffer */
	if(!(pspec->bus = (buffer_t *)malloc(bsize * sizeof(buffer_t))))
		fatal("lack of virtual memory");
	
	pspec->bsize = bsize;
	//fprintf(stderr, "bsize:%u\n", bsize);

	/* create effective capacitance data structure */
	if(!(pspec->Ceffs = (fu_Ceffs_t *)malloc(sizeof(fu_Ceffs_t))))
		fatal("lack of virtual memory");

	/* analytical model */
	if(pmodel == Analytical) 
		*(pspec->Ceffs) = estimate_sbankCeff(pspec, sCeff);
	else 
		*(pspec->Ceffs) = assign_fu_Ceffs(sCeff, iCeff * (double)ntbits, lCeff * (double)ntbits, 0.);

    /* initialize a power statistics db structure */
	if(!(pspec->pdissipation = (fu_pdissipation_t *)malloc(sizeof(fu_pdissipation_t)))) 
		fatal("lack of virtual memory");
	initialize_fu_pdissipation(pspec->pdissipation); 

    /* initialize a power monitor window */
	for(i = 0; i < MaxPMWindows; i++) 
	{
		initialize_fu_pmwindow(i, pspec->pmwindow); 
		/* put leakage power numbers in all the power window entry
		 * b/c leakage is access independent and swithing independent */
		(pspec->pmwindow + i)->leakage = estimate_pdissipation(pspec->Ceffs->lCeff, pspec->opfreq, pspec->svolt);
	}
	fprintf(stderr, "memory area: %3.4e (um^2)\n", pspec->dimension->area);
	fprintf(stderr, "external switching capacitance: %3.4e (pF)\n", pspec->Ceffs->sCeff * 1E12);
	fprintf(stderr, "internal switching capacitance: %3.4e (pF)\n", pspec->Ceffs->iCeff * 1E12);
	fprintf(stderr, "effective leakage capacitance: %3.4e (pF)\n", pspec->Ceffs->lCeff * 1E12);
	fprintf(stderr, "effective clocked capacitance: %3.4e (pF)\n", pspec->Ceffs->cnodeCeff * 1E12);

	return pspec;
}

/* sbank panalyzer: estimate pdissipatino of the structure with the given
 * in/out bus value and access statistics */
void 
sbank_panalyzer(
	fu_sbank_pspec_t *pspec, /* memory pspec*/
	buffer_t *buffer, /* out bus */
	tick_t now /* current simulation time */)
{
	fu_pdissipation_t *pmwindow;
	unsigned nswitchings;
	unsigned pmwindex;
	static tick_t past;

	/* obtain pmwindow ptr from pspec ptr */
	pmwindow = pspec->pmwindow;

	if(buffer)
		/* counting number of transitions of data bus */
		nswitchings = obtain_nswitchings(buffer, pspec->bus, pspec->bsize);
	else
		/* from fixed activity factor: see technology.h  */
		nswitchings = (unsigned)(afactor * (double)(pspec->bsize * 8));

	/* obtain pmwindex from the current simulation cycle */
	pmwindex = now % MaxPMWindows;

	if(past != now)
	{
		initialize_fu_pmwindow(pmwindex, pmwindow); 
		past = now;
	}

	/* accounting for ouput bus transition */
	(pmwindow + pmwindex)->switching 
		+= (double)nswitchings * estimate_pdissipation(pspec->Ceffs->sCeff, pspec->opfreq, pspec->svolt);
	/* transition independent pdissipation */
	(pmwindow + pmwindex)->internal 
		+= estimate_pdissipation(pspec->Ceffs->iCeff, pspec->opfreq, pspec->svolt); 
	/* switching + internal + leakage pdissipation of the current cycle */
	(pmwindow + pmwindex)->pdissipation
		= summate_fu_pdissipation(pmwindex, pmwindow);
}

/* estimate sbankCeff */
fu_Ceffs_t
estimate_sbankCeff(
	fu_sbank_pspec_t *pspec,
	double sCeff)
{
	double iCeff, lCeff, cnodeCeff;
	double trCeff, wireCeff, sdNMOSCeff;
	double rowCeff, rowbufCeff, colCeff, colbufCeff;
	double width, height, ximpact, yimpact, btr_opt;
	unsigned nports;

	/* minimum size transistor input capacitance */
	trCeff = estimate_trCeff(1.);
	/* wordline wire capacitance */
	wireCeff = estimate_sgwireCeff(1.);
	/* NMOS drain capacitance */
	sdNMOSCeff = estimate_sdCeff(NMOS, 1.);

	/* total number of ports */
	nports = pspec->nrwports + pspec->nrports + pspec->nwports;

	/* x / y dimension increase in a cell size 
	 * by increasing number of r/w, r, and wports */
	/* from CACTI 2.0 technical reports */

	ximpact = interpitch 
			* (2.0 * (double)(pspec->nwports + pspec->nrwports -1) 
				   + (double)pspec->nrports);
	yimpact = interpitch 
			* (2.0 * (double)(nports - 1));

	/* estimate 6T cell dimension */
	width = _6TWIDTH + ximpact; 
	height = _6THEIGHT + yimpact;

	/* estimate sub-bank dimension */
	pspec->dimension->width = (double)pspec->ncols * width;
	pspec->dimension->height = (double)pspec->nrows * height;
	pspec->dimension->area = pspec->dimension->width * pspec->dimension->height;
	//fprintf(stderr, "width:%e\t height:%e\t area:%e\n", pspec->dimension->width,
					//pspec->dimension->height, pspec->dimension->area);

	/* estimate leakage capacitance of a sub-bank */
	lCeff = (double)(pspec->nrows * pspec->ncols) 
			* estimate_lCeff(Wtech, pspec->opfreq, pspec->svolt);

	/* effective capacitance of one sbank word-line / port */
	rowCeff = (double)(pspec->ncols) * (2. * trCeff + wireCeff * width);
	//fprintf(stderr, "rowCeff:%e\n", rowCeff);

	/* sizing word-line driver */
	btr_opt = estimate_btr_opt(2, rowCeff);
	
	rowbufCeff = estimate_ibufchainiCeff(btr_opt, rowCeff);
	//fprintf(stderr, "rowbufCeff:%e\n", rowbufCeff);

	/* estimate leakage capacitance of a wordline driver */
	lCeff += (double)(pspec->nrows * nports) /* number of wordline driver */
			* estimate_ibufchainlCeff(2, btr_opt, pspec->opfreq, pspec->svolt);

	/* effective capacitance of one sbank bit-line / port
	 * only one of bit-lines is switching */
	colCeff = (double)(pspec->nrows) * (sdNMOSCeff + wireCeff * height);
	//fprintf(stderr, "colCeff:%e\n", colCeff);

	//fprintf(stderr, "sCeff:%e\n", sCeff);
	/* sizing output driver / bit */
	btr_opt = estimate_btr_opt(2, sCeff);
	colbufCeff = estimate_ibufchainiCeff(btr_opt, sCeff);
	//fprintf(stderr, "colbufCeff:%e\n", colbufCeff);

	/* external switching capacitance / bit */
	sCeff = sCeff + colbufCeff; 
	/* internal switching capacitance / bit */
	iCeff = rowbufCeff + rowCeff + colCeff * (double)(pspec->ncols);
	/* estimate leakage capacitance of a output driver */
	lCeff += (double)(pspec->ncols * nports)
			* estimate_ibufchainlCeff(2, btr_opt, pspec->opfreq, pspec->svolt);

	/* pre-charge circuit transistor input capacitance 
	 * the size of precharge transistor is proportional to nrows 
	 * you can adjust proportional ratio (e.g., 16 -> 32
	 * default value is 8 
	 * we also assume that 3-PMOS clocked pre-charge scheme */
	trCeff = 3. * estimate_trCeff((double)(pspec->nrows / 64 + 1));
	cnodeCeff = pspec->ncols * nports * trCeff; 
	//fprintf(stderr, "sCeff:%e\t iCeff:%e\t lCeff:%e\t cnodeCeff:%e\n", sCeff, iCeff, lCeff, cnodeCeff);

	return	assign_fu_Ceffs(sCeff, iCeff, lCeff, cnodeCeff);
}

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../misc.h"
#include "technology.h"
#include "panalyzer.h"
#include "circuit_analyzer.h"
#include "clock_panalyzer.h"

/* from "Modeling Microprocessor Performance" Chapter 5 & 7 */
fu_clock_pspec_t *
create_clock_panalyzer(
	char *name, /* memory name */
	fu_pmodel_mode_t pmodel, /* bpred power model mode */
	/* memory operating parameters: operating frequency/supply voltage */
	double opfreq, double svolt, 
	double tdarea, /* total die area in um^2 */
	double tcnodeCeff, /* total clocked node capacitnace */
	fu_clocktree_style_t ctree_style, /* clock tree style */
	double cskew, /* clock skew */
	unsigned ncbstages_opt, /* optimial number of clock buffer stages */

	/* switching/internal/lekage  effective capacitances */
	double sCeff, double iCeff, double lCeff)
{
	fu_clock_pspec_t *pspec;
	int i /* loop index */;

	fprintf(stderr, "\n**********************************************\n");
	fprintf(stderr, "\t\tclock tree panalyzer configuration\n");
	fprintf(stderr,   "**********************************************\n");

	/* create pspec data structure */
	if(!(pspec = (fu_clock_pspec_t *)malloc(sizeof(fu_clock_pspec_t))))
		fatal("lack of virtual memory!");
	pspec->name = strdup(name);
	fprintf(stderr, "name: %s\n", name);

	/* clock operating parameters */
	pspec->pmodel = pmodel;
	if(pmodel == Analytical)
	fprintf(stderr, "pmodel type: analytical power model\n");
	else
	fprintf(stderr, "pmodel type: empirical power model\n");
	pspec->opfreq = opfreq;
    pspec->svolt = svolt;

	if(ctree_style == balHtree)
	fprintf(stderr, "clock tree style: balanced H-tree\n");
	else
	fprintf(stderr, "clock tree style: H-tree\n");
	fprintf(stderr, "target clock skew: %3.4e (ps)\n", cskew * 1E12);

	if(!(pspec->dimension = (fu_dimension_t *)malloc(sizeof(fu_dimension_t))))
		fatal("lack of virtual memory!");


	pspec->dimension->height = pspec->dimension->width = sqrt(tdarea);
	pspec->dimension->area = tdarea;
	pspec->tcnodeCeff = tcnodeCeff;
	pspec->ctree_style = ctree_style; 
	pspec->cskew = cskew; 
	pspec->ncbstages_opt = ncbstages_opt; 

	fprintf(stderr, "total die area: %3.4e (um^2)\n", pspec->dimension->area);
	fprintf(stderr, "total die edge length: %3.4e (um)\n", pspec->dimension->width);
	fprintf(stderr, "total clocked node capacitance: %3.4e (pF)\n", tcnodeCeff * 1E12);

    /* create effective capacitance data structure */
    if(!(pspec->Ceffs = (fu_Ceffs_t *)malloc(sizeof(fu_Ceffs_t))))
        fatal("lack of virtual memory");

	/* analytical model */
	if(pmodel == Analytical) 
		*(pspec->Ceffs) = estimate_tcdtCeff(pspec);

	else 
		*(pspec->Ceffs) = assign_fu_Ceffs(sCeff, iCeff, lCeff, 0.);

	fprintf(stderr, "external switching capacitance: %3.4e (pF)\n", pspec->Ceffs->sCeff * 1E12);
	fprintf(stderr, "internal switching capacitance: %3.4e (pF)\n", pspec->Ceffs->iCeff * 1E12);
	fprintf(stderr, "effective leakage capacitance: %3.4e (pF)\n", pspec->Ceffs->lCeff * 1E12);


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

	return pspec;
}

/* clock panalyzer: estimate pdissipatino of the structure */
void 
clock_panalyzer(
	fu_clock_pspec_t *pspec, /* memory pspec*/
	tick_t now /* current simulation time */)
{
	tick_t tstart; /* transaction start sim_cycle */
	fu_pdissipation_t *pmwindow; /* power monitoring windows */
	unsigned pmwindex;

	pmwindow = pspec->pmwindow;

	/* transaction start time */
	tstart = now;
	pmwindex = tstart % MaxPMWindows;

	/* analyzer pdissipation  */
	(pmwindow + pmwindex)->switching 
		= estimate_pdissipation(pspec->Ceffs->sCeff, pspec->opfreq, pspec->svolt);
	(pmwindow + pmwindex)->internal 
		= estimate_pdissipation(pspec->Ceffs->iCeff, pspec->opfreq, pspec->svolt);
	/* obtain total pdissipation of the current cycle */
	(pmwindow + pmwindex)->pdissipation
		= summate_fu_pdissipation(pmwindex, pmwindow);
}

/* estimate total clock distribution tree wire */
double 
estimate_tcdtwireCeff(
	fu_clock_pspec_t *pspec)
{
	double cdtwireCeff, cdtwireReff, dcskew, tcdtwireCeff;
	int Ntree;
	int i;

	/* estimatin clock distribution wire capacitance */
	/* clock distribution tree wire unit capacitance/resistance */
	cdtwireCeff = estimate_gwireCeff(1.); 
	cdtwireReff = estimate_gwireReff(1.);
#ifdef DONT
	fprintf(stderr, "cdtwireCeff: %e\t cdtwireReff: %e\n", cdtwireCeff, cdtwireReff);
	fprintf(stderr, "cdtwireCeff x cdtwireReff: %e\n", cdtwireCeff * cdtwireReff);
#endif
	/* desired clock skew */
	dcskew 	= sqrt(pspec->cskew / (cdtwireCeff * cdtwireReff)); 
#ifdef DONT
	fprintf(stderr, "dcskew: %e\n", dcskew);
	fprintf(stderr, "area: %e\n", pspec->dimension->area);
	fprintf(stderr, "cskew: %e\n", pspec->cskew);
#endif

	/* depth of clock tree */
	Ntree 	= (int)(sqrt(pspec->dimension->area) / dcskew + 1.0); 
#ifdef DEBUG
	fprintf(stderr, "Ntree: %u\n", Ntree);
#endif

	tcdtwireCeff = 0.0;
	switch(pspec->ctree_style) 
	{
		case Htree:
		for(i = 1; i <= Ntree; i++)
		{
			tcdtwireCeff += cdtwireCeff * sqrt(pspec->dimension->area) * pow(2.,(double)(Ntree - 1)) * (1. / pow(2., floor((double)i / 2.) + 1.));
         }
#ifdef DONT
		fprintf(stderr, "tcdtwireCeff: %e\n", tcdtwireCeff);
#endif
		break;

		case balHtree: 
		for(i= 1; i <= Ntree; i++) 
		{ 
			tcdtwireCeff += cdtwireCeff * sqrt(pspec->dimension->area) * pow(2.,(double)(i - 1)) * (1. / pow(2., floor((double)i / 2.) + 1.));

		} 
#ifdef DEBUG
		fprintf(stderr, "tcdtwireCeff: %e\n", tcdtwireCeff);
#endif
		break;
		default:
      		fatal("not supported clock tree type!");
		break;
	}

	return tcdtwireCeff;
}

/* estimate clock distribution tree branch wire length */
double 
estimate_lbr(
	int N, /* Nth branch */
	double ledge /* edge length */)
{
	return ledge * 1. / (pow(2., floor((double)(N) / 2.) + 1.));
}

/* estimate clock distribution tree and buffer capacitance */
fu_Ceffs_t
estimate_tcdtCeff(
	fu_clock_pspec_t *pspec)
{
	fu_Ceffs_t tcdtCeffs;
	double sCeff, iCeff, lCeff;
	double cdtwireCeff, cdtwireReff;
	double tcnodeCeff, tcdtwireCeff, tcdtnodeCeff, tcdtCeff; 
	double cdtbrwireCeff, cbrnodeCeff, cdtbrnodeCeff, cdtbrCeff, cdtnodeCeff;
	double dcskew, lbr, btr_opt;
	int Ntree;
	int i;

	/* estimatin clock distribution wire capacitance */
	/* clock distribution tree wire unit capacitance/resistance */
	cdtwireCeff = estimate_gwireCeff(1.); 
	cdtwireReff = estimate_gwireReff(1.);

	/* desired clock skew */
	dcskew 	= sqrt(pspec->cskew / (cdtwireCeff * cdtwireReff)); 
	fprintf(stderr, "clock tree block length: %3.4e (um)\n", dcskew * 1E6);

	/* depth of clock tree */
	Ntree 	= (int)(sqrt(pspec->dimension->area) / dcskew + 1.); 
	fprintf(stderr, "depth of clock tree: %u\n", Ntree);

	/* total clocked node capacitance */
	tcnodeCeff = pspec->tcnodeCeff;
	tcdtwireCeff = estimate_tcdtwireCeff(pspec);

	switch(pspec->ctree_style) 
	{
		case Htree:
		tcdtnodeCeff = tcdtwireCeff + tcnodeCeff;
		btr_opt = estimate_btr_opt(pspec->ncbstages_opt, tcdtnodeCeff);
		sCeff = tcdtnodeCeff;
		iCeff = estimate_ibufchainiCeff(btr_opt, tcdtnodeCeff);
		lCeff = estimate_ibufchainlCeff(pspec->ncbstages_opt, btr_opt, pspec->opfreq, pspec->svolt);
		break;

		case balHtree:
		lbr = estimate_lbr(Ntree, pspec->dimension->width);
		cdtbrwireCeff = 2. * cdtwireCeff * lbr;

		cbrnodeCeff = tcnodeCeff / pow(2., (double)(Ntree - 2));

		cdtbrnodeCeff = cdtbrwireCeff + cbrnodeCeff;

		btr_opt = estimate_btr_opt(pspec->ncbstages_opt, cdtbrnodeCeff);
		cdtbrCeff = cdtbrnodeCeff * (1. / (1. - (1. / btr_opt)) + 1.);
		tcdtCeff = pow(2., (double)(Ntree - 1)) * cdtbrCeff;

		sCeff = pow(2., (double)(Ntree - 1)) * cdtbrnodeCeff;
 		iCeff = pow(2., (double)(Ntree - 1)) * estimate_ibufchainiCeff(btr_opt, cdtbrnodeCeff);
 		lCeff = pow(2., (double)(Ntree - 1)) * estimate_ibufchainlCeff(pspec->ncbstages_opt, btr_opt, pspec->opfreq, pspec->svolt);


		/* 1 to Ntree - 2 stages */
		for(i = Ntree - 2; i >= 1; i--) 
		{ 
			if(i == 1) 
			{
				lbr = estimate_lbr(1, pspec->dimension->width);
				cdtbrwireCeff = cdtwireCeff * lbr;

				lbr = estimate_lbr(i + 1, pspec->dimension->width);
				cdtbrwireCeff += 2. * cdtwireCeff * lbr;

				cbrnodeCeff 
						= 2. * cdtbrnodeCeff / pow(btr_opt, (double)(i + 1));
				cdtnodeCeff = cdtbrwireCeff + cbrnodeCeff;

				tcdtCeff += cdtnodeCeff;
				sCeff += cdtnodeCeff;
			}
			else
			{
				lbr = estimate_lbr(i + 1, pspec->dimension->width);
				cdtbrwireCeff = 2. * cdtwireCeff * lbr;
				cbrnodeCeff 
						= 2. * cdtbrnodeCeff / pow(btr_opt, (double)(i + 1));
				cdtbrnodeCeff = cdtbrwireCeff + cbrnodeCeff;

				btr_opt = estimate_btr_opt(pspec->ncbstages_opt, cdtbrnodeCeff);
				cdtbrCeff = cdtbrnodeCeff * (1. / (1. - (1. / btr_opt)) + 1.);

				tcdtCeff += pow(2., (double)(i - 1)) * cdtbrCeff;

				sCeff += pow(2., (double)(i - 1)) * cdtbrnodeCeff;
 				iCeff += pow(2., (double)(i - 1)) * estimate_ibufchainiCeff(btr_opt, cdtbrnodeCeff);
 				lCeff += pow(2., (double)(i - 1)) * estimate_ibufchainlCeff(pspec->ncbstages_opt, btr_opt, pspec->opfreq, pspec->svolt);
			}

		} 
		break;
		default:
      		fatal("not supported clock tree type!");
		break;
	}

	return assign_fu_Ceffs(sCeff, iCeff, lCeff, 0.);
}

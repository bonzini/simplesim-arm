#include <stdio.h>
#include <math.h>
#include "technology.h"
#include "circuit_analyzer.h"

__inline__
double
estimate_nandCeff(
	double k, /* transistor sizing ratio */
	double fi /* average fanin */)
{
	double sdCeff, sdNMOSCeff, ginminCeff, Ceff;

	sdCeff = estimate_sdnandCeff(k, fi);
	sdNMOSCeff = estimate_sdCeff(NMOS, k);
	ginminCeff = estimate_ginnandCeff(1., fi);

	Ceff = (k * ginminCeff * fi + sdCeff + (double)(fi - 1) * sdNMOSCeff) / 2.;

	return Ceff;
}

__inline__
double
estimate_dynamicCeff(double k, double fi)
{
	double ginminCeff, trCeff, sdCeff, sdNMOSCeff, Ceff;

	ginminCeff = estimate_ginnorCeff(1.);
	trCeff = estimate_trCeff(1.);
	sdCeff = estimate_sdnorCeff(k, fi);
	sdNMOSCeff = estimate_sdCeff(NMOS, k);


	Ceff = (k * ginminCeff * fi + beta * trCeff + sdCeff + sdNMOSCeff) / 2.;

	ginminCeff = estimate_gininvCeff(1.);
	sdCeff = estimate_sdinvCeff(1.);
	Ceff += (k * ginminCeff + sdCeff) / 2.;

	return Ceff;
}


__inline__
double
estimate_staticffCeff(double k, double fld)
{
	double ginminCeff, sdCeff, sdNMOSCeff, sdPMOSCeff, Ceff;

	ginminCeff = estimate_gininvCeff(1.);
	sdCeff = estimate_sdinvCeff(k);

	sdNMOSCeff = estimate_sdCeff(NMOS, k);
	sdPMOSCeff = estimate_sdCeff(PMOS, k);

	Ceff = ((4. * k * ginminCeff + 4. * sdCeff) 
		+ 4. * (sdNMOSCeff + sdPMOSCeff)) / (2. * fld);

	return Ceff;
}

__inline__
double
estimate_dynamicffCeff(double k, double fld)
{
	double trCeff, sdNMOSCeff, sdPMOSCeff, Ceff;

	trCeff = estimate_trCeff(1.);

	sdNMOSCeff = estimate_sdCeff(NMOS, k);
	sdPMOSCeff = estimate_sdCeff(PMOS, 2. * k);

	Ceff 
		= ((k * (2. * trCeff + 2. * 2 * trCeff)) 
		+ 3. * (sdNMOSCeff + sdPMOSCeff)) / (2. * fld);

	return Ceff;
}

__inline__
double
estimate_goutnandReff(double k, double fi)
{

	double NMOStrminReff, PMOStrminReff, cminReff, goutReff;

	NMOStrminReff = estimate_trReff(NMOS, 1.);
	PMOStrminReff = estimate_trReff(PMOS, 1.);
	cminReff = estimate_cReff(1.);

	goutReff = 	
	((fi * (NMOStrminReff / fi + PMOStrminReff / 2.)) / 2. + 2. * cminReff) / k;
#ifdef DEBUG
	fprintf(stderr, "goutnandReff: %e\n", goutReff);
#endif
	return goutReff;
}

__inline__
double
estimate_goutnorReff(double k, double fi)
{

	double NMOStrminReff, PMOStrminReff, cminReff, goutReff;

	NMOStrminReff = estimate_trReff(NMOS, 1.);
	PMOStrminReff = estimate_trReff(PMOS, 1.);
	cminReff = estimate_cReff(1.);

	goutReff = 	
	((2. * NMOStrminReff / 2. + PMOStrminReff / 2.) / 2. + 2. * cminReff) / k;
#ifdef DEBUG
	fprintf(stderr, "goutnandReff: %e\n", goutReff);
#endif
	return goutReff;
}


__inline__
double
estimate_kinv(void)
{
	double ginminCeff, goutReff, lwireCeff, lwireReff, kinv;

	ginminCeff = estimate_gininvCeff(1.);
	goutReff = estimate_goutinvReff(1.);

	lwireCeff = estimate_gwireCeff(1.);
	lwireReff = estimate_gwireReff(1.);

	kinv = sqrt((goutReff * lwireCeff) / (lwireReff * ginminCeff));
#ifdef DEBUG
	fprintf(stderr, "kinv: %e\n", kinv);
#endif
	return kinv;
}

__inline__
double
estimate_krep(void)
{
	double ginminCeff, goutReff, lwireCeff, lwireReff, krep;

	ginminCeff = estimate_gininvCeff(1.);
	goutReff = estimate_goutinvReff(1.);

	lwireCeff = estimate_gwireCeff(1.);
	lwireReff = estimate_gwireReff(1.);

	krep = sqrt((goutReff * lwireCeff) / (lwireReff * ginminCeff));
#ifdef DEBUG
	fprintf(stderr, "krep: %e\n", krep);
#endif
	return krep;
}

__inline__
double
estimate_nrep(
	double lw /* length of long wire */)
{
	double ginminCeff, goutReff, lwireCeff, lwireReff, nrep;

	ginminCeff = estimate_gininvCeff(1.);
	goutReff = estimate_goutinvReff(1.);

	lwireCeff = estimate_gwireCeff(1.);
	lwireReff = estimate_gwireReff(1.);

	nrep = lw * sqrt((0.377 * lwireReff * lwireCeff) / (0.693 * goutReff * ginminCeff));
#ifdef DEBUG
	fprintf(stderr, "nrep: %e\n", nrep);
#endif
	return nrep;
}

double
estimate_staticlogicCeff(
	double kinvb, double krep, double knand, 
	double fi, double fld, double nrep,
	unsigned Ngates, unsigned Nlw)
{
	double ginmininvCeff, sdinvCeff, invbCeff, repCeff;
	double nandCeff, staticffCeff;
	double Ceff;
	
	ginmininvCeff = estimate_gininvCeff(1.);

	sdinvCeff = estimate_sdinvCeff(kinvb);
	invbCeff = (kinvb * ginmininvCeff + sdinvCeff);

	sdinvCeff = estimate_sdinvCeff(krep);
	repCeff = (krep * ginmininvCeff + sdinvCeff);

	nandCeff = estimate_nandCeff(knand, fi);
	staticffCeff = estimate_staticCeff(knand, fld);

	Ceff = (double)Ngates * (nandCeff + staticffCeff) 
			+ (double)Nlw * invbCeff + (double)Nlw * nrep * repCeff;

	return Ceff;
}

double
estimate_dynamiclogicCeff(
	double kinvb, double krep, double knor, 
	double fi, double fld, double nrep,
	unsigned Ngates, unsigned Nlw)
{
	double ginmininvCeff, sdinvCeff, invbCeff, repCeff;
	double dynamicCeff, dynamicffCeff;
	double Ceff;
	
	ginmininvCeff = estimate_gininvCeff(1.);

	/* inverter buffer */
	sdinvCeff = estimate_sdinvCeff(kinvb);
	invbCeff = (kinvb * ginmininvCeff + sdinvCeff);

	/* repeater */
	sdinvCeff = estimate_sdinvCeff(krep);
	repCeff = (krep * ginmininvCeff + sdinvCeff);

	/* dynamic logic / flip-flop */
	dynamicCeff = estimate_dynamicCeff(knor, fi);
	dynamicffCeff = estimate_dynamicffCeff(knor, fld);

	/*  dynamic logic total capacitance */
	Ceff = (double)Ngates * (dynamicCeff + dynamicffCeff) 
			+ (double)Nlw * invbCeff + (double)Nlw * nrep * repCeff;

#ifdef DEBUG
	fprintf(stderr, "dynamiclogicCeff: %e\n", Ceff);
#endif 
	return Ceff;
}

__inline__
double 
estimate_lw(
	double area, /* logic area */ 
	unsigned Ncluster)
{
	double lw;

	lw = 2. * sqrt(area / (double)Ncluster);

	return lw;
}

__inline__
unsigned
estimate_Nlw(
	unsigned Ngates, 
	unsigned Nfunc, 
	double fg)
{

	unsigned Ngblk, blkNp, totalNp, Nlw;

	Ngblk = Ngates / Nfunc;
	blkNp = RKp * pow((double)Ngblk, Rp);
	totalNp = Nfunc * blkNp;

	Nlw = (unsigned)((fg / (fg + 1.)) * (double)totalNp);

	return Nlw;
}

#ifdef DONT
fu_logic_pspec_t
create_logic_panalyzer(
	char *name, /* memory name */
	fu_pmodel_mode_t pmodel, /* bpred power model mode */
	/* memory operating parameters: operating frequency/supply voltage */
	double opfreq, double svolt, 

	fu_logic_style_t logic_style, /* logic tree style */
	unsigned Ngates, unsigned Nfunc, fi, fld 

	/* x/y switching/internal/lekage  effective capacitances */
	double xCeff, double yCeff, double iCeff, double lCeff)
{
	fu_logic_pspec_t *pspec;
	int i /* loop index */;
	unsigned Nlw;


	/* create pspec data structure */
	if(!(pspec = (fu_logic_pspec_t *)malloc(sizeof(fu_logic_pspec_t))))
		fatal("lack of virtual memory!");
	pspec->name = strdup(name);

	/* clock operating parameters */
	pspec->pmodel = pmodel;
	pspec->opfreq = opfreq;
    pspec->svolt = svolt;

	if(!(pspec->dimension = (fu_dimension_t *)malloc(sizeof(fu_dimension_t))))
		fatal("lack of virtual memory!");
	
	pspec->logic_style = logic_style;

    /* create effective capacitance data structure */
    if(!(pspec->Ceffs = (fu_Ceffs_t *)malloc(sizeof(fu_Ceffs_t))))
        fatal("lack of virtual memory");

	Nlw = estimate_Nlw(Ngates, Nfunc, fg)
	kinvb = estimate_kinv();
	krep = estimate_krep();
	lw = estimate_lw(area, Ncluster);
	nrep = estimate_nrep(lw);

	/* analytical model */
	switch(pmodel)
	{
		case Analytical:
		switch(logic_style)
		{
			case Static:
			iCeff = estimate_staticlogicCeff(kinvb, krep, knand, fi, fld, 
														nrep, Ngates, Nlw);
			break;

			case Dynamic:
			iCeff = estimate_dynamiclogicCeff(kinvb, krep, knand, fi, fld, 
														nrep, Ngates, Nlw);
			break;
			default:
			fatal("invalid logic style!");
			break;
		}
		break;

		case Empirical:
		*(pspec->Ceffs) = assign_fu_Ceffs(xCeff, yCeff, iCeff, lCeff);
		break;

		default:
		fatal("invalid pmodel type!");
		break;
	}
	else
		
    /* initialize a power statistics db structure */
	if(!(pspec->pdissipation = (fu_pdissipation_t *)malloc(sizeof(fu_pdissipation_t)))) 
		fatal("lack of virtual memory");
	initialize_fu_pdissipation(pspec->pdissipation); 

    /* initialize a power monitor window */
	for(i = 0; i < MaxPMWindows; i++) {
		initialize_fu_pmwindow(i, pspec->pmwindow); 
		/* put leakage power numbers in all the power window entry
		 * b/c leakage is access independent and swithing independent */
		(pspec->pmwindow + i)->leakage = estimate_pdissipation(pspec->Ceffs->lCeff, pspec->opfreq, pspec->svolt);
	}

	return pspec;
}
#endif

fu_logic_pspec_t
create_logic_panalyzer(
	char *name, /* memory name */
	fu_pmodel_mode_t pmodel, /* bpred power model mode */
	/* memory operating parameters: operating frequency/supply voltage */
	double opfreq, double svolt, 

	fu_logic_style_t logic_style, /* logic tree style */
	unsigned Ngates, , double fi, double fld 

	/* x/y switching/internal/lekage  effective capacitances */
	double sCeff, double iCeff, double lCeff)
{
	fu_logic_pspec_t *pspec;
	int i /* loop index */;


	/* create pspec data structure */
	if(!(pspec = (fu_logic_pspec_t *)malloc(sizeof(fu_logic_pspec_t))))
		fatal("lack of virtual memory!");
	pspec->name = strdup(name);

	/* clock operating parameters */
	pspec->pmodel = pmodel;
	pspec->opfreq = opfreq;
    pspec->svolt = svolt;

	if(!(pspec->dimension = (fu_dimension_t *)malloc(sizeof(fu_dimension_t))))
		fatal("lack of virtual memory!");
	
	pspec->logic_style = logic_style;

    /* create effective capacitance data structure */
    if(!(pspec->Ceffs = (fu_Ceffs_t *)malloc(sizeof(fu_Ceffs_t))))
        fatal("lack of virtual memory");

	/* analytical model */
	switch(pmodel)
	{
		case Analytical:
		switch(logic_style)
		{
			case Static:
			iCeff = estimate_staticlogicCeff(0., 0., knand, fi, fld, 
														0., Ngates, 0);
			break;

			case Dynamic:
			iCeff = estimate_dynamiclogicCeff(0., 0., knor, fi, fld, 
														0., Ngates, 0);
			break;
			default:
			fatal("invalid logic style!");
			break;
		}
		break;

		case Empirical:
		*(pspec->Ceffs) = assign_fu_Ceffs(xCeff, yCeff, iCeff, lCeff);
		break;

		default:
		fatal("invalid pmodel type!");
		break;
	}
	else
		
    /* initialize a power statistics db structure */
	if(!(pspec->pdissipation = (fu_pdissipation_t *)malloc(sizeof(fu_pdissipation_t)))) 
		fatal("lack of virtual memory");
	initialize_fu_pdissipation(pspec->pdissipation); 

    /* initialize a power monitor window */
	for(i = 0; i < MaxPMWindows; i++) {
		initialize_fu_pmwindow(i, pspec->pmwindow); 
		/* put leakage power numbers in all the power window entry
		 * b/c leakage is access independent and swithing independent */
		(pspec->pmwindow + i)->leakage = estimate_pdissipation(pspec->Ceffs->lCeff, pspec->opfreq, pspec->svolt);
	}

	return pspec;
}


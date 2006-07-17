#include <math.h>
#include <stdio.h>
#include "technology.h"

__inline__
double
estimate_trCeff(
	double k /* transistor sizing ratio */)
{
	double trCeff;

	trCeff = Cox * k * Wtech * Ltech;

	return trCeff;
}

__inline__
/* estimate semi-global wire capacitance */
double 
estimate_sgwireCeff( 
	double length /* wire length */) 
{
	return (length * sgwireC);
}

__inline__
/* estimate semi-global wire resistance */
double 
estimate_gwireCeff( 
	double length /* wire length */) 
{
	return (length * gwireC);
}

__inline__
/* estimate global wire capacitance */
double 
estimate_sgwireReff( 
	double length /* wire length */) 
{
	return (length * sgwireR);
}

__inline__
/* estimate global wire resistance */
double 
estimate_gwireReff( 
	double length /* wire length */) 
{
	return (length * gwireR);
}

__inline__
double
estimate_gininvCeff(
	double k /* transistor sizing ratio */)
{
	double ginCeff;

	ginCeff = (beta + 1.) * estimate_trCeff(k);

	return ginCeff;
}


__inline__
double
estimate_ginnandCeff(
	double k, /* transistor sizing ratio */
	double fi /* average fanin */)
{
	double ginCeff;

	ginCeff = (beta + fi) * estimate_trCeff(k);

	return ginCeff;
}

__inline__
double
estimate_ginnorCeff(
	double k /* transistor sizing ratio */)
{
	double ginCeff;

	ginCeff = 2. * estimate_trCeff(k);

	return ginCeff;
}

__inline__
double
estimate_sdCeff(
	int CH, /* channel type */
	double k /* transistor sizing ratio */)
{
	double Cja0, Cjp0, sdCeff, Vbi;

	Cja0 = (CH == NMOS) ? nCja0 : pCja0;
	Cjp0 = (CH == NMOS) ? nCjp0 : pCjp0;
	Vbi = (CH == NMOS) ? nVbi : pVbi;

	sdCeff 
		= Cja0 * (k * Wtech * Ltech / 2.) * pow((1. + Vdd / (2. * Vbi)), -0.5) 
			+ Cjp0 * (k * Wtech + Ltech) * pow((1. + Vdd / (2. * Vbi)), -0.3); 

	return sdCeff;
}

__inline__
double
estimate_sdinvCeff(
	double k /* transistor sizing ratio */)
{
	double sdCeff;

	sdCeff = estimate_sdCeff(NMOS, k) + estimate_sdCeff(PMOS, beta * k);

	return sdCeff;
}

__inline__
double
estimate_sdnandCeff(
	double k, /* transistor sizing ratio */
	double fi /* average fanin*/)
{
	double sdCeff;

	sdCeff = estimate_sdCeff(NMOS, fi * k) + fi * estimate_sdCeff(PMOS, beta * k);

	return sdCeff;
}

__inline__
double
estimate_sdnorCeff(
	double k, /* transistor sizing ratio */
	double fi /* average fanin */)
{
	double sdCeff;

	sdCeff 	= fi * estimate_sdCeff(NMOS, beta * k) 
			+ estimate_sdCeff(PMOS, beta * k) + estimate_sdCeff(PMOS, beta);

	return sdCeff;
}

__inline__
/* in uS/um */
double
estimate_gm(
	int CH /* channel type*/)
{
	double mobility, Idss, gm;

	/* mobility */	
	mobility = (CH == NMOS) ? nmobility : pmobility;
	/* saturation current */	
	Idss = (CH == NMOS) ? nIdss : pIdss;

	/* transconductance / um */	
	gm = sqrt(2. * mobility * Cox / Ltech * Idss);
#ifdef DEBUG
	fprintf(stderr, "gm: %e\n", gm);
#endif 
	return gm;
}

__inline__
double
estimate_trReff(
	int CH, /* channel type*/
	double k /* transistor sizing ratio */)
{
	double gm, trReff;
	
	gm = estimate_gm(CH);
	trReff = 1. / (gm * k * Ltech);
#ifdef DEBUG
	fprintf(stderr, "trReff: %e\n", trReff);
#endif
	return trReff;
}

__inline__
double
/* 
estimate_cReff(int CH, double k)
*/
estimate_cReff(
	double k /* transistor sizing ratio */)
{
	double Rc, cReff;
/*
	Rc = (CH == NMOS) ? (nRc) : (pRc);
*/
	Rc = (nRc + pRc) / 2.;
	cReff = 2. * Rc / k;
#ifdef DEBUG
	fprintf(stderr, "cReff: %e\n", cReff);
#endif
	return cReff;
}

__inline__
double
estimate_trinvReff(
	double k /* transistor sizing ratio */)
{
	double NMOStrReff, PMOStrReff, trReff;

	NMOStrReff = estimate_trReff(NMOS, k);
	PMOStrReff = estimate_trReff(PMOS, k);

	trReff = (NMOStrReff + PMOStrReff / 2.) / 2.;
#ifdef DEBUG
	fprintf(stderr, "trinvReff: %e\n", trReff);
#endif
	return trReff;
}

__inline__
double
estimate_goutinvReff(
	double k /* transistor sizing ratio */)
{
	double trminReff, cminReff, goutReff;

	trminReff = estimate_trinvReff(1.);
	cminReff = estimate_cReff(1.);
	
	goutReff = (trminReff + 2. * cminReff) / k;
#ifdef DEBUG
	fprintf(stderr, "goutinvReff: %e\n", goutReff);
#endif

	return goutReff;
}

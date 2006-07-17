#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../misc.h"
#include "technology.h"
#include "lv1_opts.h"
#include "lv1_panalyzer.h"
#include "lv1_clock_panalyzer.h"

/* from "Modeling Microprocessor Performance" Chapter 5 & 7 */
fu_lv1_clock_pspec_t *
lv1_create_clock_panalyzer(
	char *name, /* memory name */
	/* memory operating parameters: operating frequency/supply voltage */
	double opfreq, double svolt, 
	double Ceff)
{
	fu_lv1_clock_pspec_t *pspec;
	int i /* loop index */;

	fprintf(stderr, "\n**********************************************\n");
	fprintf(stderr, "\t\tLevel 1 clock tree panalyzer configuration\n");
	fprintf(stderr,   "**********************************************\n");

	/* create pspec data structure */
	if(!(pspec = (fu_lv1_clock_pspec_t *)malloc(sizeof(fu_lv1_clock_pspec_t))))
		fatal("lack of virtual memory!");
	pspec->name = strdup(name);
	fprintf(stderr, "name: %s\n", name);

	pspec->opfreq = opfreq;
    pspec->svolt = svolt;

	pspec->Ceff = Ceff; 
	
	pspec->clock_total_power = 0;
	pspec->clock_power = 0; 
	pspec->max_power = 0;
    
	return pspec;
}

/* clock panalyzer: estimate pdissipatino of the structure */
void 
lv1_clock_panalyzer(
	fu_lv1_clock_pspec_t *pspec, /* memory pspec*/
	tick_t now /* current simulation time */)
{
	tick_t tstart; /* transaction start sim_cycle */
	double dynamic_power = 0;
	dynamic_power = 0.5 * pspec->opfreq * pspec->Ceff * ((pspec->svolt) * (pspec->svolt));   
	// power estimated from access
	pspec->clock_total_power += dynamic_power; 
	pspec->clock_power = dynamic_power; 
	pspec->max_power = 
		(pspec->max_power > dynamic_power) ? pspec->max_power:dynamic_power;     
}



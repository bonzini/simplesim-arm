#include <stdio.h>
//#include "lv1_opts.h"

/* power monitoring windows */
#define MaxPMWindows 512


/* effective capacitances type for power dissipation components */
typedef struct _fu_lv1_Ceffs_t {
	double iCeff;    	/* internal switching effective capacitance */
	double eCeff;    	/* external switching effective capacitance */
} fu_lv1_Ceffs_t;



/* initialize a frame */  
fu_lv1_pwr_frame_t * 
	init_pwr_frame();

/* create a frame and setup/insert for evaluation */
fu_lv1_pwr_frame_t *
insert_pwr_frame(
	 fu_lv1_pwr_frame_t *pwr_frame,
	 tick_t now,
	 unsigned lat
	 );

/* delete a frame */
void 
delete_pwr_frame(
	fu_lv1_pwr_frame_t *pwr_frame,
	fu_lv1_pwr_frame_t *root_frame
	);

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

/* initialize pdissipation stat */
void 
initialize_fu_lv1_pdissipation(
	fu_lv1_pdissipation_t *pdissipation /* pdissipation stat ptr */);

/* initialize pmwindow */
void 
initialize_fu_lv1_pmwindow(
	unsigned pmwindex, /* fu power monitoring window index */
	fu_lv1_pdissipation_t *pmwindow /* fu power monitoring window */);

/* integrate the current power dissipation of fu from power monitoring windows 
 * to pdissipation stat db. */ 
double 
integrate_fu_lv1_pdissipation(
	unsigned pmwindex,	/* fu power monitoring window index */
	fu_lv1_pdissipation_t *pmwindow, 	/* fu power monitoring window */
	fu_lv1_pdissipation_t *pdissipation	/* fu power dissipation statistics */);

/* summate x/y switching + internal + leakage pdissipation of the current cycle.
 * return the summated pdissipation */
double 
summate_fu_lv1_pdissipation(
	unsigned pmwindex,	/* fu power monitoring window index */
	fu_lv1_pdissipation_t *pmwindow	/* fu power monitoring window */);

/* return pdissipation 
 * caution: this function must incorporate with ntransitions */
double 
estimate_pdissipation(
	double Ceff, /* effective capacitance */
	double opfreq, /* operating frequency */
	double svolt /* supply voltage */);


enum pspec_component {lv1_alu,lv1_fpu,lv1_mult,
	lv1_bpred,lv1_rf,lv1_il1,lv1_il2,lv1_dl1,lv1_dl2,
	lv1_itlb,lv1_dtlb,lv1_clock,lv1_io,lv1_uarch,lv1_uarch_clear,
	lv1_pa_trace,lv1_pa_stats
	};

void sim_lv1_panalyzer_check_options(
	double *tech_volt,
	double *tech_freq,
	double *alu_Ceff,
	double *fpu_Ceff,
	double *mult_Ceff,
	double *rf_Ceff,
	double *bpred_Ceff,
	double *clock_Ceff,
	char *io_option,
 	int io_access_lat,
    int io_burst_lat, 
	fu_lv1_cache_Ceff_t *il1_Ceff,
	fu_lv1_cache_Ceff_t *il2_Ceff,
	fu_lv1_cache_Ceff_t *dl1_Ceff,
	fu_lv1_cache_Ceff_t *dl2_Ceff,
	fu_lv1_cache_Ceff_t *itlb_Ceff,
	fu_lv1_cache_Ceff_t *dtlb_Ceff);

/* register level 1 power analayzer stats */
void
lv1_panalyzer_reg_stats(
	struct stat_sdb_t *sdb /* stats db ptr */);

/* interface function with sim-panalyzer */
/* Assigns tasks based on */
void lv1_panalyzer(enum pspec_component pa_type,int user_data);

/* print average power stats */ 
void sim_lv1_panalyzer_stats(FILE *fd, int cycle);

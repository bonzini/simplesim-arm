
#include "lv1_opts.h"
#include "lv1_panalyzer.h" 
#include "lv1_io_panalyzer.h" 
#include "lv1_clock_panalyzer.h" 
#include "lv1_cache_panalyzer.h"
#include "lv1_bpred_panalyzer.h"
#include "lv1_alu_panalyzer.h"
#include "lv1_mult_panalyzer.h"
#include "lv1_fpu_panalyzer.h"
#include "lv1_rf_panalyzer.h"
#include "lv1_uarch_panalyzer.h" 

fu_lv1_alu_pspec_t lv1_alu_panal;
fu_lv1_fpu_pspec_t lv1_fpu_panal;
fu_lv1_bpred_pspec_t lv1_bpred_panal;
fu_lv1_mult_pspec_t lv1_mult_panal;
fu_lv1_rf_pspec_t lv1_rf_panal;
fu_lv1_cache_pspec_t lv1_il1_panal;
fu_lv1_cache_pspec_t lv1_dl1_panal;
fu_lv1_cache_pspec_t lv1_il2_panal;
fu_lv1_cache_pspec_t lv1_dl2_panal;
fu_lv1_cache_pspec_t lv1_itlb_panal;
fu_lv1_cache_pspec_t lv1_dtlb_panal;

fu_lv1_io_pspec_t *lv1_io_panal;

fu_lv1_clock_pspec_t *lv1_clock_panal;
fu_lv1_uarch_pspec_t *lv1_uarch_panal;

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
	fu_lv1_cache_Ceff_t *dtlb_Ceff) 
{
	fu_lv1_io_opts_t lv1_io_opts;
	char name[10];
	char io_style;
	
#ifdef DEBUG_PANALYZER_H
	fprintf(stderr,"alu Ceff = %lf\n",*alu_Ceff);
	fprintf(stderr,"fpu Ceff = %lf\n",*fpu_Ceff);
	fprintf(stderr,"mult Ceff = %lf\n",*mult_Ceff);
	fprintf(stderr,"rf Ceff = %lf\n",*rf_Ceff);
	fprintf(stderr,"bpred Ceff = %lf\n",*bpred_Ceff);
//	getchar();
#endif
	/* initialize power structures */
	lv1_panalyzer_init();
	/* assign Ceff to designated components */
	lv1_uarch_panal->volt = 
		lv1_alu_panal.volt = 
		lv1_fpu_panal.volt = 
		lv1_bpred_panal.volt = 
		lv1_mult_panal.volt = 
		lv1_rf_panal.volt = 
		lv1_bpred_panal.volt = 
		lv1_il1_panal.volt = 
		lv1_dl1_panal.volt = 
		lv1_il2_panal.volt = 
		lv1_dl2_panal.volt = 
		lv1_itlb_panal.volt = 
		lv1_dtlb_panal.volt = *tech_volt;
	lv1_uarch_panal->opfreq = 
		lv1_alu_panal.freq = 
		lv1_fpu_panal.freq = 
		lv1_bpred_panal.freq = 
		lv1_mult_panal.freq = 
		lv1_rf_panal.freq = 
		lv1_bpred_panal.freq = 
		lv1_il1_panal.freq = 
		lv1_dl1_panal.freq = 
		lv1_il2_panal.freq = 
		lv1_dl2_panal.freq = 
		lv1_itlb_panal.freq = 
		lv1_dtlb_panal.freq = *tech_freq;
	
	lv1_clock_panal = 
		lv1_create_clock_panalyzer("clock",
		*tech_freq,*tech_volt,
		*clock_Ceff);
	

// extract io options from string	
	sscanf(io_option,"%[^:]:%c:%lf:%d:%d:%lf:%lf",
		name,
		&io_style,
		&(lv1_io_opts.svolt),
		&(lv1_io_opts.buswidth),
		&(lv1_io_opts.bsize),
		&(lv1_io_opts.iCeff),
		&(lv1_io_opts.eCeff));

	   lv1_io_opts.nacycles = io_access_lat;
	   lv1_io_opts.nctcycles = io_burst_lat;

#ifdef DEBUG_PANALYZER_H
	fprintf(stderr,"io option  = %s\n",io_option);
	fprintf(stderr,"bus width  = %d\n",lv1_io_opts.buswidth);
	fprintf(stderr,"nacycles  = %d\n",lv1_io_opts.nacycles);
	fprintf(stderr,"nctcycles = %d\n",lv1_io_opts.nctcycles);
	fprintf(stderr,"bsize = %d\n",lv1_io_opts.bsize);
	fprintf(stderr,"iCeff = %lf\n",lv1_io_opts.iCeff);
	fprintf(stderr,"eCeff = %lf\n",lv1_io_opts.eCeff);
//	getchar();
#endif

	
	switch(io_style) {
		case 'i':
			lv1_io_opts.style = idirBuffer; 
			break;
		case 'b':
			lv1_io_opts.style = bidirBuffer; 
			break;
		case 'o':
			lv1_io_opts.style = odirBuffer; 
			break;		
	}
	
	lv1_io_panal = 
		lv1_create_io_panalyzer("io",*tech_freq,
			lv1_io_opts.svolt,lv1_io_opts.style,
			lv1_io_opts.buswidth,
			lv1_io_opts.nacycles,
			lv1_io_opts.nctcycles,
			lv1_io_opts.bsize,
			lv1_io_opts.iCeff,
			lv1_io_opts.eCeff);
		
		 
	lv1_alu_panal.iCeff = *alu_Ceff;
	lv1_fpu_panal.iCeff = *fpu_Ceff;
	lv1_mult_panal.iCeff = *mult_Ceff;
	lv1_rf_panal.iCeff = *rf_Ceff;
	lv1_bpred_panal.iCeff = *bpred_Ceff;
	lv1_il1_panal.iCeff = il1_Ceff->iCeff;
	lv1_il1_panal.eCeff = il1_Ceff->eCeff;
	lv1_dl1_panal.iCeff = dl1_Ceff->iCeff;
	lv1_dl1_panal.eCeff = dl1_Ceff->eCeff;
	lv1_il2_panal.iCeff = il2_Ceff->iCeff;
	lv1_il2_panal.eCeff = il2_Ceff->eCeff;
	lv1_dl2_panal.iCeff = dl2_Ceff->iCeff;
	lv1_dl2_panal.eCeff = dl2_Ceff->eCeff;
	lv1_itlb_panal.iCeff = itlb_Ceff->iCeff;
	lv1_itlb_panal.eCeff = itlb_Ceff->eCeff;
	lv1_dtlb_panal.iCeff = dtlb_Ceff->iCeff;
	lv1_dtlb_panal.eCeff = dtlb_Ceff->eCeff;
}

void lv1_panalyzer_init() {
	memset(&lv1_alu_panal,0,sizeof(fu_lv1_alu_pspec_t)); 
	memset(&lv1_fpu_panal,0,sizeof(fu_lv1_fpu_pspec_t)); 
	memset(&lv1_bpred_panal,0,sizeof(fu_lv1_bpred_pspec_t)); 
	memset(&lv1_rf_panal,0,sizeof(fu_lv1_rf_pspec_t)); 
	memset(&lv1_mult_panal,0,sizeof(fu_lv1_mult_pspec_t)); 
	memset(&lv1_il1_panal,0,sizeof(fu_lv1_cache_pspec_t)); 
	memset(&lv1_il2_panal,0,sizeof(fu_lv1_cache_pspec_t)); 
	memset(&lv1_dl1_panal,0,sizeof(fu_lv1_cache_pspec_t)); 
	memset(&lv1_dl2_panal,0,sizeof(fu_lv1_cache_pspec_t)); 
	memset(&lv1_itlb_panal,0,sizeof(fu_lv1_cache_pspec_t)); 
	memset(&lv1_dtlb_panal,0,sizeof(fu_lv1_cache_pspec_t)); 
	lv1_uarch_panal = create_lv1_uarch_panalyzer("uarch",0,0,
		&lv1_alu_panal,
		&lv1_fpu_panal,
		&lv1_bpred_panal,
		&lv1_mult_panal,
		&lv1_rf_panal,
		&lv1_il1_panal,
		&lv1_dl1_panal,
		&lv1_il2_panal,
		&lv1_dl2_panal,
		&lv1_itlb_panal,
		&lv1_dtlb_panal
		);
}

/* register level 1 power analayzer stats */
void
lv1_panalyzer_reg_stats(
	struct stat_sdb_t *sdb /* stats db ptr */)
{
	stat_reg_int(sdb, "alu access", "number of times alu is accessed", 
		&(lv1_alu_panal.alu_access), 0, NULL);	
	stat_reg_double(sdb, "alu max power", "Maximum power for alu", 
		&(lv1_alu_panal.max_power), 0, NULL);

	stat_reg_int(sdb, "fpu access", "number of times fpu is accessed", 
		&(lv1_fpu_panal.fpu_access), 0, NULL);
	stat_reg_double(sdb, "fpu max power", "Maximum power for fpu", 
		&(lv1_fpu_panal.max_power), 0, NULL);


	stat_reg_int(sdb, "mult access", "number of times mult is accessed", 
		&(lv1_mult_panal.mult_access), 0, NULL);
	stat_reg_double(sdb, "mult max power", "Maximum power for mult", 
		&(lv1_mult_panal.max_power), 0, NULL);

	stat_reg_int(sdb, "rf access", "number of times rf is accessed", 
		&(lv1_rf_panal.rf_access), 0, NULL);


	stat_reg_double(sdb, "rf max power", "Maximum power for rf", 
		&(lv1_rf_panal.max_power), 0, NULL);

	stat_reg_int(sdb, "bpred access", "number of times bpred is accessed", 
		&(lv1_bpred_panal.bpred_access), 0, NULL);
	stat_reg_double(sdb, "bpred max power", "Maximum power for bpred", 
		&(lv1_bpred_panal.max_power), 0, NULL);

	stat_reg_int(sdb, "il1 access", "number of times il1 access is accessed", 
		&(lv1_il1_panal.cache_access), 0, NULL);
	stat_reg_double(sdb, "il1 max power", "Maximum power for il1", 
		&(lv1_il1_panal.max_power), 0, NULL);

	stat_reg_int(sdb, "il2 access", "number of times il2 access is accessed", 
		&(lv1_il2_panal.cache_access), 0, NULL);
	stat_reg_double(sdb, "il2 max power", "Maximum power for il2", 
		&(lv1_il2_panal.max_power), 0, NULL);

	stat_reg_int(sdb, "dl1 access", "number of times dl1 access is accessed", 
		&(lv1_dl1_panal.cache_access), 0, NULL);
	stat_reg_double(sdb, "dl1 max power", "Maximum power for dl1", 
		&(lv1_dl1_panal.max_power), 0, NULL);

	stat_reg_int(sdb, "dl2 access", "number of times dl2 access is accessed", 
		&(lv1_dl2_panal.cache_access), 0, NULL);
	stat_reg_double(sdb, "dl2 max power", "Maximum power for dl2", 
		&(lv1_dl2_panal.max_power), 0, NULL);

	stat_reg_int(sdb, "itlb access", "number of times itlb access is accessed", 
		&(lv1_itlb_panal.cache_access), 0, NULL);
	stat_reg_double(sdb, "itlb max power", "Maximum power for itlb", 
		&(lv1_itlb_panal.max_power), 0, NULL);

	stat_reg_int(sdb, "dtlb access", "number of times dtlb access is accessed", 
		&(lv1_dtlb_panal.cache_access), 0, NULL);
	stat_reg_double(sdb, "dtlb max power", "Maximum power for dtlb", 
		&(lv1_dtlb_panal.max_power), 0, NULL);

	stat_reg_int(sdb, "io access", "number of times io access is accessed", 
		&(lv1_io_panal->io_access), 0, NULL);
	stat_reg_double(sdb, "aio max power", "Maximum power for aio", 
		&(lv1_io_panal->max_aio_power), 0, NULL);
	stat_reg_double(sdb, "dio max power", "Maximum power for dio", 
		&(lv1_io_panal->max_dio_power), 0, NULL);

	stat_reg_double(sdb, "clock max power", "Maximum power for clock", 
		&(lv1_clock_panal->max_power), 0, NULL);

	stat_reg_double(sdb, "uarch max power", "Maximum power for uarch", 
		&(lv1_uarch_panal->max_power), 0, NULL);

		
}

/* interface function with sim-panalyzer */
/* Assigns tasks based on */
void lv1_panalyzer(enum pspec_component pa_type,int user_data)
{
	switch(pa_type)
	{
		case lv1_alu:
			lv1_alu_panalyzer(&lv1_alu_panal);
			break;
		case lv1_fpu:
			lv1_fpu_panalyzer(&lv1_fpu_panal);
			break;
		case lv1_mult:
			lv1_mult_panalyzer(&lv1_mult_panal);
			break;
		case lv1_rf:
			lv1_rf_panalyzer(&lv1_rf_panal);
			break;
		case lv1_bpred:
			lv1_bpred_panalyzer(&lv1_bpred_panal);
			break;				
		case lv1_il1:
			lv1_cache_panalyzer(&lv1_il1_panal,(fu_mcommand_t) user_data);
			break;
		case lv1_il2:
			lv1_cache_panalyzer(&lv1_il2_panal,(fu_mcommand_t) user_data);
			break;
		case lv1_dl1:
			lv1_cache_panalyzer(&lv1_dl1_panal,(fu_mcommand_t) user_data);
			break;
		case lv1_dl2:
			lv1_cache_panalyzer(&lv1_dl2_panal,(fu_mcommand_t) user_data);
			break;
		case lv1_itlb:
			lv1_cache_panalyzer(&lv1_itlb_panal,(fu_mcommand_t) user_data);
			break;
		case lv1_dtlb:
			lv1_cache_panalyzer(&lv1_dtlb_panal,(fu_mcommand_t) user_data);
			break;
		case lv1_io:
		{
			fu_lv1_io_arg_t * io_arg = (fu_lv1_io_arg_t *) user_data;
			
			/*fprintf(stderr,"lv1 io arg = %d\n",io_arg);  
			fprintf(stderr,"lv1 io arg frame_size = %d\n",io_arg->lat);  
			fprintf(stderr,"lv1 io arg pwr_frame = %d\n",io_arg->pwr_frame);  
			getchar();
			*/
			// pwr_frame holds new power frame 
			fu_lv1_pwr_frame_t * pwr_frame = 
				insert_pwr_frame(
					 io_arg->pwr_frame,
					 io_arg->now,
					 io_arg->lat
	 			);

			lv1_aio_panalyzer(lv1_io_panal, io_arg->command, 
				io_arg->address, io_arg->buffer, pwr_frame, 
				io_arg->bsize, io_arg->now, io_arg->lat);
			lv1_dio_panalyzer(lv1_io_panal, io_arg->command, 
				io_arg->address, io_arg->buffer, pwr_frame,
				io_arg->bsize, io_arg->now, io_arg->lat);
			break;
			/* io accesses occur in pairs  */
		}
		case lv1_clock:
			lv1_clock_panalyzer(lv1_clock_panal,0);
			break;	
		case lv1_uarch:
		{
			fu_lv1_uarch_arg_t * uarch_arg = (fu_lv1_uarch_arg_t *) user_data;
			lv1_uarch_panalyzer(lv1_uarch_panal,uarch_arg->now,
				(void*) uarch_arg->pwr_frame);	
			break;
		}
		
		case lv1_uarch_clear:
		{
			lv1_uarch_clear_panalyzer(lv1_uarch_panal);	
			break;
		}
		
		case lv1_pa_trace:
		{
			fu_lv1_pa_trace_arg_t * pa_trace_arg = (fu_lv1_pa_trace_arg_t *) user_data;
			int windex = pa_trace_arg->now % 100000;
			pa_trace_arg->window[windex] = lv1_uarch_panal->uarch_power;
			#ifdef PA_TRANS_COUNT 
			pa_trace_arg->aio_ham_window[windex] = lv1_uarch_panal->aio_trans; // address transition
			pa_trace_arg->dio_ham_window[windex] = lv1_uarch_panal->dio_trans; // data transition
			#endif   
			pa_trace_arg->miss_window[windex] = pa_trace_arg->cache_miss;
		}
			break;

		case lv1_pa_stats:
		{
			fu_lv1_pa_trace_arg_t * pa_trace_arg = (fu_lv1_pa_trace_arg_t *) user_data;
			FILE * fp = pa_trace_arg->fp;
			int  i;
			fprintf(stderr,"power window \n");
			for(i = 0;i< 100000;i++)
			{
				fprintf(stderr,"%lf  ",pa_trace_arg->window[i]);
			#ifdef PA_TRANS_COUNT 
				fprintf(stderr,"%d  ",pa_trace_arg->aio_ham_window[i]);
				fprintf(stderr,"%d  ",pa_trace_arg->dio_ham_window[i]);
			/*	if(pa_trace_arg->dio_ham_window[i] != 0)
					getchar(); */
			#endif   

				if(pa_trace_arg->miss_window[i]) 
					fprintf(stderr,"miss\n");
				else 
					fprintf(stderr,"hit\n");
			}
		}
			break;	
		default:
			break; 	
	}
}

void sim_lv1_panalyzer_stats(FILE *fd,int cycle) {
	fprintf(fd,"average alu power \t\t%lf\n", lv1_alu_panal.alu_total_power / 
		(double) (cycle));
	fprintf(fd,"average fpu power \t\t%lf\n", lv1_fpu_panal.fpu_total_power / 
		(double) (cycle));
	fprintf(fd,"average mult power \t\t%lf\n", lv1_mult_panal.mult_total_power / 
		(double) (cycle));
	fprintf(fd,"average bpred power \t\t%lf\n", lv1_bpred_panal.bpred_total_power / 
		(double) (cycle));
	fprintf(fd,"average rf power \t\t%lf\n", lv1_rf_panal.rf_total_power / 
		(double) (cycle));
	fprintf(fd,"average il1 power \t\t%lf\n", lv1_il1_panal.cache_total_power / 
		(double) (cycle));
	fprintf(fd,"average il2 power \t\t%lf\n", lv1_il2_panal.cache_total_power / 
		(double) (cycle));
	fprintf(fd,"average dl1 power \t\t%lf\n", lv1_dl1_panal.cache_total_power / 
		(double) (cycle));
	fprintf(fd,"average dl2 power \t\t%lf\n", lv1_dl2_panal.cache_total_power / 
		(double) (cycle));
	fprintf(fd,"average itlb power \t\t%lf\n", lv1_itlb_panal.cache_total_power / 
		(double) (cycle));
	fprintf(fd,"average dtlb power \t\t%lf\n", lv1_dtlb_panal.cache_total_power / 
		(double) (cycle));
	fprintf(fd,"average aio power \t\t%lf\n", lv1_io_panal->aio_total_power / 
	(double) (cycle));
	fprintf(fd,"average dio power \t\t%lf\n", lv1_io_panal->dio_total_power / 
	(double) (cycle));


	fprintf(fd,"average clock power \t\t%lf\n", lv1_clock_panal->clock_total_power / 
	(double) (cycle));

	fprintf(fd,"average uarch power \t\t%lf\n", lv1_uarch_panal->uarch_total_power / 
	(double) (cycle));

}

/* initialize a frame */  
fu_lv1_pwr_frame_t * 
	init_pwr_frame()
{
	fu_lv1_pwr_frame_t *pwr_frame;
	pwr_frame = (fu_lv1_pwr_frame_t *) malloc(sizeof(fu_lv1_pwr_frame_t));
	if(pwr_frame == NULL)
		fatal("Not enough memory\n");
	pwr_frame->type = root;
	pwr_frame->frame_size = 0;	
	return pwr_frame;
}

/* create a frame and setup/insert for evaluation */
fu_lv1_pwr_frame_t * 
insert_pwr_frame(
	 fu_lv1_pwr_frame_t *pwr_frame,
	 tick_t now,
	 unsigned lat
	 )
{
	fu_lv1_pwr_frame_t *frame = pwr_frame;
	fu_lv1_pwr_frame_t *new_frame;
	/* go to the end of the frame */
	while(frame->next != NULL) {
		frame = frame->next;
	}
	new_frame = (fu_lv1_pwr_frame_t *) malloc(sizeof(fu_lv1_pwr_frame_t));
	if(new_frame == NULL)
		fatal("Not enough Memory\n");
	new_frame->type = start;
	new_frame->frame_size = lat;
	new_frame->frame_start = now;
	new_frame->frame_contents = (fu_lv1_pdissipation_t *) malloc(sizeof(fu_lv1_pdissipation_t) * lat);
	new_frame->next = NULL;
	frame->next = new_frame;
	return new_frame;
}

/* delete a frame */
void 
delete_pwr_frame(
	fu_lv1_pwr_frame_t *pwr_frame,
	fu_lv1_pwr_frame_t *root_frame
	)
{
	fu_lv1_pwr_frame_t *frame = pwr_frame;
	if(root_frame->next != pwr_frame)
		fatal("Root frame and frames incorrectly aligned\n");
	// reconnect root with next frame
	root_frame->next = pwr_frame->next; 
	
	// delete frame_contents first;
	free(pwr_frame->frame_contents);
	// delete others
	free(pwr_frame);	
}




__inline__
/* create/converte to buffer_t type data structure for various data types.
 * return an allocated location pointer.
 * caution: please deallocate the memory space  */
buffer_t *
create_buffer_t(
	void *val, 	/* target conversion value */
	int bsize 	/* conversion value data type size in bytes */)
{
	int i;
	buffer_t *p, *vp;

	if(!(p = (buffer_t *)calloc(bsize, sizeof(buffer_t))))
		fatal("out of virtual memory!");
	vp = val;
	for(i = 0; i < bsize; i++)
		*(p+i) = *(vp+(bsize-1-i));

	return p;
}

__inline__
/* copy buffer_t type data structure */
void
copy_buffer_t(
	buffer_t *ibus, /* the source bus */ 
	buffer_t *jbus, /* the destination bus */
	unsigned bsize /* size of the bus in bytes */)
{
	int i;

	for(i = 0; i < bsize; i++)
		*(jbus + i) = *(ibus + i);
}

__inline__
/* return number of switching for the give previous bus and the current bus */
unsigned
obtain_nswitchings(
	buffer_t *ibus, /* the current bus */ 
	buffer_t *jbus, /* the previous bus */
	unsigned bsize /* size of the bus in bytes */)
{
	unsigned i;
	unsigned nswitchings;
	buffer_t tbus;

	nswitchings = 0;
	for(i = 0; i < bsize; i++) {
		tbus = *(ibus + i) ^ *(jbus + i);
		while(tbus) {
			if(tbus & 0x01)
				nswitchings++;
			tbus >>= 1;
		}
	}
	return nswitchings;
}

__inline__
/* initialize pdissipation stat */
void 
initialize_fu_lv1_pdissipation(
	fu_lv1_pdissipation_t *pdissipation /* pdissipation stat ptr */)
{
	pdissipation->external = 0.0;
	pdissipation->internal = 0.0;
}

__inline__
/* initialize pmwindow 
 * we dont touch the leakage power and peak power in pmwindows */
void 
initialize_fu_lv1_pmwindow(
	unsigned pmwindex, /* fu power monitoring window index */
	fu_lv1_pdissipation_t *pmwindow /* fu power monitoring window */)
{
	(pmwindow+pmwindex)->external = 0.0;
	(pmwindow+pmwindex)->internal = 0.0;
}

__inline__
/* integrate the current power dissipation of fu from power monitoring windows 
 * to pdissipation stat db. */ 
double 
integrate_fu_lv1_pdissipation(
	unsigned pmwindex,	/* fu power monitoring window index */
	fu_lv1_pdissipation_t *pmwindow, /* fu power monitoring window */
	fu_lv1_pdissipation_t *pdissipation	/* fu power dissipation statistics */)
{
	/* for each funtional unit */
	pdissipation->external += (pmwindow + pmwindex)->external;
	pdissipation->internal += (pmwindow + pmwindex)->internal;
	/* is this a peak power value? */
	
	return (pmwindow + pmwindex)->pdissipation;
}

__inline__
/* summate switching + internal + leakage pdissipation of the current cycle.
 * return the summated pdissipation */
double 
summate_fu_lv1_pdissipation(
	unsigned pmwindex,				/* fu power monitoring window index */
	fu_lv1_pdissipation_t *pmwindow	/* fu power monitoring window */)
{
	double pdissipation;

	pdissipation 
		= (pmwindow + pmwindex)->external
		+ (pmwindow + pmwindex)->internal;

	return pdissipation;
}

__inline__
/* return pdissipation 
 * caution: this function must incorporate with ntransitions */
double 
estimate_pdissipation(
	double Ceff, /* effective capacitance */
	double opfreq, /* operating frequency */
	double svolt /* supply voltage */) 
{
	return (Ceff * opfreq * (svolt * svolt));
}


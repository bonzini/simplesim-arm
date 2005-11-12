/* I inclued this copyright since we're using Cacti for some stuff */

/*------------------------------------------------------------
 *  Copyright 1994 Digital Equipment Corporation and Steve Wilton
 *                         All Rights Reserved
 *
 * Permission to use, copy, and modify this software and its documentation is
 * hereby granted only under the following terms and conditions.  Both the
 * above copyright notice and this permission notice must appear in all copies
 * of the software, derivative works or modified versions, and any portions
 * thereof, and both notices must appear in supporting documentation.
 *
 * Users of this software agree to the terms and conditions set forth herein,
 * and hereby grant back to Digital a non-exclusive, unrestricted, royalty-
 * free right and license under any changes, enhancements or extensions
 * made to the core functions of the software, including but not limited to
 * those affording compatibility with other hardware or software
 * environments, but excluding applications which incorporate this software.
 * Users further agree to use their best efforts to return to Digital any
 * such changes, enhancements or extensions that they make and inform Digital
 * of noteworthy uses of this software.  Correspondence should be provided
 * to Digital at:
 *
 *                       Director of Licensing
 *                       Western Research Laboratory
 *                       Digital Equipment Corporation
 *                       100 Hamilton Avenue
 *                       Palo Alto, California  94301
 *
 * This software may be distributed (but not offered for sale or transferred
 * for compensation) to third parties, provided such third parties agree to
 * abide by the terms and conditions of this notice.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND DIGITAL EQUIPMENT CORP. DISCLAIMS ALL
 * WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS.   IN NO EVENT SHALL DIGITAL EQUIPMENT
 * CORPORATION BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS
 * ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 *------------------------------------------------------------*/

#include <math.h>
#include <assert.h>
#include "power.h"
#include "machine.h"
#include "cache.h"
#include "sim-outorder.h"

#define SensePowerfactor (Mhz)*(Vdd/2)*(Vdd/2)
#define Sense2Powerfactor (Mhz)*(2*.3+.1*Vdd)
#define Powerfactor (Mhz)*Vdd*Vdd
#define LowSwingPowerfactor (Mhz)*.2*.2
/* set scale for crossover (vdd->gnd) currents */
static double crossover_scaling = 1.2;
/* set non-ideal turnoff percentage */
static double turnoff_factor = 0.1;
static double inst_width = (double)sizeof(md_inst_t);

#define MSCALE (LSCALE * .624 / .2250)

/*----------------------------------------------------------------------*/

/* static power model results */
power_result_type power;

int pow2(int x) {
  return((int)pow(2.0,(double)x));
}

double logfour(x)
     double x;
{
  if (x<=0) fprintf(stderr,"%e\n",x);
  return( (double) (log(x)/log(4.0)) );
}

/* safer pop count to validate the fast algorithm */
int pop_count_slow(qword_t bits)
{
  int count = 0; 
  qword_t tmpbits = bits; 
  while (tmpbits) { 
    if (tmpbits & 1) ++count; 
    tmpbits >>= 1; 
  } 
  return count; 
}

/* fast pop count */
int pop_count(qword_t bits)
{
#define T unsigned long long
#define PONES ((T)(-1)) 
#define TWO(k) ((T)1 << (k)) 
#define CYCL(k) (PONES/(1 + (TWO(TWO(k))))) 
#define BSUM(x,k) ((x)+=(x) >> TWO(k), (x) &= CYCL(k)) 
  qword_t x = bits; 
  x = (x & CYCL(0)) + ((x>>TWO(0)) & CYCL(0)); 
  x = (x & CYCL(1)) + ((x>>TWO(1)) & CYCL(1)); 
  BSUM(x,2); 
  BSUM(x,3); 
  BSUM(x,4); 
  BSUM(x,5); 
  return x; 
}


int opcode_length = 4;
int inst_length = 32;

int nvreg_width;
int npreg_width;

double global_clockcap;

static double rename_power=0;
static double bpred_power=0;
static double window_power=0;
static double lsq_power=0;
static double regfile_power=0;
static double icache_power=0;
static double icache2_power=0;
static double dcache_power=0;
static double dcache2_power=0;
/* Added by kimns */
/*
static double itlb_power=0;
static double dtlb_power=0;
*/
static double alu_power=0;
static double falu_power=0;
static double resultbus_power=0;
static double clock_power=0;

static double rename_power_cc1=0;
static double hd_bpred_power_cc1=0;
static double bpred_power_cc1=0;
static double window_power_cc1=0;
static double lsq_power_cc1=0;
static double regfile_power_cc1=0;
static double icache_power_cc1=0;
static double hd_icache_power_cc1=0;
static double icache2_power_cc1=0;
static double hd_icache2_power_cc1=0;
static double dcache_power_cc1=0;
static double hd_dcache_power_cc1=0;
static double dcache2_power_cc1=0;
static double hd_dcache2_power_cc1=0;
static double alu_power_cc1=0;
static double resultbus_power_cc1=0;
static double clock_power_cc1=0;

static double rename_power_cc2=0;
static double bpred_power_cc2=0;
static double hd_bpred_power_cc2=0;
static double window_power_cc2=0;
static double lsq_power_cc2=0;
static double regfile_power_cc2=0;
static double icache_power_cc2=0;
static double hd_icache_power_cc2=0;
static double icache2_power_cc2=0;
static double hd_icache2_power_cc2=0;
static double dcache_power_cc2=0;
static double hd_dcache_power_cc2=0;
static double dcache2_power_cc2=0;
static double hd_dcache2_power_cc2=0;
static double alu_power_cc2=0;
static double resultbus_power_cc2=0;
static double clock_power_cc2=0;

static double rename_power_cc3=0;
static double bpred_power_cc3=0;
static double hd_bpred_power_cc3=0;
static double window_power_cc3=0;
static double lsq_power_cc3=0;
static double regfile_power_cc3=0;
static double icache_power_cc3=0;
static double hd_icache_power_cc3=0;
static double icache2_power_cc3=0;
static double hd_icache2_power_cc3=0;
static double dcache_power_cc3=0;
static double hd_dcache_power_cc3=0;
static double dcache2_power_cc3=0;
static double hd_dcache2_power_cc3=0;
static double alu_power_cc3=0;
static double resultbus_power_cc3=0;
static double clock_power_cc3=0;

static double total_cycle_power;
static double total_cycle_power_cc1;
static double total_cycle_power_cc2;
static double total_cycle_power_cc3;
static double hd_total_cycle_power_cc1;
static double hd_total_cycle_power_cc2;
static double hd_total_cycle_power_cc3;

static double last_single_total_cycle_power_cc1 = 0.0;
static double last_single_total_cycle_power_cc2 = 0.0;
static double last_single_total_cycle_power_cc3 = 0.0;
static double current_total_cycle_power_cc1;
static double current_total_cycle_power_cc2;
static double current_total_cycle_power_cc3;

static double max_cycle_power_cc1 = 0.0;
static double max_cycle_power_cc2 = 0.0;
static double max_cycle_power_cc3 = 0.0;


static counter_t total_rename_access=0;
static counter_t total_bpred_access=0;
static counter_t total_window_access=0;
static counter_t total_lsq_access=0;
static counter_t total_regfile_access=0;
static counter_t total_icache_access=0;
static counter_t total_icache2_access=0;
static counter_t total_dcache_access=0;
static counter_t total_dcache2_access=0;
static counter_t total_alu_access=0;
static counter_t total_resultbus_access=0;

static counter_t max_rename_access;
static counter_t max_bpred_access;
static counter_t max_window_access;
static counter_t max_lsq_access;
static counter_t max_regfile_access;
static counter_t max_icache_access;
static counter_t max_icache2_access;
static counter_t max_dcache_access;
static counter_t max_dcache2_access;
static counter_t max_alu_access;
static counter_t max_resultbus_access;

/* used in sim-outorder */
void clear_access_stats()
{
  rename_access=0;
  bpred_access=0;
  window_access=0;
  lsq_access=0;
  regfile_access=0;
  icache_access=0;
  icache2_access=0;
  dcache_access=0;
  dcache2_access=0;
  alu_access=0;
  ialu_access=0;
  falu_access=0;
  resultbus_access=0;

  window_preg_access=0;
  window_selection_access=0;
  window_wakeup_access=0;
  lsq_store_data_access=0;
  lsq_load_data_access=0;
  lsq_wakeup_access=0;
  lsq_preg_access=0;

  window_total_pop_count_cycle=0;
  window_num_pop_count_cycle=0;
  lsq_total_pop_count_cycle=0;
  lsq_num_pop_count_cycle=0;
  regfile_total_pop_count_cycle=0;
  regfile_num_pop_count_cycle=0;
  resultbus_total_pop_count_cycle=0;
  resultbus_num_pop_count_cycle=0;

}


/* 
   compute bitline activity factors which we use to scale bitline power 
   Here it is very important whether we assume 0's or 1's are
   responsible for dissipating power in pre-charged stuctures. (since
   most of the bits are 0's, we assume the design is power-efficient
   enough to allow 0's to _not_ discharge 
*/


/* for internal use of power.c */
double compute_af(counter_t num_pop_count_cycle,counter_t total_pop_count_cycle,int pop_width) {
  double avg_pop_count;
  double af,af_b;

  if(num_pop_count_cycle)
    avg_pop_count = (double)total_pop_count_cycle / (double)num_pop_count_cycle;
  else
    avg_pop_count = 0;

  af = avg_pop_count / (double)pop_width;
  
  af_b = 1.0 - af;

  /*  printf("af == %f%%, af_b == %f%%, total_pop == %d, num_pop == %d\n",100*af,100*af_b,total_pop_count_cycle,num_pop_count_cycle); */

  return(af_b);
}


/* 
  
 compute power statistics on each cycle, for each conditional clocking style.  Obviously
 most of the speed penalty comes here, so if you don't want per-cycle power estimates
 you could post-process 

 See README.wattch for details on the various clock gating styles.

*/


/* used in sim-outorder */
void update_power_stats()
{
  double window_af_b, lsq_af_b, regfile_af_b, resultbus_af_b;

#ifdef DYNAMIC_AF
  window_af_b 	 = compute_af(window_num_pop_count_cycle,window_total_pop_count_cycle,data_width);
  lsq_af_b 	 = compute_af(lsq_num_pop_count_cycle,lsq_total_pop_count_cycle,data_width);
  regfile_af_b 	 = compute_af(regfile_num_pop_count_cycle,regfile_total_pop_count_cycle,data_width);
  resultbus_af_b = compute_af(resultbus_num_pop_count_cycle,resultbus_total_pop_count_cycle,data_width);
#endif
 
  rename_power	 += power.rename_power;
  bpred_power	 += power.bpred_power;
  window_power	 += power.window_power;
  lsq_power	 += power.lsq_power;
  regfile_power	 += power.regfile_power;
  icache_power	 += power.icache_power+power.itlb;
  icache2_power	 += power.icache2_power;
  dcache_power	 += power.dcache_power+power.dtlb;
  dcache2_power	 += power.dcache2_power;
  alu_power	 += power.ialu_power + power.falu_power;
  falu_power	 += power.falu_power;
  resultbus_power+= power.resultbus;
  clock_power	 += power.clock_power;

  total_rename_access	+= rename_access;
  total_bpred_access	+= bpred_access;
  total_window_access	+= window_access;
  total_lsq_access	+= lsq_access;
  total_regfile_access	+= regfile_access;
  total_icache_access	+= icache_access;
  total_icache2_access	+= icache2_access;
  total_dcache_access	+= dcache_access;
  total_dcache2_access	+= dcache2_access;
  total_alu_access	+= alu_access;
  total_resultbus_access+= resultbus_access;

  max_rename_access	= MAX(rename_access,max_rename_access);
  max_bpred_access	= MAX(bpred_access,max_bpred_access);
  max_window_access	= MAX(window_access,max_window_access);
  max_lsq_access	= MAX(lsq_access,max_lsq_access);
  max_regfile_access	= MAX(regfile_access,max_regfile_access);
  max_icache_access	= MAX(icache_access,max_icache_access);
  max_icache2_access	= MAX(icache2_access,max_icache2_access);
  max_dcache_access	= MAX(dcache_access,max_dcache_access);
  max_dcache2_access	= MAX(dcache2_access,max_dcache2_access);
  max_alu_access	= MAX(alu_access,max_alu_access);
  max_resultbus_access	= MAX(resultbus_access,max_resultbus_access);

  if(rename_access) {
    rename_power_cc1 += power.rename_power;
    rename_power_cc2 += ((double)rename_access/(double)ruu_decode_width)*power.rename_power;
    rename_power_cc3 += ((double)rename_access/(double)ruu_decode_width)*power.rename_power;
  }
  else 
    rename_power_cc3 += turnoff_factor*power.rename_power;


  if(bpred_access) {
    double tmp_bpred_power_cc = (double)btb_decaddr_hd * power.hd_btb_decoder_1ststg 
	             +  ((btb_mem_cmd == Read) ? power.hd_btb_bitline_rd:power.hd_btb_bitline_wr) 
		     +  (double)btaddr_hd * power.hd_btb_senseamp
		     +  (double)btb_decaddr_hd * power.hd_btb_tag_decoder_1ststg
		     +  ((btb_mem_cmd == Read) ? power.hd_btb_tag_bitline_rd:power.hd_btb_tag_bitline_wr)
		     +  (double)btb_tagaddr_hd * power.hd_btb_tag_senseamp
		     +  power.hd_btb
		     +  power.bpred_power - power.btb; 
    if(bpred_access <= 2){
      bpred_power_cc1    += power.bpred_power;
      hd_bpred_power_cc1 += tmp_bpred_power_cc;
    }
    else{
      bpred_power_cc1    += ((double)bpred_access/2.0) * power.bpred_power;
      hd_bpred_power_cc1 += ((double)bpred_access/2.0) * tmp_bpred_power_cc;
    }
    bpred_power_cc2    += ((double)bpred_access/2.0) * power.bpred_power;
    hd_bpred_power_cc2 += ((double)bpred_access/2.0) * tmp_bpred_power_cc;
    
    btb_decaddr_hd = 0;
    btaddr_hd = 0;
    btb_tagaddr_hd = 0;
  }
  else
  {
    bpred_power_cc3    += turnoff_factor * power.bpred_power;
    hd_bpred_power_cc3 += turnoff_factor * power.bpred_power;
  }

#ifdef STATIC_AF
  if(window_preg_access) {
    if(window_preg_access <= 3*ruu_issue_width)
      window_power_cc1 += power.rs_power;
    else
      window_power_cc1 += ((double)window_preg_access/(3.0*(double)ruu_issue_width))*power.rs_power;

    window_power_cc2 += ((double)window_preg_access/(3.0*(double)ruu_issue_width))*power.rs_power;
    window_power_cc3 += ((double)window_preg_access/(3.0*(double)ruu_issue_width))*power.rs_power;
  }
  else
    window_power_cc3 += turnoff_factor*power.rs_power;
#elif defined(DYNAMIC_AF)
  if(window_preg_access) {
    
    if(window_preg_access <= 3*ruu_issue_width)
      window_power_cc1 += power.rs_power_nobit+window_af_b*power.rs_bitline;
    else
      window_power_cc1 += ((double)window_preg_access/(3.0*(double)ruu_issue_width))*(power.rs_power_nobit + window_af_b*power.rs_bitline);
    
    window_power_cc2 += ((double)window_preg_access/(3.0*(double)ruu_issue_width))*(power.rs_power_nobit + window_af_b*power.rs_bitline);
    window_power_cc3 += ((double)window_preg_access/(3.0*(double)ruu_issue_width))*(power.rs_power_nobit + window_af_b*power.rs_bitline);
  }
  else
    window_power_cc3 += turnoff_factor*power.rs_power;
#else
  panic("no AF-style defined\n");
#endif

  if(window_selection_access) {
    if(window_selection_access <= ruu_issue_width)
      window_power_cc1 += power.selection;
    else
      window_power_cc1 += ((double)window_selection_access/((double)ruu_issue_width))*power.selection;
    
    window_power_cc2 += ((double)window_selection_access/((double)ruu_issue_width))*power.selection;
    window_power_cc3 += ((double)window_selection_access/((double)ruu_issue_width))*power.selection;
  }
  else
    window_power_cc3 += turnoff_factor*power.selection;

  if(window_wakeup_access) {
    if(window_wakeup_access <= ruu_issue_width)
      window_power_cc1 += power.wakeup_power;
    else
      window_power_cc1 += ((double)window_wakeup_access/((double)ruu_issue_width))*power.wakeup_power;
    
    window_power_cc2 += ((double)window_wakeup_access/((double)ruu_issue_width))*power.wakeup_power;
    window_power_cc3 += ((double)window_wakeup_access/((double)ruu_issue_width))*power.wakeup_power;
  }
  else
    window_power_cc3 += turnoff_factor*power.wakeup_power;

  if(lsq_wakeup_access) {
    if(lsq_wakeup_access <= res_memport)
      lsq_power_cc1 += power.lsq_wakeup_power;
    else
      lsq_power_cc1 += ((double)lsq_wakeup_access/((double)res_memport))*power.lsq_wakeup_power;

    lsq_power_cc2 += ((double)lsq_wakeup_access/((double)res_memport))*power.lsq_wakeup_power;
    lsq_power_cc3 += ((double)lsq_wakeup_access/((double)res_memport))*power.lsq_wakeup_power;
  }
  else
    lsq_power_cc3 += turnoff_factor*power.lsq_wakeup_power;

#ifdef STATIC_AF
  if(lsq_preg_access) {
    if(lsq_preg_access <= res_memport)
      lsq_power_cc1+=power.lsq_rs_power;
    else
      lsq_power_cc1+=((double)lsq_preg_access/((double)res_memport))*power.lsq_rs_power;
    lsq_power_cc2+=((double)lsq_preg_access/((double)res_memport))*power.lsq_rs_power;
    lsq_power_cc3+=((double)lsq_preg_access/((double)res_memport))*power.lsq_rs_power;
  }
  else
    lsq_power_cc3+=turnoff_factor*power.lsq_rs_power;
#else
  if(lsq_preg_access) {
    if(lsq_preg_access <= res_memport)
      lsq_power_cc1+=power.lsq_rs_power_nobit + lsq_af_b*power.lsq_rs_bitline;
    else
      lsq_power_cc1+=((double)lsq_preg_access/((double)res_memport))*(power.lsq_rs_power_nobit + lsq_af_b*power.lsq_rs_bitline);
    lsq_power_cc2+=((double)lsq_preg_access/((double)res_memport))*(power.lsq_rs_power_nobit + lsq_af_b*power.lsq_rs_bitline);
    lsq_power_cc3+=((double)lsq_preg_access/((double)res_memport))*(power.lsq_rs_power_nobit + lsq_af_b*power.lsq_rs_bitline);
  }
  else
    lsq_power_cc3+=turnoff_factor*power.lsq_rs_power;
#endif

#ifdef STATIC_AF
  if(regfile_access) {
    if(regfile_access <= (3.0*ruu_commit_width))
      regfile_power_cc1+=power.regfile_power;
    else
      regfile_power_cc1+=((double)regfile_access/(3.0*(double)ruu_commit_width))*power.regfile_power;
    regfile_power_cc2+=((double)regfile_access/(3.0*(double)ruu_commit_width))*power.regfile_power;
    regfile_power_cc3+=((double)regfile_access/(3.0*(double)ruu_commit_width))*power.regfile_power;
  }
  else
    regfile_power_cc3+=turnoff_factor*power.regfile_power;
#else
  if(regfile_access) {
    if(regfile_access <= (3.0*ruu_commit_width))
      regfile_power_cc1+=power.regfile_power_nobit + regfile_af_b*power.regfile_bitline;
    else
      regfile_power_cc1+=((double)regfile_access/(3.0*(double)ruu_commit_width))*(power.regfile_power_nobit + regfile_af_b*power.regfile_bitline);
    regfile_power_cc2+=((double)regfile_access/(3.0*(double)ruu_commit_width))*(power.regfile_power_nobit + regfile_af_b*power.regfile_bitline);
    regfile_power_cc3+=((double)regfile_access/(3.0*(double)ruu_commit_width))*(power.regfile_power_nobit + regfile_af_b*power.regfile_bitline);
  }
  else
    regfile_power_cc3+=turnoff_factor*power.regfile_power;
#endif

  /* for HD Wattch Model */
  if(icache_access) {
    /* don't scale icache because we assume 1 line is fetched, unless fetch stalls */
    double tmp_icache_power_cc 
	             =  (double)icache_decaddr_hd * power.hd_icache_decoder_1ststg 
	             +  ((icache_mem_cmd == Read) ? power.hd_icache_bitline_rd:power.hd_icache_bitline_wr) 
		     +  (double)icache_inst_hd * power.hd_icache_senseamp
		     +  (double)icache_decaddr_hd * power.hd_icache_tag_decoder_1ststg
		     +  ((icache_mem_cmd == Read) ? power.hd_icache_tag_bitline_rd:power.hd_icache_tag_bitline_wr)
		     +  (double)icache_tagaddr_hd * power.hd_icache_tag_senseamp
		     +  power.hd_icache_power                 
		     +  power.itlb;

    icache_power_cc1+=power.icache_power+power.itlb;
    icache_power_cc2+=power.icache_power+power.itlb;
    icache_power_cc3+=power.icache_power+power.itlb;

    hd_icache_power_cc1+=tmp_icache_power_cc;
    hd_icache_power_cc2+=tmp_icache_power_cc;
    hd_icache_power_cc3+=tmp_icache_power_cc;
    
    icache_decaddr_hd = 0;
    icache_tagaddr_hd = 0;
    icache_inst_hd = 0;
  }
  else
  {
    icache_power_cc3+=turnoff_factor*(power.icache_power+power.itlb);
    hd_icache_power_cc3+=turnoff_factor*(power.icache_power+power.itlb);
  }
  
  if(icache2_access) {
    /* don't scale icache2 because we assume 1 line is fetched, unless fetch stalls */
    double tmp_icache2_power_cc 
	             =  (double)icache2_decaddr_hd * power.hd_icache2_decoder_1ststg 
	             +  ((icache2_mem_cmd == Read) ? power.hd_icache2_bitline_rd:power.hd_icache2_bitline_wr) 
		     +  (double)icache2_inst_hd * power.hd_icache2_senseamp
		     +  (double)icache2_decaddr_hd * power.hd_icache2_tag_decoder_1ststg
		     +  ((icache2_mem_cmd == Read) ? power.hd_icache2_tag_bitline_rd:power.hd_icache2_tag_bitline_wr)
		     +  (double)icache2_tagaddr_hd * power.hd_icache2_tag_senseamp
		     +  power.hd_icache2_power                 
		     +  power.itlb;

    icache2_power_cc1+=power.icache2_power+power.itlb;
    icache2_power_cc2+=power.icache2_power+power.itlb;
    icache2_power_cc3+=power.icache2_power+power.itlb;

    hd_icache2_power_cc1+=tmp_icache2_power_cc;
    hd_icache2_power_cc2+=tmp_icache2_power_cc;
    hd_icache2_power_cc3+=tmp_icache2_power_cc;
    
    icache2_decaddr_hd = 0;
    icache2_tagaddr_hd = 0;
    icache2_inst_hd = 0;
  }
  else
  {
    icache2_power_cc3+=turnoff_factor*(power.icache2_power+power.itlb);
    hd_icache2_power_cc3+=turnoff_factor*(power.icache2_power+power.itlb);
  }
  

  if(dcache_access) {
    double tmp_dcache_power_cc 
	             =  (double)dcache_decaddr_hd * power.hd_dcache_decoder_1ststg 
	             +  ((dcache_mem_cmd == Read) ? power.hd_dcache_bitline_rd:power.hd_dcache_bitline_wr) 
		     +  (double)dcache_data_hd * power.hd_dcache_senseamp
		     +  (double)dcache_decaddr_hd * power.hd_dcache_tag_decoder_1ststg
		     +  ((dcache_mem_cmd == Read) ? power.hd_dcache_tag_bitline_rd:power.hd_dcache_tag_bitline_wr)
		     +  (double)dcache_tagaddr_hd * power.hd_dcache_tag_senseamp
		     +  power.hd_dcache_power                 
		     +  power.dtlb;

    if(dcache_access <= res_memport)
    {
      dcache_power_cc1    += power.dcache_power+power.dtlb;
      hd_dcache_power_cc1 += tmp_dcache_power_cc;
    }
    else
    {
      dcache_power_cc1+=((double)dcache_access/(double)res_memport)*(power.dcache_power + power.dtlb);
      hd_dcache_power_cc1+=((double)dcache_access/(double)res_memport)*(tmp_dcache_power_cc);
    }
    dcache_power_cc2+=((double)dcache_access/(double)res_memport)*(power.dcache_power + power.dtlb);
    dcache_power_cc3+=((double)dcache_access/(double)res_memport)*(power.dcache_power + power.dtlb);

    hd_dcache_power_cc2+=((double)dcache_access/(double)res_memport)*(tmp_dcache_power_cc);
    hd_dcache_power_cc3+=((double)dcache_access/(double)res_memport)*(tmp_dcache_power_cc);
    
    dcache_decaddr_hd = 0;
    dcache_tagaddr_hd = 0;
    dcache_data_hd = 0;
  }
  else
  {
    dcache_power_cc3+=turnoff_factor*(power.dcache_power+power.dtlb);
    hd_dcache_power_cc3+=turnoff_factor*(power.dcache_power+power.dtlb);
  }
  
  if(dcache2_access) {
    double tmp_dcache2_power_cc 
	             =  (double)dcache2_decaddr_hd * power.hd_dcache2_decoder_1ststg 
	             +  ((dcache2_mem_cmd == Read) ? power.hd_dcache2_bitline_rd:power.hd_dcache2_bitline_wr) 
		     +  (double)dcache2_data_hd * power.hd_dcache2_senseamp
		     +  (double)dcache2_decaddr_hd * power.hd_dcache2_tag_decoder_1ststg
		     +  ((dcache2_mem_cmd == Read) ? power.hd_dcache2_tag_bitline_rd:power.hd_dcache2_tag_bitline_wr)
		     +  (double)dcache2_tagaddr_hd * power.hd_dcache2_tag_senseamp
		     +  power.hd_dcache2_power                 
		     +  power.dtlb;

    if(dcache2_access <= res_memport)
    {
      dcache2_power_cc1    += power.dcache2_power+power.dtlb;
      hd_dcache2_power_cc1 += tmp_dcache2_power_cc;
    }
    else
    {
      dcache2_power_cc1+=((double)dcache2_access/(double)res_memport)*(power.dcache2_power + power.dtlb);
      hd_dcache2_power_cc1+=((double)dcache2_access/(double)res_memport)*(tmp_dcache2_power_cc);
    }
    dcache2_power_cc2+=((double)dcache2_access/(double)res_memport)*(power.dcache2_power + power.dtlb);
    dcache2_power_cc3+=((double)dcache2_access/(double)res_memport)*(power.dcache2_power + power.dtlb);

    hd_dcache2_power_cc2+=((double)dcache2_access/(double)res_memport)*(tmp_dcache2_power_cc);
    hd_dcache2_power_cc3+=((double)dcache2_access/(double)res_memport)*(tmp_dcache2_power_cc);
    
    dcache2_decaddr_hd = 0;
    dcache2_tagaddr_hd = 0;
    dcache2_data_hd = 0;
  }
  else
  {
    dcache2_power_cc3+=turnoff_factor*(power.dcache2_power+power.dtlb);
    hd_dcache2_power_cc3+=turnoff_factor*(power.dcache2_power+power.dtlb);
  }


  if(alu_access) {
    if(ialu_access)
      alu_power_cc1+=power.ialu_power;
    else
      alu_power_cc3+=turnoff_factor*power.ialu_power;
    if(falu_access)
      alu_power_cc1+=power.falu_power;
    else
      alu_power_cc3+=turnoff_factor*power.falu_power;

    alu_power_cc2+=((double)ialu_access/(double)res_ialu)*power.ialu_power +
      ((double)falu_access/(double)res_fpalu)*power.falu_power;
    alu_power_cc3+=((double)ialu_access/(double)res_ialu)*power.ialu_power +
      ((double)falu_access/(double)res_fpalu)*power.falu_power;
  }
  else
    alu_power_cc3+=turnoff_factor*(power.ialu_power + power.falu_power);

#ifdef STATIC_AF
  if(resultbus_access) {
    assert(ruu_issue_width != 0);
    if(resultbus_access <= ruu_issue_width) {
      resultbus_power_cc1+=power.resultbus;
    }
    else {
      resultbus_power_cc1+=((double)resultbus_access/(double)ruu_issue_width)*power.resultbus;
    }
    resultbus_power_cc2+=((double)resultbus_access/(double)ruu_issue_width)*power.resultbus;
    resultbus_power_cc3+=((double)resultbus_access/(double)ruu_issue_width)*power.resultbus;
  }
  else
    resultbus_power_cc3+=turnoff_factor*power.resultbus;
#else
  if(resultbus_access) {
    assert(ruu_issue_width != 0);
    if(resultbus_access <= ruu_issue_width) {
      resultbus_power_cc1+=resultbus_af_b*power.resultbus;
    }
    else {
      resultbus_power_cc1+=((double)resultbus_access/(double)ruu_issue_width)*resultbus_af_b*power.resultbus;
    }
    resultbus_power_cc2+=((double)resultbus_access/(double)ruu_issue_width)*resultbus_af_b*power.resultbus;
    resultbus_power_cc3+=((double)resultbus_access/(double)ruu_issue_width)*resultbus_af_b*power.resultbus;
  }
  else
    resultbus_power_cc3+=turnoff_factor*power.resultbus;
#endif

  total_cycle_power 
	  = rename_power 
	  + bpred_power 
	  + window_power 
	  + lsq_power 
	  + regfile_power 
	  + icache_power 
	  + icache2_power 
	  + dcache_power 
	  + dcache2_power 
	  + alu_power 
	  + resultbus_power;

  total_cycle_power_cc1 
	  = rename_power_cc1 
	  + bpred_power_cc1 
	  + window_power_cc1 
	  + lsq_power_cc1 
	  + regfile_power_cc1 
	  + icache_power_cc1 
	  + icache2_power_cc1 
	  + dcache_power_cc1 
	  + dcache2_power_cc1 
	  + alu_power_cc1 
	  + resultbus_power_cc1;

  hd_total_cycle_power_cc1 
	  = rename_power_cc1 
	  + hd_bpred_power_cc1 
	  + window_power_cc1 
	  + lsq_power_cc1 
	  + regfile_power_cc1 
	  + hd_icache_power_cc1 
	  + hd_icache2_power_cc1 
	  + hd_dcache_power_cc1 
	  + hd_dcache2_power_cc1 
	  + alu_power_cc1 
	  + resultbus_power_cc1;

  total_cycle_power_cc2 
	  = rename_power_cc2 
	  + bpred_power_cc2 
	  + window_power_cc2 
	  + lsq_power_cc2 
	  + regfile_power_cc2 
	  + icache_power_cc2 
	  + icache2_power_cc2 
	  + dcache_power_cc2 
	  + dcache2_power_cc2 
	  + alu_power_cc2 
	  + resultbus_power_cc2;
  
  hd_total_cycle_power_cc2 
	  = rename_power_cc2 
	  + hd_bpred_power_cc2 
	  + window_power_cc2 
	  + lsq_power_cc2 
	  + regfile_power_cc2 
	  + hd_icache_power_cc2 
	  + hd_icache2_power_cc2 
	  + hd_dcache_power_cc2 
	  + hd_dcache2_power_cc2 
	  + alu_power_cc2 
	  + resultbus_power_cc2;


  total_cycle_power_cc3 
	  = rename_power_cc3 
	  + bpred_power_cc3 
	  + window_power_cc3 
	  + lsq_power_cc3 
	  + regfile_power_cc3 
	  + icache_power_cc3 
	  + icache2_power_cc3 
	  + dcache_power_cc3 
	  + dcache2_power_cc3 
	  + alu_power_cc3 
	  + resultbus_power_cc3;
  
  hd_total_cycle_power_cc3 
	  = rename_power_cc3 
	  + hd_bpred_power_cc3 
	  + window_power_cc3 
	  + lsq_power_cc3 
	  + regfile_power_cc3 
	  + hd_icache_power_cc3 
	  + hd_icache2_power_cc3 
	  + hd_dcache_power_cc3 
	  + hd_dcache2_power_cc3 
	  + alu_power_cc3 
	  + resultbus_power_cc3;


  clock_power_cc1 += power.clock_power*(total_cycle_power_cc1/total_cycle_power);
  clock_power_cc2 += power.clock_power*(total_cycle_power_cc2/total_cycle_power);
  clock_power_cc3 += power.clock_power*(total_cycle_power_cc3/total_cycle_power);

  total_cycle_power_cc1 += clock_power_cc1;
  total_cycle_power_cc2 += clock_power_cc2;
  total_cycle_power_cc3 += clock_power_cc3;

  current_total_cycle_power_cc1 = total_cycle_power_cc1
    -last_single_total_cycle_power_cc1;
  current_total_cycle_power_cc2 = total_cycle_power_cc2
    -last_single_total_cycle_power_cc2;
  current_total_cycle_power_cc3 = total_cycle_power_cc3
    -last_single_total_cycle_power_cc3;

  max_cycle_power_cc1 = MAX(max_cycle_power_cc1,current_total_cycle_power_cc1);
  max_cycle_power_cc2 = MAX(max_cycle_power_cc2,current_total_cycle_power_cc2);
  max_cycle_power_cc3 = MAX(max_cycle_power_cc3,current_total_cycle_power_cc3);

  last_single_total_cycle_power_cc1 = total_cycle_power_cc1;
  last_single_total_cycle_power_cc2 = total_cycle_power_cc2;
  last_single_total_cycle_power_cc3 = total_cycle_power_cc3;

}


/* used in sim-outorder */
void
power_reg_stats(struct stat_sdb_t *sdb)	/* stats database */
{
  stat_reg_double(sdb, 
		  "rename_power", 
		  "total power usage of rename unit", 
		  &rename_power, 0, NULL);

  stat_reg_double(sdb, 
		  "bpred_power", 
		  "total power usage of bpred unit", 
		  &bpred_power, 0, NULL);

  stat_reg_double(sdb, 
		  "window_power", 
		  "total power usage of instruction window", 
		  &window_power, 0, NULL);

  stat_reg_double(sdb, 
		  "lsq_power", 
		  "total power usage of load/store queue", 
		  &lsq_power, 0, NULL);

  stat_reg_double(sdb, 
		  "regfile_power", 
		  "total power usage of arch. regfile", 
		  &regfile_power, 0, NULL);

  stat_reg_double(sdb, 
		  "icache_power", 
		  "total power usage of icache", 
		  &icache_power, 0, NULL);
  
  stat_reg_double(sdb, 
		  "icache2_power", 
		  "total power usage of icache2", 
		  &icache2_power, 0, NULL);

  stat_reg_double(sdb, 
		  "dcache_power", 
		  "total power usage of dcache", 
		  &dcache_power, 0, NULL);

  stat_reg_double(sdb, 
		  "dcache2_power", 
		  "total power usage of dcache2", &dcache2_power, 0, NULL);

  stat_reg_double(sdb, 
		  "alu_power", 
		  "total power usage of alu", &alu_power, 0, NULL);

  stat_reg_double(sdb, 
		  "falu_power", 
		  "total power usage of falu", 
		  &falu_power, 0, NULL);

  stat_reg_double(sdb, 
		  "resultbus_power", 
		  "total power usage of resultbus", 
		  &resultbus_power, 0, NULL);

  stat_reg_double(sdb, 
		  "clock_power", 
		  "total power usage of clock", 
		  &clock_power, 0, NULL);

  stat_reg_formula(sdb, 
		  "avg_rename_power", 
		  "avg power usage of rename unit", 
		  "rename_power/sim_cycle", NULL);

  stat_reg_formula(sdb, 
		  "avg_bpred_power", 
		  "avg power usage of bpred unit", 
		  "bpred_power/sim_cycle", NULL);

  stat_reg_formula(sdb, 
		  "avg_window_power", 
		  "avg power usage of instruction window", 
		  "window_power/sim_cycle",  NULL);

  stat_reg_formula(sdb, 
		  "avg_lsq_power", 
		  "avg power usage of lsq", 
		  "lsq_power/sim_cycle",  NULL);

  stat_reg_formula(sdb, 
		  "avg_regfile_power", 
		  "avg power usage of arch. regfile", 
		  "regfile_power/sim_cycle",  NULL);

  stat_reg_formula(sdb, 
		  "avg_icache_power", 
		  "avg power usage of icache", 
		  "icache_power/sim_cycle",  NULL);
  
  stat_reg_formula(sdb, 
		  "avg_icache2_power", 
		  "avg power usage of icache2", 
		  "icache2_power/sim_cycle",  NULL); 

  stat_reg_formula(sdb, 
		  "avg_dcache_power", 
		  "avg power usage of dcache", 
		  "dcache_power/sim_cycle",  NULL);

  stat_reg_formula(sdb, 
		  "avg_dcache2_power", 
		  "avg power usage of dcache2", 
		  "dcache2_power/sim_cycle",  NULL);

  stat_reg_formula(sdb, 
		  "avg_alu_power", 
		  "avg power usage of alu", 
		  "alu_power/sim_cycle",  NULL);

  stat_reg_formula(sdb, 
		  "avg_falu_power", 
		  "avg power usage of falu", 
		  "falu_power/sim_cycle",  NULL);

  stat_reg_formula(sdb, 
		  "avg_resultbus_power", 
		  "avg power usage of resultbus", 
		  "resultbus_power/sim_cycle",  NULL);

  stat_reg_formula(sdb, 
		  "avg_clock_power", 
		  "avg power usage of clock", 
		  "clock_power/sim_cycle",  NULL);

  stat_reg_formula(sdb, 
		  "fetch_stage_power", 
		  "total power usage of fetch stage", 
		  "icache_power + icache2_power + bpred_power", NULL);
  
  stat_reg_formula(sdb, 
		  "dispatch_stage_power", 
		  "total power usage of dispatch stage", 
		  "rename_power", NULL);

  stat_reg_formula(sdb, 
		  "issue_stage_power", 
		  "total power usage of issue stage", 
		  "resultbus_power + alu_power + dcache_power + dcache2_power + window_power + lsq_power", NULL);

  stat_reg_formula(sdb, 
		  "avg_fetch_power", 
		  "average power of fetch unit per cycle", 
		  "(icache_power + icache2_power + bpred_power)/ sim_cycle", /* format */NULL);

  stat_reg_formula(sdb, 
		  "avg_dispatch_power", 
		  "average power of dispatch unit per cycle", 
		  "(rename_power)/ sim_cycle", /* format */NULL);

  stat_reg_formula(sdb, 
		  "avg_issue_power", 
		  "average power of issue unit per cycle", 
		  "(resultbus_power + alu_power + dcache_power + dcache2_power + window_power + lsq_power)/ sim_cycle", /* format */NULL);

  stat_reg_formula(sdb, 
		  "total_power", 
		  "total power per cycle",
		  "(rename_power + bpred_power + window_power + lsq_power + regfile_power + icache_power + icache2_power + resultbus_power + clock_power + alu_power + dcache_power + dcache2_power)", NULL);

  stat_reg_formula(sdb, 
		  "avg_total_power_cycle", 
		  "average total power per cycle",
		  "(rename_power + bpred_power + window_power + lsq_power + regfile_power + icache_power + icache2_power + resultbus_power + clock_power + alu_power + dcache_power + dcache2_power)/sim_cycle", NULL);

  stat_reg_formula(sdb, 
		  "avg_total_power_cycle_nofp_nod2", 
		  "average total power per cycle",
		  "(rename_power + bpred_power + window_power + lsq_power + regfile_power + icache_power +icache2_power + resultbus_power + clock_power + alu_power + dcache_power - falu_power )/sim_cycle", NULL);

  stat_reg_formula(sdb, 
		  "avg_total_power_insn", 
		  "average total power per insn",
		  "(rename_power + bpred_power + window_power + lsq_power + regfile_power + icache_power + icache2_power + resultbus_power + clock_power + alu_power + dcache_power + dcache2_power)/sim_total_insn", NULL);

  stat_reg_formula(sdb, 
		  "avg_total_power_insn_nofp_nod2", 
		  "average total power per insn (no fpu)",
		  "(rename_power + bpred_power + window_power + lsq_power + regfile_power + icache_power + icache2_power + resultbus_power + clock_power + alu_power + dcache_power - falu_power )/sim_total_insn", NULL);

  stat_reg_double(sdb, 
		  "rename_power_cc1", 
		  "total power usage of rename unit_cc1", 
		  &rename_power_cc1, 0, NULL);

  stat_reg_double(sdb, 
		  "bpred_power_cc1", 
		  "total power usage of bpred unit_cc1", &bpred_power_cc1, 0, NULL);

  stat_reg_double(sdb, 
		  "hd_bpred_power_cc1", 
		  "total power usage of bpred unit_cc1 considering data sensitivity", 
		  &hd_bpred_power_cc1, 0, NULL);

  stat_reg_double(sdb, 
		  "window_power_cc1", 
		  "total power usage of instruction window_cc1", 
		  &window_power_cc1, 0, NULL);

  stat_reg_double(sdb, 
		  "lsq_power_cc1", 
		  "total power usage of lsq_cc1", 
		  &lsq_power_cc1, 0, NULL);

  stat_reg_double(sdb, 
		  "regfile_power_cc1", 
		  "total power usage of arch. regfile_cc1", 
		  &regfile_power_cc1, 0, NULL);

  stat_reg_double(sdb, 
		  "icache_power_cc1", 
		  "total power usage of icache_cc1", 
		  &icache_power_cc1, 0, NULL);
  
  stat_reg_double(sdb, 
		  "hd_icache_power_cc1", 
		  "total power usage of icache_cc1 considering data sensitivity", 
		  &hd_icache_power_cc1, 0, NULL);
  
  stat_reg_double(sdb, 
		  "icache2_power_cc1", 
		  "total power usage of icache2_cc1", 
		  &icache2_power_cc1, 0, NULL);

  stat_reg_double(sdb, 
		  "hd_icache2_power_cc1", 
		  "total power usage of icache2_cc1 considering data sensitivity", 
		  &hd_icache2_power_cc1, 0, NULL);

  stat_reg_double(sdb, 
		  "dcache_power_cc1", 
		  "total power usage of dcache_cc1", 
		  &dcache_power_cc1, 0, NULL);

  stat_reg_double(sdb, 
		  "hd_dcache_power_cc1", 
		  "total power usage of dcache_cc1 considering data sensitivity", 
		  &hd_dcache_power_cc1, 0, NULL);

  stat_reg_double(sdb, 
		  "dcache2_power_cc1", 
		  "total power usage of dcache2_cc1", 
		  &dcache2_power_cc1, 0, NULL);

  stat_reg_double(sdb, 
		  "hd_dcache2_power_cc1", 
		  "total power usage of dcache2_cc1 considering data sensitivity", 
		  &hd_dcache2_power_cc1, 0, NULL);

  stat_reg_double(sdb, 
		  "alu_power_cc1", 
		  "total power usage of alu_cc1", 
		  &alu_power_cc1, 0, NULL);

  stat_reg_double(sdb, 
		  "resultbus_power_cc1", 
		  "total power usage of resultbus_cc1", 
		  &resultbus_power_cc1, 0, NULL);

  stat_reg_double(sdb, 
		  "clock_power_cc1", 
		  "total power usage of clock_cc1", 
		  &clock_power_cc1, 0, NULL);

  stat_reg_formula(sdb, 
		  "avg_rename_power_cc1", 
		  "avg power usage of rename unit_cc1", 
		  "rename_power_cc1/sim_cycle", NULL);

  stat_reg_formula(sdb, 
		  "avg_bpred_power_cc1", 
		  "avg power usage of bpred unit_cc1", 
		  "bpred_power_cc1/sim_cycle", NULL);

  stat_reg_formula(sdb, 
		  "hd_avg_bpred_power_cc1", 
		  "avg power usage of bpred unit_cc1 considering data sensitivity", 
		  "hd_bpred_power_cc1/sim_cycle", NULL);

  stat_reg_formula(sdb, 
		  "avg_window_power_cc1", 
		  "avg power usage of instruction window_cc1", 
		  "window_power_cc1/sim_cycle",  NULL);

  stat_reg_formula(sdb, 
		  "avg_lsq_power_cc1", 
		  "avg power usage of lsq_cc1", 
		  "lsq_power_cc1/sim_cycle",  NULL);

  stat_reg_formula(sdb, 
		  "avg_regfile_power_cc1", 
		  "avg power usage of arch. regfile_cc1", 
		  "regfile_power_cc1/sim_cycle",  NULL);

  stat_reg_formula(sdb, 
		  "avg_icache_power_cc1", 
		  "avg power usage of icache_cc1", 
		  "icache_power_cc1/sim_cycle",  NULL);
  
  stat_reg_formula(sdb, 
		  "hd_avg_icache_power_cc1", 
		  "avg power usage of icache_cc1 considering data sensitivity", 
		  "hd_icache_power_cc1/sim_cycle",  NULL);
  
  stat_reg_formula(sdb, 
		  "avg_icache2_power_cc1", 
		  "avg power usage of icache2_cc1", 
		  "icache2_power_cc1/sim_cycle",  NULL);

  stat_reg_formula(sdb, 
		  "hd_avg_icache2_power_cc1", 
		  "avg power usage of icache2_cc1 considering data sensitivity", 
		  "hd_icache2_power_cc1/sim_cycle",  NULL);

  stat_reg_formula(sdb, 
		  "avg_dcache_power_cc1", 
		  "avg power usage of dcache_cc1", 
		  "dcache_power_cc1/sim_cycle",  NULL);

  stat_reg_formula(sdb, 
		  "hd_avg_dcache_power_cc1", 
		  "avg power usage of dcache_cc1 considering data sensitivity", 
		  "hd_dcache_power_cc1/sim_cycle",  NULL);

  stat_reg_formula(sdb, 
		  "avg_dcache2_power_cc1", 
		  "avg power usage of dcache2_cc1", 
		  "dcache2_power_cc1/sim_cycle",  NULL);

  stat_reg_formula(sdb, 
		  "hd_avg_dcache2_power_cc1", 
		  "avg power usage of dcache2_cc1 considering data sensitivity", 
		  "hd_dcache2_power_cc1/sim_cycle",  NULL);

  stat_reg_formula(sdb, 
		  "avg_alu_power_cc1", 
		  "avg power usage of alu_cc1", 
		  "alu_power_cc1/sim_cycle",  NULL);

  stat_reg_formula(sdb, 
		  "avg_resultbus_power_cc1", 
		  "avg power usage of resultbus_cc1", 
		  "resultbus_power_cc1/sim_cycle",  NULL);

  stat_reg_formula(sdb, 
		  "avg_clock_power_cc1", 
		  "avg power usage of clock_cc1", 
		  "clock_power_cc1/sim_cycle",  NULL);

  stat_reg_formula(sdb, 
		  "fetch_stage_power_cc1", 
		  "total power usage of fetch stage_cc1", 
		  "icache_power_cc1 + icache2_power + bpred_power_cc1", NULL);

  stat_reg_formula(sdb, 
		  "hd_fetch_stage_power_cc1", 
		  "total power usage of fetch stage_cc1 considering data sensitivity", 
		  "hd_icache_power_cc1 + hd_icache2_power_cc1 + hd_bpred_power_cc1", NULL);

  stat_reg_formula(sdb, 
		  "dispatch_stage_power_cc1", 
		  "total power usage of dispatch stage_cc1", 
		  "rename_power_cc1", NULL);

  stat_reg_formula(sdb, 
		  "issue_stage_power_cc1", 
		  "total power usage of issue stage_cc1", 
		  "resultbus_power_cc1 + alu_power_cc1 + dcache_power_cc1 + dcache2_power_cc1 + lsq_power_cc1 + window_power_cc1", NULL);

  stat_reg_formula(sdb, 
		  "hd_issue_stage_power_cc1", 
		  "total power usage of issue stage_cc1 considering data sensitivity", 
		  "resultbus_power_cc1 + alu_power_cc1 + hd_dcache_power_cc1 + hd_dcache2_power_cc1 + lsq_power_cc1 + window_power_cc1", NULL);

  stat_reg_formula(sdb, 
		  "avg_fetch_power_cc1", 
		  "average power of fetch unit per cycle_cc1", 
		  "(icache_power_cc1 + icache2_power_cc1 + bpred_power_cc1) / sim_cycle", /* format */NULL);

  stat_reg_formula(sdb, 
		  "hd_avg_fetch_power_cc1", 
		  "average power of fetch unit per cycle_cc1 considering data sensitivity", 
		  "(hd_icache_power_cc1 + hd_icache2_power_cc1 + hd_bpred_power_cc1) / sim_cycle", /* format */NULL);

  stat_reg_formula(sdb, 
		  "avg_dispatch_power_cc1", 
		  "average power of dispatch unit per cycle_cc1", 
		  "(rename_power_cc1)/ sim_cycle", /* format */NULL);

  stat_reg_formula(sdb, 
		  "avg_issue_power_cc1", 
		  "average power of issue unit per cycle_cc1", 
		  "(resultbus_power_cc1 + alu_power_cc1 + dcache_power_cc1 + dcache2_power_cc1 + lsq_power_cc1 + window_power_cc1)/ sim_cycle", /* format */NULL);

  stat_reg_formula(sdb, 
		  "hd_avg_issue_power_cc1", 
		  "average power of issue unit per cycle_cc1 considering data sensitivity", 
		  "(resultbus_power_cc1 + alu_power_cc1 + hd_dcache_power_cc1 + hd_dcache2_power_cc1 + lsq_power_cc1 + window_power_cc1)/ sim_cycle", /* format */NULL);

  stat_reg_formula(sdb, 
		  "total_power_cycle_cc1", 
		  "total power per cycle_cc1",
		  "(rename_power_cc1 + bpred_power_cc1 + lsq_power_cc1 + window_power_cc1 + regfile_power_cc1 + icache_power_cc1 +icache2_power_cc1 + resultbus_power_cc1 + clock_power_cc1 + alu_power_cc1 + dcache_power_cc1 + dcache2_power_cc1)", NULL);

  stat_reg_formula(sdb, 
		  "hd_total_power_cycle_cc1", 
		  "total power per cycle_cc1 considering data sensitivity",
		  "(rename_power_cc1 + hd_bpred_power_cc1 + lsq_power_cc1 + window_power_cc1 + regfile_power_cc1 + hd_icache_power_cc1 +hd_icache2_power_cc1 + resultbus_power_cc1 + clock_power_cc1 + alu_power_cc1 + hd_dcache_power_cc1 + hd_dcache2_power_cc1)", NULL);

  stat_reg_formula(sdb, 
		  "avg_total_power_cycle_cc1", 
		  "average total power per cycle_cc1",
		  "(rename_power_cc1 + bpred_power_cc1 + lsq_power_cc1 + window_power_cc1 + regfile_power_cc1 + icache_power_cc1 + icache2_power_cc1 + resultbus_power_cc1 + clock_power_cc1 + alu_power_cc1 + dcache_power_cc1 +dcache2_power_cc1)/sim_cycle", NULL);
  
  stat_reg_formula(sdb, 
		  "hd_avg_total_power_cycle_cc1", 
		  "average total power per cycle_cc1 considering data sensitivity",
		  "(rename_power_cc1 + hd_bpred_power_cc1 + lsq_power_cc1 + window_power_cc1 + regfile_power_cc1 + hd_icache_power_cc1 + hd_icache2_power_cc1 + resultbus_power_cc1 + clock_power_cc1 + alu_power_cc1 + hd_dcache_power_cc1 +hd_dcache2_power_cc1)/sim_cycle", NULL);


  stat_reg_formula(sdb, 
		  "avg_total_power_insn_cc1", 
		  "average total power per insn_cc1",
		  "(rename_power_cc1 + bpred_power_cc1 + lsq_power_cc1 + window_power_cc1 + regfile_power_cc1 + icache_power_cc1 + icache2_power_cc1 + resultbus_power_cc1 + clock_power_cc1 +  alu_power_cc1 + dcache_power_cc1 + dcache2_power_cc1) / sim_total_insn", NULL);

  stat_reg_formula(sdb, 
		  "hd_avg_total_power_insn_cc1", 
		  "average total power per insn_cc1 considering data sensitivity",
		  "(rename_power_cc1 + hd_bpred_power_cc1 + lsq_power_cc1 + window_power_cc1 + regfile_power_cc1 + hd_icache_power_cc1 + hd_icache2_power_cc1 + resultbus_power_cc1 + clock_power_cc1 +  alu_power_cc1 + hd_dcache_power_cc1 + hd_dcache2_power_cc1) / sim_total_insn", NULL);

  stat_reg_double(sdb, 
		  "rename_power_cc2", 
		  "total power usage of rename unit_cc2", 
		  &rename_power_cc2, 0, NULL);

  stat_reg_double(sdb, 
		  "bpred_power_cc2", 
		  "total power usage of bpred unit_cc2", &bpred_power_cc2, 0, NULL);

  stat_reg_double(sdb, 
		  "hd_bpred_power_cc2", 
		  "total power usage of bpred unit_cc2 considering data sensitivity", 
		  &hd_bpred_power_cc2, 0, NULL);

  stat_reg_double(sdb, 
		  "window_power_cc2", 
		  "total power usage of instruction window_cc2", 
		  &window_power_cc2, 0, NULL);

  stat_reg_double(sdb, 
		  "lsq_power_cc2", 
		  "total power usage of lsq_cc2", 
		  &lsq_power_cc2, 0, NULL);

  stat_reg_double(sdb, 
		  "regfile_power_cc2", 
		  "total power usage of arch. regfile_cc2", 
		  &regfile_power_cc2, 0, NULL);

  stat_reg_double(sdb, 
		  "icache_power_cc2", 
		  "total power usage of icache_cc2", 
		  &icache_power_cc2, 0, NULL);
  
  stat_reg_double(sdb, 
		  "hd_icache_power_cc2", 
		  "total power usage of icache_cc2 considering data sensitivity", 
		  &hd_icache_power_cc2, 0, NULL);
  
  stat_reg_double(sdb, 
		  "icache2_power_cc2", 
		  "total power usage of icache2_cc2", 
		  &icache2_power_cc2, 0, NULL);

  stat_reg_double(sdb, 
		  "hd_icache2_power_cc2", 
		  "total power usage of icache2_cc2 considering data sensitivity", 
		  &hd_icache2_power_cc2, 0, NULL);

  stat_reg_double(sdb, 
		  "dcache_power_cc2", 
		  "total power usage of dcache_cc2", 
		  &dcache_power_cc2, 0, NULL);

  stat_reg_double(sdb, 
		  "hd_dcache_power_cc2", 
		  "total power usage of dcache_cc2 considering data sensitivity", 
		  &hd_dcache_power_cc2, 0, NULL);

  stat_reg_double(sdb, 
		  "dcache2_power_cc2", 
		  "total power usage of dcache2_cc2", 
		  &dcache2_power_cc2, 0, NULL);

  stat_reg_double(sdb, 
		  "hd_dcache2_power_cc2", 
		  "total power usage of dcache2_cc2 considering data sensitivity", 
		  &hd_dcache2_power_cc2, 0, NULL);

  stat_reg_double(sdb, 
		  "alu_power_cc2", 
		  "total power usage of alu_cc2", 
		  &alu_power_cc2, 0, NULL);

  stat_reg_double(sdb, 
		  "resultbus_power_cc2", 
		  "total power usage of resultbus_cc2", 
		  &resultbus_power_cc2, 0, NULL);

  stat_reg_double(sdb, 
		  "clock_power_cc2", 
		  "total power usage of clock_cc2", 
		  &clock_power_cc2, 0, NULL);

  stat_reg_formula(sdb, 
		  "avg_rename_power_cc2", 
		  "avg power usage of rename unit_cc2", 
		  "rename_power_cc2/sim_cycle", NULL);

  stat_reg_formula(sdb, 
		  "avg_bpred_power_cc2", 
		  "avg power usage of bpred unit_cc2", 
		  "bpred_power_cc2/sim_cycle", NULL);

  stat_reg_formula(sdb, 
		  "hd_avg_bpred_power_cc2", 
		  "avg power usage of bpred unit_cc2 considering data sensitivity", 
		  "hd_bpred_power_cc2/sim_cycle", NULL);

  stat_reg_formula(sdb, 
		  "avg_window_power_cc2", 
		  "avg power usage of instruction window_cc2", 
		  "window_power_cc2/sim_cycle",  NULL);

  stat_reg_formula(sdb, 
		  "avg_lsq_power_cc2", 
		  "avg power usage of lsq_cc2", 
		  "lsq_power_cc2/sim_cycle",  NULL);

  stat_reg_formula(sdb, 
		  "avg_regfile_power_cc2", 
		  "avg power usage of arch. regfile_cc2", 
		  "regfile_power_cc2/sim_cycle",  NULL);

  stat_reg_formula(sdb, 
		  "avg_icache_power_cc2", 
		  "avg power usage of icache_cc2", 
		  "icache_power_cc2/sim_cycle",  NULL);
  
  stat_reg_formula(sdb, 
		  "hd_avg_icache_power_cc2", 
		  "avg power usage of icache_cc2 considering data sensitivity", 
		  "hd_icache_power_cc2/sim_cycle",  NULL);
  
  stat_reg_formula(sdb, 
		  "avg_icache2_power_cc2", 
		  "avg power usage of icache2_cc2", 
		  "icache2_power_cc2/sim_cycle",  NULL);

  stat_reg_formula(sdb, 
		  "hd_avg_icache2_power_cc2", 
		  "avg power usage of icache2_cc2 considering data sensitivity", 
		  "hd_icache2_power_cc2/sim_cycle",  NULL);

  stat_reg_formula(sdb, 
		  "avg_dcache_power_cc2", 
		  "avg power usage of dcache_cc2", 
		  "dcache_power_cc2/sim_cycle",  NULL);

  stat_reg_formula(sdb, 
		  "hd_avg_dcache_power_cc2", 
		  "avg power usage of dcache_cc2 considering data sensitivity", 
		  "hd_dcache_power_cc2/sim_cycle",  NULL);

  stat_reg_formula(sdb, 
		  "avg_dcache2_power_cc2", 
		  "avg power usage of dcache2_cc2", 
		  "dcache2_power_cc2/sim_cycle",  NULL);

  stat_reg_formula(sdb, 
		  "hd_avg_dcache2_power_cc2", 
		  "avg power usage of dcache2_cc2 considering data sensitivity", 
		  "hd_dcache2_power_cc2/sim_cycle",  NULL);

  stat_reg_formula(sdb, 
		  "avg_alu_power_cc2", 
		  "avg power usage of alu_cc2", 
		  "alu_power_cc2/sim_cycle",  NULL);

  stat_reg_formula(sdb, 
		  "avg_resultbus_power_cc2", 
		  "avg power usage of resultbus_cc2", 
		  "resultbus_power_cc2/sim_cycle",  NULL);

  stat_reg_formula(sdb, 
		  "avg_clock_power_cc2", 
		  "avg power usage of clock_cc2", 
		  "clock_power_cc2/sim_cycle",  NULL);

  stat_reg_formula(sdb, 
		  "fetch_stage_power_cc2", 
		  "total power usage of fetch stage_cc2", 
		  "icache_power_cc2 + icache2_power + bpred_power_cc2", NULL);

  stat_reg_formula(sdb, 
		  "hd_fetch_stage_power_cc2", 
		  "total power usage of fetch stage_cc2 considering data sensitivity", 
		  "hd_icache_power_cc2 + hd_icache2_power_cc2 + hd_bpred_power_cc2", NULL);

  stat_reg_formula(sdb, 
		  "dispatch_stage_power_cc2", 
		  "total power usage of dispatch stage_cc2", 
		  "rename_power_cc2", NULL);

  stat_reg_formula(sdb, 
		  "issue_stage_power_cc2", 
		  "total power usage of issue stage_cc2", 
		  "resultbus_power_cc2 + alu_power_cc2 + dcache_power_cc2 + dcache2_power_cc2 + lsq_power_cc2 + window_power_cc2", NULL);

  stat_reg_formula(sdb, 
		  "hd_issue_stage_power_cc2", 
		  "total power usage of issue stage_cc2 considering data sensitivity", 
		  "resultbus_power_cc2 + alu_power_cc2 + hd_dcache_power_cc2 + hd_dcache2_power_cc2 + lsq_power_cc2 + window_power_cc2", NULL);

  stat_reg_formula(sdb, 
		  "avg_fetch_power_cc2", 
		  "average power of fetch unit per cycle_cc2", 
		  "(icache_power_cc2 + icache2_power_cc2 + bpred_power_cc2) / sim_cycle", /* format */NULL);

  stat_reg_formula(sdb, 
		  "hd_avg_fetch_power_cc2", 
		  "average power of fetch unit per cycle_cc2 considering data sensitivity", 
		  "(hd_icache_power_cc2 + hd_icache2_power_cc2 + hd_bpred_power_cc2) / sim_cycle", /* format */NULL);

  stat_reg_formula(sdb, 
		  "avg_dispatch_power_cc2", 
		  "average power of dispatch unit per cycle_cc2", 
		  "(rename_power_cc2)/ sim_cycle", /* format */NULL);

  stat_reg_formula(sdb, 
		  "avg_issue_power_cc2", 
		  "average power of issue unit per cycle_cc2", 
		  "(resultbus_power_cc2 + alu_power_cc2 + dcache_power_cc2 + dcache2_power_cc2 + lsq_power_cc2 + window_power_cc2)/ sim_cycle", /* format */NULL);

  stat_reg_formula(sdb, 
		  "hd_avg_issue_power_cc2", 
		  "average power of issue unit per cycle_cc2 considering data sensitivity", 
		  "(resultbus_power_cc2 + alu_power_cc2 + hd_dcache_power_cc2 + hd_dcache2_power_cc2 + lsq_power_cc2 + window_power_cc2)/ sim_cycle", /* format */NULL);

  stat_reg_formula(sdb, 
		  "total_power_cycle_cc2", 
		  "total power per cycle_cc2",
		  "(rename_power_cc2 + bpred_power_cc2 + lsq_power_cc2 + window_power_cc2 + regfile_power_cc2 + icache_power_cc2 +icache2_power_cc2 + resultbus_power_cc2 + clock_power_cc2 + alu_power_cc2 + dcache_power_cc2 + dcache2_power_cc2)", NULL);

  stat_reg_formula(sdb, 
		  "hd_total_power_cycle_cc2", 
		  "total power per cycle_cc2 considering data sensitivity",
		  "(rename_power_cc2 + hd_bpred_power_cc2 + lsq_power_cc2 + window_power_cc2 + regfile_power_cc2 + hd_icache_power_cc2 +hd_icache2_power_cc2 + resultbus_power_cc2 + clock_power_cc2 + alu_power_cc2 + hd_dcache_power_cc2 + hd_dcache2_power_cc2)", NULL);

  stat_reg_formula(sdb, 
		  "avg_total_power_cycle_cc2", 
		  "average total power per cycle_cc2",
		  "(rename_power_cc2 + bpred_power_cc2 + lsq_power_cc2 + window_power_cc2 + regfile_power_cc2 + icache_power_cc2 + icache2_power_cc2 + resultbus_power_cc2 + clock_power_cc2 + alu_power_cc2 + dcache_power_cc2 +dcache2_power_cc2)/sim_cycle", NULL);
  
  stat_reg_formula(sdb, 
		  "hd_avg_total_power_cycle_cc2", 
		  "average total power per cycle_cc2 considering data sensitivity",
		  "(rename_power_cc2 + hd_bpred_power_cc2 + lsq_power_cc2 + window_power_cc2 + regfile_power_cc2 + hd_icache_power_cc2 + hd_icache2_power_cc2 + resultbus_power_cc2 + clock_power_cc2 + alu_power_cc2 + hd_dcache_power_cc2 +hd_dcache2_power_cc2)/sim_cycle", NULL);


  stat_reg_formula(sdb, 
		  "avg_total_power_insn_cc2", 
		  "average total power per insn_cc2",
		  "(rename_power_cc2 + bpred_power_cc2 + lsq_power_cc2 + window_power_cc2 + regfile_power_cc2 + icache_power_cc2 + icache2_power_cc2 + resultbus_power_cc2 + clock_power_cc2 +  alu_power_cc2 + dcache_power_cc2 + dcache2_power_cc2) / sim_total_insn", NULL);

  stat_reg_formula(sdb, 
		  "hd_avg_total_power_insn_cc2", 
		  "average total power per insn_cc2 considering data sensitivity",
		  "(rename_power_cc2 + hd_bpred_power_cc2 + lsq_power_cc2 + window_power_cc2 + regfile_power_cc2 + hd_icache_power_cc2 + hd_icache2_power_cc2 + resultbus_power_cc2 + clock_power_cc2 +  alu_power_cc2 + hd_dcache_power_cc2 + hd_dcache2_power_cc2) / sim_total_insn", NULL);


  stat_reg_double(sdb, 
		  "rename_power_cc3", 
		  "total power usage of rename unit_cc3", 
		  &rename_power_cc3, 0, NULL);

  stat_reg_double(sdb, 
		  "bpred_power_cc3", 
		  "total power usage of bpred unit_cc3", &bpred_power_cc3, 0, NULL);

  stat_reg_double(sdb, 
		  "hd_bpred_power_cc3", 
		  "total power usage of bpred unit_cc3 considering data sensitivity", 
		  &hd_bpred_power_cc3, 0, NULL);

  stat_reg_double(sdb, 
		  "window_power_cc3", 
		  "total power usage of instruction window_cc3", 
		  &window_power_cc3, 0, NULL);

  stat_reg_double(sdb, 
		  "lsq_power_cc3", 
		  "total power usage of lsq_cc3", 
		  &lsq_power_cc3, 0, NULL);

  stat_reg_double(sdb, 
		  "regfile_power_cc3", 
		  "total power usage of arch. regfile_cc3", 
		  &regfile_power_cc3, 0, NULL);

  stat_reg_double(sdb, 
		  "icache_power_cc3", 
		  "total power usage of icache_cc3", 
		  &icache_power_cc3, 0, NULL);
  
  stat_reg_double(sdb, 
		  "hd_icache_power_cc3", 
		  "total power usage of icache_cc3 considering data sensitivity", 
		  &hd_icache_power_cc3, 0, NULL);
  
  stat_reg_double(sdb, 
		  "icache2_power_cc3", 
		  "total power usage of icache2_cc3", 
		  &icache2_power_cc3, 0, NULL);

  stat_reg_double(sdb, 
		  "hd_icache2_power_cc3", 
		  "total power usage of icache2_cc3 considering data sensitivity", 
		  &hd_icache2_power_cc3, 0, NULL);

  stat_reg_double(sdb, 
		  "dcache_power_cc3", 
		  "total power usage of dcache_cc3", 
		  &dcache_power_cc3, 0, NULL);

  stat_reg_double(sdb, 
		  "hd_dcache_power_cc3", 
		  "total power usage of dcache_cc3 considering data sensitivity", 
		  &hd_dcache_power_cc3, 0, NULL);

  stat_reg_double(sdb, 
		  "dcache2_power_cc3", 
		  "total power usage of dcache2_cc3", 
		  &dcache2_power_cc3, 0, NULL);

  stat_reg_double(sdb, 
		  "hd_dcache2_power_cc3", 
		  "total power usage of dcache2_cc3 considering data sensitivity", 
		  &hd_dcache2_power_cc3, 0, NULL);

  stat_reg_double(sdb, 
		  "alu_power_cc3", 
		  "total power usage of alu_cc3", 
		  &alu_power_cc3, 0, NULL);

  stat_reg_double(sdb, 
		  "resultbus_power_cc3", 
		  "total power usage of resultbus_cc3", 
		  &resultbus_power_cc3, 0, NULL);

  stat_reg_double(sdb, 
		  "clock_power_cc3", 
		  "total power usage of clock_cc3", 
		  &clock_power_cc3, 0, NULL);

  stat_reg_formula(sdb, 
		  "avg_rename_power_cc3", 
		  "avg power usage of rename unit_cc3", 
		  "rename_power_cc3/sim_cycle", NULL);

  stat_reg_formula(sdb, 
		  "avg_bpred_power_cc3", 
		  "avg power usage of bpred unit_cc3", 
		  "bpred_power_cc3/sim_cycle", NULL);

  stat_reg_formula(sdb, 
		  "hd_avg_bpred_power_cc3", 
		  "avg power usage of bpred unit_cc3 considering data sensitivity", 
		  "hd_bpred_power_cc3/sim_cycle", NULL);

  stat_reg_formula(sdb, 
		  "avg_window_power_cc3", 
		  "avg power usage of instruction window_cc3", 
		  "window_power_cc3/sim_cycle",  NULL);

  stat_reg_formula(sdb, 
		  "avg_lsq_power_cc3", 
		  "avg power usage of lsq_cc3", 
		  "lsq_power_cc3/sim_cycle",  NULL);

  stat_reg_formula(sdb, 
		  "avg_regfile_power_cc3", 
		  "avg power usage of arch. regfile_cc3", 
		  "regfile_power_cc3/sim_cycle",  NULL);

  stat_reg_formula(sdb, 
		  "avg_icache_power_cc3", 
		  "avg power usage of icache_cc3", 
		  "icache_power_cc3/sim_cycle",  NULL);
  
  stat_reg_formula(sdb, 
		  "hd_avg_icache_power_cc3", 
		  "avg power usage of icache_cc3 considering data sensitivity", 
		  "hd_icache_power_cc3/sim_cycle",  NULL);
  
  stat_reg_formula(sdb, 
		  "avg_icache2_power_cc3", 
		  "avg power usage of icache2_cc3", 
		  "icache2_power_cc3/sim_cycle",  NULL);

  stat_reg_formula(sdb, 
		  "hd_avg_icache2_power_cc3", 
		  "avg power usage of icache2_cc3 considering data sensitivity", 
		  "hd_icache2_power_cc3/sim_cycle",  NULL);

  stat_reg_formula(sdb, 
		  "avg_dcache_power_cc3", 
		  "avg power usage of dcache_cc3", 
		  "dcache_power_cc3/sim_cycle",  NULL);

  stat_reg_formula(sdb, 
		  "hd_avg_dcache_power_cc3", 
		  "avg power usage of dcache_cc3 considering data sensitivity", 
		  "hd_dcache_power_cc3/sim_cycle",  NULL);

  stat_reg_formula(sdb, 
		  "avg_dcache2_power_cc3", 
		  "avg power usage of dcache2_cc3", 
		  "dcache2_power_cc3/sim_cycle",  NULL);

  stat_reg_formula(sdb, 
		  "hd_avg_dcache2_power_cc3", 
		  "avg power usage of dcache2_cc3 considering data sensitivity", 
		  "hd_dcache2_power_cc3/sim_cycle",  NULL);

  stat_reg_formula(sdb, 
		  "avg_alu_power_cc3", 
		  "avg power usage of alu_cc3", 
		  "alu_power_cc3/sim_cycle",  NULL);

  stat_reg_formula(sdb, 
		  "avg_resultbus_power_cc3", 
		  "avg power usage of resultbus_cc3", 
		  "resultbus_power_cc3/sim_cycle",  NULL);

  stat_reg_formula(sdb, 
		  "avg_clock_power_cc3", 
		  "avg power usage of clock_cc3", 
		  "clock_power_cc3/sim_cycle",  NULL);

  stat_reg_formula(sdb, 
		  "fetch_stage_power_cc3", 
		  "total power usage of fetch stage_cc3", 
		  "icache_power_cc3 + icache2_power + bpred_power_cc3", NULL);

  stat_reg_formula(sdb, 
		  "hd_fetch_stage_power_cc3", 
		  "total power usage of fetch stage_cc3 considering data sensitivity", 
		  "hd_icache_power_cc3 + hd_icache2_power_cc3 + hd_bpred_power_cc3", NULL);

  stat_reg_formula(sdb, 
		  "dispatch_stage_power_cc3", 
		  "total power usage of dispatch stage_cc3", 
		  "rename_power_cc3", NULL);

  stat_reg_formula(sdb, 
		  "issue_stage_power_cc3", 
		  "total power usage of issue stage_cc3", 
		  "resultbus_power_cc3 + alu_power_cc3 + dcache_power_cc3 + dcache2_power_cc3 + lsq_power_cc3 + window_power_cc3", NULL);

  stat_reg_formula(sdb, 
		  "hd_issue_stage_power_cc3", 
		  "total power usage of issue stage_cc3 considering data sensitivity", 
		  "resultbus_power_cc3 + alu_power_cc3 + hd_dcache_power_cc3 + hd_dcache2_power_cc3 + lsq_power_cc3 + window_power_cc3", NULL);

  stat_reg_formula(sdb, 
		  "avg_fetch_power_cc3", 
		  "average power of fetch unit per cycle_cc3", 
		  "(icache_power_cc3 + icache2_power_cc3 + bpred_power_cc3) / sim_cycle", /* format */NULL);

  stat_reg_formula(sdb, 
		  "hd_avg_fetch_power_cc3", 
		  "average power of fetch unit per cycle_cc3 considering data sensitivity", 
		  "(hd_icache_power_cc3 + hd_icache2_power_cc3 + hd_bpred_power_cc3) / sim_cycle", /* format */NULL);

  stat_reg_formula(sdb, 
		  "avg_dispatch_power_cc3", 
		  "average power of dispatch unit per cycle_cc3", 
		  "(rename_power_cc3)/ sim_cycle", /* format */NULL);

  stat_reg_formula(sdb, 
		  "avg_issue_power_cc3", 
		  "average power of issue unit per cycle_cc3", 
		  "(resultbus_power_cc3 + alu_power_cc3 + dcache_power_cc3 + dcache2_power_cc3 + lsq_power_cc3 + window_power_cc3)/ sim_cycle", /* format */NULL);

  stat_reg_formula(sdb, 
		  "hd_avg_issue_power_cc3", 
		  "average power of issue unit per cycle_cc3 considering data sensitivity", 
		  "(resultbus_power_cc3 + alu_power_cc3 + hd_dcache_power_cc3 + hd_dcache2_power_cc3 + lsq_power_cc3 + window_power_cc3)/ sim_cycle", /* format */NULL);

  stat_reg_formula(sdb, 
		  "total_power_cycle_cc3", 
		  "total power per cycle_cc3",
		  "(rename_power_cc3 + bpred_power_cc3 + lsq_power_cc3 + window_power_cc3 + regfile_power_cc3 + icache_power_cc3 +icache2_power_cc3 + resultbus_power_cc3 + clock_power_cc3 + alu_power_cc3 + dcache_power_cc3 + dcache2_power_cc3)", NULL);

  stat_reg_formula(sdb, 
		  "hd_total_power_cycle_cc3", 
		  "total power per cycle_cc3 considering data sensitivity",
		  "(rename_power_cc3 + hd_bpred_power_cc3 + lsq_power_cc3 + window_power_cc3 + regfile_power_cc3 + hd_icache_power_cc3 +hd_icache2_power_cc3 + resultbus_power_cc3 + clock_power_cc3 + alu_power_cc3 + hd_dcache_power_cc3 + hd_dcache2_power_cc3)", NULL);

  stat_reg_formula(sdb, 
		  "avg_total_power_cycle_cc3", 
		  "average total power per cycle_cc3",
		  "(rename_power_cc3 + bpred_power_cc3 + lsq_power_cc3 + window_power_cc3 + regfile_power_cc3 + icache_power_cc3 + icache2_power_cc3 + resultbus_power_cc3 + clock_power_cc3 + alu_power_cc3 + dcache_power_cc3 +dcache2_power_cc3)/sim_cycle", NULL);
  
  stat_reg_formula(sdb, 
		  "hd_avg_total_power_cycle_cc3", 
		  "average total power per cycle_cc3 considering data sensitivity",
		  "(rename_power_cc3 + hd_bpred_power_cc3 + lsq_power_cc3 + window_power_cc3 + regfile_power_cc3 + hd_icache_power_cc3 + hd_icache2_power_cc3 + resultbus_power_cc3 + clock_power_cc3 + alu_power_cc3 + hd_dcache_power_cc3 +hd_dcache2_power_cc3)/sim_cycle", NULL);


  stat_reg_formula(sdb, 
		  "avg_total_power_insn_cc3", 
		  "average total power per insn_cc3",
		  "(rename_power_cc3 + bpred_power_cc3 + lsq_power_cc3 + window_power_cc3 + regfile_power_cc3 + icache_power_cc3 + icache2_power_cc3 + resultbus_power_cc3 + clock_power_cc3 +  alu_power_cc3 + dcache_power_cc3 + dcache2_power_cc3) / sim_total_insn", NULL);

  stat_reg_formula(sdb, 
		  "hd_avg_total_power_insn_cc3", 
		  "average total power per insn_cc3 considering data sensitivity",
		  "(rename_power_cc3 + hd_bpred_power_cc3 + lsq_power_cc3 + window_power_cc3 + regfile_power_cc3 + hd_icache_power_cc3 + hd_icache2_power_cc3 + resultbus_power_cc3 + clock_power_cc3 +  alu_power_cc3 + hd_dcache_power_cc3 + hd_dcache2_power_cc3) / sim_total_insn", NULL);


  stat_reg_counter(sdb, 
		  "total_rename_access", 
		  "total number accesses of rename unit", 
		  &total_rename_access, 0, NULL);

  stat_reg_counter(sdb, 
		  "total_bpred_access", 
		  "total number accesses of bpred unit", 
		  &total_bpred_access, 0, NULL);

  stat_reg_counter(sdb, 
		  "total_window_access", 
		  "total number accesses of instruction window", 
		  &total_window_access, 0, NULL);

  stat_reg_counter(sdb, 
		  "total_lsq_access", 
		  "total number accesses of load/store queue", 
		  &total_lsq_access, 0, NULL);

  stat_reg_counter(sdb, 
		  "total_regfile_access", 
		  "total number accesses of arch. regfile", 
		  &total_regfile_access, 0, NULL);

  stat_reg_counter(sdb, 
		  "total_icache_access", 
		  "total number accesses of icache", 
		  &total_icache_access, 0, NULL);
  
  stat_reg_counter(sdb, 
		  "total_icache2_access", 
		  "total number accesses of icache2", 
		  &total_icache2_access, 0, NULL);

  stat_reg_counter(sdb, 
		  "total_dcache_access", 
		  "total number accesses of dcache", 
		  &total_dcache_access, 0, NULL);

  stat_reg_counter(sdb, 
		  "total_dcache2_access", 
		  "total number accesses of dcache2", 
		  &total_dcache2_access, 0, NULL);

  stat_reg_counter(sdb, "total_alu_access", "total number accesses of alu", &total_alu_access, 0, NULL);

  stat_reg_counter(sdb, "total_resultbus_access", "total number accesses of resultbus", &total_resultbus_access, 0, NULL);

  stat_reg_formula(sdb, "avg_rename_access", "avg number accesses of rename unit", "total_rename_access/sim_cycle", NULL);

  stat_reg_formula(sdb, "avg_bpred_access", "avg number accesses of bpred unit", "total_bpred_access/sim_cycle", NULL);

  stat_reg_formula(sdb, "avg_window_access", "avg number accesses of instruction window", "total_window_access/sim_cycle",  NULL);

  stat_reg_formula(sdb, "avg_lsq_access", "avg number accesses of lsq", "total_lsq_access/sim_cycle",  NULL);

  stat_reg_formula(sdb, "avg_regfile_access", "avg number accesses of arch. regfile", "total_regfile_access/sim_cycle",  NULL);

  stat_reg_formula(sdb, "avg_icache_access", "avg number accesses of icache", "total_icache_access/sim_cycle",  NULL);
  
  stat_reg_formula(sdb, "avg_icache2_access", "avg number accesses of icache2", "total_icache2_access/sim_cycle",  NULL);

  stat_reg_formula(sdb, "avg_dcache_access", "avg number accesses of dcache", "total_dcache_access/sim_cycle",  NULL);

  stat_reg_formula(sdb, "avg_dcache2_access", "avg number accesses of dcache2", "total_dcache2_access/sim_cycle",  NULL);

  stat_reg_formula(sdb, "avg_alu_access", "avg number accesses of alu", "total_alu_access/sim_cycle",  NULL);

  stat_reg_formula(sdb, "avg_resultbus_access", "avg number accesses of resultbus", "total_resultbus_access/sim_cycle",  NULL);

  stat_reg_counter(sdb, "max_rename_access", "max number accesses of rename unit", &max_rename_access, 0, NULL);

  stat_reg_counter(sdb, "max_bpred_access", "max number accesses of bpred unit", &max_bpred_access, 0, NULL);

  stat_reg_counter(sdb, "max_window_access", "max number accesses of instruction window", &max_window_access, 0, NULL);

  stat_reg_counter(sdb, "max_lsq_access", "max number accesses of load/store queue", &max_lsq_access, 0, NULL);

  stat_reg_counter(sdb, "max_regfile_access", "max number accesses of arch. regfile", &max_regfile_access, 0, NULL);

  stat_reg_counter(sdb, "max_icache_access", "max number accesses of icache", &max_icache_access, 0, NULL);
  
  stat_reg_counter(sdb, "max_icache2_access", "max number accesses of icache2", &max_icache2_access, 0, NULL);

  stat_reg_counter(sdb, "max_dcache_access", "max number accesses of dcache", &max_dcache_access, 0, NULL);

  stat_reg_counter(sdb, "max_dcache2_access", "max number accesses of dcache2", &max_dcache2_access, 0, NULL);

  stat_reg_counter(sdb, "max_alu_access", "max number accesses of alu", &max_alu_access, 0, NULL);

  stat_reg_counter(sdb, "max_resultbus_access", "max number accesses of resultbus", &max_resultbus_access, 0, NULL);

  stat_reg_double(sdb, "max_cycle_power_cc1", "maximum cycle power usage of cc1", &max_cycle_power_cc1, 0, NULL);

  stat_reg_double(sdb, "max_cycle_power_cc2", "maximum cycle power usage of cc2", &max_cycle_power_cc2, 0, NULL);

  stat_reg_double(sdb, "max_cycle_power_cc3", "maximum cycle power usage of cc3", &max_cycle_power_cc3, 0, NULL);

}


/* this routine takes the number of rows and cols of an array structure
   and attemps to make it make it more of a reasonable circuit structure
   by trying to make the number of rows and cols as close as possible.
   (scaling both by factors of 2 in opposite directions).  it returns
   a scale factor which is the amount that the rows should be divided
   by and the columns should be multiplied by.
*/
int squarify(int rows, int cols)
{
  int scale_factor = 1;

  if(rows == cols)
    return 1;

  /*
  printf("init rows == %d\n",rows);
  printf("init cols == %d\n",cols);
  */

  while(rows > cols) {
    rows = rows/2;
    cols = cols*2;

    /*
    printf("rows == %d\n",rows);
    printf("cols == %d\n",cols);
    printf("scale_factor == %d (2^ == %d)\n\n",scale_factor,(int)pow(2.0,(double)scale_factor));
    */

    if (rows/2 <= cols)
      return((int)pow(2.0,(double)scale_factor));
    scale_factor++;
  }

  return 1;
}

/* could improve squarify to work when rows < cols */

double squarify_new(int rows, int cols)
{
  double scale_factor = 0.0;

  if(rows==cols)
    return(pow(2.0,scale_factor));

  while(rows > cols) {
    rows = rows/2;
    cols = cols*2;
    if (rows <= cols)
      return(pow(2.0,scale_factor));
    scale_factor++;
  }

  while(cols > rows) {
    rows = rows*2;
    cols = cols/2;
    if (cols <= rows)
      return(pow(2.0,scale_factor));
    scale_factor--;
  }

  return 1;

}

void dump_power_stats(power)
     power_result_type *power;
{
  /*
  double total_power;
  double bpred_power;
  double rename_power;
  double rat_power;
  double dcl_power;
  double lsq_power;
  double window_power;
  double wakeup_power;
  double rs_power;
  double lsq_wakeup_power;
  double lsq_rs_power;
  double regfile_power;
  double reorder_power;
  double icache_power;
  double icache2_power;
  double dcache_power;
  double dcache2_power;
  double dtlb_power;
  double itlb_power;
  double ambient_power = 2.0;

  icache_power 	= power->icache_power;
  icache2_power = power->icache2_power;

  dcache_power 	= power->dcache_power;
  dcache2_power = power->dcache2_power;

  itlb_power 	= power->itlb;
  dtlb_power 	= power->dtlb;

  bpred_power 
	  = power->btb 
	  + power->local_predict 
	  + power->global_predict 
	  + power->chooser 
	  + power->ras;

  rat_power 
	  = power->rat_decoder 
	  + power->rat_wordline 
	  + power->rat_bitline 
	  + power->rat_senseamp;

  dcl_power 
	  = power->dcl_compare 
	  + power->dcl_pencode;

  rename_power 
	  = power->rat_power 
	  + power->dcl_power 
	  + power->inst_decoder_power;

  wakeup_power 
	  = power->wakeup_tagdrive 
	  + power->wakeup_tagmatch 
	  + power->wakeup_ormatch;
   
  rs_power 
	  = power->rs_decoder 
	  + power->rs_wordline 
	  + power->rs_bitline 
	  + power->rs_senseamp;

  window_power 
	  = wakeup_power 
	  + rs_power 
	  + power->selection;

  lsq_rs_power 
	  = power->lsq_rs_decoder 
	  + power->lsq_rs_wordline 
	  + power->lsq_rs_bitline 
	  + power->lsq_rs_senseamp;

  lsq_wakeup_power 
	  = power->lsq_wakeup_tagdrive 
	  + power->lsq_wakeup_tagmatch 
	  + power->lsq_wakeup_ormatch;

  lsq_power 
	  = lsq_wakeup_power 
	  + lsq_rs_power;

  reorder_power 
	  = power->reorder_decoder 
	  + power->reorder_wordline 
	  + power->reorder_bitline 
	  + power->reorder_senseamp;

  regfile_power 
	  = power->regfile_decoder 
	  + power->regfile_wordline 
	  + power->regfile_bitline 
	  + power->regfile_senseamp;

  total_power 
	  = bpred_power 
	  + rename_power 
	  + window_power 
	  + regfile_power 
	  + power->resultbus 
	  + lsq_power 
	  + icache_power 
	  + icache2_power 
	  + dcache_power 
	  + dcache2_power 
	  + dtlb_power 
	  + itlb_power 
	  + power->clock_power 
	  + power->ialu_power 
	  + power->falu_power;

  fprintf(stderr,"\nProcessor Parameters:\n");
  fprintf(stderr,"Issue Width: %d\n",ruu_issue_width);
  fprintf(stderr,"Window Size: %d\n",RUU_size);
  fprintf(stderr,"Number of Virtual Registers: %d\n",MD_NUM_IREGS);
  fprintf(stderr,"Number of Physical Registers: %d\n",RUU_size);
  fprintf(stderr,"Datapath Width: %d\n",data_width);

  fprintf(stderr,"Total Power Consumption: %g\n",total_power+ambient_power);
  fprintf(stderr,"Branch Predictor Power Consumption: %g  (%.3g%%)\n",bpred_power,100*bpred_power/total_power);
  fprintf(stderr," branch target buffer power (W): %g\n",power->btb);
  fprintf(stderr," local predict power (W): %g\n",power->local_predict);
  fprintf(stderr," global predict power (W): %g\n",power->global_predict);
  fprintf(stderr," chooser power (W): %g\n",power->chooser);
  fprintf(stderr," RAS power (W): %g\n",power->ras);
  fprintf(stderr,"Rename Logic Power Consumption: %g  (%.3g%%)\n",rename_power,100*rename_power/total_power);
  fprintf(stderr," Instruction Decode Power (W): %g\n",power->inst_decoder_power);
  fprintf(stderr," RAT decode_power (W): %g\n",power->rat_decoder);
  fprintf(stderr," RAT wordline_power (W): %g\n",power->rat_wordline);
  fprintf(stderr," RAT bitline_power (W): %g\n",power->rat_bitline);
  fprintf(stderr," DCL Comparators (W): %g\n",power->dcl_compare);
  fprintf(stderr,"Instruction Window Power Consumption: %g  (%.3g%%)\n",window_power,100*window_power/total_power);
  fprintf(stderr," tagdrive (W): %g\n",power->wakeup_tagdrive);
  fprintf(stderr," tagmatch (W): %g\n",power->wakeup_tagmatch);
  fprintf(stderr," Selection Logic (W): %g\n",power->selection);
  fprintf(stderr," decode_power (W): %g\n",power->rs_decoder);
  fprintf(stderr," wordline_power (W): %g\n",power->rs_wordline);
  fprintf(stderr," bitline_power (W): %g\n",power->rs_bitline);
  fprintf(stderr,"Load/Store Queue Power Consumption: %g  (%.3g%%)\n",lsq_power,100*lsq_power/total_power);
  fprintf(stderr," tagdrive (W): %g\n",power->lsq_wakeup_tagdrive);
  fprintf(stderr," tagmatch (W): %g\n",power->lsq_wakeup_tagmatch);
  fprintf(stderr," decode_power (W): %g\n",power->lsq_rs_decoder);
  fprintf(stderr," wordline_power (W): %g\n",power->lsq_rs_wordline);
  fprintf(stderr," bitline_power (W): %g\n",power->lsq_rs_bitline);
  fprintf(stderr,"Arch. Register File Power Consumption: %g  (%.3g%%)\n",regfile_power,100*regfile_power/total_power);
  fprintf(stderr," decode_power (W): %g\n",power->regfile_decoder);
  fprintf(stderr," wordline_power (W): %g\n",power->regfile_wordline);
  fprintf(stderr," bitline_power (W): %g\n",power->regfile_bitline);
  fprintf(stderr,"Result Bus Power Consumption: %g  (%.3g%%)\n",power->resultbus,100*power->resultbus/total_power);
  fprintf(stderr,"Total Clock Power: %g  (%.3g%%)\n",power->clock_power,100*power->clock_power/total_power);
  fprintf(stderr,"Int ALU Power: %g  (%.3g%%)\n",power->ialu_power,100*power->ialu_power/total_power);
  fprintf(stderr,"FP ALU Power: %g  (%.3g%%)\n",power->falu_power,100*power->falu_power/total_power);
  fprintf(stderr,"Level 1 Instruction Cache Power Consumption: %g  (%.3g%%)\n",icache_power,100*icache_power/total_power);
  fprintf(stderr," decode_power (W): %g\n",power->icache_decoder); 
  fprintf(stderr," wordline_power (W): %g\n",power->icache_wordline);
  fprintf(stderr," bitline_power (W): %g\n",power->icache_bitline); 
  fprintf(stderr," senseamp_power (W): %g\n",power->icache_senseamp);
  fprintf(stderr," tagarray_power (W): %g\n",power->icache_tagarray);
  fprintf(stderr,"Level 2 Instruction Cache Power Consumption: %g  (%.3g%%)\n",icache2_power,100*icache2_power/total_power);
  fprintf(stderr," decode_power (W): %g\n",power->icache2_decoder);
  fprintf(stderr," wordline_power (W): %g\n",power->icache2_wordline);
  fprintf(stderr," bitline_power (W): %g\n",power->icache2_bitline);
  fprintf(stderr," senseamp_power (W): %g\n",power->icache2_senseamp);
  fprintf(stderr," tagarray_power (W): %g\n",power->icache2_tagarray);
  fprintf(stderr,"Itlb_power (W): %g (%.3g%%)\n",power->itlb,100*power->itlb/total_power);
  fprintf(stderr,"Data Cache Power Consumption: %g  (%.3g%%)\n",dcache_power,100*dcache_power/total_power);
  fprintf(stderr," decode_power (W): %g\n",power->dcache_decoder);
  fprintf(stderr," wordline_power (W): %g\n",power->dcache_wordline);
  fprintf(stderr," bitline_power (W): %g\n",power->dcache_bitline);
  fprintf(stderr," senseamp_power (W): %g\n",power->dcache_senseamp);
  fprintf(stderr," tagarray_power (W): %g\n",power->dcache_tagarray);
  fprintf(stderr,"Dtlb_power (W): %g (%.3g%%)\n",power->dtlb,100*power->dtlb/total_power);
  fprintf(stderr,"Level 2 Cache Power Consumption: %g (%.3g%%)\n",dcache2_power,100*dcache2_power/total_power);
  fprintf(stderr," decode_power (W): %g\n",power->dcache2_decoder);
  fprintf(stderr," wordline_power (W): %g\n",power->dcache2_wordline);
  fprintf(stderr," bitline_power (W): %g\n",power->dcache2_bitline);
  fprintf(stderr," senseamp_power (W): %g\n",power->dcache2_senseamp);
  fprintf(stderr," tagarray_power (W): %g\n",power->dcache2_tagarray);
  */
}

/*======================================================================*/



/* 
 * This part of the code contains routines for each section as
 * described in the tech report.  See the tech report for more details
 * and explanations */

/*----------------------------------------------------------------------*/

double driver_size(double driving_cap, double desiredrisetime) {
  double nsize, psize;
  double Rpdrive; 

  Rpdrive = desiredrisetime/(driving_cap*log(VSINV)*-1.0);
  psize = restowidth(Rpdrive,PCH);
  nsize = restowidth(Rpdrive,NCH);
  if (psize > Wworddrivemax) {
    psize = Wworddrivemax;
  }
  if (psize < 4.0 * LSCALE)
    psize = 4.0 * LSCALE;

  return (psize);

}

/* Decoder delay:  (see section 6.1 of tech report) */
double array_decoder_power_1ststg(rows,cols,predeclength,rports,wports,cache)
     int rows,cols;
     double predeclength;
     int rports,wports;
     int cache;
{
  double Ctotal=0;
  double Ceq=0;
  int decode_bits=0;
  int ports;
  double rowsb;

  /* read and write ports are the same here */
  ports = rports + wports;

  rowsb = (double)rows;

  /* number of input bits to be decoded */
  decode_bits=ceil((logtwo(rowsb)));

  /* First stage: driving the decoders */

  /* this part directly proportional to HD */
  /* This is the capacitance for driving one bit (and its complement).
     -There are #rowsb 3->8 decoders contributing gatecap.
     - 2.0 factor from 2 identical sets of drivers in parallel
  */
  Ceq = 2.0*(draincap(Wdecdrivep,PCH,1)+draincap(Wdecdriven,NCH,1)) +
    gatecap(Wdec3to8n+Wdec3to8p,10.0)*rowsb;


  /* There are ports * #decode_bits total */
  /* HD Model Ctotal+=ports* HD *Ceq; */
  Ctotal= ports*Ceq; 

  if(verbose)
    fprintf(stderr,"Decoder -- Driving decoders            == %g\n",.3*Ctotal*Powerfactor);

  return(Ctotal*Powerfactor);
}

double array_decoder_power_2ndstg(rows,cols,predeclength,rports,wports,cache)
     int rows,cols;
     double predeclength;
     int rports,wports;
     int cache;
{
  double Ctotal=0;
  double Ceq=0;
  int numstack;
  int decode_bits=0;
  int ports;
  double rowsb;

  /* read and write ports are the same here */
  ports = rports + wports;

  rowsb = (double)rows;

  /* number of input bits to be decoded */
  decode_bits=ceil((logtwo(rowsb)));

  /* 
    second stage: driving a bunch of nor gates with a nand 
    numstack is the size of the nor gates -- ie. a 7-128 decoder has
    3-input NAND followed by 3-input NOR  
  */

  numstack = ceil((1.0/3.0)*logtwo(rows));

  if (numstack<=0) numstack = 1;
  if (numstack>5) numstack = 5;

  /* There are #rowsb NOR gates being driven*/
  Ceq = (3.0*draincap(Wdec3to8p,PCH,1) +draincap(Wdec3to8n,NCH,3) +
	 gatecap(WdecNORn+WdecNORp,((numstack*40)+20.0)))*rowsb;

  Ctotal+=ports*Ceq;

  if(verbose)
    fprintf(stderr,"Decoder -- Driving nor w/ nand         == %g\n",.3*ports*Ceq*Powerfactor);

  /* Final stage: driving an inverter with the nor 
     (inverter preceding wordline driver) -- wordline driver is in the next section*/

  Ceq = (gatecap(Wdecinvn+Wdecinvp,20.0)+
	 numstack*draincap(WdecNORn,NCH,1)+
         draincap(WdecNORp,PCH,numstack));

  if(verbose)
    fprintf(stderr,"Decoder -- Driving inverter w/ nor     == %g\n",.3*ports*Ceq*Powerfactor);

  Ctotal+=ports*Ceq;

  /* assume HD Activity Factor */
  return(Ctotal*Powerfactor);
}


double array_decoder_power(rows,cols,predeclength,rports,wports,cache)
     int rows,cols;
     double predeclength;
     int rports,wports;
     int cache;
{
  double Ctotal=0;
  double Ceq=0;
  int numstack;
  int decode_bits=0;
  int ports;
  double rowsb;

  /* read and write ports are the same here */
  ports = rports + wports;

  rowsb = (double)rows;

  /* number of input bits to be decoded */
  decode_bits=ceil((logtwo(rowsb)));

  /* First stage: driving the decoders */

  /* this part directly proportional to HD */
  /* This is the capacitance for driving one bit (and its complement).
     -There are #rowsb 3->8 decoders contributing gatecap.
     - 2.0 factor from 2 identical sets of drivers in parallel
  */
  Ceq = 2.0*(draincap(Wdecdrivep,PCH,1)+draincap(Wdecdriven,NCH,1)) +
    gatecap(Wdec3to8n+Wdec3to8p,10.0)*rowsb;


  /* There are ports * #decode_bits total */
  /* HD Model Ctotal+=ports* HD *Ceq; */
  Ctotal+=ports*decode_bits*Ceq; 

  if(verbose)
    fprintf(stderr,"Decoder -- Driving decoders            == %g\n",.3*Ctotal*Powerfactor);

  /* this part  to HD */
  /* 
    second stage: driving a bunch of nor gates with a nand 
    numstack is the size of the nor gates -- ie. a 7-128 decoder has
    3-input NAND followed by 3-input NOR  
  */

  numstack = ceil((1.0/3.0)*logtwo(rows));

  if (numstack<=0) numstack = 1;
  if (numstack>5) numstack = 5;

  /* There are #rowsb NOR gates being driven*/
  Ceq = (3.0*draincap(Wdec3to8p,PCH,1) +draincap(Wdec3to8n,NCH,3) +
	 gatecap(WdecNORn+WdecNORp,((numstack*40)+20.0)))*rowsb;

  Ctotal+=ports*Ceq;

  if(verbose)
    fprintf(stderr,"Decoder -- Driving nor w/ nand         == %g\n",.3*ports*Ceq*Powerfactor);

  /* Final stage: driving an inverter with the nor 
     (inverter preceding wordline driver) -- wordline driver is in the next section*/

  Ceq = (gatecap(Wdecinvn+Wdecinvp,20.0)+
	 numstack*draincap(WdecNORn,NCH,1)+
         draincap(WdecNORp,PCH,numstack));

  if(verbose)
    fprintf(stderr,"Decoder -- Driving inverter w/ nor     == %g\n",.3*ports*Ceq*Powerfactor);

  Ctotal+=ports*Ceq;

  /* assume Activity Factor 0.3 */
  return(0.3*Ctotal*Powerfactor);
}

double simple_array_decoder_power(rows,cols,rports,wports,cache)
     int rows,cols;
     int rports,wports;
     int cache;
{
  double predeclength=0.0;
  return(array_decoder_power(rows,cols,predeclength,rports,wports,cache));
}


double array_wordline_power(rows,cols,wordlinelength,rports,wports,cache)
     int rows,cols;
     double wordlinelength;
     int rports,wports;
     int cache;
{
  double Ctotal=0;
  double Ceq=0;
  double Cline=0;
  double Cliner, Clinew=0;
  double desiredrisetime,psize,nsize;
  int ports;
  double colsb;

  ports = rports+wports;

  colsb = (double)cols;

  /* Calculate size of wordline drivers assuming rise time == Period / 8 
     - estimate cap on line 
     - compute min resistance to achieve this with RC 
     - compute width needed to achieve this resistance */

  desiredrisetime = Period/16;
  Cline = (gatecappass(Wmemcellr,1.0))*colsb + wordlinelength*CM3metal;
  psize = driver_size(Cline,desiredrisetime);
  
  /* how do we want to do p-n ratioing? -- here we just assume the same ratio 
     from an inverter pair  */
  nsize = psize * Wdecinvn/Wdecinvp; 
  
  if(verbose)
    fprintf(stderr,"Wordline Driver Sizes -- nsize == %f, psize == %f\n",nsize,psize);

  Ceq = draincap(Wdecinvn,NCH,1) + draincap(Wdecinvp,PCH,1) +
    gatecap(nsize+psize,20.0);

  Ctotal+=ports*Ceq;

  if(verbose)
    fprintf(stderr,"Wordline -- Inverter -> Driver         == %g\n",ports*Ceq*Powerfactor);

  /* Compute caps of read wordline and write wordlines 
     - wordline driver caps, given computed width from above
     - read wordlines have 1 nmos access tx, size ~4
     - write wordlines have 2 nmos access tx, size ~2
     - metal line cap
  */

  Cliner = (gatecappass(Wmemcellr,(BitWidth-2*Wmemcellr)/2.0))*colsb+
    wordlinelength*CM3metal+
    2.0*(draincap(nsize,NCH,1) + draincap(psize,PCH,1));
  Clinew = (2.0*gatecappass(Wmemcellw,(BitWidth-2*Wmemcellw)/2.0))*colsb+
    wordlinelength*CM3metal+
    2.0*(draincap(nsize,NCH,1) + draincap(psize,PCH,1));

  if(verbose) {
    fprintf(stderr,"Wordline -- Line                       == %g\n",1e12*Cline);
    fprintf(stderr,"Wordline -- Line -- access -- gatecap  == %g\n",1e12*colsb*2*gatecappass(Wmemcella,(BitWidth-2*Wmemcella)/2.0));
    fprintf(stderr,"Wordline -- Line -- driver -- draincap == %g\n",1e12*draincap(nsize,NCH,1) + draincap(psize,PCH,1));
    fprintf(stderr,"Wordline -- Line -- metal              == %g\n",1e12*wordlinelength*CM3metal);
  }
  Ctotal+=rports*Cliner+wports*Clinew;

  /* AF == 1 assuming a different wordline is charged each cycle, but only
     1 wordline (per port) is actually used */

  return(Ctotal*Powerfactor);
}

double simple_array_wordline_power(rows,cols,rports,wports,cache)
     int rows,cols;
     int rports,wports;
     int cache;
{
  double wordlinelength;
  int ports = rports + wports;
  wordlinelength = cols *  (RegCellWidth + 2 * ports * BitlineSpacing);
  return(array_wordline_power(rows,cols,wordlinelength,rports,wports,cache));
}

double array_bitline_power_rd(rows,cols,bitlinelength,rports,wports,cache)
     int rows,cols;
     double bitlinelength;
     int rports,wports;
     int cache;
{
  double Ctotal=0;
  double Ccolmux=0;
  double Cbitrowr=0;
  double Cprerow=0;
  double Cpregate=0;
  double Cliner=0;
  int ports;
  double rowsb;
  double colsb;

  double desiredrisetime, Cline, psize;

  ports = rports + wports;

  rowsb = (double)rows;
  colsb = (double)cols;

  /* Draincaps of access tx's */

  Cbitrowr = draincap(Wmemcellr,NCH,1);

  /* Cprerow -- precharge cap on the bitline
     -simple scheme to estimate size of pre-charge tx's in a similar fashion
      to wordline driver size estimation.
     -FIXME: it would be better to use precharge/keeper pairs, i've omitted this
      from this version because it couldn't autosize as easily.
  */

  desiredrisetime = Period/8;

  Cline = rowsb*Cbitrowr+CM2metal*bitlinelength;
  psize = driver_size(Cline,desiredrisetime);

  /* compensate for not having an nmos pre-charging */
  psize = psize + psize * Wdecinvn/Wdecinvp; 

  if(verbose)
    printf("Cprerow auto   == %g (psize == %g)\n",draincap(psize,PCH,1),psize);

  Cprerow = draincap(psize,PCH,1);

  /* Cpregate -- cap due to gatecap of precharge transistors -- tack this
     onto bitline cap, again this could have a keeper */
  Cpregate = 4.0*gatecap(psize,10.0);
  //global_clockcap+=rports*cols*2.0*Cpregate;

  if (cache == 0) {
    /* compute the total line cap for read/write bitlines */
    Cliner = rowsb*Cbitrowr+CM2metal*bitlinelength+Cprerow;
    /* Bitline inverters at the end of the bitlines (replaced w/ sense amps
       in cache styles) */
    Ccolmux = gatecap(MSCALE*(29.9+7.8),0.0)+gatecap(MSCALE*(47.0+12.0),0.0);
    Ctotal  = rports*cols*(Cliner+Ccolmux+2.0*Cpregate);
  } 
  else { 
    Cliner = rowsb*Cbitrowr+CM2metal*bitlinelength+Cprerow + draincap(Wbitmuxn,NCH,1);
    Ccolmux = (draincap(Wbitmuxn,NCH,1))+2.0*gatecap(WsenseQ1to4,10.0);
    Ctotal =.5*rports*2.0*cols*(Cliner+Ccolmux+2.0*Cpregate);
  }

  if(cache==0)
    return(Ctotal*Powerfactor);
  else
    return(Ctotal*SensePowerfactor);
  
}

double array_bitline_power_wr(rows,cols,bitlinelength,rports,wports,cache)
     int rows,cols;
     double bitlinelength;
     int rports,wports;
     int cache;
{
  double Ctotal=0;
  double Cbitroww=0;
  double Cwritebitdrive=0;
  double Clinew=0;
  int ports;
  double rowsb;
  double colsb;

  double desiredrisetime, Cline, psize, nsize;

  ports = rports + wports;

  rowsb = (double)rows;
  colsb = (double)cols;

  /* Draincaps of access tx's */

  Cbitroww = draincap(Wmemcellw,NCH,1);

  /* Cprerow -- precharge cap on the bitline
     -simple scheme to estimate size of pre-charge tx's in a similar fashion
      to wordline driver size estimation.
     -FIXME: it would be better to use precharge/keeper pairs, i've omitted this
      from this version because it couldn't autosize as easily.
  */

  desiredrisetime = Period/8;

  /* Cwritebitdrive -- write bitline drivers are used instead of the precharge
     stuff for write bitlines
     - 2 inverter drivers within each driver pair */

  Cline = rowsb*Cbitroww+CM2metal*bitlinelength;

  psize = driver_size(Cline,desiredrisetime);
  nsize = psize * Wdecinvn/Wdecinvp; 

  Cwritebitdrive = 2.0*(draincap(psize,PCH,1)+draincap(nsize,NCH,1));

  if (cache == 0) {
    /* compute the total line cap for read/write bitlines */
    Clinew = rowsb*Cbitroww+CM2metal*bitlinelength+Cwritebitdrive;

    /* Bitline inverters at the end of the bitlines (replaced w/ sense amps
       in cache styles) */
    Ctotal  = .3*wports*cols*(Clinew+Cwritebitdrive);
  } 
  else { 
    Clinew = rowsb*Cbitroww+CM2metal*bitlinelength+Cwritebitdrive;
    Ctotal =.5*wports*2.0*cols*(Clinew+Cwritebitdrive);
  }

  if(cache==0)
    return(Ctotal*Powerfactor);
  else
    return(Ctotal*SensePowerfactor);
  
}

double array_bitline_power(rows,cols,bitlinelength,rports,wports,cache)
     int rows,cols;
     double bitlinelength;
     int rports,wports;
     int cache;
{
  double Ctotal=0;
  double Ccolmux=0;
  double Cbitrowr=0;
  double Cbitroww=0;
  double Cprerow=0;
  double Cwritebitdrive=0;
  double Cpregate=0;
  double Cliner=0;
  double Clinew=0;
  int ports;
  double rowsb;
  double colsb;

  double desiredrisetime, Cline, psize, nsize;

  ports = rports + wports;

  rowsb = (double)rows;
  colsb = (double)cols;

  /* Draincaps of access tx's */

  Cbitrowr = draincap(Wmemcellr,NCH,1);
  Cbitroww = draincap(Wmemcellw,NCH,1);

  /* Cprerow -- precharge cap on the bitline
     -simple scheme to estimate size of pre-charge tx's in a similar fashion
      to wordline driver size estimation.
     -FIXME: it would be better to use precharge/keeper pairs, i've omitted this
      from this version because it couldn't autosize as easily.
  */

  desiredrisetime = Period/8;

  Cline = rowsb*Cbitrowr+CM2metal*bitlinelength;
  psize = driver_size(Cline,desiredrisetime);

  /* compensate for not having an nmos pre-charging */
  psize = psize + psize * Wdecinvn/Wdecinvp; 

  if(verbose)
    printf("Cprerow auto   == %g (psize == %g)\n",draincap(psize,PCH,1),psize);

  Cprerow = draincap(psize,PCH,1);

  /* Cpregate -- cap due to gatecap of precharge transistors -- tack this
     onto bitline cap, again this could have a keeper */
  Cpregate = 4.0*gatecap(psize,10.0);
  global_clockcap+=rports*cols*2.0*Cpregate;

  /* Cwritebitdrive -- write bitline drivers are used instead of the precharge
     stuff for write bitlines
     - 2 inverter drivers within each driver pair */

  Cline = rowsb*Cbitroww+CM2metal*bitlinelength;

  psize = driver_size(Cline,desiredrisetime);
  nsize = psize * Wdecinvn/Wdecinvp; 

  Cwritebitdrive = 2.0*(draincap(psize,PCH,1)+draincap(nsize,NCH,1));

  /* 
     reg files (cache==0) 
     => single ended bitlines (1 bitline/col)
     => AFs from pop_count
     caches (cache ==1)
     => double-ended bitlines (2 bitlines/col)
     => AFs = .5 (since one of the two bitlines is always charging/discharging)
  */

#ifdef STATIC_AF
  if (cache == 0) {
    /* compute the total line cap for read/write bitlines */
    Cliner = rowsb*Cbitrowr+CM2metal*bitlinelength+Cprerow;
    Clinew = rowsb*Cbitroww+CM2metal*bitlinelength+Cwritebitdrive;

    /* Bitline inverters at the end of the bitlines (replaced w/ sense amps
       in cache styles) */
    Ccolmux = gatecap(MSCALE*(29.9+7.8),0.0)+gatecap(MSCALE*(47.0+12.0),0.0);
    Ctotal+=(1.0-POPCOUNT_AF)*rports*cols*(Cliner+Ccolmux+2.0*Cpregate);
    Ctotal+=.3*wports*cols*(Clinew+Cwritebitdrive);
  } 
  else { 
    Cliner = rowsb*Cbitrowr+CM2metal*bitlinelength+Cprerow + draincap(Wbitmuxn,NCH,1);
    Clinew = rowsb*Cbitroww+CM2metal*bitlinelength+Cwritebitdrive;
    Ccolmux = (draincap(Wbitmuxn,NCH,1))+2.0*gatecap(WsenseQ1to4,10.0);
    Ctotal+=.5*rports*2.0*cols*(Cliner+Ccolmux+2.0*Cpregate);
    Ctotal+=.5*wports*2.0*cols*(Clinew+Cwritebitdrive);
  }
#else
  if (cache == 0) {
    /* compute the total line cap for read/write bitlines */
    Cliner = rowsb*Cbitrowr+CM2metal*bitlinelength+Cprerow;
    Clinew = rowsb*Cbitroww+CM2metal*bitlinelength+Cwritebitdrive;

    /* Bitline inverters at the end of the bitlines (replaced w/ sense amps
       in cache styles) */
    Ccolmux = gatecap(MSCALE*(29.9+7.8),0.0)+gatecap(MSCALE*(47.0+12.0),0.0);
    Ctotal += rports*cols*(Cliner+Ccolmux+2.0*Cpregate);
    Ctotal += .3*wports*cols*(Clinew+Cwritebitdrive);
  } 
  else { 
    Cliner = rowsb*Cbitrowr+CM2metal*bitlinelength+Cprerow + draincap(Wbitmuxn,NCH,1);
    Clinew = rowsb*Cbitroww+CM2metal*bitlinelength+Cwritebitdrive;
    Ccolmux = (draincap(Wbitmuxn,NCH,1))+2.0*gatecap(WsenseQ1to4,10.0);
    Ctotal+=.5*rports*2.0*cols*(Cliner+Ccolmux+2.0*Cpregate);
    Ctotal+=.5*wports*2.0*cols*(Clinew+Cwritebitdrive);
  }
#endif

  if(verbose) {
    fprintf(stderr,"Bitline -- Precharge                   == %g\n",1e12*Cpregate);
    fprintf(stderr,"Bitline -- Line                        == %g\n",1e12*(Cliner+Clinew));
    fprintf(stderr,"Bitline -- Line -- access draincap     == %g\n",1e12*rowsb*Cbitrowr);
    fprintf(stderr,"Bitline -- Line -- precharge draincap  == %g\n",1e12*Cprerow);
    fprintf(stderr,"Bitline -- Line -- metal               == %g\n",1e12*bitlinelength*CM2metal);
    fprintf(stderr,"Bitline -- Colmux                      == %g\n",1e12*Ccolmux);

    fprintf(stderr,"\n");
  }


  if(cache==0)
    return(Ctotal*Powerfactor);
  else
    return(Ctotal*SensePowerfactor*.4);
  
}


double simple_array_bitline_power(rows,cols,rports,wports,cache)
     int rows,cols;
     int rports,wports;
     int cache;
{
  double bitlinelength;

  int ports = rports + wports;

  bitlinelength = rows * (RegCellHeight + ports * WordlineSpacing);

  return (array_bitline_power(rows,cols,bitlinelength,rports,wports,cache));

}

/* estimate senseamp power dissipation in cache structures (Zyuban's method) */
/*
double senseamp_power(int cols)
{
  return((double)cols * Vdd/8 * .5e-3);
}
*/

double senseamp_power(int cols)
{
  /* multiplied by HD */
  return ((double)cols * Vdd/8 * .5e-3);
}

/* estimate comparator power consumption (this comparator is similar
   to the tag-match structure in a CAM */
double compare_cap(int compare_bits)
{
  double c1, c2;
  /* bottom part of comparator */
  c2 = (compare_bits)*(draincap(Wcompn,NCH,1)+draincap(Wcompn,NCH,2))+
    draincap(Wevalinvp,PCH,1) + draincap(Wevalinvn,NCH,1);

  /* top part of comparator */
  c1 = (compare_bits)*(draincap(Wcompn,NCH,1)+draincap(Wcompn,NCH,2)+
		       draincap(Wcomppreequ,NCH,1)) +
    gatecap(WdecNORn,1.0)+
    gatecap(WdecNORp,3.0);

  return(c1 + c2);
}

/* power of depency check logic */
double dcl_compare_power(int compare_bits)
{
  double Ctotal;
  int num_comparators;
  
  num_comparators = (ruu_decode_width - 1) * (ruu_decode_width);

  Ctotal = num_comparators * compare_cap(compare_bits);

  return(Ctotal*Powerfactor*AF);
}

double simple_array_power(rows,cols,rports,wports,cache)
     int rows,cols;
     int rports,wports;
     int cache;
{
  if(cache==0)
    return( simple_array_decoder_power(rows,cols,rports,wports,cache)+
	    simple_array_wordline_power(rows,cols,rports,wports,cache)+
	    simple_array_bitline_power(rows,cols,rports,wports,cache));
  else
    return( simple_array_decoder_power(rows,cols,rports,wports,cache)+
	    simple_array_wordline_power(rows,cols,rports,wports,cache)+
	    simple_array_bitline_power(rows,cols,rports,wports,cache)+
	    senseamp_power(cols));
}


double cam_tagdrive(rows,cols,rports,wports)
     int rows,cols,rports,wports;
{
  double Ctotal, Ctlcap, Cblcap, Cwlcap;
  double taglinelength;
  double wordlinelength;
  double nsize, psize;
  int ports;
  Ctotal=0;

  ports = rports + wports;

  taglinelength = rows * 
    (CamCellHeight + ports * MatchlineSpacing);

  wordlinelength = cols * 
    (CamCellWidth + ports * TaglineSpacing);

  /* Compute tagline cap */
  Ctlcap = Cmetal * taglinelength + 
    rows * gatecappass(Wcomparen2,2.0) +
    draincap(Wcompdrivern,NCH,1)+draincap(Wcompdriverp,PCH,1);

  /* Compute bitline cap (for writing new tags) */
  Cblcap = Cmetal * taglinelength +
    rows * draincap(Wmemcellr,NCH,2);

  /* autosize wordline driver */
  psize = driver_size(Cmetal * wordlinelength + 2 * cols * gatecap(Wmemcellr,2.0),Period/8);
  nsize = psize * Wdecinvn/Wdecinvp; 

  /* Compute wordline cap (for writing new tags) */
  Cwlcap = Cmetal * wordlinelength + 
    draincap(nsize,NCH,1)+draincap(psize,PCH,1) +
    2 * cols * gatecap(Wmemcellr,2.0);
    
  Ctotal += (rports * cols * 2 * Ctlcap) + 
    (wports * ((cols * 2 * Cblcap) + (rows * Cwlcap)));

  return(Ctotal*Powerfactor*AF);
}

double cam_tagmatch(rows,cols,rports,wports)
     int rows,cols,rports,wports;
{
  double Ctotal, Cmlcap;
  double matchlinelength;
  int ports;
  Ctotal=0;

  ports = rports + wports;

  matchlinelength = cols * 
    (CamCellWidth + ports * TaglineSpacing);

  Cmlcap = 2 * cols * draincap(Wcomparen1,NCH,2) + 
    Cmetal * matchlinelength + draincap(Wmatchpchg,NCH,1) +
    gatecap(Wmatchinvn+Wmatchinvp,10.0) +
    gatecap(Wmatchnandn+Wmatchnandp,10.0);

  Ctotal += rports * rows * Cmlcap;

  global_clockcap += rports * rows * gatecap(Wmatchpchg,5.0);
  
  /* noring the nanded match lines */
  if(ruu_issue_width >= 8)
    Ctotal += 2 * gatecap(Wmatchnorn+Wmatchnorp,10.0);

  return(Ctotal*Powerfactor*AF);
}

double cam_array(rows,cols,rports,wports)
     int rows,cols,rports,wports;
{
  return(cam_tagdrive(rows,cols,rports,wports) +
	 cam_tagmatch(rows,cols,rports,wports));
}


double selection_power(int win_entries)
{
  double Ctotal, Cor, Cpencode;
  int num_arbiter=1;

  Ctotal=0;

  while(win_entries > 4)
    {
      win_entries = (int)ceil((double)win_entries / 4.0);
      num_arbiter += win_entries;
    }

  Cor = 4 * draincap(WSelORn,NCH,1) + draincap(WSelORprequ,PCH,1);

  Cpencode = draincap(WSelPn,NCH,1) + draincap(WSelPp,PCH,1) + 
    2*draincap(WSelPn,NCH,1) + draincap(WSelPp,PCH,2) + 
    3*draincap(WSelPn,NCH,1) + draincap(WSelPp,PCH,3) + 
    4*draincap(WSelPn,NCH,1) + draincap(WSelPp,PCH,4) + 
    4*gatecap(WSelEnn+WSelEnp,20.0) + 
    4*draincap(WSelEnn,NCH,1) + 4*draincap(WSelEnp,PCH,1);

  Ctotal += ruu_issue_width * num_arbiter*(Cor+Cpencode);

  return(Ctotal*Powerfactor*AF);
} 

/* very rough clock power estimates */
double total_clockpower(double die_length)
{

  double clocklinelength;
  double Cline,Cline2,Ctotal;
  double pipereg_clockcap=0;
  double global_buffercap = 0;
  double Clockpower;

  double num_piperegs;

  int npreg_width = (int)ceil(logtwo((double)RUU_size));

  /* Assume say 8 stages (kinda low now).
     FIXME: this could be a lot better; user could input
     number of pipestages, etc  */

  /* assume 8 pipe stages and try to estimate bits per pipe stage */
  /* pipe stage 0/1 */
  num_piperegs = ruu_issue_width*inst_length + data_width;
  /* pipe stage 1/2 */
  num_piperegs += ruu_issue_width*(inst_length + 3 * RUU_size);
  /* pipe stage 2/3 */
  num_piperegs += ruu_issue_width*(inst_length + 3 * RUU_size);
  /* pipe stage 3/4 */
  num_piperegs += ruu_issue_width*(3 * npreg_width + pow2(opcode_length));
  /* pipe stage 4/5 */
  num_piperegs += ruu_issue_width*(2*data_width + pow2(opcode_length));
  /* pipe stage 5/6 */
  num_piperegs += ruu_issue_width*(data_width + pow2(opcode_length));
  /* pipe stage 6/7 */
  num_piperegs += ruu_issue_width*(data_width + pow2(opcode_length));
  /* pipe stage 7/8 */
  num_piperegs += ruu_issue_width*(data_width + pow2(opcode_length));

  /* assume 50% extra in control signals (rule of thumb) */
  num_piperegs = num_piperegs * 1.5;

  pipereg_clockcap = num_piperegs * 4*gatecap(10.0,0);

  /* estimate based on 3% of die being in clock metal */
  Cline2 = Cmetal * (.03 * die_length * die_length/BitlineSpacing) * 1e6 * 1e6;

  /* another estimate */
  clocklinelength = die_length*(.5 + 4 * (.25 + 2*(.25) + 4 * (.125)));
  Cline = 20 * Cmetal * (clocklinelength) * 1e6;
  global_buffercap = 12*gatecap(1000.0,10.0)+16*gatecap(200,10.0)+16*8*2*gatecap(100.0,10.00) + 2*gatecap(.29*1e6,10.0);
  /* global_clockcap is computed within each array structure for pre-charge tx's*/
  Ctotal = Cline+global_clockcap+pipereg_clockcap+global_buffercap;

  if(verbose)
    fprintf(stderr,"num_piperegs == %f\n",num_piperegs);

  /* add I_ADD Clockcap and F_ADD Clockcap */
  Clockpower = Ctotal*Powerfactor + res_ialu*I_ADD_CLOCK + res_fpalu*F_ADD_CLOCK;

  if(verbose) {
    fprintf(stderr,"Global Clock Power: %g\n",Clockpower);
    fprintf(stderr," Global Metal Lines   (W): %g\n",Cline*Powerfactor);
    fprintf(stderr," Global Metal Lines (3%%) (W): %g\n",Cline2*Powerfactor);
    fprintf(stderr," Global Clock Buffers (W): %g\n",global_buffercap*Powerfactor);
    fprintf(stderr," Global Clock Cap (Explicit) (W): %g\n",global_clockcap*Powerfactor+I_ADD_CLOCK+F_ADD_CLOCK);
    fprintf(stderr," Global Clock Cap (Implicit) (W): %g\n",pipereg_clockcap*Powerfactor);
  }
  return(Clockpower);

}

/* very rough global clock power estimates */
double global_clockpower(double die_length)
{

  double clocklinelength;
  double Cline,Cline2,Ctotal;
  double global_buffercap = 0;

  Cline2 = Cmetal * (.03 * die_length * die_length/BitlineSpacing) * 1e6 * 1e6;

  clocklinelength = die_length*(.5 + 4 * (.25 + 2*(.25) + 4 * (.125)));
  Cline = 20 * Cmetal * (clocklinelength) * 1e6;
  global_buffercap = 12*gatecap(1000.0,10.0)+16*gatecap(200,10.0)+16*8*2*gatecap(100.0,10.00) + 2*gatecap(.29*1e6,10.0);
  Ctotal = Cline+global_buffercap;

  if(verbose) {
    fprintf(stderr,"Global Clock Power: %g\n",Ctotal*Powerfactor);
    fprintf(stderr," Global Metal Lines   (W): %g\n",Cline*Powerfactor);
    fprintf(stderr," Global Metal Lines (3%%) (W): %g\n",Cline2*Powerfactor);
    fprintf(stderr," Global Clock Buffers (W): %g\n",global_buffercap*Powerfactor);
  }

  return(Ctotal*Powerfactor);

}


double compute_resultbus_power()
{
  double Ctotal, Cline;

  double regfile_height;

  /* compute size of result bus tags */
  int npreg_width = (int)ceil(logtwo((double)RUU_size));

  Ctotal=0;

  regfile_height = RUU_size * (RegCellHeight + 
			       WordlineSpacing * 3 * ruu_issue_width); 

  /* assume num alu's == ialu  (FIXME: generate a more detailed result bus network model*/
  Cline = Cmetal * (regfile_height + .5 * res_ialu * 3200.0 * LSCALE);

  /* or use result bus length measured from 21264 die photo */
  /*  Cline = Cmetal * 3.3*1000;*/

  /* Assume ruu_issue_width result busses -- power can be scaled linearly
     for number of result busses (scale by writeback_access) */
  Ctotal += 2.0 * (data_width + npreg_width) * (ruu_issue_width)* Cline;

#ifdef STATIC_AF
  return(Ctotal*Powerfactor*AF);
#else
  return(Ctotal*Powerfactor);
#endif
  
}

/* used in sim-outorder */
void calculate_power(power)
     power_result_type *power;
{
  double clockpower;
  double predeclength, wordlinelength, dwordlinelength, twordlinelength, bitlinelength;
  int ndwl, ndbl, nspd, ntwl, ntbl, ntspd, c,b,a,cache, rowsb, colsb;
  int trowsb, tcolsb, tagsize;
  int va_size = 32;

  int npreg_width = (int)ceil(logtwo((double)RUU_size));

  /* these variables are needed to use Cacti to auto-size cache arrays 
     (for optimal delay) */
  time_result_type time_result;
  time_parameter_type time_parameters;

  /* used to autosize other structures, like bpred tables */
  int scale_factor;

  global_clockcap = 0;

  cache=0;


  /* 
     FIXME: ALU power is a simple constant, it would be better
     to include bit AFs and have different numbers for different
     types of operations 
  */
  power->ialu_power 
	  = res_ialu * I_ADD;
  power->falu_power 
	  = res_fpalu * F_ADD;

  /* MD_NUM_IREGS: number of integer registers */
  nvreg_width = (int)ceil(logtwo((double)MD_NUM_IREGS));
  npreg_width = (int)ceil(logtwo((double)RUU_size));


  /* 
     RAT (Register Alias Table) has shadow bits stored in each cell, this makes the
     cell size larger than normal array structures, so we must
     compute it here 
  */

  /* pre-decoder length */
  predeclength 
	  = MD_NUM_IREGS * (RatCellHeight + 3 * ruu_decode_width * WordlineSpacing);

  /* word-line length */
  /* what the hell is "npreg_width"? */
  wordlinelength 
	  = npreg_width * (RatCellWidth + 6 * ruu_decode_width * BitlineSpacing + RatShiftRegWidth*RatNumShift);

  /* bit-line length */
  bitlinelength 
	  = MD_NUM_IREGS * (RatCellHeight + 3 * ruu_decode_width * WordlineSpacing);

  if(verbose)
    fprintf(stderr,"rat power stats\n");
  power->rat_decoder = array_decoder_power(MD_NUM_IREGS,npreg_width,predeclength,2*ruu_decode_width,ruu_decode_width,cache);
  power->rat_wordline = array_wordline_power(MD_NUM_IREGS,npreg_width,wordlinelength,2*ruu_decode_width,ruu_decode_width,cache);
  power->rat_bitline = array_bitline_power(MD_NUM_IREGS,npreg_width,bitlinelength,2*ruu_decode_width,ruu_decode_width,cache);
  power->rat_senseamp = 0;

  power->dcl_compare 
	  = dcl_compare_power(nvreg_width);
  power->dcl_pencode 
	  = 0;
  power->inst_decoder_power 
	  = ruu_decode_width * simple_array_decoder_power(opcode_length,1,1,1,cache);
  power->wakeup_tagdrive 
	  =cam_tagdrive(RUU_size,npreg_width,ruu_issue_width,ruu_issue_width);
  power->wakeup_tagmatch 
	  =cam_tagmatch(RUU_size,npreg_width,ruu_issue_width,ruu_issue_width);
  power->wakeup_ormatch 
	  =0; 
  
  power->selection = selection_power(RUU_size);

  
  /* 
     
    Register File Power Estimation

  */

  /* pre-decoder length */
  predeclength 
	  = MD_NUM_IREGS * (RegCellHeight + 3 * ruu_issue_width * WordlineSpacing);

  /* word-line length */
  wordlinelength 
	  = data_width * (RegCellWidth + 6 * ruu_issue_width * BitlineSpacing);

  /* bit-line length */
  bitlinelength 
	  = MD_NUM_IREGS * (RegCellHeight + 3 * ruu_issue_width * WordlineSpacing);

  if(verbose)
    fprintf(stderr,"regfile power stats\n");

  power->regfile_decoder 
	  = array_decoder_power(MD_NUM_IREGS,data_width,predeclength,2*ruu_issue_width,ruu_issue_width,cache);
  power->regfile_wordline 
	  = array_wordline_power(MD_NUM_IREGS,data_width,wordlinelength,2*ruu_issue_width,ruu_issue_width,cache);
  power->regfile_bitline 
	  = array_bitline_power(MD_NUM_IREGS,data_width,bitlinelength,2*ruu_issue_width,ruu_issue_width,cache);
  power->regfile_senseamp 
	  =0;

  /* 
     
    Reservation Station Power Estimation

  */

  /* pre-decoder length */
  predeclength 
	  = RUU_size * (RegCellHeight + 3 * ruu_issue_width * WordlineSpacing);

  /* word-line length */
  wordlinelength 
	  = data_width * (RegCellWidth + 6 * ruu_issue_width * BitlineSpacing);

  /* bit-line length */
  bitlinelength 
	  = RUU_size * (RegCellHeight + 3 * ruu_issue_width * WordlineSpacing);

  if(verbose)
    fprintf(stderr,"res station power stats\n");
  power->rs_decoder 
	  = array_decoder_power(RUU_size,data_width,predeclength,2*ruu_issue_width,ruu_issue_width,cache);
  power->rs_wordline 
	  = array_wordline_power(RUU_size,data_width,wordlinelength,2*ruu_issue_width,ruu_issue_width,cache);
  power->rs_bitline 
	  = array_bitline_power(RUU_size,data_width,bitlinelength,2*ruu_issue_width,ruu_issue_width,cache);
  /* no senseamps in reg file structures (only caches) */
  power->rs_senseamp 
	  =0;

  /* 
     
    Load/Store Queue Power Estimation

  */

  /* addresses go into lsq tag's */
  power->lsq_wakeup_tagdrive 
	  =cam_tagdrive(LSQ_size,data_width,res_memport,res_memport);
  power->lsq_wakeup_tagmatch 
	  =cam_tagmatch(LSQ_size,data_width,res_memport,res_memport);
  power->lsq_wakeup_ormatch 
	  =0; 

  /* word-line length */
  wordlinelength 
	  = data_width * (RegCellWidth + 4 * res_memport * BitlineSpacing);

  /* bit-line length */
  bitlinelength 
	  = RUU_size * (RegCellHeight + 4 * res_memport * WordlineSpacing);

  /* rs's hold data */
  if(verbose)
    fprintf(stderr,"lsq station power stats\n");
  power->lsq_rs_decoder 
	  = array_decoder_power(LSQ_size,data_width,predeclength,res_memport,res_memport,cache);
  power->lsq_rs_wordline 
	  = array_wordline_power(LSQ_size,data_width,wordlinelength,res_memport,res_memport,cache);
  power->lsq_rs_bitline 
	  = array_bitline_power(LSQ_size,data_width,bitlinelength,res_memport,res_memport,cache);
  power->lsq_rs_senseamp 
	  =0;

  /* 
     
    BUS Power Estimation

    What a poor estimation method!!

  */

  power->resultbus = compute_resultbus_power();


  /* Load cache values into what cacti is expecting */
  time_parameters.cache_size = btb_config[0] * (data_width/8) * btb_config[1]; /* C */
  time_parameters.block_size = (data_width/8); /* B */
  time_parameters.associativity = btb_config[1]; /* A */
  time_parameters.number_of_sets = btb_config[0]; /* C/(B*A) */

  /* have Cacti compute optimal cache config */
  calculate_time(&time_result,&time_parameters);
  output_data(&time_result,&time_parameters);

  /* extract Cacti results */
  ndwl=time_result.best_Ndwl;
  ndbl=time_result.best_Ndbl;
  nspd=time_result.best_Nspd;
  ntwl=time_result.best_Ntwl;
  ntbl=time_result.best_Ntbl;
  ntspd=time_result.best_Ntspd;

  c = time_parameters.cache_size;
  b = time_parameters.block_size;
  a = time_parameters.associativity; 

  cache=1;

  /* Figure out how many rows/cols there are now */
  rowsb = c/(b*a*ndbl*nspd);
  colsb = 8*b*a*nspd/ndwl;

  tagsize = va_size - ((int)logtwo(cache_il1->nsets) + (int)logtwo(cache_il1->bsize));
  trowsb = c/(b*a*ntbl*ntspd);
  tcolsb = a * (tagsize + 1 + 6) * ntspd/ntwl;

  if(verbose) {
    fprintf(stderr,"%d KB %d-way btb (%d-byte block size):\n",c,a,b);
    fprintf(stderr,"ndwl == %d, ndbl == %d, nspd == %d\n",ndwl,ndbl,nspd);
    fprintf(stderr,"%d sets of %d rows x %d cols\n",ndwl*ndbl,rowsb,colsb);
  }

  predeclength 
	  = rowsb * (RegCellHeight + WordlineSpacing);
  wordlinelength 
	  = colsb * (RegCellWidth + BitlineSpacing);
  bitlinelength 
	  = rowsb * (RegCellHeight + WordlineSpacing);

  if(verbose)
    fprintf(stderr,"btb power stats\n");

  power->btb 
	  = ndwl*ndbl*(array_decoder_power(rowsb,colsb,predeclength,1,1,cache) 
	  + array_wordline_power(rowsb,colsb,wordlinelength,1,1,cache) 
	  + array_bitline_power(rowsb,colsb,bitlinelength,1,1,cache) 
	  + senseamp_power(colsb));

  power->hd_btb_decoder_1ststg 
          = ndwl*ndbl*array_decoder_power_1ststg(rowsb,colsb,predeclength,1,1,cache);
  power->hd_btb_tag_decoder_1ststg 
	  = 0.0;
  /*
  power->hd_btb_tag_decoder_1ststg 
	  = ntwl*ntbl*array_decoder_power_1ststg(trowsb,tcolsb,predeclength,1,1,cache);
  */
  power->hd_btb_decoder_2ndstg 
	  = ndwl*ndbl*array_decoder_power_2ndstg(rowsb,colsb,predeclength,1,1,cache);
  power->hd_btb_tag_decoder_2ndstg 
	  = 0.0;
  /*
  power->hd_btb_tag_decoder_2ndstg 
	  = ntwl*ntbl*array_decoder_power_2ndstg(trowsb,tcolsb,predeclength,1,1,cache);
  */
  power->hd_btb_wordline 
	  = ndwl*ndbl*array_wordline_power(rowsb,colsb,dwordlinelength,1,1,cache);
  power->hd_btb_tag_wordline 
	  = 0.0;
  /*
  power->hd_btb_tag_wordline 
	  = ntwl*ntbl*array_wordline_power(trowsb,tcolsb,twordlinelength,1,1,cache);
  */
  power->hd_btb_bitline_rd 
	  = ndwl*ndbl*array_bitline_power_rd(rowsb,colsb,bitlinelength,1,1,cache);
  power->hd_btb_tag_bitline_rd 
	  = 0.0;
  /*
  power->hd_btb_tag_bitline_rd 
	  = ntwl*ntbl*array_bitline_power_rd(trowsb,tcolsb,bitlinelength,1,1,cache);
  */
  power->hd_btb_bitline_wr 
	  = ndwl*ndbl*array_bitline_power_wr(rowsb,colsb,bitlinelength,1,1,cache);
  power->hd_btb_tag_bitline_wr 
	  = 0.0;
  /*
  power->hd_btb_tag_bitline_wr 
	  = ntwl*ntbl*array_bitline_power_wr(trowsb,tcolsb,bitlinelength,1,1,cache);
  */
  power->hd_btb_senseamp 
	  = ndwl*ndbl*senseamp_power(colsb)/inst_width;
  power->hd_btb_tag_senseamp 
	  = 0.0;
  /*
  power->hd_btb_tag_senseamp 
	  = ntwl*ntbl*senseamp_power(tcolsb)/(double)tagsize;
  */
    
  power->hd_btb_tagarray 
	  = power->hd_btb_tag_decoder_2ndstg
	  + power->hd_btb_tag_wordline;

  power->hd_btb 
	  = power->hd_btb_decoder_2ndstg
	  + power->hd_btb_wordline 
	  + power->hd_btb_tagarray;
  
  cache=1;

  scale_factor 
	  = squarify(twolev_config[0],twolev_config[2]);
  predeclength 
	  = (twolev_config[0] / scale_factor) * (RegCellHeight + WordlineSpacing);
  wordlinelength 
	  = (twolev_config[2] * scale_factor) * (RegCellWidth + BitlineSpacing);
  bitlinelength 
	  = (twolev_config[0] / scale_factor) * (RegCellHeight + WordlineSpacing);

  if(verbose)
    fprintf(stderr,"local predict power stats\n");

  power->local_predict 
	  = array_decoder_power(twolev_config[0]/scale_factor,twolev_config[2]*scale_factor,predeclength,1,1,cache) 
	  + array_wordline_power(twolev_config[0]/scale_factor,twolev_config[2]*scale_factor,wordlinelength,1,1,cache) 
	  + array_bitline_power(twolev_config[0]/scale_factor,twolev_config[2]*scale_factor,bitlinelength,1,1,cache) 
	  + senseamp_power(twolev_config[2]*scale_factor);

  scale_factor 
	  = squarify(twolev_config[1],3);

  predeclength 
	  = (twolev_config[1] / scale_factor)* (RegCellHeight + WordlineSpacing);
  wordlinelength 
	  = 3 * scale_factor *  (RegCellWidth + BitlineSpacing);
  bitlinelength 
	  = (twolev_config[1] / scale_factor) * (RegCellHeight + WordlineSpacing);


  if(verbose)
    fprintf(stderr,"local predict power stats\n");
  power->local_predict 
	  += array_decoder_power(twolev_config[1]/scale_factor,3*scale_factor,predeclength,1,1,cache) 
	  +  array_wordline_power(twolev_config[1]/scale_factor,3*scale_factor,wordlinelength,1,1,cache) 
	  +  array_bitline_power(twolev_config[1]/scale_factor,3*scale_factor,bitlinelength,1,1,cache) 
	  +  senseamp_power(3*scale_factor);

  if(verbose)
    fprintf(stderr,"bimod_config[0] == %d\n",bimod_config[0]);

  scale_factor 
	  = squarify(bimod_config[0],2);

  predeclength 
	  = bimod_config[0]/scale_factor * (RegCellHeight + WordlineSpacing);
  wordlinelength 
	  = 2*scale_factor * (RegCellWidth + BitlineSpacing);
  bitlinelength 
	  = bimod_config[0]/scale_factor * (RegCellHeight + WordlineSpacing);


  if(verbose)
    fprintf(stderr,"global predict power stats\n");
  power->global_predict 
	  = array_decoder_power(bimod_config[0]/scale_factor,2*scale_factor,predeclength,1,1,cache) 
	  + array_wordline_power(bimod_config[0]/scale_factor,2*scale_factor,wordlinelength,1,1,cache) 
	  + array_bitline_power(bimod_config[0]/scale_factor,2*scale_factor,bitlinelength,1,1,cache) 
	  + senseamp_power(2*scale_factor);

  scale_factor 
	  = squarify(comb_config[0],2);

  predeclength 
	  = comb_config[0]/scale_factor * (RegCellHeight + WordlineSpacing);
  wordlinelength 
	  = 2*scale_factor *  (RegCellWidth + BitlineSpacing);
  bitlinelength 
	  = comb_config[0]/scale_factor * (RegCellHeight + WordlineSpacing);

  if(verbose)
    fprintf(stderr,"chooser predict power stats\n");
  power->chooser 
	  = array_decoder_power(comb_config[0]/scale_factor,2*scale_factor,predeclength,1,1,cache) 
	  + array_wordline_power(comb_config[0]/scale_factor,2*scale_factor,wordlinelength,1,1,cache) 
	  + array_bitline_power(comb_config[0]/scale_factor,2*scale_factor,bitlinelength,1,1,cache) 
	  + senseamp_power(2*scale_factor);

  if(verbose)
    fprintf(stderr,"RAS predict power stats\n");
  power->ras 
	  = simple_array_power(ras_size,data_width,1,1,0); 
  
  if (!mystricmp(cache_dl1_opt, "none")) /* none dl1 cache configuration case */ 
    tagsize = 0; 
  else /* none dl1 cache configuration case */ 
    tagsize = va_size - ((int)logtwo(cache_dl1->nsets) + (int)logtwo(cache_dl1->bsize));

  if(verbose)
    fprintf(stderr,"dtlb predict power stats\n");

  if (!mystricmp(dtlb_opt, "none")) /* none dtlb configuration case */ 
    power->dtlb 
	    = 0;
  else /* none dtlb configuration case */ 
    power->dtlb 
	    = res_memport*(cam_array(dtlb->nsets, va_size - (int)logtwo((double)dtlb->bsize),1,1) 
	    + simple_array_power(dtlb->nsets,tagsize,1,1,cache));

  if (!mystricmp(cache_il1_opt, "none")) /* none il1 cache configuration case */ 
    tagsize = 0;
  else
    tagsize = va_size - ((int)logtwo(cache_il1->nsets) + (int)logtwo(cache_il1->bsize));

  if (!mystricmp(itlb_opt, "none")) /* none itlb configuration case */ 
  {  
    predeclength   = 0;
    wordlinelength = 0;
    bitlinelength  = 0;
    power->itlb    = 0;
  }
  else{ /* itlb configuration case */ 
    predeclength   = itlb->nsets * (RegCellHeight + WordlineSpacing);
    wordlinelength = logtwo((double)itlb->bsize) * (RegCellWidth + BitlineSpacing);
    bitlinelength  = itlb->nsets * (RegCellHeight + WordlineSpacing);

    if(verbose)
      fprintf(stderr,"itlb predict power stats\n");
    power->itlb 
	    = cam_array(itlb->nsets, va_size - (int)logtwo((double)itlb->bsize),1,1) 
	    + simple_array_power(itlb->nsets,tagsize,1,1,cache);
  }

  cache=1;

  /* no icache level 1 configuration */
  if (!mystricmp(cache_il1_opt, "none"))
  {
    time_parameters.cache_size 		= 0;
    time_parameters.block_size 		= 0;
    time_parameters.associativity 	= 0;
    time_parameters.number_of_sets 	= 0;

    power->icache_power = 0.0; 
  }
  else
  {
    time_parameters.cache_size 		= cache_il1->nsets * cache_il1->bsize * cache_il1->assoc; /* C */
    time_parameters.block_size 		= cache_il1->bsize; /* B */
    time_parameters.associativity 	= cache_il1->assoc; /* A */
    time_parameters.number_of_sets 	= cache_il1->nsets; /* C/(B*A) */

    calculate_time(&time_result,&time_parameters);
    output_data(&time_result,&time_parameters);

    ndwl  = time_result.best_Ndwl;
    ndbl  = time_result.best_Ndbl;
    nspd  = time_result.best_Nspd;
    ntwl  = time_result.best_Ntwl;
    ntbl  = time_result.best_Ntbl;
    ntspd = time_result.best_Ntspd;

    c = time_parameters.cache_size;
    b = time_parameters.block_size;
    a = time_parameters.associativity;

    rowsb = c/(b*a*ndbl*nspd);
    colsb = 8*b*a*nspd/ndwl;


    tagsize = va_size - ((int)logtwo(cache_il1->nsets) + (int)logtwo(cache_il1->bsize));
    trowsb = c/(b*a*ntbl*ntspd);
    tcolsb = a * (tagsize + 1 + 6) * ntspd/ntwl;
 
    if(verbose) {
      fprintf(stderr,"%d KB %d-way cache (%d-byte block size):\n",c,a,b);
      fprintf(stderr,"ndwl == %d, ndbl == %d, nspd == %d\n",ndwl,ndbl,nspd);
      fprintf(stderr,"%d sets of %d rows x %d cols\n",ndwl*ndbl,rowsb,colsb);
      fprintf(stderr,"tagsize == %d\n",tagsize);
    }

    predeclength 
	    = rowsb * (RegCellHeight + WordlineSpacing);
    dwordlinelength 
	    = colsb * (RegCellWidth + BitlineSpacing);
    twordlinelength 
	    = tcolsb * (RegCellWidth + BitlineSpacing);
    bitlinelength 
	    = rowsb * (RegCellHeight + WordlineSpacing);

    if(verbose)
      fprintf(stderr,"icache power stats\n");

    power->icache_decoder 
	    = ndwl*ndbl*array_decoder_power(rowsb,colsb,predeclength,1,1,cache);
    power->icache_wordline 
	    = ndwl*ndbl*array_wordline_power(rowsb,colsb,wordlinelength,1,1,cache);
    power->icache_bitline 
	    = ndwl*ndbl*array_bitline_power(rowsb,colsb,bitlinelength,1,1,cache);
    power->icache_senseamp 
	    = ndwl*ndbl*senseamp_power(colsb);
    power->icache_tagarray 
	    = ntwl*ntbl*(simple_array_power(trowsb,tcolsb,1,1,cache));

    power->icache_power 
	    = power->icache_decoder 
	    + power->icache_wordline 
	    + power->icache_bitline 
	    + power->icache_senseamp 
	    + power->icache_tagarray;

   
    power->hd_icache_decoder_1ststg 
	    = ndwl*ndbl*array_decoder_power_1ststg(rowsb,colsb,predeclength,1,1,cache);
    power->hd_icache_tag_decoder_1ststg 
	    = ntwl*ntbl*array_decoder_power_1ststg(trowsb,tcolsb,predeclength,1,1,cache);
    power->hd_icache_decoder_2ndstg 
	    = ndwl*ndbl*array_decoder_power_2ndstg(rowsb,colsb,predeclength,1,1,cache);
    power->hd_icache_tag_decoder_2ndstg 
	    = ntwl*ntbl*array_decoder_power_2ndstg(trowsb,tcolsb,predeclength,1,1,cache);
    power->hd_icache_wordline 
	    = ndwl*ndbl*array_wordline_power(rowsb,colsb,dwordlinelength,1,1,cache);
    power->hd_icache_tag_wordline 
	    = ntwl*ntbl*array_wordline_power(trowsb,tcolsb,twordlinelength,1,1,cache);
    power->hd_icache_bitline_rd 
	    = ndwl*ndbl*array_bitline_power_rd(rowsb,colsb,bitlinelength,1,1,cache);
    power->hd_icache_tag_bitline_rd 
	    = ntwl*ntbl*array_bitline_power_rd(trowsb,tcolsb,bitlinelength,1,1,cache);
    power->hd_icache_bitline_wr 
	    = ndwl*ndbl*array_bitline_power_wr(rowsb,colsb,bitlinelength,1,1,cache);
    power->hd_icache_tag_bitline_wr 
	    = ntwl*ntbl*array_bitline_power_wr(trowsb,tcolsb,bitlinelength,1,1,cache);
    power->hd_icache_senseamp 
	    = ndwl*ndbl*senseamp_power(colsb)/inst_width;
    power->hd_icache_tag_senseamp 
	    = ntwl*ntbl*senseamp_power(tcolsb)/(double)tagsize;

    
    power->hd_icache_tagarray 
	    = power->hd_icache_tag_decoder_2ndstg
	    + power->hd_icache_tag_wordline;

    power->hd_icache_power 
	    = power->hd_icache_decoder_2ndstg
	    + power->hd_icache_wordline 
	    + power->hd_icache_tagarray;
  }
  
  /* no icache level 2 configuration */
  if (!mystricmp(cache_il2_opt, "none") || !mystricmp(cache_il2_opt, "dl2"))
  {
    time_parameters.cache_size = 0;
    time_parameters.block_size = 0;
    time_parameters.associativity = 0;
    time_parameters.number_of_sets = 0;

    power->icache2_power = 0.0;
  }
  else
  {
    time_parameters.cache_size = cache_il2->nsets * cache_il2->bsize * cache_il2->assoc; /* C */
    time_parameters.block_size = cache_il2->bsize; /* B */
    time_parameters.associativity = cache_il2->assoc; /* A */
    time_parameters.number_of_sets = cache_il2->nsets; /* C/(B*A) */

    calculate_time(&time_result,&time_parameters);
    output_data(&time_result,&time_parameters);

    ndwl=time_result.best_Ndwl;
    ndbl=time_result.best_Ndbl;
    nspd=time_result.best_Nspd;
    ntwl=time_result.best_Ntwl;
    ntbl=time_result.best_Ntbl;
    ntspd=time_result.best_Ntspd;

    c = time_parameters.cache_size;
    b = time_parameters.block_size;
    a = time_parameters.associativity;

    rowsb = c/(b*a*ndbl*nspd);
    colsb = 8*b*a*nspd/ndwl;

    tagsize = va_size - ((int)logtwo(cache_il2->nsets) + (int)logtwo(cache_il2->bsize));
    trowsb = c/(b*a*ntbl*ntspd);
    tcolsb = a * (tagsize + 1 + 6) * ntspd/ntwl;
 
    if(verbose) {
      fprintf(stderr,"%d KB %d-way cache (%d-byte block size):\n",c,a,b);
      fprintf(stderr,"ndwl == %d, ndbl == %d, nspd == %d\n",ndwl,ndbl,nspd);
      fprintf(stderr,"%d sets of %d rows x %d cols\n",ndwl*ndbl,rowsb,colsb);
      fprintf(stderr,"tagsize == %d\n",tagsize);
    
    if(verbose)
      fprintf(stderr,"icache2 power stats\n");
    power->icache2_decoder 
	    = ndwl*ndbl*array_decoder_power(rowsb,colsb,predeclength,1,1,cache);
    power->icache2_wordline 
	    = ndwl*ndbl*array_wordline_power(rowsb,colsb,wordlinelength,1,1,cache);
    power->icache2_bitline 
	    = ndwl*ndbl*array_bitline_power(rowsb,colsb,bitlinelength,1,1,cache);
    power->icache2_senseamp 
	    = ndwl*ndbl*senseamp_power(colsb);
    power->icache2_tagarray 
	    = ntwl*ntbl*(simple_array_power(trowsb,tcolsb,1,1,cache));

    power->icache2_power 
	    = power->icache2_decoder 
	    + power->icache2_wordline 
	    + power->icache2_bitline 
	    + power->icache2_senseamp 
	    + power->icache2_tagarray;

    power->hd_icache2_decoder_1ststg 
	    = ndwl*ndbl*array_decoder_power_1ststg(rowsb,colsb,predeclength,1,1,cache);
    power->hd_icache2_tag_decoder_1ststg 
	    = ntwl*ntbl*array_decoder_power_1ststg(trowsb,tcolsb,predeclength,1,1,cache);
    power->hd_icache2_decoder_2ndstg 
	    = ndwl*ndbl*array_decoder_power_2ndstg(rowsb,colsb,predeclength,1,1,cache);
    power->hd_icache2_tag_decoder_2ndstg 
	    = ntwl*ntbl*array_decoder_power_2ndstg(trowsb,tcolsb,predeclength,1,1,cache);
    power->hd_icache2_wordline 
	    = ndwl*ndbl*array_wordline_power(rowsb,colsb,dwordlinelength,1,1,cache);
    power->hd_icache2_tag_wordline 
	    = ntwl*ntbl*array_wordline_power(trowsb,tcolsb,twordlinelength,1,1,cache);
    power->hd_icache2_bitline_rd 
	    = ndwl*ndbl*array_bitline_power_rd(rowsb,colsb,bitlinelength,1,1,cache);
    power->hd_icache2_tag_bitline_rd 
	    = ntwl*ntbl*array_bitline_power_rd(trowsb,tcolsb,bitlinelength,1,1,cache);
    power->hd_icache2_bitline_wr 
	    = ndwl*ndbl*array_bitline_power_wr(rowsb,colsb,bitlinelength,1,1,cache);
    power->hd_icache2_tag_bitline_wr 
	    = ntwl*ntbl*array_bitline_power_wr(trowsb,tcolsb,bitlinelength,1,1,cache);
    power->hd_icache2_senseamp 
	    = ndwl*ndbl*senseamp_power(colsb)/inst_width;
    power->hd_icache2_tag_senseamp 
	    = ntwl*ntbl*senseamp_power(tcolsb)/(double)tagsize;

    
    power->hd_icache2_tagarray 
	    = power->hd_icache2_tag_decoder_2ndstg
	    + power->hd_icache2_tag_wordline;

    power->hd_icache2_power 
	    = power->hd_icache2_decoder_2ndstg
	    + power->hd_icache2_wordline 
	    + power->hd_icache2_tagarray;
    }

    predeclength 
	    = rowsb * (RegCellHeight + WordlineSpacing);
    wordlinelength 
	    = colsb * (RegCellWidth + BitlineSpacing);
    bitlinelength 
	    = rowsb * (RegCellHeight + WordlineSpacing);

  }

  /* no dcache level 1 configuration */
  if (!mystricmp(cache_dl1_opt, "none"))
  {
    time_parameters.cache_size 	   = 0;
    time_parameters.block_size     = 0;
    time_parameters.associativity  = 0;
    time_parameters.number_of_sets = 0;

    power->dcache_power = 0.0;
  }
  else
  {
    time_parameters.cache_size     = cache_dl1->nsets * cache_dl1->bsize * cache_dl1->assoc; /* C */
    time_parameters.block_size     = cache_dl1->bsize; /* B */
    time_parameters.associativity  = cache_dl1->assoc; /* A */
    time_parameters.number_of_sets = cache_dl1->nsets; /* C/(B*A) */

    calculate_time(&time_result,&time_parameters);
    output_data(&time_result,&time_parameters);

    ndwl  = time_result.best_Ndwl;
    ndbl  = time_result.best_Ndbl;
    nspd  = time_result.best_Nspd;
    ntwl  = time_result.best_Ntwl;
    ntbl  = time_result.best_Ntbl;
    ntspd = time_result.best_Ntspd;
    
    c = time_parameters.cache_size;
    b = time_parameters.block_size;
    a = time_parameters.associativity; 

    cache=1;

    rowsb = c/(b*a*ndbl*nspd);
    colsb = 8*b*a*nspd/ndwl;

    tagsize = va_size - ((int)logtwo(cache_dl1->nsets) + (int)logtwo(cache_dl1->bsize));
    trowsb  = c/(b*a*ntbl*ntspd);
    tcolsb  = a * (tagsize + 1 + 6) * ntspd/ntwl;

    if(verbose) {
      fprintf(stderr,"%d KB %d-way cache (%d-byte block size):\n",c,a,b);
      fprintf(stderr,"ndwl == %d, ndbl == %d, nspd == %d\n",ndwl,ndbl,nspd);
      fprintf(stderr,"%d sets of %d rows x %d cols\n",ndwl*ndbl,rowsb,colsb);
      fprintf(stderr,"tagsize == %d\n",tagsize);

      fprintf(stderr,"\nntwl == %d, ntbl == %d, ntspd == %d\n",ntwl,ntbl,ntspd);
      fprintf(stderr,"%d sets of %d rows x %d cols\n",ntwl*ntbl,trowsb,tcolsb);
    }

    predeclength   = rowsb * (RegCellHeight + WordlineSpacing);
    wordlinelength = colsb * (RegCellWidth + BitlineSpacing);
    bitlinelength  = rowsb * (RegCellHeight + WordlineSpacing);

    if(verbose)
      fprintf(stderr,"dcache power stats\n");
    power->dcache_decoder 
	    = res_memport*ndwl*ndbl*array_decoder_power(rowsb,colsb,predeclength,1,1,cache);
    power->dcache_wordline 
	    = res_memport*ndwl*ndbl*array_wordline_power(rowsb,colsb,wordlinelength,1,1,cache);
    power->dcache_bitline 
	    = res_memport*ndwl*ndbl*array_bitline_power(rowsb,colsb,bitlinelength,1,1,cache);
    power->dcache_senseamp 
	    = res_memport*ndwl*ndbl*senseamp_power(colsb);
    power->dcache_tagarray 
	    = res_memport*ntwl*ntbl*(simple_array_power(trowsb,tcolsb,1,1,cache));

    power->dcache_power 
	    = power->dcache_decoder 
	    + power->dcache_wordline 
	    + power->dcache_bitline 
	    + power->dcache_senseamp 
	    + power->dcache_tagarray;

    power->hd_dcache_decoder_1ststg
        = ndwl*ndbl*array_decoder_power_1ststg(rowsb,colsb,predeclength,1,1,cache);
    power->hd_dcache_tag_decoder_1ststg
        = ntwl*ntbl*array_decoder_power_1ststg(trowsb,tcolsb,predeclength,1,1,cache);
    power->hd_dcache_decoder_2ndstg
        = ndwl*ndbl*array_decoder_power_2ndstg(rowsb,colsb,predeclength,1,1,cache);
    power->hd_dcache_tag_decoder_2ndstg
        = ntwl*ntbl*array_decoder_power_2ndstg(trowsb,tcolsb,predeclength,1,1,cache);
    power->hd_dcache_wordline
        = ndwl*ndbl*array_wordline_power(rowsb,colsb,dwordlinelength,1,1,cache);
    power->hd_dcache_tag_wordline
        = ntwl*ntbl*array_wordline_power(trowsb,tcolsb,twordlinelength,1,1,cache);
    power->hd_dcache_bitline_rd
        = ndwl*ndbl*array_bitline_power_rd(rowsb,colsb,bitlinelength,1,1,cache);
    power->hd_dcache_tag_bitline_rd
        = ntwl*ntbl*array_bitline_power_rd(trowsb,tcolsb,bitlinelength,1,1,cache);
    power->hd_dcache_bitline_wr
        = ndwl*ndbl*array_bitline_power_wr(rowsb,colsb,bitlinelength,1,1,cache);
    power->hd_dcache_tag_bitline_wr
        = ntwl*ntbl*array_bitline_power_wr(trowsb,tcolsb,bitlinelength,1,1,cache);
    power->hd_dcache_senseamp
        = ndwl*ndbl*senseamp_power(colsb)/data_width;
    power->hd_dcache_tag_senseamp
	= ntwl*ntbl*senseamp_power(tcolsb)/(double)tagsize;

    /* Total consumed power by dcache */ 
    power->hd_dcache_tagarray
        = power->hd_dcache_tag_decoder_2ndstg
        + power->hd_dcache_tag_wordline;

    power->hd_dcache_power
        = power->hd_dcache_decoder_2ndstg
        + power->hd_dcache_wordline
        + power->hd_dcache_tagarray;
  }


  clockpower = total_clockpower(.018);
  power->clock_power = clockpower;
  if(verbose) {
    fprintf(stderr,"result bus power == %f\n",power->resultbus);
    fprintf(stderr,"global clock power == %f\n",clockpower);
  }

  /* no dcache level 2 configuration */
  if (!mystricmp(cache_dl2_opt, "none"))
  {
    time_parameters.cache_size     = 0;
    time_parameters.block_size     = 0;
    time_parameters.associativity  = 0;
    time_parameters.number_of_sets = 0;
    
    power->dcache2_power = 0.0; 
  }
  else
  {
    time_parameters.cache_size     = cache_dl2->nsets * cache_dl2->bsize * cache_dl2->assoc; /* C */
    time_parameters.block_size     = cache_dl2->bsize; /* B */
    time_parameters.associativity  = cache_dl2->assoc; /* A */
    time_parameters.number_of_sets = cache_dl2->nsets; /* C/(B*A) */

    calculate_time(&time_result,&time_parameters);
    output_data(&time_result,&time_parameters);

    ndwl  = time_result.best_Ndwl;
    ndbl  = time_result.best_Ndbl;
    nspd  = time_result.best_Nspd;
    ntwl  = time_result.best_Ntwl;
    ntbl  = time_result.best_Ntbl;
    ntspd = time_result.best_Ntspd;

    c = time_parameters.cache_size;
    b = time_parameters.block_size;
    a = time_parameters.associativity;

    rowsb = c/(b*a*ndbl*nspd);
    colsb = 8*b*a*nspd/ndwl;

    tagsize = va_size - ((int)logtwo(cache_dl2->nsets) + (int)logtwo(cache_dl2->bsize));
    trowsb  = c/(b*a*ntbl*ntspd);
    tcolsb  = a * (tagsize + 1 + 6) * ntspd/ntwl;

    if(verbose) {
      fprintf(stderr,"%d KB %d-way cache (%d-byte block size):\n",c,a,b);
      fprintf(stderr,"ndwl == %d, ndbl == %d, nspd == %d\n",ndwl,ndbl,nspd);
      fprintf(stderr,"%d sets of %d rows x %d cols\n",ndwl*ndbl,rowsb,colsb);
      fprintf(stderr,"tagsize == %d\n",tagsize);
    }

    predeclength   = rowsb * (RegCellHeight + WordlineSpacing);
    wordlinelength = colsb *  (RegCellWidth + BitlineSpacing);
    bitlinelength  = rowsb * (RegCellHeight + WordlineSpacing);

    if(verbose)
      fprintf(stderr,"dcache2 power stats\n");
    power->dcache2_decoder 
	    = res_memport*ndwl*ndbl*array_decoder_power(rowsb,colsb,predeclength,1,1,cache);
    power->dcache2_wordline 
	    = res_memport*ndwl*ndbl*array_wordline_power(rowsb,colsb,wordlinelength,1,1,cache);
    power->dcache2_bitline 
	    = res_memport*ndwl*ndbl*array_bitline_power(rowsb,colsb,bitlinelength,1,1,cache);
    power->dcache2_senseamp 
	    = res_memport*ndwl*ndbl*senseamp_power(colsb);
    power->dcache2_tagarray 
	    = res_memport*ntwl*ntbl*(simple_array_power(trowsb,tcolsb,1,1,cache));

    power->dcache2_power 
	    = power->dcache2_decoder 
	    + power->dcache2_wordline 
	    + power->dcache2_bitline 
	    + power->dcache2_senseamp 
	    + power->dcache2_tagarray;

    power->hd_dcache2_decoder_1ststg
        = ndwl*ndbl*array_decoder_power_1ststg(rowsb,colsb,predeclength,1,1,cache);
    power->hd_dcache2_tag_decoder_1ststg
        = ntwl*ntbl*array_decoder_power_1ststg(trowsb,tcolsb,predeclength,1,1,cache);
    power->hd_dcache2_decoder_2ndstg
        = ndwl*ndbl*array_decoder_power_2ndstg(rowsb,colsb,predeclength,1,1,cache);
    power->hd_dcache2_tag_decoder_2ndstg
        = ntwl*ntbl*array_decoder_power_2ndstg(trowsb,tcolsb,predeclength,1,1,cache);
    power->hd_dcache2_wordline
        = ndwl*ndbl*array_wordline_power(rowsb,colsb,dwordlinelength,1,1,cache);
    power->hd_dcache2_tag_wordline
        = ntwl*ntbl*array_wordline_power(trowsb,tcolsb,twordlinelength,1,1,cache);
    power->hd_dcache2_bitline_rd
        = ndwl*ndbl*array_bitline_power_rd(rowsb,colsb,bitlinelength,1,1,cache);
    power->hd_dcache2_tag_bitline_rd
        = ntwl*ntbl*array_bitline_power_rd(trowsb,tcolsb,bitlinelength,1,1,cache);
    power->hd_dcache2_bitline_wr
        = ndwl*ndbl*array_bitline_power_wr(rowsb,colsb,bitlinelength,1,1,cache);
    power->hd_dcache2_tag_bitline_wr
        = ntwl*ntbl*array_bitline_power_wr(trowsb,tcolsb,bitlinelength,1,1,cache);
    power->hd_dcache2_senseamp
        = ndwl*ndbl*senseamp_power(colsb)/data_width;
    power->hd_dcache2_tag_senseamp
	= ntwl*ntbl*senseamp_power(tcolsb)/(double)tagsize;

    /* Total consumed power by dcache2 */ 
    power->hd_dcache2_tagarray
        = power->hd_dcache2_tag_decoder_2ndstg
        + power->hd_dcache2_tag_wordline;

    power->hd_dcache2_power
        = power->hd_dcache2_decoder_2ndstg
        + power->hd_dcache2_wordline
        + power->hd_dcache2_tagarray;
  }
	
  power->rat_decoder  
	  *= crossover_scaling;
  power->rat_wordline 
	  *= crossover_scaling;
  power->rat_bitline  
	  *= crossover_scaling;

  power->dcl_compare  		*= crossover_scaling;
  power->dcl_pencode  		*= crossover_scaling;
  power->inst_decoder_power	*= crossover_scaling;
  power->wakeup_tagdrive 	*= crossover_scaling;
  power->wakeup_tagmatch 	*= crossover_scaling;
  power->wakeup_ormatch 	*= crossover_scaling;
  power->selection 		*= crossover_scaling;

  power->regfile_decoder 	*= crossover_scaling;
  power->regfile_wordline 	*= crossover_scaling;
  power->regfile_bitline 	*= crossover_scaling;
  power->regfile_senseamp 	*= crossover_scaling;

  power->rs_decoder 		*= crossover_scaling;
  power->rs_wordline 		*= crossover_scaling;
  power->rs_bitline 		*= crossover_scaling;
  power->rs_senseamp 		*= crossover_scaling;

  power->lsq_wakeup_tagdrive 	*= crossover_scaling;
  power->lsq_wakeup_tagmatch 	*= crossover_scaling;

  power->lsq_rs_decoder 	*= crossover_scaling;
  power->lsq_rs_wordline 	*= crossover_scaling;
  power->lsq_rs_bitline 	*= crossover_scaling;
  power->lsq_rs_senseamp 	*= crossover_scaling;
 
  power->resultbus 		*= crossover_scaling;

  power->btb 			*= crossover_scaling;
  power->hd_btb 		*= crossover_scaling;
  power->local_predict 		*= crossover_scaling;
  power->global_predict 	*= crossover_scaling;
  power->chooser 		*= crossover_scaling;

  power->dtlb 			*= crossover_scaling;
  power->itlb 			*= crossover_scaling;

  power->icache_decoder 	*= crossover_scaling;
  power->icache_wordline	*= crossover_scaling;
  power->icache_bitline 	*= crossover_scaling;
  power->icache_senseamp	*= crossover_scaling;
  power->icache_tagarray	*= crossover_scaling;

  power->hd_icache_decoder_1ststg 	*= crossover_scaling;
  power->hd_icache_decoder_2ndstg 	*= crossover_scaling;
  power->hd_icache_wordline	*= crossover_scaling;
  power->hd_icache_bitline_rd 	*= crossover_scaling;
  power->hd_icache_bitline_wr 	*= crossover_scaling;
  power->hd_icache_senseamp	*= crossover_scaling;
  power->hd_icache_tagarray	*= crossover_scaling;

  power->icache_power 		*= crossover_scaling;

  power->icache2_decoder 	*= crossover_scaling;
  power->icache2_wordline	*= crossover_scaling;
  power->icache2_bitline 	*= crossover_scaling;
  power->icache2_senseamp	*= crossover_scaling;
  power->icache2_tagarray	*= crossover_scaling;

  power->hd_icache2_decoder_1ststg 	*= crossover_scaling;
  power->hd_icache2_decoder_2ndstg 	*= crossover_scaling;
  power->hd_icache2_wordline	*= crossover_scaling;
  power->hd_icache2_bitline_rd 	*= crossover_scaling;
  power->hd_icache2_bitline_wr 	*= crossover_scaling;
  power->hd_icache2_senseamp	*= crossover_scaling;
  power->hd_icache2_tagarray	*= crossover_scaling;

  power->icache2_power 		*= crossover_scaling;

  power->dcache_decoder 	*= crossover_scaling;
  power->dcache_wordline	*= crossover_scaling;
  power->dcache_bitline 	*= crossover_scaling;
  power->dcache_senseamp	*= crossover_scaling;
  power->dcache_tagarray	*= crossover_scaling;

  power->hd_dcache_decoder_1ststg 	*= crossover_scaling;
  power->hd_dcache_decoder_2ndstg 	*= crossover_scaling;
  power->hd_dcache_wordline 	*= crossover_scaling;
  power->hd_dcache_bitline_rd 	*= crossover_scaling;
  power->hd_dcache_bitline_wr 	*= crossover_scaling;
  power->hd_dcache_senseamp 	*= crossover_scaling;
  power->hd_dcache_tagarray 	*= crossover_scaling;

  power->dcache_power 		*= crossover_scaling;
  
  power->dcache2_decoder 	*= crossover_scaling;
  power->dcache2_wordline	*= crossover_scaling;
  power->dcache2_bitline 	*= crossover_scaling;
  power->dcache2_senseamp	*= crossover_scaling;
  power->dcache2_tagarray	*= crossover_scaling;

  power->hd_dcache2_decoder_1ststg 	*= crossover_scaling;
  power->hd_dcache2_decoder_2ndstg 	*= crossover_scaling;
  power->hd_dcache2_wordline 	*= crossover_scaling;
  power->hd_dcache2_bitline_rd 	*= crossover_scaling;
  power->hd_dcache2_bitline_wr 	*= crossover_scaling;
  power->hd_dcache2_senseamp 	*= crossover_scaling;
  power->hd_dcache2_tagarray 	*= crossover_scaling;

  power->dcache2_power 		*= crossover_scaling;
  
  power->clock_power 		*= crossover_scaling;

  power->total_power 
	  = power->local_predict 
	  + power->global_predict 
	  + power->chooser 
	  + power->btb 
	  + power->rat_decoder 
	  + power->rat_wordline 
	  + power->rat_bitline 
	  + power->rat_senseamp 
	  + power->dcl_compare 
	  + power->dcl_pencode 
	  + power->inst_decoder_power 
	  + power->wakeup_tagdrive 
	  + power->wakeup_tagmatch 
	  + power->selection 
	  + power->regfile_decoder 
	  + power->regfile_wordline 
	  + power->regfile_bitline 
	  + power->regfile_senseamp 
	  + power->rs_decoder 
	  + power->rs_wordline 
	  + power->rs_bitline 
	  + power->rs_senseamp 
	  + power->lsq_wakeup_tagdrive 
	  + power->lsq_wakeup_tagmatch 
	  + power->lsq_rs_decoder 
	  + power->lsq_rs_wordline 
	  + power->lsq_rs_bitline 
	  + power->lsq_rs_senseamp 
	  + power->resultbus 
	  + power->clock_power 
	  + power->icache_power 
	  + power->icache2_power 
	  + power->itlb 
	  + power->dcache_power 
	  + power->dtlb 
	  + power->dcache2_power;

  power->total_power_nodcache2 
	  = power->local_predict 
	  + power->global_predict 
	  + power->chooser 
	  + power->btb 
	  + power->rat_decoder 
	  + power->rat_wordline 
	  + power->rat_bitline 
	  + power->rat_senseamp 
	  + power->dcl_compare 
	  + power->dcl_pencode 
	  + power->inst_decoder_power 
	  + power->wakeup_tagdrive 
	  + power->wakeup_tagmatch 
	  + power->selection 
	  + power->regfile_decoder 
	  + power->regfile_wordline 
	  + power->regfile_bitline 
	  + power->regfile_senseamp 
	  + power->rs_decoder 
	  + power->rs_wordline 
	  + power->rs_bitline 
	  + power->rs_senseamp 
	  + power->lsq_wakeup_tagdrive 
	  + power->lsq_wakeup_tagmatch 
	  + power->lsq_rs_decoder 
	  + power->lsq_rs_wordline 
	  + power->lsq_rs_bitline 
	  + power->lsq_rs_senseamp 
	  + power->resultbus 
	  + power->clock_power 
	  + power->icache_power 
	  + power->itlb 
	  + power->dcache_power 
	  + power->dtlb; 

  power->bpred_power 
	  = power->btb
	  + power->local_predict 
	  + power->global_predict 
	  + power->chooser 
	  + power->ras;

  power->hd_bpred_power 
	  = power->hd_btb
	  + power->local_predict 
	  + power->global_predict 
	  + power->chooser 
	  + power->ras;

  power->rat_power 
	  = power->rat_decoder 
	  + power->rat_wordline 
	  + power->rat_bitline 
	  + power->rat_senseamp; 

  power->dcl_power 
	  = power->dcl_compare 
	  + power->dcl_pencode;

  power->rename_power 
	  = power->rat_power 
	  + power->dcl_power 
	  + power->inst_decoder_power;

  power->wakeup_power 
	  = power->wakeup_tagdrive 
	  + power->wakeup_tagmatch 
	  + power->wakeup_ormatch;

  power->rs_power 
	  = power->rs_decoder 
	  + power->rs_wordline 
	  + power->rs_bitline 
	  + power->rs_senseamp; 
  power->rs_power_nobit 
	  = power->rs_decoder 
	  + power->rs_wordline 
	  + power->rs_senseamp;

  power->window_power 
	  = power->wakeup_power 
	  + power->rs_power 
	  + power->selection;

  power->lsq_rs_power 
	  = power->lsq_rs_decoder 
	  + power->lsq_rs_wordline 
	  + power->lsq_rs_bitline 
	  + power->lsq_rs_senseamp; 

  power->lsq_rs_power_nobit 
	  = power->lsq_rs_decoder 
	  + power->lsq_rs_wordline 
	  + power->lsq_rs_senseamp;
   
  power->lsq_wakeup_power 
	  = power->lsq_wakeup_tagdrive 
	  + power->lsq_wakeup_tagmatch;

  power->lsq_power 
	  = power->lsq_wakeup_power 
	  + power->lsq_rs_power;

  power->regfile_power 
	  = power->regfile_decoder 
	  + power->regfile_wordline 
	  + power->regfile_bitline 
	  + power->regfile_senseamp;

  /* no bit-line case power */
  power->regfile_power_nobit 
	  = power->regfile_decoder 
	  + power->regfile_wordline 
	  + power->regfile_senseamp; 

  /* dump_power_stats(power); */

}
    
/*
    tot_icache_decaddr_hd += icache_decaddr_hd;
    tot_icache_tagaddr_hd += icache_tagaddr_hd;
    tot_icache_inst_hd += icache_inst_hd;
*/
/*
printf("ddecaddr_hd:%x\t", dcache_decaddr_hd);
printf("dcache_tagaddr_hd:%x\t", dcache_tagaddr_hd);
printf("dcache_data_hd:%x\n", dcache_data_hd);
*/

/*
    tot_dcache_decaddr_hd += dcache_decaddr_hd;
    tot_dcache_tagaddr_hd += dcache_tagaddr_hd;
    tot_dcache_data_hd += dcache_data_hd;
*/   
    
/*
    printf("tot_ddecaddr_hd:%x\t", tot_dcache_decaddr_hd);
    printf("tot_dcache_tagaddr_hd:%x\t", tot_dcache_tagaddr_hd);
    printf("tot_dcache_data_hd:%x\n", tot_dcache_data_hd);
*/


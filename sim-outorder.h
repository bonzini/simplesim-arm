/*
 * sim-outorder.h - sample out-of-order issue perf simulator implementation
 *
 * This file is a part of the SimpleScalar tool suite written by
 * Todd M. Austin as a part of the Multiscalar Research Project.
 *  
 * The tool suite is currently maintained by Doug Burger and Todd M. Austin.
 * 
 * Copyright (C) 1994, 1995, 1996, 1997, 1998 by Todd M. Austin
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
 *
 * 
 * This file holds variables shared between sim-outorder.c and other
 * modular files (such as power.c)
 *
 *
 */


#include "machine.h"
#include "cache.h"

#define ADWATTCH 

extern int ruu_decode_width;
extern int ruu_issue_width;
extern int ruu_commit_width;
extern int RUU_size;
extern int LSQ_size;
extern int data_width;
extern int res_ialu;
extern int res_fpalu;
extern int res_memport;

extern int bimod_config[];

extern struct cache_t *cache_dl1;
extern struct cache_t *cache_il1;
extern struct cache_t *cache_dl2;
extern struct cache_t *cache_il2;

extern struct cache_t *dtlb;
extern struct cache_t *itlb;

extern char *cache_dl1_opt;
extern char *cache_dl2_opt;
extern char *cache_il1_opt;
extern char *cache_il2_opt;
extern char *itlb_opt;
extern char *dtlb_opt;

/* 2-level predictor config (<l1size> <l2size> <hist_size> <xor>) */
extern int twolev_config[];

/* combining predictor config (<meta_table_size> */
extern int comb_config[];

/* return address stack (RAS) size */
extern int ras_size;

/* BTB predictor config (<num_sets> <associativity>) */
extern int btb_config[];

extern counter_t rename_access;
extern counter_t bpred_access;
extern counter_t window_access;
extern counter_t lsq_access;
extern counter_t regfile_access;
extern counter_t icache_access;
extern counter_t dcache_access;
extern counter_t dcache2_access;

extern counter_t itlb_access;
extern counter_t dtlb_access;

extern counter_t alu_access;
extern counter_t ialu_access;
extern counter_t falu_access;
extern counter_t resultbus_access;

extern counter_t window_selection_access;
extern counter_t window_wakeup_access;
extern counter_t window_preg_access;
extern counter_t lsq_preg_access;
extern counter_t lsq_wakeup_access;
extern counter_t lsq_store_data_access;
extern counter_t lsq_load_data_access;

extern counter_t window_total_pop_count_cycle;
extern counter_t window_num_pop_count_cycle;
extern counter_t lsq_total_pop_count_cycle;
extern counter_t lsq_num_pop_count_cycle;
extern counter_t regfile_total_pop_count_cycle;
extern counter_t regfile_num_pop_count_cycle;
extern counter_t resultbus_total_pop_count_cycle;
extern counter_t resultbus_num_pop_count_cycle;

#ifdef ADWATTCH
/*
extern qword_t    icache_decaddr_hd;
extern qword_t    icache_tagaddr_hd;
extern qword_t    icache_inst_hd;
extern qword_t    tot_icache_decaddr_hd;
extern qword_t    tot_icache_tagaddr_hd;
extern qword_t    tot_icache_inst_hd;
extern qword_t    icache2_decaddr_hd;
extern qword_t    icache2_tagaddr_hd;
extern qword_t    icache2_inst_hd;
extern qword_t    dcache_decaddr_hd;
extern qword_t    dcache_tagaddr_hd;
extern qword_t    dcache_data_hd;
extern qword_t    tot_dcache_decaddr_hd;
extern qword_t    tot_dcache_tagaddr_hd;
extern qword_t    tot_dcache_data_hd;
extern qword_t    dcache2_decaddr_hd;
extern qword_t    dcache2_tagaddr_hd;
extern qword_t    dcache2_data_hd;
*/
qword_t get_hd_decaddr(struct cache_t *cache, md_addr_t prev_addr, md_addr_t curr_addr);
qword_t get_hd_tagaddr(struct cache_t *cache, md_addr_t prev_addr, md_addr_t curr_addr);
qword_t get_hd_btb_decaddr( int btb_config[], md_addr_t prev_addr, md_addr_t curr_addr);
qword_t get_hd_btb_tagaddr( int btb_config[], md_addr_t prev_addr, md_addr_t curr_addr);

qword_t get_hd_inst(md_inst_t prev_inst, md_inst_t curr_inst);
qword_t get_hd_data(word_t prev_data, word_t curr_data);
qword_t get_hd_btaddr(md_addr_t prev_btaddr, md_addr_t curr_btaddr);

void icache_power_hd(md_addr_t icache_access_addr, md_inst_t icache_inst);
void dcache_power_hd(md_addr_t dcache_access_addr, enum mem_cmd cmd );
void btb_power_hd(md_addr_t curr_btb_addr, md_addr_t curr_btaddr, enum mem_cmd cmd);
void icache2_power_hd(void);
void dcache2_power_hd(void);
#endif

#define WATTCH

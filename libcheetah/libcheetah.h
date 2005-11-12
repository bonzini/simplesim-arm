
#ifndef LIBCHEETAH_H
#define LIBCHEETAH_H

#include <stdio.h>

/* initialize libcheetah */
void
cheetah_init(int argc, char **argv);	/* cheetah arguments */

/* output the analysis configuration */
void
cheetah_config(FILE *fd);		/* output stream */

/* pass a memory address to libcheetah */
void
cheetah_access(md_addr_t addr);		/* address of access */

/* output the trace statistics */
void
cheetah_stats(FILE *fd,			/* output stream */
	      int mid);			/* intermediate stats? */

/* set-associative statistics
     - ok to access after calling cheetah_stats()
     - arranged as:
	 sac_hits[set size: min-sets..max-sets][assoc: min-depth..max-depth]
*/
extern unsigned **sac_hits;

/* internal interfaces */
void init_saclru(void);
void sacnmul_woarr(md_addr_t addr);
void outpr_saclru(FILE *fd);

void init_sacopt(void);
int
stack_proc_sa(int start,		/* index of starting array location */
	      int end);			/* index of ending array location */
void inf_handler_sa(md_addr_t addr, int cur_time);
void outpr_sacopt(FILE *fd);

void init_dmvl(void);
void dmvl(md_addr_t addr);
void outpr_dmvl(FILE *fd);

void init_faclru(void);
void ptc(md_addr_t addr);
void outpr_faclru(FILE *fd);

void init_facopt(void);
int
stack_proc_fa(int start,		/* index of starting array location */
	      int end);			/* index of ending array location */
void inf_handler_fa(md_addr_t addr, int cur_time);
void outpr_facopt(FILE *fd);

void init_optpp(void);
void optpp(md_addr_t addr, int L,
	   int (*stack_proc)(int start, int end),
	   void (*inf_handler)(md_addr_t addr, int cur_time));
void ft_hash_del(md_addr_t addr);
void term_optpp(int (*stack_proc)());

#endif /* LIBCHEETAH_H */

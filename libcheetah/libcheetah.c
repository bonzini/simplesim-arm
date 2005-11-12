/************************************************************************
* Copyright (C) 1989, 1990, 1991, 1992, 1993                    	*
*                   Rabin A. Sugumar and Santosh G. Abraham		*
*									*
* This software is distributed absolutely without warranty. You are	*
* free to use and modify the software as you wish.  You are also free	*
* to distribute the software as long as it is not for commercial gain,	*
* you retain the above copyright notice, and you make clear what your	*
* modifications were.							*
*									*
* Send comments and bug reports to rabin@eecs.umich.edu			*
*									*
************************************************************************/

/* Processes command line arguments and calls routines as appropriate. */

#include <stdio.h>
#include <stdlib.h>

#include "../host.h"
#include "../misc.h"
#include "../machine.h"
#include "util.h"
#include "libcheetah.h"

#define NOT !
#define ONE 1U
#define MAX_PHYSICAL_MEM 8388608

#define LRU 0
#define OPT 1
#define SA 2
#define FA 3
#define DM 4
#define BASIC 5
#define PIXIE 6
#define DIN 7

#define BYTES_PER_LINE 35  /* Storage requirement per line */
  
int N;		/* Degree of associativity = 2^N */
int B;		/* Max set field width */
int A;		/* Min set field width */
int L;		/* Line field width */
int C;          /* Cache size for variable line size Dm simulation */
int T;		/* Max no of addresses to be processed */
int P_INTERVAL; /* Intervals at which progress output is done */

unsigned MAX_CACHE_SIZE;	/* Maximum cache size of interest */
unsigned MAX_LINES;	/* Calculated from max cache size and line size */
int MISS_RATIO_INTERVAL;	/* Output interval; fully-associative cache */
int SAVE_INTERVAL;     /* Intervals at which output is saved */


char *outstrings[] = {  "LRU",
			"OPT",
			"Set-associative",
			"Fully-associative",
			"Direct-mapped",
			"Basic",
			"Pixie",
			"DIN",
			"an instruction",
			"a load",
			" ",
			"a store",
			" ",
			"a data",
			"a unified"
		     };


/**************************************************************
Main routine. Reads in command line parameters and set variable values.

Input: None
Output: None
Side effects: Allocates space for arrays and sets parameter values.
**************************************************************/

/* cache configuration */
int trace_config;

/* replacement strategy */
short repl;

/* non-zero after cheetah_init() has been called */
static int initialized = 0;

/* initialize libcheetah */
void            
cheetah_init(int argc, char **argv)	/* cheetah arguments */
{
  short i;

  if (initialized)
    {
      fprintf(stderr, "libcheetah: already initialized\n");
      exit(1);
    }
  initialized = 1;

  /* Default Settings */
  A = 7;
  B = 14;
  L = 4;
  N = 1;
  C = 16;
  T = 0x7fffffff;
  MAX_CACHE_SIZE = 524288;
  MISS_RATIO_INTERVAL = 512;
  P_INTERVAL = 0x7fffffff;
  SAVE_INTERVAL = 0x7fffffff;
  trace_config = SA;
  repl = LRU;

  /* Command line settings */
  for (i=0; i < argc; i++)
    {
      if (argv[i][0] != '-')
	{
	  fprintf(stderr, "libcheetah: illegal argument `%s'\n", argv[i]);
	  exit(1);
	}
      else
	{
	  switch(argv[i][1])
	    {
	    /* -R<repl> replacement policy */
	    case 'R':
	      if ((NOT strcmp ("lru", &argv[i][2]))
		  || (NOT strcmp ("LRU", &argv[i][2])))
		repl = LRU;
	      else if ((NOT strcmp ("opt", &argv[i][2]))
		       || (NOT strcmp ("OPT", &argv[i][2])))
		{
		  repl = OPT;
		}
	      else
		{
		  fprintf (stderr,
			   "libcheetah: replacement policy `%s' "
			   "not supported\n",
			   &argv[i][2]);
		  exit (1);
		}
	      break;

	    /* -C<config> cache configuration */
	    case 'C':
	      if ((NOT strcmp ("fa", &argv[i][2]))
		  || (NOT strcmp ("FA", &argv[i][2])))
		trace_config = FA;
	      else if ((NOT strcmp ("sa", &argv[i][2]))
		       || (NOT strcmp ("SA", &argv[i][2])))
		trace_config = SA;
	      else if ((NOT strcmp ("dm", &argv[i][2]))
		       || (NOT strcmp ("DM", &argv[i][2])))
		trace_config = DM;
	      else
		{
		  fprintf (stderr,
			   "libcheetah: configuration `%s' not supported\n",
			   &argv[i][2]);
		  exit (1);
		}
	      break;

	    /* -a<num> minimum number of sets */
	    case 'a':
	      A = atoi(&argv[i][2]);
	      break;

	    /* -b<num> maximum number of sets */
	    case 'b':
	      B = atoi(&argv[i][2]);
	      break;

	    /* -l<num> log of the line size of the caches */
	    case 'l':
	      L = atoi(&argv[i][2]);
	      break;

	    /* -n<num> log of the maximum degree of associativity */
	    case 'n':
	      N = atoi(&argv[i][2]);
	      break;

	    /* -i<num> cache size intervals at which miss ratio is desired */
	    case 'i':
	      MISS_RATIO_INTERVAL = atoi(&argv[i][2]);
	      break;

	    /* -M<num> maximum cache size of interest */
	    case 'M':
	      MAX_CACHE_SIZE = atoi(&argv[i][2]);
	      break;

	    case 'c':
	      C = atoi(&argv[i][2]);
	      break;

#if 0 /* unneeded */
	    case 't':
	      T = atoi(&argv[i][2]);
	      break;
#endif

#if 0 /* unneeded */
	    case 's':
	      SAVE_INTERVAL = atoi(&argv[i][2]);
	      break;
#endif

#if 0 /* unneeded */
	    case 'p':
	      P_INTERVAL = atoi(&argv[i][2]);
	      break;
#endif

	    default:
	      fprintf(stderr, "libcheetah: `-%c' is not a valid option\n",
		      argv[i][1]);
	    }
	}
    }

  /* initialize modules */
  switch (trace_config)
    {
    case SA:
      if (A > B)
	{
	  fprintf(stderr, "libcheetah: min number of sets greater than max\n");
	  exit (1);
	}
    
      if (repl == LRU)
	init_saclru();
      else if (repl == OPT)
	{
	  init_sacopt();
	  init_optpp();
	}
      break;
    
    case FA:
      if ((int)(ONE << L) > MISS_RATIO_INTERVAL)
	{
	  fprintf(stderr, "libcheetah: line size > output interval!!\n");
	  fprintf(stderr,
		  "libcheetah: output interval changed to line size\n");
	  MISS_RATIO_INTERVAL = 1;
	}
      else
	MISS_RATIO_INTERVAL = MISS_RATIO_INTERVAL/(ONE << L);
    
      if (MAX_CACHE_SIZE < (ONE << L))
	{
	  fprintf(stderr,
		  "libcheetah: max cache size is less than line size\n");
	  exit (1);
	}
      else
	{
	  MAX_LINES = MAX_CACHE_SIZE / (ONE << L);
	  if ((MAX_LINES*BYTES_PER_LINE) > MAX_PHYSICAL_MEM)
	    fprintf(stderr,
		    "libcheetah: warning: mem limit may be exceeded\n");
	}
    
      if (repl == LRU)
	init_faclru();
      else if (repl == OPT)
	{
	  init_facopt();
	  init_optpp();
	}
      break;

    case DM:
      if (A > B)
	{
	  fprintf (stderr,
		   "libcheetah: min line size greater than max line\n");
	  exit (1);
	}
      if (B > C)
	{
	  fprintf (stderr,
		   "libcheetah: max line size greater than cache size\n");
	  exit (1);
	}
    
      init_dmvl();
      break;
    
    default:
      fprintf (stderr, "libcheetah: configuration wrongly set\n");
      exit (1);
    }
}

/* output the analysis configuration */
void
cheetah_config(FILE *fd)		/* output stream */
{
  fprintf(fd, "\nlibcheetah: ** simulation parameters **\n");
  if (trace_config != DM)
    fprintf(fd, "libcheetah: %s %s caches being simulated\n",
	    outstrings[repl], outstrings[trace_config]);
  else
    fprintf(fd, "libcheetah: %s caches being simulated\n",
	    outstrings[trace_config]);

  switch (trace_config)
    {
    case SA:
      fprintf(fd, "libcheetah: number of sets from %d to %d\n",
	      (ONE << A), (ONE << B));
      fprintf(fd, "libcheetah: maximum associativity is %d\n", (ONE << N));
      fprintf(fd, "libcheetah: line size is %d bytes\n", (ONE << L));
      break;
    
    case FA:
      fprintf(fd, "libcheetah: max cache size is %d bytes\n", MAX_CACHE_SIZE);
      fprintf(fd, "libcheetah: line size is %d bytes\n", (ONE << L));
      break;

    case DM:
      fprintf(fd, "libcheetah: cache size is %d bytes\n", (ONE << C));
      fprintf(fd, "libcheetah: line sizes from %d to %d bytes\n",
	      (ONE << A), (ONE << B));
      break;

    default:
      fprintf(stderr, "libcheetah: configuration wrongly set.\n");
      exit (1);
    }
}

/* pass a memory address to libcheetah */
void
cheetah_access(md_addr_t addr)		/* address of access */
{
  switch (trace_config)
    {
    case SA:
      if (repl == LRU)
	sacnmul_woarr(addr);
      else if (repl == OPT)
	optpp(addr, L, stack_proc_sa, inf_handler_sa);
      break;

    case FA:
      if (repl == LRU)
	ptc(addr);
      else if (repl == OPT)
	optpp(addr, L, stack_proc_fa, inf_handler_fa);
      break;

    case DM:
      dmvl(addr);
      break;
    
    default:
      fprintf (stderr, "libcheetah: configuration wrongly set\n");
      exit (1);
    }
}

/* output the trace statistics */
void
cheetah_stats(FILE *fd,			/* output stream */
	      int mid)			/* intermediate stats? */
{
  fprintf(fd, "\nlibcheetah: ** end of simulation **\n");

  switch (trace_config)
    {
    case SA:
      if (repl == LRU)
	outpr_saclru(fd);
      else if (repl == OPT)
	{
	  if (!mid) term_optpp(stack_proc_sa);
	  outpr_sacopt(fd);
	}
      break;

    case FA:
      if (repl == LRU)
	outpr_faclru(fd);
      else if (repl == OPT)
	{
	  if (!mid) term_optpp(stack_proc_fa);
	  outpr_facopt(fd);
	}
      break;

    case DM:
      outpr_dmvl(fd);
      break;

    default:
      fprintf(stderr, "libcheetah: configuration wrongly set\n");
      exit(1);
    }
}

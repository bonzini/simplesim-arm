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

/* Algorithm for simulating direct mapped caches of a range of line 	*
 * sizes, all caches being of the same size (same tag length)		*/


#include <stdio.h>
#include <stdlib.h>

#include "../host.h"
#include "../misc.h"
#include "../machine.h"
#include "util.h"
#include "libcheetah.h"

#define ONE 1
#define INVALID 0
#define SUB_ZERO 0xffffffff       /* substitute for zero tag */

extern int A,
           B,
           C,
           T,		/* No. of entries processed (may be set by user) */
           SAVE_INTERVAL,      	/* Interval at which results are saved */
           P_INTERVAL;  /* Intervals at which progress output is done */

static unsigned CACHE_SZ,	/* Size of the caches */
                MIN_LINE_SZ,	/* Min line size (field width) (may be set by user) */
                MAX_LINE_SZ,	/* Maximum line size (fw) (may be set by user) */

                NO_LINES,	/* No. of line sizes processed */
                MAX_SET_SZ,	/* Maximum set size (fw) */
                MIN_SET_SZ,	/* Minimum set size (fw) */

                TAG_MASK,	/* Masks */
                TAG_SET1_MASK,


                **tag_arr,	/* Tag store array */
                **set_arr,	/* Set field store array */
                **hits_arr,	/* Two-dimensional hit count array */
                *lm_arr,	/* Left-match array */
	        *hits;		/* Used in output processing */


static int t_entries;		/* Count of no. of entries seen */

#ifdef PERF
int compares, skips;
#endif

/********************************************************************
Simulation routine which does most of the work. Reads in trace addresses
and does the binomial tree lookup and update for each.

Input: None
Output: None
Side effects: Changes the binomial tree arrays to reflect current state
	      of caches. Also updates the hit array.
********************************************************************/

static unsigned **rtag_arr, **rset_arr, **rhits_arr;
static unsigned RTAG_MASK, RMAX_LINE_SZ;
static unsigned RCACHE_SZ;
static unsigned RTAG_SET1_MASK;
static unsigned RMIN_LINE_SZ;
static unsigned RNO_LINES;
static int next_save_time;


/****************************************************************
Output routine

Input: None
Output: None
Side effects: None
****************************************************************/

void
outpr_dmvl(FILE *fd)
{
  unsigned i, j, k;

  fprintf(fd, "Direct Mapped Caches\n");
  fprintf(fd, "Addresses processed %d\n", t_entries);
  fprintf(fd, "Cache size: %d bytes\n", (ONE << CACHE_SZ));
#ifdef PERF
  fprintf(fd, "compares %d\n", compares);
#endif
  fprintf(fd, "Line size (bytes)\tMiss ratio\n");
  for (i= 0; i <= NO_LINES-1; i++)
    {
      hits[i+MIN_LINE_SZ] = 0;
      for (j= 0; j <= (NO_LINES- i - 1); j++)
	{
	  for (k=(NO_LINES-i-1); k <= (NO_LINES-1); k++)
	    hits[i+MIN_LINE_SZ] += hits_arr[j][k];
	}
      fprintf(fd, "%d\t\t\t%f\n", (ONE << (i+MIN_LINE_SZ)), 
	      (1.0 - ((double) hits[i+MIN_LINE_SZ]/(double) t_entries)));
    }
  fprintf(fd, "\n");
}


void
dmvl(md_addr_t addr)
{
  unsigned orig_tag,		/* Tag field of address */
	tag;		/* Tag removed from prev level */
	
  /* addr = tt111112222llll . t-tag 1-set;fld 1;2-set fld 2;l-ln fld */
  unsigned set1,		/* Set field 1 of addr. For selecting BT */
	orig_set2,		/* Set field 2 of addr. Stored in BT */
	set2;		/* Set field 2 removed from prev level */
  unsigned st, tg;		/* Temps used ot reduce array refns */
  int entry,	                /* Scratch */
	pos;	                /* Array posn. being examined */
  int depth,			/* Level being examined */
	set2_match;		/* Scratch */
  unsigned *rtag_arr_set1, *rset_arr_set1;

  t_entries++;
  if ((t_entries % P_INTERVAL) == 0)
    fprintf(stderr, "libcheetah: addresses processed  %d\n", t_entries);

  set1 = (addr & RTAG_MASK) >> RMAX_LINE_SZ;
  orig_tag = addr >> RCACHE_SZ;
  orig_set2 = (addr & RTAG_SET1_MASK) >> RMIN_LINE_SZ;
#ifdef PERF
  ++compares;
#endif
  if (rset_arr[set1][0] == orig_set2)
    {
      if (rtag_arr[set1][0] == orig_tag)
	{
	  /* Level 0 hit */
	  ++rhits_arr[0][RNO_LINES-1];
	} 
      else
	rtag_arr[set1][0] = orig_tag;
    }
  else
    {
      rtag_arr_set1 = *(rtag_arr + set1);
      rset_arr_set1 = *(rset_arr + set1);
      tg = rtag_arr_set1[0];
      st = rset_arr_set1[0];
      depth = 0;
      set2_match = lm_arr[(st ^ orig_set2)];
      if (tg == orig_tag)
	++rhits_arr[0][set2_match];
      depth = set2_match + 1;
      tag = tg;
      rtag_arr_set1[0] = orig_tag;
      set2 = st;
      rset_arr_set1[0] = orig_set2;
      entry = set2 >> (RNO_LINES - depth );
      pos = entry + (ONE << (depth - 1));

      while ((unsigned)depth < RNO_LINES)
	{
#ifdef PERF
	  ++compares;
#endif
	  tg = rtag_arr_set1[pos];
	  st = rset_arr_set1[pos];
	  if (st == orig_set2)
	    {
	      if (tg == orig_tag)
		{ 
		  /* Hit at level 1 or greater */
		  ++rhits_arr[depth][RNO_LINES-1];
		}
	      rtag_arr_set1[pos] = tag;
	      rset_arr_set1[pos] = set2;
	      break;
	    }
	  set2_match = lm_arr[(st ^ orig_set2)];
	  if (tg == orig_tag)
	    ++rhits_arr[depth][set2_match];
	  depth = set2_match + 1;
	  rtag_arr_set1[pos] = tag;
	  tag = tg;
	  rset_arr_set1[pos] = set2;
	  set2 = st;
	  entry = set2 >> (RNO_LINES - depth );
	  pos = entry + (ONE << (depth - 1));
	}
    }
  if (t_entries > next_save_time)
    {
      outpr_dmvl(stderr);
      next_save_time += SAVE_INTERVAL;
    }
}


/*********************************************************************
Initialization routine.

Input: None
Output: None
Side effects: Allocates space for the various arrays and initializes them.
*********************************************************************/
void
init_dmvl(void)
{
  int i;
  unsigned depth, k;
  unsigned st, t;

  MIN_LINE_SZ = A;
  MAX_LINE_SZ = B;
  CACHE_SZ = C;

  NO_LINES = MAX_LINE_SZ - MIN_LINE_SZ + 1;
  MAX_SET_SZ =  (CACHE_SZ - MIN_LINE_SZ);
  MIN_SET_SZ =  (CACHE_SZ - MAX_LINE_SZ);

  TAG_MASK = (( ONE << CACHE_SZ) - ONE );
  TAG_SET1_MASK = ((ONE << MAX_LINE_SZ) - ONE);


  tag_arr = idim2((ONE << MIN_SET_SZ), (ONE << (NO_LINES-1)));
  set_arr = idim2((ONE << MIN_SET_SZ), (ONE << (NO_LINES-1)));
  hits_arr = idim2((NO_LINES), (NO_LINES));

  lm_arr = calloc((ONE << (NO_LINES - 1)), sizeof(int));
  if (!lm_arr)
    fatal("out of virtual memory");

  hits = calloc((MAX_LINE_SZ+1), sizeof(int));
  if (!hits)
    fatal("out of virtual memory");
  
  for (i=0; i < (ONE<<MIN_SET_SZ); i++)
    {
      set_arr[i][0] = 0;
      tag_arr[i][0] = SUB_ZERO;
      for (depth= 1; depth < NO_LINES; depth++)
	{
	  st = ONE << (depth-1);
	  t = ONE << (NO_LINES - depth);
	  set_arr[i][st] = ONE << (NO_LINES-depth-1);
	  tag_arr[i][st] = SUB_ZERO;
	  for (k=1; k<st; k++)
	    {
	      set_arr[i][st+k] = set_arr[i][st+k-1] + t;
	      tag_arr[i][st+k] = SUB_ZERO;
	    }
	}
    }

  t = ONE;
  st = NO_LINES - 1;
  for (i=0; i < (ONE << (NO_LINES - 1)); i++)
    {
      if (t > (unsigned)i)
	lm_arr[i] = st;
      else
	{
	  t <<= ONE;
	  st -= 1;
	  lm_arr[i] = st;
	}
    }

  rtag_arr = tag_arr;
  rset_arr = set_arr;
  rhits_arr = hits_arr;
  next_save_time = SAVE_INTERVAL;

  RTAG_MASK=TAG_MASK;
  RMAX_LINE_SZ=MAX_LINE_SZ;
  RCACHE_SZ=CACHE_SZ;
  RTAG_SET1_MASK=TAG_SET1_MASK;
  RMIN_LINE_SZ=MIN_LINE_SZ;
  RNO_LINES=NO_LINES;
}

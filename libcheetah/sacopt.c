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

/* Simulates multiple set associative cache configurations */



#include <stdio.h>
#include <stdlib.h>

#include "../host.h"
#include "../misc.h"
#include "../machine.h"
#include "util.h"
#include "libcheetah.h"

#define ONE 1
#define TWO 2U
#define B80000000 0x80000000
#define INVALID 0
#define MEM_AVAIL_HITARR 2097152	/* Memory available for hitarr */
#define MAXINT 2147483647   /* 2^31-1 */
#define HASHNO 7211
 
extern int N,		/* Degree of associativity = 2^N */
           B,		/* Max set field width */
           A,		/* Min set field width */
           L,		/* Line field width */
           T,		/* Max no of addresses to be processed */
           SAVE_INTERVAL,      	/* Interval at which results are saved */
           P_INTERVAL;  /* Intervals at which progress output is done */

static int TWO_PWR_N,	/* 2^N. i.e., Degree of associativity */
           MAX_DEPTH,	/* B-A, Number of range of sets simulated */
           TWO_POWER_MAX_DEPTH,	/* 2^MAX_DEPTH */
           SET_MASK,	/* Masks */
           DIFF_SET_MASK,
           SET_SIZE,
           BASE_CACHE_SIZE,
           CLEAN_INTERVAL,
           time_of_next_clean;

#define WORD_SIZE 1

static struct hash_table *slot;	/* Hash table */
static short *slot_flag;

static unsigned *tag_arr,
                **hits; /* Hit counts in caches */
static unsigned hit0;
static short *all_hit_flag;
static int *prty_arr;

static unsigned t_entries; /* Count of addresses processed */
static unsigned unknowns;

#ifdef PERF
int compares=0;		/* Number of comparisons */
#endif

extern unsigned addr_array[],
		time_array[];
static unsigned next_save_time;


/********************************************************************
Given the index of the address in the address array it determines the
priority of the address using the time_array.

Input: Index of current address in the addr_array.
Output: The priority of the current address
Side effects: None
********************************************************************/
int
Priority_sa(int i)
{
  static int inf_count=0;

  if (time_array[i] > 0)
    return (MAXINT - time_array[i]); /* inf_handler assumes this fn.
					inf_handler has to be changed
					if this is changed */
  else if (time_array[i] == 0)
    return --inf_count;
  else
    {
      fprintf(stderr, "libcheetah: error in priority function\n");
      return 0;
    }
}


/**********************************************************************
Used to add unknowns to a hash table. The hash table is used by inf_handler
to get the dummy priority of the unknown.

Input: The address and the dummy prty of the address.
Output: None
Side effects: The address is added to the hash table
**************************************************************************/
void
unk_hash_add_sa (md_addr_t addr, int prty)
{
  int loc;
  struct hash_table *ptr, *oldptr;

  ++unknowns;

  loc = addr & ((ONE << B) - 1);
  /* loc = addr % HASHNO; */
  if (slot_flag[loc] == 0)
    {
      slot_flag[loc] = 1;
      slot[loc].addr = addr;
      slot[loc].prty = prty;
      slot[loc].nxt = NULL;
      return;
    }
  else
    {
      ptr = &slot[loc];
      while (ptr)
	{
	  oldptr = ptr;
	  if (ptr->addr == addr)
	    {
              fprintf(stderr, "libcheetah: address already in hash table\n"); 
	      myfprintf(stderr, "addr: 0x%p; t_entries: %u; prty: %d\n",
			addr, t_entries, ptr->prty);
	      myfprintf(stderr, "slot addr: 0x%p; prty: %d\n",
			slot[loc].addr, slot[loc].prty);
	    }
	  ptr = ptr->nxt;
        }
      if ((oldptr->nxt = UHT_Get_from_free_list()) == NULL)
	{
	  oldptr->nxt = calloc(1, sizeof(struct hash_table));
	  if (!oldptr->nxt)
	    fatal("out of virtual memory");
	}
      oldptr->nxt->addr = addr;
      oldptr->nxt->nxt = NULL;
      oldptr->nxt->prty = prty;
      return;
    }
}


/*************************************************************************
Deletes unknowns from the hash table. Returns the dummy priority of
the address.

Input: Address
Outpur: The dummy priority
Side effects: The entry is deleted from the hash table
*************************************************************************/
int
unk_hash_del_sa(md_addr_t addr)
{
  int loc;
  struct hash_table *ptr, *oldptr;
  int ret_prty;

  loc = addr & ((ONE << B) - 1);
  /* loc = addr % HASHNO; */
  if (slot_flag[loc] == 0)
    return 0;
  else if (slot[loc].addr == addr)
    {
      if (slot[loc].nxt == NULL)
	{
	  slot_flag[loc] = 0;
	  return slot[loc].prty;
	}
      else
	{
	  ret_prty = slot[loc].prty;
	  slot[loc].addr = slot[loc].nxt->addr;
	  slot[loc].prty = slot[loc].nxt->prty;
	  ptr = slot[loc].nxt;
	  slot[loc].nxt = slot[loc].nxt->nxt;
	  UHT_Add_to_free_list(ptr);
	  return ret_prty;
	}
    }
  else
    {
      ptr = &slot[loc];
      while (ptr)
	{
          if (ptr->addr == addr)
            break;
          oldptr = ptr;
          ptr = ptr->nxt;
        }
      if (ptr)
	{
	  ret_prty = ptr->prty;
	  oldptr->nxt = ptr->nxt;
	  UHT_Add_to_free_list(ptr);
	  return ret_prty;
        }
      else
	return 0;
    }
}


/****************************************************************
Output routine.

Input: None
Output: None
Side effects: None
****************************************************************/

void
outpr_sacopt(FILE *fd)
{
  int i, j;
  int sum;

  for (i=0; i <= (B-A); i++)
    hits[i][0] += hit0;
  hit0 = 0;

  fprintf(fd, "Addresses processed: %d\n", t_entries);
  fprintf(fd, "Line size: %d bytes\n", (ONE << L));
#ifdef PERF
  fprintf(fd, "compares  %d\n", compares);
#endif
  fprintf(fd, "\n");
  fprintf(fd, "Miss Ratios\n");
  fprintf(fd, "___________\n\n");
  fprintf(fd, "\t\tAssociativity\n");
  fprintf(fd, "\t\t");
  for (i=0;i<TWO_PWR_N;i++)
    fprintf(fd, "%d\t\t", (i+1));
  fprintf(fd, "\n");
  fprintf(fd, "No. of sets\n");
  for (i=0; i<=MAX_DEPTH; i++)
    {
      sum = 0;
      fprintf(fd, "%d\t\t", (ONE << (i+A)));
      for (j=0; j<TWO_PWR_N; j++)
	{
	  sum += hits[i][j];
	  fprintf(fd, "%f\t", (1.0 - ((double)sum/(double)t_entries)));
	}
      fprintf(fd, "\n");
    }
  fprintf (fd, "\n\n\n");
}


/********************************************************************
Given the address and max_prty, goes through the stack that adress maps
to and removes unknowns of priority greater than max_prty

Input: Address and maximum priority of unknown in stack
Output: None
Side effects: Removes inessential unknowns from the hash table
********************************************************************/
void
hash_clean_sa(md_addr_t addr, int max_prty)
{
  int loc;
  struct hash_table *ptr, *oldptr;

  if (max_prty >= 0)
    fprintf(stderr, "libcheetah: max unknown priority non-negative\n");

  loc = addr & ((ONE << B) - 1);
  while ((slot_flag[loc] != 0) && (slot[loc].prty > max_prty))
    {
      --unknowns;
      if (slot[loc].nxt == NULL)
	{
	  slot_flag[loc] = 0;
	  continue;
	}
      else
	{
	  slot[loc].addr = slot[loc].nxt->addr;
	  slot[loc].grptime = slot[loc].nxt->grptime;
	  slot[loc].prty = slot[loc].nxt->prty;
	  ptr = slot[loc].nxt;
	  slot[loc].nxt = slot[loc].nxt->nxt;
	  UHT_Add_to_free_list(ptr);
	}
    } /* while */

  if (slot_flag[loc] != 0)
    {
      ptr = &slot[loc];
      while (ptr)
	{
	  if (ptr->prty > max_prty)
	    {
              --unknowns;
              oldptr->nxt = ptr->nxt;
              UHT_Add_to_free_list(ptr);
              ptr = oldptr;
            }
	  oldptr = ptr;
	  ptr = ptr->nxt;
	}
    }
}


/********************************************************************
Routine that checks and updates the tag stores, and maintains the hit counts.

Input: Trace address and priority of address
Output: None
Side effects: Changes the tag stores, and updates the hit array
********************************************************************/
void
gfsoptls(md_addr_t addr, int priority)
{
  unsigned setno, base_setno, orig_tag,
	rem_tag, de_tag, prev_addr;
  int rem_prty, de_prty, real_prty, max_prty;
  unsigned cache_size, cache_ptr, set_ptr;
  int i, j;
  short hit;

  orig_tag = addr >> A;
  base_setno = addr & ((ONE << A) - 1);
  if (all_hit_flag[base_setno])
    {
      if (tag_arr[base_setno * SET_SIZE] == orig_tag)
	{
	  ++ hit0;
	  prty_arr[base_setno * SET_SIZE] = priority;
	  return;
	}
      else
	{
	  real_prty = prty_arr[base_setno * SET_SIZE];
	  prev_addr = (tag_arr[base_setno * SET_SIZE] << A) | base_setno;
	  cache_ptr = BASE_CACHE_SIZE;
	  for (i=A+1; i<=B; i++)
	    {
	      setno = prev_addr & ((ONE << i) - 1);
	      set_ptr = cache_ptr + (setno * SET_SIZE);
	      prty_arr[set_ptr] = real_prty;
	      cache_ptr = TWO*cache_ptr + BASE_CACHE_SIZE;
	    }
	}
    }
  all_hit_flag[base_setno] = 1;
  i = A;
  cache_size = BASE_CACHE_SIZE;
  cache_ptr = 0;
  max_prty = (5 - MAXINT);
  while (i <= B)
    {
      de_tag = orig_tag;
      de_prty = priority;
      setno = addr & ((ONE << i) - 1);
      set_ptr = cache_ptr + (setno * SET_SIZE);
      hit = 0;
      for (j=0; j < TWO_PWR_N; j++)
	{
#ifdef PERF
	  ++compares;
#endif
	  if (tag_arr[set_ptr + j] == orig_tag)
	    {
	      hit = 1;
	      ++hits[i-A][j];
	      tag_arr[set_ptr + j] = de_tag;
	      prty_arr[set_ptr + j] = de_prty;
	      break;
	    }
	  else
	    {
	      if (de_prty > prty_arr[set_ptr + j])
		{
		  rem_tag = tag_arr[set_ptr + j];
		  tag_arr[set_ptr + j] = de_tag;
		  de_tag = rem_tag;
		  rem_prty = prty_arr[set_ptr + j];
		  prty_arr[set_ptr + j] = de_prty;
		  de_prty = rem_prty;
		}
	      if (i == B)
		{
		  if (prty_arr[set_ptr + j] < 0)
		    {
		      if (max_prty < prty_arr[set_ptr + j])
			max_prty = prty_arr[set_ptr + j];
		    }
		}
	    }
	} /* for */

      if (j > 0)
	{
	  all_hit_flag[base_setno] = 0;
	  if (j >= TWO_PWR_N)
	    {
	      if ((i == B) && (de_prty < 0) && (de_prty > (5-MAXINT)))
		ft_hash_del((de_tag << A) | setno);   /* reforming address */
	    }
	}
      cache_ptr += cache_size;
      cache_size += cache_size;
      ++i;
    } /* while */

  if ((i > B) && (hit == 0))
    hash_clean_sa(addr, max_prty);
}


/**********************************************************************
Main simulation routine. Processes the tag_array and time_array obtained
from the pre-processing routine.

Input: The range in the addr_array (time_array) to be processed
Output: -1 if limit on the addresses to be processed is reached
       0 otherwise
Side effects: The tag stores are updated by its child routine 'gfsoptls'
	      as the simulation proceeds.
**********************************************************************/

int
stack_proc_sa(int start,		/* index of starting array location */
	      int end)			/* index of ending array location */
{
  int l;
  unsigned addr;
  int priority;

  for (l=start; l<end; l++)
    {
      ++t_entries;

      if ((t_entries % P_INTERVAL) == 0)
	fprintf(stderr, "libcheetah: addresses processed %d\n", t_entries);
      if (t_entries > next_save_time)
	{
	  outpr_sacopt(stderr);
	  next_save_time += SAVE_INTERVAL;
	}

      addr = addr_array[l];

      priority = Priority_sa(l);
      if (priority < 0)
	unk_hash_add_sa(addr, priority);

      gfsoptls(addr, priority);

      addr_array[l] = 0;
      time_array[l] = 0;
      if (t_entries > (unsigned)T)
	return 1;
    } /* for */

  return 0;
}


/**********************************************************************
Initialization routine. Allocates space for the various arrays and
initializes them.

Input: None
Output: None
Side effects: Allocates space for the arrays and initializes the array
	      locations.
**********************************************************************/
void
init_sacopt(void)

{
  unsigned i;

  TWO_PWR_N = (ONE << N);
  MAX_DEPTH = B-A;
  TWO_POWER_MAX_DEPTH = (ONE << MAX_DEPTH);
  SET_MASK = ((ONE << A) - 1);
  DIFF_SET_MASK = ((ONE << MAX_DEPTH) - 1);
  SET_SIZE = WORD_SIZE*TWO_PWR_N;
  BASE_CACHE_SIZE = (ONE<<A)*SET_SIZE;

  next_save_time = SAVE_INTERVAL;

  tag_arr = calloc((TWO * (ONE << B) * TWO_PWR_N), sizeof(unsigned));
  if (!tag_arr)
    fatal("out of virtual memory");

  prty_arr = calloc((TWO * (ONE << B) * TWO_PWR_N), sizeof(unsigned));
  if (!prty_arr)
    fatal("out of virtual memory");

  all_hit_flag = calloc((ONE << A), sizeof(short));
  if (!all_hit_flag)
    fatal("out of virtual memory");

  hits = idim2 ((MAX_DEPTH + 2), (TWO_PWR_N));

  slot = calloc((ONE << B), sizeof(struct hash_table));
  if (!slot)
    fatal("out of virtual memory");

  slot_flag = calloc((ONE << B), sizeof(short));
  if (!slot_flag)
    fatal("out of virtual memory");

  for (i=0; i < (TWO * (ONE << B) * TWO_PWR_N); i++)
    {
      tag_arr[i] = 0xffffffff;
      prty_arr[i] = (5-MAXINT);
    }

  time_of_next_clean = CLEAN_INTERVAL;
}


/*******************************************************************
Rearranges the positions of unknowns in the stack, when an unknown
becomes known.

Input: Address of unknown that becomes known, and the current time at
       pre-processor.
Output: None
Side effects: Rearranges the positions of unknowns in the stacks that addr
	      maps to
*******************************************************************/

void
inf_handler_sa(md_addr_t addr, int cur_time)
{
  unsigned setno, orig_tag, cache_size,
	de_tag, rem_tag, prev_addr,
	cache_ptr, set_ptr, base_setno;
  int de_prty, rem_prty,
	old_prty, priority,
	real_prty;
  int i, j;

  old_prty = unk_hash_del_sa(addr);
  if (old_prty == 0)
    return;

  --unknowns;

  priority = MAXINT - cur_time; /* Assumes a priority function */

  base_setno = addr & ((ONE << A) -1);
  if (all_hit_flag[base_setno])
    {
      all_hit_flag[base_setno] = 0;
      real_prty = prty_arr[base_setno * SET_SIZE];
      prev_addr = (tag_arr[base_setno * SET_SIZE] << A) | base_setno;
      cache_ptr = BASE_CACHE_SIZE;
      for (i=A+1; i<=B; i++)
	{
	  setno = prev_addr & ((ONE << i) - 1);
	  set_ptr = cache_ptr + (setno * SET_SIZE);
	  prty_arr[set_ptr] = real_prty;
	  cache_ptr = TWO*cache_ptr + BASE_CACHE_SIZE;
	}
    }
  orig_tag = addr >> A;
  i = A;
  cache_size = BASE_CACHE_SIZE;
  cache_ptr = 0;
  while (i <= B)
    {
      de_tag = orig_tag;
      de_prty = priority;
      setno = addr & ((ONE << i) - 1);
      set_ptr = cache_ptr + (setno * SET_SIZE);
      for (j=0; j < TWO_PWR_N; j++)
	{
#ifdef PERF
	  ++compares;
#endif
	  if (tag_arr[set_ptr + j] == orig_tag)
	    {
	      if (prty_arr[set_ptr + j] > 0)
		fprintf(stderr, "libcheetah: inconsistency in inf_handler\n");
	      else
		{
		  tag_arr[set_ptr + j] = de_tag;
		  prty_arr[set_ptr + j] = de_prty;
		  break;
		}
	    }
	  else if (prty_arr[set_ptr + j] < 0)
	    {
	      if (old_prty < prty_arr[set_ptr + j])
		{
		  if (de_prty > prty_arr[set_ptr + j])
		    {
		      rem_tag = tag_arr[set_ptr + j];
		      tag_arr[set_ptr + j] = de_tag;
		      de_tag = rem_tag;
		      rem_prty = prty_arr[set_ptr + j];
		      prty_arr[set_ptr + j] = de_prty;
		      de_prty = rem_prty;
		    }
		}
	    }
	} /*for*/
      cache_ptr += cache_size;
      cache_size += cache_size;
      ++i;
    } /*while*/
}

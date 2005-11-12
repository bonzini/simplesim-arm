
/* Pre-processing routines to for OPT simulation	       */

#include <stdio.h>
#include <stdlib.h>

#include "../host.h"
#include "../misc.h"
#include "../machine.h"
#include "libcheetah.h"

#define toggle(i) if(i==1)i=0;else{i=1;}

#define LA_DIST 100000		/* Lookahead distance */
#define HASHNO 7211

struct ft_hash_table {
  md_addr_t addr;
  int pt;
  struct ft_hash_table *nxt;
} ft_slot[HASHNO];

short ft_slot_flag[HASHNO];	/* Hash table */

unsigned addr_array[2*LA_DIST],	/* Trace input to stack_proc */
         time_array[2*LA_DIST];	/* Next time of arrival of addresses */

static int la_limit,	/* Look-ahead limit in current iteration */
	       base,	/* Base time in current iteration */
	  oe_flag=1;   /* Whether top or bottom half of array */

static int p_inum;	/* Address count */


/***************************************************************
Free list handlers
***************************************************************/

static struct ft_hash_table *head_free_list;

void
LA_Add_to_free_list(struct ft_hash_table *free_ptr)
{
  free_ptr->nxt = head_free_list;
  head_free_list = free_ptr;
}


struct ft_hash_table *
LA_Get_from_free_list(void)
{
  struct ft_hash_table *free_ptr;

  if (head_free_list == NULL)
    return NULL;
  else
    {
      free_ptr = head_free_list;
      head_free_list = head_free_list->nxt;
      return free_ptr;
    }
}


/***************************************************************
Hashing routine. Adds addr to the hash table

Input: address
Output: -1 if addr is not found
        previous time of arrival if it is
Side effects: An entry for address is added to the hash table if address
	      is not found.
***************************************************************/

int
ft_hash(md_addr_t addr)			/* address to be looked up */
{
  int loc;
  int prev_time = 0;
  struct ft_hash_table *ptr, *oldptr;

  loc = addr % HASHNO;
  if (ft_slot_flag[loc] == 0)
    {
      /* ++tot_addrs;*/
      ft_slot_flag[loc] = 1;
      ft_slot[loc].addr = addr;
      ft_slot[loc].nxt = NULL;
      ft_slot[loc].pt = p_inum;
      return -1;
    }
  else
    {
      ptr = &ft_slot[loc];
      while (ptr)
	{
	  oldptr = ptr;
	  if (ptr->addr == addr)
	    break;
	  ptr = ptr->nxt;
        }
      if (ptr)
	{
	  prev_time = ptr->pt;
	  ptr->pt = p_inum;
	  return(prev_time);
        }
      else
	{
	  /*++tot_addrs;*/
	  if ((oldptr->nxt = LA_Get_from_free_list()) == NULL)
	    {
	      oldptr->nxt = calloc(1, sizeof(struct ft_hash_table));
	      if (!oldptr->nxt)
		fatal("out of virtual memory");
	    }
	  oldptr->nxt->addr = addr;
	  oldptr->nxt->nxt = NULL;
	  oldptr->nxt->pt = p_inum;
	  return -1;
        }
    }
}


/************************************************************************
Deletes addresses from the hash table. It is used when entries are deleted
from the stack due to stack overflow.

Input: Address
Output: None
Side effects: The entry for the address is deleted from the hash table
	      (The address need not necessarily be found, (e.g.) when
	       an address is referenced multiple times in the same window
	       and both references overflow).
*************************************************************************/
void
ft_hash_del(md_addr_t addr)
{
  int loc;             /* Scratch variables */
  struct ft_hash_table *ptr, *oldptr;

  loc = addr % HASHNO;
  if (ft_slot_flag[loc] == 0)
    printf ("Error: addr not found in hash_table\n");
  else if (ft_slot[loc].addr == addr)
    {
      if (ft_slot[loc].nxt == NULL)
	ft_slot_flag[loc] = 0;
      else
	{
	  ft_slot[loc].addr = ft_slot[loc].nxt->addr;
	  ft_slot[loc].pt = ft_slot[loc].nxt->pt;
	  ptr = ft_slot[loc].nxt;
	  ft_slot[loc].nxt = ft_slot[loc].nxt->nxt;
	  LA_Add_to_free_list(ptr);
	}
    }
  else
    {
      ptr = &ft_slot[loc];
      while (ptr)
	{
	  if (ptr->addr == addr)
            break;
          oldptr = ptr;
          ptr = ptr->nxt;
        }
      if (ptr)
	{
          oldptr->nxt = ptr->nxt;
          LA_Add_to_free_list(ptr);
        }
      else
	printf("Error: addr not found in hash_table\n");
    }
}


/*********************************************************************
Pre-processor for optimal simulation. Reads in trace addresses and goes back
to their previous point of reference and stores current time. The stack
processing routine can use this information to determine the priority of
addresses. There is a lookahead of at least LA_DIST always.
Calls 'inf_handler' to correct the stack when an address
whose priority was unknown is encountered.

Input: the line size
Output: None
Side effects: Does pre-processing on the trace and calls stack_proc till
	      the trace is processed or the limit on the address count is
	      reached (checked inside stack_proc).
*********************************************************************/

static int start, end;

void
init_optpp(void)
{
  la_limit = 2*LA_DIST;
  base = 0;
}

void
optpp(md_addr_t addr, int L,
      int (*stack_proc)(int start, int end),
      void (*inf_handler)(md_addr_t addr, int cur_time))
{
  int prev_time, sf;

  if (p_inum > la_limit-1)
    {
      start = (la_limit % (2*LA_DIST));
      end = start + LA_DIST;
      if ((sf = (*stack_proc)(start, end))  == 1)
	return;
      la_limit += LA_DIST;
      toggle(oe_flag);		
      if (oe_flag)
	base += 2*LA_DIST;
    }
  addr >>= L;
  addr_array[(p_inum - base) % (2*LA_DIST)] = addr;
  if ((prev_time = ft_hash(addr)) >= 0)
    {
      if ((la_limit - prev_time) <= 2*LA_DIST)
	time_array[(prev_time - base) % (2*LA_DIST)] = p_inum;
      else
	(*inf_handler)(addr, p_inum);
    }
  else
    (*inf_handler)(addr, p_inum);
  ++p_inum;
}


/* residual processing */
void
term_optpp(int (*stack_proc)())
{
  start = la_limit % (2*LA_DIST);
  end = start + LA_DIST;
  if (p_inum < LA_DIST)
    end = p_inum; /* For small traces */
  (*stack_proc)(start, end);
  if (p_inum >= LA_DIST)
    {
      toggle(oe_flag);
      if (oe_flag)
	base += 2*LA_DIST;
      la_limit += LA_DIST;
      start = la_limit % (2*LA_DIST);
      end = start + (((p_inum % LA_DIST) == 0) ? LA_DIST : (p_inum % LA_DIST));
      (*stack_proc)(start, end);
    }
}

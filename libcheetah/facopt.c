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

/* Simulates fully associative caches of a range of sizes but of *
 * a constant line size with OPT replacement.                    */

#include <stdio.h>
#include <stdlib.h>

#include "../host.h"
#include "../misc.h"
#include "../machine.h"
#include "util.h"
#include "libcheetah.h"

/* Macros */
#define toggle(i) if(i==1)i=0;else{i=1;}
#define min(a,b) ((a<b) ? a : b)

#define ONE 1
#define MAX_PHYSICAL_MEM 2097152  /* Physical mem available on machine */
#define BYTES_PER_LINE 35  /* Storage requirement per line */

#define HASHNO 7211    /* number of slots in hash table */
#define TREE_HEIGHT 50000  /* maximum height of tree */
#define CACHE_SIZE 50000  /* no of lines in cache */
#define MAXINT 2147483647   /* 2^31-1 */

struct group_desc {
  struct tree_node *first;
  struct tree_node *last;
  int grpno;
  int wt;
  struct group_desc *nxt;
};

static struct group_desc headgrp;	/* Head of group list */
static struct tree_node *root,	/* Root of tree */
		     **p_stack;	/* Stack used in tree operations */
static struct hash_table slot[HASHNO];	/* Hash table */
static short slot_flag [HASHNO];	/* Flag for hash slots */
static unsigned *out_stack;	/* Stack depth hits array */
static int comp=0, comp_del=0, comp_ins=0,
           no_splay_steps, tot_addrs, grps_passd;
static int t_entries;		/* Count of addresses processed */
extern unsigned addr_array[],
 	        time_array[];
static int ihcount;  /* To count no of time inf_handler called */
static int grp_ct=MAXINT;  /* To number groups */
static short size_exceeded=0;		/* Stack overflow flag */
static int log_tot_addrs;
static unsigned next_two_pwr;
static unsigned unknowns;

static int CLEAN_INTERVAL,
           next_clean_time;

extern int L,		/* Line size */
           T;		/* Max addresses processed */
extern unsigned MAX_CACHE_SIZE,	/* Maximum cache size of interest */
                MAX_LINES;	/* Calculated from max cache size and line size */
extern int MISS_RATIO_INTERVAL,	/* Output interval */
           SAVE_INTERVAL,      	/* Interval at which results are saved */
           P_INTERVAL;  /* Intervals at which progress output is done */

static int next_save_time;


/********************************************************************
Given the index of the address in the address array it determines the
priority of the address using the time_array.

Input: Index of current address in the addr_array.
Output: The priority of the current address
Side effects: None
********************************************************************/
int
Priority_fa(int i)
{
  static int inf_count = 0;

  /* inf_handler assumes this fn inf_handler has to be changed if this is
     changed */
  if (time_array[i] > 0)
    return (MAXINT - time_array[i]);
  else if (time_array[i] == 0)
    return --inf_count;
  else
    {
      fprintf(stderr, "libcheetah: error in Priority function\n");
      return 0;
    }
}


/**********************************************************************
Used to add infinities to a hash table. The hash table is used by inf_handler
to get the dummy priority of the unknown and its grptime.

Input: The address, the current group number of the first group and
       the dummy prty of the address.
Output: None
Side effects: The address is added to the hash table
**************************************************************************/
void
unk_hash_add_fa(md_addr_t addr, int grpno, int prty)
{
  int loc;
  struct hash_table *ptr, *oldptr;

  ++unknowns;

  loc = addr % HASHNO;
  if (slot_flag[loc] == 0)
    {
      slot_flag[loc] = 1;
      slot[loc].addr = addr;
      slot[loc].grptime = grpno;
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
              fprintf(stderr, "libcheetah: addr already in hash table\n"); 
	      myfprintf(stderr,
			"addr: 0x%p; t_entries: %u; prty: %d\n",
			addr, t_entries, ptr->prty);
	      myfprintf(stderr,
			"slot addr: 0x%p; prty: %d\n",
			slot[loc].addr, slot[loc].prty);
	    }
	  ptr = ptr->nxt;
        }
      if ((oldptr->nxt = UHT_Get_from_free_list ()) == NULL)
	{
	  oldptr->nxt = calloc(1, sizeof(struct hash_table));
	  if (!oldptr->nxt)
	    fatal("out of virtual memory");
	}
      oldptr->nxt->addr = addr;
      oldptr->nxt->nxt = NULL;
      oldptr->nxt->grptime = grpno;
      oldptr->nxt->prty = prty;
      return;
    }
}


/*************************************************************************
Deletes unknowns from the hash table. Returns the grptime and the dummy
priority of the address.

Input: Address
Outpur: The grptime and the dummy priority
Side effects: The entry is deleted from the hash table
*************************************************************************/
int
unk_hash_del_fa(md_addr_t addr, int *prty_ptr)
{
  int loc, grpno;
  struct hash_table *ptr, *oldptr;

  loc = addr % HASHNO;
  if (slot_flag[loc] == 0)
    return 0;
  else if (slot[loc].addr == addr)
    {
      if (slot[loc].nxt == NULL)
	{
	  slot_flag[loc] = 0;
	  *prty_ptr = slot[loc].prty;
	  return slot[loc].grptime;
	}
      else
	{
	  grpno = slot[loc].grptime;
	  *prty_ptr = slot[loc].prty;
	  slot[loc].addr = slot[loc].nxt->addr;
	  slot[loc].grptime = slot[loc].nxt->grptime;
	  slot[loc].prty = slot[loc].nxt->prty;
	  ptr = slot[loc].nxt;
	  slot[loc].nxt = slot[loc].nxt->nxt;
	  UHT_Add_to_free_list(ptr);
	  return grpno;
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
          grpno = ptr->grptime;
	  *prty_ptr = ptr->prty;
	  oldptr->nxt = ptr->nxt;
	  UHT_Add_to_free_list (ptr);
	  return grpno;
        }
      else
	return 0;
    }
}


/******************************************************************
Determines the maximum priority of an unknown in the stack by doing an
inorder traversal of the tree.
Adapted from Harry Smith "Data structures: Form and Function"

Input: None
Output: Maximum unknown priority in stack
Side effects: None
******************************************************************/
int
Get_max_prty_unknown(void)
{
  int top;
  struct tree_node *ptr;
  int max_prty = (5 - MAXINT);

  ptr = root;
  top = 0;
  while (ptr != NULL)
    {
      ++top;
      p_stack[top] = ptr;
      ptr = ptr->lft;
    }
   while (top > 0)
     {
       ptr = p_stack[top];
       --top;
       if ((ptr->prty < 0) && (ptr->prty > max_prty))
	 max_prty = ptr->prty;
       if (ptr->rt != NULL)
	 {
	   ptr = ptr->rt;
	   while (ptr != NULL)
	     {
	       ++top;
	       p_stack[top] = ptr;
	       ptr = ptr->lft;
	     }
	 }
     }
  return max_prty;
}


/***********************************************************************
Cleans the unknown hash table. Get the maximum unknown priority in the
stack by calling the routine Get_max_prty_unknown

Input: None
Output: None
Side effects: Removes inessential unknowns from the hash table
***********************************************************************/
void
hash_clean_fa(void)
{
  int loc,		/* Scratch */
	max_prty;	/* Max unknown priority in stack */
  struct hash_table *ptr, *oldptr; 	/* Scratch */

  max_prty = Get_max_prty_unknown();
  if (max_prty >= 0)
    fprintf(stderr, "libcheetah: max unknown priority non-negative\n");

  for (loc=0; loc<HASHNO; loc++)
    {
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
    } /* for */
}


/********************************************************************
Output routine.

Input: None
Output: None
Side effects: None
********************************************************************/
void
outpr_facopt(FILE *fd)
{
  int sum = 0, i;
  int stack_end;

  fprintf(fd, "Addresses processed : %d\n", t_entries);
  fprintf(fd, "Line size : %d bytes \n", (ONE << L));
  fprintf(fd, "Number of distinct addresses  %d\n", tot_addrs);
#ifdef PERF
  fprintf(fd, "Total Number of groups passed\t%d\n\n", grps_passd);
  fprintf(fd, "Total Number of splay steps\t%d\n\n", no_splay_steps);
  fprintf(fd, "Number of insert comparisons\t%d\n", comp_ins);
  fprintf(fd, "Number of delete comparisons\t%d\n", comp_del);
  fprintf(fd, "Number of times inf_handler called\t%d\n", ihcount);
#endif

  fprintf(fd, "Cache size (bytes)\tMiss Ratio\n");
  stack_end = min ((unsigned)tot_addrs, MAX_LINES);
   
  for (i = 1; i <= stack_end; i++)
    {
      sum += out_stack[i];
      if ((i % MISS_RATIO_INTERVAL) == 0)
	fprintf(fd, "%d\t\t\t%1.6f\n", (i * (ONE << L)),
		(1.0 - ((double)sum/(double)t_entries)));
    }
  if (stack_end == tot_addrs)
    fprintf(fd, "Miss ratio is %f for all bigger caches\n",
	    (1.0 - ((double)sum/(double)t_entries)));
  fprintf(fd, "\n\n\n");
}


/**********************************************************************
Locates the referenced address, deletes it and inserts the address deleted
from the previous group in its place.

Input: Referenced address and address deleted from earlier group
Output: Pointer to node at which insertion occurred.
Side effects.: The deletion and insertion are done and the inserted
node is splayed to the root of the tree.
***********************************************************************/
struct tree_node *
Lookup_Delete_Insert(struct tree_node *del_entry,	/* ent to delete */
		     struct tree_node *ins_entry)	/* deleted line */
{
  struct tree_node *ptr, *inserted_node;
  int top, pos, at;
  int grpno, prty;

  grpno = del_entry->grpno;
  prty = del_entry->prty;
  if (root->grpno == grpno)
    {
      fprintf(stderr, "libcheetah: hit at root\n");
      return 0;
    }
  else
    {
      top = 0;
      ptr = root->lft;	/* Starts at root->lft to avoid involving
			   root in balance operations */
      while (ptr)
	{
	  ++comp;
	  ++top;
	  p_stack[top] = ptr;
	  if ((ptr->grpno < grpno)
	      || ((ptr->grpno == grpno) && (ptr->prty > prty)))
	    ptr = ptr->lft;
	  else
	    {
	      if ((ptr->grpno == grpno) && (ptr->prty == prty))
		{
		  pos = top;
		  break;
		}
	      ptr = ptr->rt;
	    }
        }

      p_stack[pos]->grpno = ins_entry->grpno;
      p_stack[pos]->prty  = ins_entry->prty;
      p_stack[pos]->addr = ins_entry->addr;
      inserted_node = p_stack[pos];

      at = pos;
      while (at > 1)
	{
	  ++no_splay_steps;   /* Counts the number of basic operations */
	  splay(at, p_stack);
	  at = at - 2;
        }
      root->lft = p_stack[1];
      return inserted_node;
    } /* else */
}


/*********************************************************************
Given an entry in the stack returns the previous entry

Input: Pointer to entry
Output: Pointer to previous entry
Side effects: None
*********************************************************************/
struct tree_node *
Lookup_prev(struct tree_node *entry)
{
  struct tree_node *ptr, *lstlft_node;
  int grpno, prty;

  grpno = entry->grpno;
  prty = entry->prty;
  ptr = root;
  while (ptr)
    {
      ++comp;
      if ((ptr->grpno < grpno)
	  || ((ptr->grpno == grpno) && (ptr->prty > prty)))
	{
	  lstlft_node = ptr;
	  ptr = ptr->lft;
	}
      else
	{
	  if ((ptr->grpno == grpno) && (ptr->prty == prty))
	    break;
	  ptr = ptr->rt;
	}
    }                                                            

  if ((ptr == NULL) || (ptr->rt == NULL)) 
    return lstlft_node;
  else
    {
      ptr = ptr->rt;
      while (ptr->lft != NULL)
	ptr = ptr->lft;
      return ptr;
    }
}


/*********************************************************************
Given an entry in the stack returns the next entry

Input: Pointer to entry
Output: Pointer to next entry
Side effects: None
*********************************************************************/
struct tree_node *
Lookup_next(struct tree_node *entry)
{
  struct tree_node *ptr, *lstrt_node;
  int grpno, prty;

  grpno = entry->grpno;
  prty = entry->prty;
  ptr = root;
  while (ptr)
    {
      ++comp;
      if ((ptr->grpno < grpno)
	  || ((ptr->grpno == grpno) && (ptr->prty > prty)))
	ptr = ptr->lft;
      else
	{
	  if ((ptr->grpno == grpno) && (ptr->prty == prty))
	    break;
	  lstrt_node = ptr;
	  ptr = ptr->rt;
	}
    }                                                            

  if ((ptr == NULL) || (ptr->lft == NULL))
    return lstrt_node;
  else
    {
      ptr = ptr->lft;
      while (ptr->rt != NULL)
	ptr = ptr->rt;
      return ptr;
    }
}


/**************************************************************
Inserts node in the tree

Input: Node to be inserted
Output: None
Side effects: The node is inserted in its proper position
**************************************************************/
void
Insert_no_Balance(struct tree_node *ins_node)
{
  struct tree_node *ptr,	    /* Scratch pointers */
	*oldptr;
  register int grpno,		    /* Group no. to be searched */
	prty;		    /* Priority of entry to be searched */
  int lstlft;                      /* Flag for type of last branch */

  ins_node->lft = ins_node->rt = NULL;
  if (root == NULL)
    root = ins_node;
  else
    {
      grpno = ins_node->grpno;
      prty = ins_node->prty;
      ptr = root;
      while (ptr)
	{
	  oldptr = ptr;
	  ++comp_ins;
	  if (ptr->grpno < grpno)
	    {
	      ptr = ptr->lft;
	      lstlft = 1;
	    }
	  else if (ptr->grpno == grpno)
	    {
	      if (ptr->prty > prty)
		{
		  ptr = ptr->lft;
		  lstlft = 1;
		}
	      else if (ptr->prty < prty)
		{
		  ptr = ptr->rt;
		  lstlft = 0;
		}
	      else
		{
		  fprintf(stderr, "libcheetah: error in insert\n");
		  break;
		}
	    }
	  else
	    {
	      ptr = ptr->rt;
	      lstlft = 0;
	    }
	}
      if (lstlft)
	oldptr->lft = ins_node;
      else
	oldptr->rt = ins_node;
    }
}


/********************************************************************
Same as above but the inserted node is splayed to the top

Input: Node to be inserted
Output: None
Side effects: The node is added to the tree and splayed to the top
********************************************************************/
void
Insert_and_Balance(struct tree_node *ins_node)
{
  struct tree_node *ptr,	    /* Scratch pointers */
	*oldptr;
  int lstlft,                      /* Flag for type of last branch */
	grpno,			    /* Group no. to be searched */
	prty,			    /* Priority of entry to be searched */
	top,
	at;

  ins_node->lft = ins_node->rt = NULL;
  if (root == NULL)
    root = ins_node;
  else
    {
      if (root->lft == NULL)
	root->lft = ins_node;
      else
	{
	  grpno = ins_node->grpno;
	  prty = ins_node->prty;
	  ptr = root->lft;
	  top = 0;
	  while (ptr)
	    {
	      ++top;
	      p_stack[top] = ptr;
	      oldptr = ptr;
	      ++comp_ins;
	      if ((ptr->grpno < grpno)
		  || ((ptr->grpno == grpno) && (ptr->prty > prty)))
		{
		  ptr = ptr->lft;
		  lstlft = 1;
		}
	      else
		{
		  if ((ptr->grpno == grpno) && (ptr->prty == prty))
		    {
		      fprintf(stderr, "libcheetah: error in Insert\n");
		      break;
		    }
		  ptr = ptr->rt;
		  lstlft = 0;
		}
	    }
	  ++top;
	  p_stack[top] = ins_node;
	  if (lstlft)
	    oldptr->lft = ins_node;
	  else
	    oldptr->rt = ins_node;

	  /* Splay only if path > log(tot_addrs) */
	  if (top > log_tot_addrs)
	    {
	      at = top;
	      while (at > 1)
		{
		  ++no_splay_steps;   /* Count number of basic operations */
		  splay(at, p_stack);
		  at = at - 2;
		}
	      root->lft = p_stack[1];
	    }
	}
    }
}


/******************************************************************
Routine to decide whether to call Insert_no_Balance() or
Insert_and_Balance(). Currently calls the routines alternately.

Input: Node to be inserted
Output: None
Side effects: Calls an insertion rutine which inserts ins_node
and balances stack if required.
******************************************************************/
void
Insert(struct tree_node *ins_node)
{
  static short insert_flag = 0;

  if (insert_flag)
    Insert_and_Balance(ins_node);
  else
    Insert_no_Balance(ins_node);
  toggle(insert_flag);
}


/********************************************************************
Deletes node and returns a pointer to the previous node in the stack

Input: Entry to be deleted
Output: Pointer to entry deleted and
        pointer to prev entry (returned using prev_entry
Side effects: An entry is deleted from the stack
********************************************************************/
struct tree_node *
Delete(struct tree_node *entry, struct tree_node **prev_entry)
{
  register int grpno,
	prty;
  struct tree_node *ptr,     /* Scratch pointers */
	*oldptr_dn,
	*oldptr_mn,
	*dn = NULL,     /* Node deleted */
	*mn,	/* Node moved */
	*lstlft_node;
  int lstlft_dn,		/* Flags indicating type of last branch */
	lstlft_mn;

  grpno = entry->grpno;
  prty = entry->prty;
  ptr = root;
  while (ptr)
    {
      ++comp_del;
      if ((ptr->grpno < grpno)
	  || ((ptr->grpno == grpno ) && (ptr->prty > prty)))
	{
	  oldptr_dn = ptr;
	  lstlft_node = ptr;
	  ptr = ptr->lft;
	  lstlft_dn = 1;
	}
      else
	{
	  if ((ptr->grpno == grpno) && (ptr->prty == prty))
	    {
	      dn = ptr;
	      break;
	    }
	  oldptr_dn = ptr;
	  ptr = ptr->rt;
	  lstlft_dn = 0;
	}
    } /* while */
  if (dn == NULL)
    {
      fprintf(stderr, "libcheetah: error in delete\n");
      return (0);
    }
  else
    {
      if (ptr->rt == NULL)
	{
	  if (ptr == root)
	    root = ptr->lft;
	  else if (lstlft_dn)
	    oldptr_dn->lft = ptr->lft;
	  else
	    oldptr_dn->rt = ptr->lft;
	  *prev_entry = lstlft_node;
	}
      else if (ptr->lft == NULL)
	{
	  if (ptr == root)
	    root = ptr->rt;
	  else if (lstlft_dn)
	    oldptr_dn->lft = ptr->rt;
	  else
	    oldptr_dn->rt = ptr->rt;
	  ptr = ptr->rt;
	  while (ptr->lft != NULL)
	    ptr = ptr->lft;
	  *prev_entry = ptr;
	}
      else
	{
	  oldptr_mn = ptr;
	  ptr = ptr->rt;
	  lstlft_mn = 0;
	  while (ptr->lft != NULL)
	    {
	      oldptr_mn = ptr;
	      ptr = ptr->lft;
	      lstlft_mn = 1;
	    }
	  mn = ptr;
	  *prev_entry = mn;
	  if (lstlft_mn)
	    oldptr_mn->lft = mn->rt;
	  else
	    oldptr_mn->rt = mn->rt;
	  mn->lft = dn->lft;
	  mn->rt = dn->rt;
	  if (dn == root)
	    root = mn;
	  else if (lstlft_dn)
	    oldptr_dn->lft = mn;
	  else
	    oldptr_dn->rt = mn;
	}
      return dn;
    }
}


/**********************************************************
  Does the stack processing for each address in the trace for
  OPT replacement.

  The stack is maintained as a tree. A list of groups constituting
  the stack is maintained. This list is traversed by the procedure.
  It calls special tree handling routines to perform some stack operations
  and to maintain the group list.

  Input: address referenced and its priority(a fn of its time of next refn)
  Output: none
  Side effects: Updates the stack depth hit array
		Updates the group list and the tree.
**********************************************************/
int
process_groups(md_addr_t addr,		/* address referenced */
	       int priority)		/* new priority of address */
{
  int depth;		/* Stack depth counter */
  struct group_desc *grpptr,		/* Scratch pointer */
	*oldgrpptr,
	*newgrpptr;
  struct tree_node *prev_entry, *grpsecond;
  static struct tree_node dummy_for_del_line;
  /* One extra node is allocated for del_line which changes */
  static struct tree_node *del_line = &dummy_for_del_line;

  /* Hit at top */
  if (headgrp.first->addr == addr)
    {
      ++ out_stack[1];
      headgrp.first->prty = priority;
      headgrp.first->addr = addr;
    }
  else
    {
      /* Depth two hit */
      grpptr = headgrp.nxt;
      ++grps_passd;

      /* For inf_handler */
      if (headgrp.first->prty < 0)
	{
	  /* A kludge to set grptime correctly on a depth two hit */
	  if (headgrp.nxt->first->addr == addr)
	    unk_hash_add_fa(headgrp.first->addr,
			    (headgrp.nxt->grpno-1), headgrp.first->prty);
	  else
	    unk_hash_add_fa(headgrp.first->addr,
			    headgrp.nxt->grpno, headgrp.first->prty);
	}

      if (grpptr->first->addr == addr)
	{
	  ++out_stack[2];

	  if (grpptr->wt == 1)
	    {
	      /* One entry in first group */
	      grpptr->first->addr = headgrp.first->addr;
	      grpptr->first->prty = headgrp.first->prty;
	    }
	  else
	    {
	      grpsecond = Lookup_next (grpptr->first);
	      if (grpsecond->grpno != grpptr->grpno)
		fprintf(stderr, "libcheetah: inconsistent next entry\n");
	      if (grpsecond->prty < headgrp.first->prty)
		{
		  /* Coalescing. new group not needed */
		  grpptr->first->addr = headgrp.first->addr;
		  grpptr->first->prty = headgrp.first->prty;
		}
	      else
		{
		  --grp_ct;

		  /* A new group is created */
		  newgrpptr = calloc(1, sizeof (struct group_desc)) ;
		  if (!newgrpptr)
		    fatal("out of virtual memory");

		  newgrpptr->first = grpptr->first;
		  newgrpptr->last = grpptr->first;
		  newgrpptr->grpno = grp_ct;
		  newgrpptr->wt = 1;
		  newgrpptr->first->addr = headgrp.first->addr;
		  newgrpptr->first->prty = headgrp.first->prty;
		  newgrpptr->first->grpno = grp_ct;
		  newgrpptr->nxt = grpptr;
		  headgrp.nxt = newgrpptr;

		  /* adjust next group */
		  grpptr->first = grpsecond;
		  grpptr->wt -= 1;
		}
	    }
	  headgrp.first->addr = addr;
	  headgrp.first->prty = priority;
	}
      else
	{
	  /* Hit at second or higher group */
	  del_line->prty = headgrp.first->prty;
	  del_line->addr = headgrp.first->addr;
	  headgrp.first->addr = addr;
	  headgrp.first->prty = priority;

	  if (del_line->prty > grpptr->last->prty)
	    {
	      /* del_line enters group. last entry bcomes del_line */
	      del_line->grpno = grpptr->grpno;
	      Insert (del_line);
	      if (del_line->prty > grpptr->first->prty)
		grpptr->first = del_line;
	      del_line = Delete (grpptr->last, &prev_entry);
	      grpptr->last = prev_entry;
	      if (grpptr->last->grpno != grpptr->grpno)
		fprintf(stderr, "libcheetah: inconsistent prev entry\n");
	    }

	  oldgrpptr = grpptr;
	  depth = grpptr->wt + 1;
	  grpptr = grpptr->nxt;
   
	  while (grpptr != NULL)
	    {
	      ++grps_passd;
	      if (grpptr->first->addr == addr)
		{
		  /* Hit */
		  if ((unsigned)(depth+1) <= MAX_LINES)
		    ++out_stack[depth+1];

		  del_line->grpno = oldgrpptr->grpno;
		  oldgrpptr->last =
		    Lookup_Delete_Insert(grpptr->first, del_line);
		  oldgrpptr->wt += 1;

		  if (grpptr->first == grpptr->last)
		    {
		      /* delete group if it had only the refned entry */
		      oldgrpptr->nxt = grpptr->nxt;
		      free (grpptr);
		      grpptr = oldgrpptr; /* grpptr should not be NULL */
		    }
		  else
		    {
		      /* else set group-first to next entry */
		      grpptr->first  = Lookup_next(grpptr->first);
		      grpptr->wt -= 1;
		      if (grpptr->first->grpno != grpptr->grpno)
			fprintf(stderr,
				"libcheetah: inconsistent next entry\n");
		    }
		  break;
		}
	      else if (del_line->prty > grpptr->last->prty)
		{
		  /* del_line enters group. last entry bcomes del_line */
		  del_line->grpno = grpptr->grpno;
		  Insert (del_line);
		  if (del_line->prty > grpptr->first->prty)
		    grpptr->first = del_line;
		  del_line = Delete (grpptr->last, &prev_entry);

		  grpptr->last = prev_entry;
		  if (grpptr->last->grpno != grpptr->grpno)
		    fprintf(stderr, "libcheetah: inconsistent prev entry\n");
		}
	      oldgrpptr = grpptr;
	      depth += grpptr->wt;
	      grpptr = grpptr->nxt;
	    } /* while */
	} /* else */

      if (grpptr == NULL)
	{
	  /* refned address is new (compulsory miss)  */
	  /* If stack has overflowed, del_line is not added to the end
	     of the stack. Also, if del_line is an unknown, it is deleted
	     from the ft_hash_table as well. Unknowns are deleted from
	     the unknown hash table only during the cleaning step */
	  if (size_exceeded)
	    {
	      if (del_line->prty < 0)
		ft_hash_del(del_line->addr);
	    }
	  else
	    {
	      del_line->grpno = oldgrpptr->grpno;
	      Insert(del_line);
	      oldgrpptr->last = del_line;
	      oldgrpptr->wt += 1;
	      del_line = calloc(1, sizeof(struct tree_node));
	      if (!del_line)
		fatal("out of virtual memory");
	    }
	  return 1;
	}
    } /* else */

  return 0;
}


/**********************************************************************
Main simulation routine. Processes the addr_array and time_array obtained
from the pre-processing routine.

Input: The range in the addr_array (time_array) to be processed
Output: -1 if limit on the addresses to be processed is reached
	 0 otherwise
Side effects: The stack and group list are updated as the addresses are
	      preocessed.
**********************************************************************/
int
stack_proc_fa(int start,		/* index of starting array location */
	      int end)			/* index of ending array location */
{
   int i, l;
   unsigned addr;
   int priority, new_addrs;
   struct tree_node *nnode;

   if (t_entries == 0)
     {
       root = calloc(1, sizeof(struct tree_node));
       if (!root)
	 fatal("out of virtual memory");

       root->rt = root->lft = NULL;
       root->prty = Priority_fa(0);
       root->addr = addr_array[0];
       root->grpno = 0;

       l = 0;
       while ((addr_array[l] == addr_array[0]) && (l < end))
	 ++l;

       nnode = calloc(1, sizeof(struct tree_node));
       if (!nnode)
	 fatal("out of virtual memory");

       nnode->rt = nnode->lft = NULL;
       priority = Priority_fa(l-1);
       nnode->prty = priority;
       nnode->addr = root->addr;
       if (nnode->prty < 0)
	 unk_hash_add_fa(addr_array[l-1], MAXINT, priority);
       nnode->grpno = MAXINT;
       priority = Priority_fa(l);
       root->prty = priority;
       root->addr = addr_array[l];
       root->lft = nnode;
       headgrp.first = headgrp.last = root;
       headgrp.grpno = 0;

       headgrp.nxt = calloc(1, sizeof (struct group_desc));
       if (!headgrp.nxt)
	 fatal("out of virtual memory");

       headgrp.nxt->first = nnode;
       headgrp.nxt->last = nnode;
       headgrp.nxt->grpno = MAXINT;
       headgrp.nxt->wt = 1;
       headgrp.nxt->nxt = NULL;

       out_stack[1] += l-1;
       tot_addrs = 2;
       log_tot_addrs = 1;
       next_two_pwr = 4;
       for (i=0; i<=l; i++)
	 {
	   addr_array[i] = 0;
	   time_array[i] = 0;
	 }
       start = l+1;
       t_entries = l+1;
     }

   for (l=start; l<end; l++)
     {
       ++t_entries;
       addr = addr_array[l];
       if (t_entries > next_save_time)
	 {
	   outpr_facopt(stderr);
	   next_save_time += SAVE_INTERVAL;
	 }
       if (t_entries > next_clean_time)
	 {
	   if (size_exceeded)
	     hash_clean_fa();
	   next_clean_time += CLEAN_INTERVAL;
	 }

       if ((t_entries % P_INTERVAL) == 0)
	 fprintf(stderr, "libcheetah: addresses processed  %d\n", t_entries);

       priority = Priority_fa(l);

       if ((new_addrs = process_groups(addr, priority)) == 1)
	 {
	   ++tot_addrs;
	   if ((unsigned)tot_addrs > next_two_pwr)
	     {
	       ++log_tot_addrs;
	       next_two_pwr *= 2;
	     }
	   if (size_exceeded == 0)
	     {
	       if ((unsigned)tot_addrs > MAX_LINES)
		 {
		   toggle(size_exceeded);
		   fprintf(stderr,
			   "libcheetah: tot_addrs limit exceeded, "
			   "t_entries=%d\n",
			   t_entries);
		 }
	     }
	 }
       addr_array[l] = 0;
       time_array[l] = 0;
       if (t_entries > T)
	 return 1;
     } /* for */

   return 0;
}


/*********************************************************************
Initialization routine. Creates a root for the splay tree
and allocates space for the arrays.

Input: None
Output: None
Side effects: Creates a root for the tree. Adds the address to the splay tree.
*********************************************************************/
void
init_facopt(void)
{
  CLEAN_INTERVAL = 10000000;
  next_save_time = SAVE_INTERVAL;

  /* Stack is not cut off precisely at MAX_LINES */
  out_stack = calloc(MAX_LINES, sizeof (unsigned));
  if (!out_stack)
    fatal("out of virtual memory");

  p_stack = calloc((MAX_LINES+1000), sizeof (struct tree_node *));
  if (!p_stack)
    fatal("out of virtual memory");

  next_clean_time = CLEAN_INTERVAL;
}


/*********************************************************************
Inorder traversal using a stack.
Adapted from Harry Smith (page 214)

Starts at the bottom most entry of a group and searches upward for
the first node such that
1. It is that of the referenced address
2. It has a priority less than the del_prty but greater than search_prty.
   search_prty is the -ve old prty of the referenced address and the second
   second condition ensures that only entries that have interacted with
   the referenced address are rearranged.
   del_prty is the priority of the address deleted from
   the previous group and the first condition ensures that it displaces
   only entries of lower priority. For the first group del_prty is set
   to 0 which ensures that the entry deleted is of negative priority.

Input: last entry of group, dummy priority of refned unknown and
       dummy priority of deleted unknown
Output: Returns pointer to node matching the above condition
Side effects: None
************************************************************************/
struct tree_node *
Get_first_unknown(struct tree_node *entry, int search_prty, int del_prty)
{
  int grpno, prty, top;
  struct tree_node *ptr;

  grpno = entry->grpno;
  prty = entry->prty;
  ptr = root;
  top = 0;
  while (ptr)
    {
      if ((ptr->grpno < grpno)
	  || ((ptr->grpno == grpno) && (ptr->prty > prty)))
	{
	  ++top;
	  p_stack[top] = ptr;
	  ptr = ptr->lft;
	}
      else
	{
	  if ((ptr->grpno == grpno) && (ptr->prty == prty))
	    {
	      ++top;
	      p_stack[top] = ptr;
	      break;
	    }
	  ptr = ptr->rt;
	}
    }

  while (top > 0)
    {
      ptr = p_stack[top];
      --top;
      /* visit node */
      /* printf("%d  %d   %d\n", ptr->addr, ptr->grpno, ptr->prty); */
      if ((ptr->prty > 0) || (ptr->grpno != grpno))
	return NULL;
      else if ((ptr->prty == search_prty)
	       || ((ptr->prty < del_prty) && (ptr->prty > search_prty)))
	return ptr;

      ptr = ptr->rt;
      while (ptr)
	{
	  ++top;
	  p_stack[top] = ptr;
	  ptr = ptr->lft;
	}
    }

  /* should not get here... */
  return NULL;
}


/*********************************************************************
Given an entry in the stack returns the previous entry

Input: Pointer to entry
Output: Pointer to previous entry
Side effects: None
*********************************************************************/

struct tree_node *
Get_first_unknown_wobacking(int grpno, int search_prty, int del_prty)
{
  struct tree_node *ptr, *lstlft_node;

  ptr = root;
  while (ptr)
    {
      ++comp;
      if ((ptr->grpno < grpno)
	  || ((ptr->grpno == grpno) && (ptr->prty > search_prty)))
	{
	  lstlft_node = ptr;
	  ptr = ptr->lft;
	}
      else
	{
	  if ((ptr->grpno == grpno) && (ptr->prty == search_prty))
	    return ptr;
	  ptr = ptr->rt;
	}
    }                                                            

  if ((lstlft_node->grpno == grpno) && (lstlft_node->prty < del_prty))
    return lstlft_node;
  else
    return NULL;
}        


/********************************************************************
When an unknown is referenced this routine moves it to its right position
in the stack and rearranges the other unknowns as required

Input: Address and the current time of the pre-processing routine
Output: None
Side effects: The stack is rearranged and the group list is updated
	      if required.
*********************************************************************/
void
inf_handler_fa(md_addr_t addr, int cur_time)
{
  struct tree_node *line_to_be_ins, /* scratch pointers */
	*line_to_be_del,
	*prev_entry;
  struct group_desc *grpptr;	/* scratch */
  int addr_grptime,		/* group time marker for unknown */
	addr_prty;
  int priority,	/* fn of cur_time */
	del_prty;	/* prty of deleted entry (during rearrangement) */
  static struct tree_node dummy_for_del_line;
  /* One extra node is allocated for del_line which changes */
  static struct tree_node *del_line = &dummy_for_del_line;

  ++ihcount;

  priority = MAXINT - cur_time; /* assumes a priority function */

  if ((headgrp.first != NULL) && (headgrp.first->addr == addr))
    {
      if (headgrp.first->prty > 0)
	fprintf(stderr, "libcheetah: unknown has > 0 prty\n");
      else
	headgrp.first->prty = priority;
      return;
    }

  addr_grptime = unk_hash_del_fa (addr, &addr_prty);
  if (addr_grptime == 0)
    return;

  --unknowns;

  grpptr = headgrp.nxt; /* assumed that a unknown is not fixed
			   when it is at the top */

  /*printf ("inf_handler addr: %d %d\n", addr, addr_grptime);
    traverse(root);*/

  while (grpptr)
    {
      if (grpptr->grpno < addr_grptime)
	grpptr = grpptr->nxt;
      else
	break;
    }

  /* if (grpptr == NULL)
     printf ("Error in inf_handler: addr_grptime not found\n"); */

  while (grpptr)
    {
      if (grpptr->last->prty > 0)
	grpptr  = grpptr->nxt;
      else
	break;
    }

  /* if (grpptr == NULL)
     printf ("Error in inf_handler: no unknowns found\n"); */

  line_to_be_ins = del_line;
  line_to_be_ins->prty = priority;
  line_to_be_ins->addr = addr;

  del_prty = 0;
  while (grpptr)
    {
      line_to_be_del =
	Get_first_unknown_wobacking (grpptr->grpno, addr_prty, del_prty);
      if (line_to_be_del != NULL)
	{
	  line_to_be_ins->grpno = grpptr->grpno;
	  Insert(line_to_be_ins);
	  if (line_to_be_ins->prty > grpptr->first->prty)
	    grpptr->first = line_to_be_ins;
	  line_to_be_ins = Delete(line_to_be_del, &prev_entry);
	  if (grpptr->last == line_to_be_del)
	    grpptr->last = prev_entry;
	  if (line_to_be_del->prty == addr_prty)
	    break;
	  del_prty = line_to_be_ins->prty;
	}
      grpptr = grpptr->nxt;
    }
  del_line = line_to_be_ins;

  /* if (grpptr == NULL)
     printf ("Error in inf_handler: unknown not found\n"); */
}

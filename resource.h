/*
 * resource.h - resource manager interfaces
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
 * INTERNET: dburger@cs.wisc.edu
 * US Mail:  1210 W. Dayton Street, Madison, WI 53706
 *
 * $Id: resource.h,v 1.1.1.1 2000/11/29 14:53:54 cu-cs Exp $
 *
 * $Log: resource.h,v $
 * Revision 1.1.1.1  2000/11/29 14:53:54  cu-cs
 * Grand unification of arm sources.
 *
 *
 * Revision 1.1.1.1.2.1  2000/11/06 13:18:22  taustin
 * Fixed a commit bug.
 *
 * Added perfect disambiguation support.
 *
 * Revision 1.1.1.1  2000/05/26 15:18:58  taustin
 * SimpleScalar Tool Set
 *
 *
 * Revision 1.4  1998/08/27 15:54:19  taustin
 * implemented host interface description in host.h
 * added target interface support
 *
 * Revision 1.3  1997/03/11  01:26:49  taustin
 * updated copyright
 *
 * Revision 1.1  1996/12/05  18:50:23  taustin
 * Initial revision
 *
 *
 */

#ifndef RESOURCE_H
#define RESOURCE_H

#include <stdio.h>

/* maximum number of resource classes supported */
#define MAX_RES_CLASSES		16

/* maximum number of resource instances for a class supported */
#define MAX_INSTS_PER_CLASS	16

/* resource descriptor */
struct res_desc {
  char *name;				/* name of functional unit */
  int quantity;				/* total instances of this unit */
  int busy;				/* non-zero if this unit is busy */
  struct res_template {
    int class;				/* matching resource class: insts
					   with this resource class will be
					   able to execute on this unit */
    int oplat;				/* operation latency: cycles until
					   result is ready for use */
    int issuelat;			/* issue latency: number of cycles
					   before another operation can be
					   issued on this resource */
    struct res_desc *master;		/* master resource record */
  } x[MAX_RES_CLASSES];
};

/* resource pool: one entry per resource instance */
struct res_pool {
  char *name;				/* pool name */
  int num_resources;			/* total number of res instances */
  struct res_desc *resources;		/* resource instances */
  /* res class -> res template mapping table, lists are NULL terminated */
  int nents[MAX_RES_CLASSES];
  struct res_template *table[MAX_RES_CLASSES][MAX_INSTS_PER_CLASS];
};

/* create a resource pool */
struct res_pool *res_create_pool(char *name, struct res_desc *pool, int ndesc);

/* get a free resource from resource pool POOL that can execute a
   operation of class CLASS, returns a pointer to the resource template,
   returns NULL, if there are currently no free resources available,
   follow the MASTER link to the master resource descriptor;
   NOTE: caller is responsible for reseting the busy flag in the beginning
   of the cycle when the resource can once again accept a new operation */
struct res_template *res_get(struct res_pool *pool, int class);

/* dump the resource pool POOL to stream STREAM */
void res_dump(struct res_pool *pool, FILE *stream);

#endif /* RESOURCE_H */

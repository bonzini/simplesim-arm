/*
 * version.h - simulator suite version definitions
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
 * $Id: version.h,v 1.2 2000/11/29 15:12:53 cu-cs Exp $
 *
 * $Log: version.h,v $
 * Revision 1.2  2000/11/29 15:12:53  cu-cs
 * Updated versions for external release.
 *
 * Revision 1.1.1.1  2000/11/29 14:53:54  cu-cs
 * Grand unification of arm sources.
 *
 *
 * Revision 1.1.1.1.2.1  2000/07/21 18:30:55  taustin
 * More progress on the SimpleScalar/ARM target.
 *
 * Revision 1.1.1.1  2000/05/26 15:18:59  taustin
 * SimpleScalar Tool Set
 *
 *
 * Revision 1.5  1999/03/08 06:39:01  taustin
 * new version
 *
 * Revision 1.4  1998/08/27 16:42:15  taustin
 * added target designator
 * updated version
 *
 * Revision 1.3  1997/03/11  01:35:16  taustin
 * update copyright
 * version updated for second release
 *
 * Revision 1.1  1996/12/05  18:50:23  taustin
 * Initial revision
 *
 *
 */

#if defined(TARGET_PISA)
#define VER_TARGET		"PISA"
#elif defined(TARGET_ALPHA)
#define VER_TARGET		"Alpha"
#elif defined(TARGET_ARM)
#define VER_TARGET		"ARM"
#else
#error Cannot decode SimpleScalar target...
#endif

#define VER_MAJOR		3	/* third release */
#define VER_MINOR		0

#define VER_UPDATE	        "November, 2000"

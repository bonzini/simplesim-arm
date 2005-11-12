/*
 * exo-test.c - EXO library test driver
 *
 * This file is a part of the SimpleScalar tool suite written by
 * Todd M. Austin as a part of the Multiscalar Research Project.
 *
 * The tool suite is currently maintained by Doug Burger and Todd M. Austin.
 *
 * Copyright (C) 1997 by Todd M. Austin
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
 * $Id: exo-test.c,v 1.1.1.1 2000/11/29 14:53:54 cu-cs Exp $
 *
 * $Log: exo-test.c,v $
 * Revision 1.1.1.1  2000/11/29 14:53:54  cu-cs
 * Grand unification of arm sources.
 *
 *
 * Revision 1.1.1.1  2000/05/26 15:21:53  taustin
 * SimpleScalar Tool Set
 *
 *
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include "misc.h"
#include "options.h"
#include "libexo.h"

/* options database */
static struct opt_odb_t *odb;

/* dump help information */
static int help_me;

/* make internal defs */
static int make_defs;

/* EXO file to load */
static char *load_file;

/* EXO file to save */
static char *save_file;

/* print the EXO DB to stdout */
static int print_db;

/* EXO definition data base */
static struct exo_term_t *exo_db = NULL;

static void
usage(FILE *stream, int argc, char **argv)
{
  fprintf(stream, "Usage: %s {-options}\n", argv[0]);
  opt_print_help(odb, stream);
}

void
main(int argc, char **argv)
{
  /* build the command line options database */
  odb = opt_new(/* no orphan fn */NULL);
  opt_reg_flag(odb, "-h", "print help message",
               &help_me, /* default */FALSE, /* !print */FALSE, NULL);
  opt_reg_flag(odb, "-defs", "make internal defs",
               &make_defs, /* default */FALSE, /* print */TRUE, NULL);
  opt_reg_string(odb, "-load", "load an EXO file",
		 &load_file, /* default */NULL,
		 /* print */TRUE, /* format */NULL);
  opt_reg_string(odb, "-save", "save an EXO file",
		 &save_file, /* default */NULL,
		 /* print */TRUE, /* format */NULL);
  opt_reg_flag(odb, "-print", "print the EXO DB to stdout",
	       &print_db, /* default */FALSE,
	       /* print */TRUE, /* format */NULL);

  /* process the command line options */
  opt_process_options(odb, argc, argv);

  if (help_me)
    {
      /* print help message and exit */
      usage(stderr, argc, argv);
      exit(1);
    }

  /* print options used */
  opt_print_options(odb, stderr, /* short */TRUE, /* notes */TRUE);

  if (load_file)
    {
      ZFILE *exo_stream;
      struct exo_term_t *exo;

      exo_stream = myzfopen(load_file, "r");
      if (!exo_stream)
	fatal("could not open EXO file `%s'", load_file);

      while ((exo = exo_read(exo_stream->fd)) != NULL)
	exo_db = exo_chain(exo_db, exo);

      myzfclose(exo_stream);
    }

  if (make_defs)
    {
      struct exo_term_t *list, *array, *a, *b, *c, *d, *e, *f, *g, *h, *i;
      char *data = "This is a test to see if blobs really work...";
      char *data1 = "This is a test to see if blobs really work..."
	"This is a test to see if blobs really work..."
	  "This is a test to see if blobs really work..."
	    "This is a test to see if blobs really work..."
	      "This is a test to see if blobs really work..."
		"This is a test to see if blobs really work..."
		  "This is a test to see if blobs really work..."
		    "This is a test to see if blobs really work...";
      unsigned char data2[16] =
	{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };

      exo_db =
	exo_chain(exo_db, exo_new(ec_string, "** basic types tests **"));
      exo_db = exo_chain(exo_db, a = exo_new(ec_integer, (exo_integer_t)42));
      exo_db = exo_chain(b = exo_new(ec_float, (exo_float_t)42.0), exo_db);
      exo_db = exo_chain(exo_db, c = exo_new(ec_char, (int)'x'));
      exo_db = exo_chain(exo_db, exo_new(ec_char, (int)'\n'));
      exo_db = exo_chain(exo_db, exo_new(ec_char, (int)'\b'));
      exo_db = exo_chain(exo_db, exo_new(ec_char, (int)'\x02'));
      exo_db = exo_chain(exo_db, exo_new(ec_char, (int)'\xab'));
      exo_db = exo_chain(exo_db, exo_new(ec_string, "this is a test..."));
      exo_db =
	exo_chain(exo_db, d = exo_new(ec_string, "this is\na test...\n"));
      exo_db = exo_chain(exo_db, exo_new(ec_string, "a test... <<\\\b>>\n"));

      exo_db = exo_chain(exo_db, exo_new(ec_string, "** deep copy tests **"));
      exo_db = exo_chain(exo_db, exo_deepcopy(d));
      exo_db = exo_chain(exo_db, exo_deepcopy(c));
      exo_db = exo_chain(exo_db, exo_deepcopy(b));
      exo_db = exo_chain(exo_db, exo_deepcopy(a));

      exo_db = exo_chain(exo_db, exo_new(ec_string, "** list tests **"));
      exo_db =
	exo_chain(exo_db, exo_new(ec_list,
				  exo_deepcopy(d), exo_deepcopy(c),
				  exo_deepcopy(b), exo_deepcopy(a), NULL));
      exo_db = exo_chain(exo_db, exo_new(ec_list, NULL));
      exo_db =
	exo_chain(exo_db, exo_new(ec_list,
				  exo_new(ec_list, NULL),
				  exo_new(ec_list, NULL),
				  exo_new(ec_list, exo_deepcopy(a), NULL),
				  NULL));
      list = exo_deepcopy(a);
      list = exo_chain(list, exo_deepcopy(b));
      list = exo_chain(list, exo_deepcopy(a));
      list = exo_chain(list, exo_deepcopy(b));
      list = exo_chain(exo_deepcopy(c), list);
      exo_db = exo_chain(exo_db, e = exo_new(ec_list, list, NULL));
      exo_db = exo_chain(exo_db, exo_new(ec_list,
					 exo_deepcopy(e),
					 exo_new(ec_list, NULL),
					 exo_deepcopy(e),
					 exo_deepcopy(a), NULL));

      exo_db = exo_chain(exo_db, exo_new(ec_string, "** array tests **"));
      exo_db = exo_chain(exo_db, exo_new(ec_array, 16, NULL));
      f = array = exo_new(ec_array, 16, NULL);
      EXO_ARR(array, 2) = exo_deepcopy(e);
      EXO_ARR(array, 3) = exo_deepcopy(a);
      EXO_ARR(array, 4) = exo_deepcopy(c);
      EXO_ARR(array, 6) = exo_deepcopy(EXO_ARR(array, 2));
      EXO_ARR(array, 7) = exo_deepcopy(EXO_ARR(array, 1));
      exo_db = exo_chain(exo_db, array);
      exo_db =
	exo_chain(exo_db, exo_new(ec_array, 4,
				  exo_deepcopy(a),
				  exo_deepcopy(e),
				  exo_deepcopy(c),
				  exo_deepcopy(f),
				  NULL));

      exo_db = exo_chain(exo_db, exo_new(ec_string, "** token tests **"));
#define SYM1		1
#define SYM2		2
      exo_intern_as("sym1", SYM1);
      exo_intern_as("sym2", SYM2);
      g = exo_new(ec_token, "sym1"),
      exo_db = exo_chain(exo_db,
			 exo_new(ec_list,
				 g,
				 exo_new(ec_integer,
					 (exo_integer_t)
					 g->as_token.ent->token),
				 NULL));
      h = exo_new(ec_token, "sym2"),
      exo_db = exo_chain(exo_db,
			 exo_new(ec_list,
				 h,
				 exo_new(ec_integer,
					 (exo_integer_t)
					 h->as_token.ent->token),
				 NULL));
      i = exo_new(ec_token, "sym3"),
      exo_db = exo_chain(exo_db,
			 exo_new(ec_list,
				 i,
				 exo_new(ec_integer,
					 (exo_integer_t)
					 i->as_token.ent->token),
				 NULL));

      /* das blobs */
      exo_db = exo_chain(exo_db, exo_new(ec_blob, strlen(data), data));
      exo_db = exo_chain(exo_db, exo_new(ec_blob, strlen(data1), data1));
      exo_db = exo_chain(exo_db, exo_new(ec_blob, sizeof(data2), data2));
    }

  if (print_db)
    {
      struct exo_term_t *exo;

      /* emit header comment */
      fprintf(stdout, "\n/* EXO DB */\n\n");
      fprintf(stdout,
	      "/* EXO save file, file format version %d.%d */\n\n",
	      EXO_FMT_MAJOR, EXO_FMT_MINOR);

      /* emit all defs */
      for (exo=exo_db; exo != NULL; exo=exo->next)
	{
	  exo_print(exo, stdout);
	  fprintf(stdout, "\n\n");
	}
    }

  if (save_file)
    {
      ZFILE *exo_stream;
      struct exo_term_t *exo;

      exo_stream = myzfopen(save_file, "w");
      if (!exo_stream)
	fatal("could not open EXO file `%s'", save_file);

      /* emit header comment */
      fprintf(exo_stream->fd,
	      "/* EXO save file, file format version %d.%d */\n\n",
	      EXO_FMT_MAJOR, EXO_FMT_MINOR);

      /* emit all defs */
      for (exo=exo_db; exo != NULL; exo=exo->next)
	{
	  exo_print(exo, exo_stream->fd);
	  fprintf(exo_stream->fd, "\n\n");
	}
      myzfclose(exo_stream);
    }
}

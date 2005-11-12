/*
 * loader.c - program loader routines
 *
 * This file is a part of the SimpleScalar tool suite written by
 * Todd M. Austin as a part of the Multiscalar Research Project.
 *  
 * The tool suite is currently maintained by Doug Burger and Todd M. Austin.
 * 
 * Copyright (C) 1994, 1995, 1996, 1997 by Todd M. Austin
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
 * $Id: loader.c,v 1.1.1.1 2000/11/29 14:53:54 cu-cs Exp $
 *
 * $Log: loader.c,v $
 * Revision 1.1.1.1  2000/11/29 14:53:54  cu-cs
 * Grand unification of arm sources.
 *
 *
 * Revision 1.1.2.3  2000/08/12 20:15:32  taustin
 * Fixed loader problems.
 * Improved heap management.
 * Added improved bogus address checks.
 *
 * Revision 1.1.2.2  2000/08/02 08:52:15  taustin
 * SimpleScalar/ARM co-simulation component, based on the ARMulator.
 * More fixes to the SimpleScalar/ARM target.
 *
 * Revision 1.1.2.1  2000/07/21 18:34:59  taustin
 * More progress on the SimpleScalar/ARM target.
 *
 * Revision 1.1.1.1  2000/05/26 15:22:27  taustin
 * SimpleScalar Tool Set
 *
 *
 * Revision 1.3  1999/12/31 18:59:46  taustin
 * quad_t naming conflicts removed
 * cross-endian execution support added (w/ limited syscall support)
 *
 * Revision 1.2  1999/12/13 19:00:08  taustin
 * cross endian execution support added
 *
 * Revision 1.1  1998/08/27 16:54:16  taustin
 * Initial revision
 *
 * Revision 1.1  1998/05/06  01:08:39  calder
 * Initial revision
 *
 * Revision 1.6  1997/04/16  22:09:05  taustin
 * added standalone loader support
 *
 * Revision 1.5  1997/03/11  01:12:39  taustin
 * updated copyright
 * swapping supported disabled until it can be tested further
 *
 * Revision 1.4  1997/01/06  15:59:22  taustin
 * stat_reg calls now do not initialize stat variable values
 * ld_prog_fname variable exported
 *
 * Revision 1.3  1996/12/27  15:51:28  taustin
 * updated comments
 *
 * Revision 1.1  1996/12/05  18:52:32  taustin
 * Initial revision
 *
 *
 */

#include <stdio.h>
#include <stdlib.h>

#include "host.h"
#include "misc.h"
#include "machine.h"
#include "endian.h"
#include "regs.h"
#include "memory.h"
#include "sim.h"
#include "eio.h"
#include "loader.h"
#include "target-arm/elf.h"

/* amount of tail padding added to all loaded text segments */
#define TEXT_TAIL_PADDING 0 /* was: 128 */

/* program text (code) segment base and bound */
md_addr_t ld_text_base = 0xffffffff;
md_addr_t ld_text_bound = 0;

/* program text (code) size in bytes */
unsigned int ld_text_size = 0;

/* program initialized data segment base and bound */
md_addr_t ld_data_base = 0xffffffff;
md_addr_t ld_data_bound = 0;

/* top of the data segment */
md_addr_t ld_brk_point = 0;

/* program initialized ".data" and uninitialized ".bss" size in bytes */
unsigned int ld_data_size = 0;

/* program stack segment base (highest address in stack) */
md_addr_t ld_stack_base = 0;

/* program initial stack size */
unsigned int ld_stack_size = 0;

/* lowest address accessed on the stack */
md_addr_t ld_stack_min = -1;

/* program file name */
char *ld_prog_fname = NULL;

/* program entry point (initial PC) */
md_addr_t ld_prog_entry = 0;

/* program environment base address address */
md_addr_t ld_environ_base = 0;

/* target executable endian-ness, non-zero if big endian */
int ld_target_big_endian;

/* register simulator-specific statistics */
void
ld_reg_stats(struct stat_sdb_t *sdb)	/* stats data base */
{
  stat_reg_addr(sdb, "ld_text_base",
		"program text (code) segment base",
		&ld_text_base, ld_text_base, "0x%08p");
  stat_reg_addr(sdb, "ld_text_bound",
		"program text (code) segment bound",
		&ld_text_bound, ld_text_bound, "0x%08p");
  stat_reg_uint(sdb, "ld_text_size",
		"program text (code) size in bytes",
		&ld_text_size, ld_text_size, NULL);
  stat_reg_addr(sdb, "ld_data_base",
		"program initialized data segment base",
		&ld_data_base, ld_data_base, "0x%08p");
  stat_reg_addr(sdb, "ld_data_bound",
		"program initialized data segment bound",
		&ld_data_bound, ld_data_bound, "0x%08p");
  stat_reg_uint(sdb, "ld_data_size",
		"program init'ed `.data' and uninit'ed `.bss' size in bytes",
		&ld_data_size, ld_data_size, NULL);
  stat_reg_addr(sdb, "ld_stack_base",
		"program stack segment base (highest address in stack)",
		&ld_stack_base, ld_stack_base, "0x%08p");
#if 0 /* FIXME: broken... */
  stat_reg_addr(sdb, "ld_stack_min",
		"program stack segment lowest address",
		&ld_stack_min, ld_stack_min, "0x%08p");
#endif
  stat_reg_uint(sdb, "ld_stack_size",
		"program initial stack size",
		&ld_stack_size, ld_stack_size, NULL);
  stat_reg_addr(sdb, "ld_prog_entry",
		"program entry point (initial PC)",
		&ld_prog_entry, ld_prog_entry, "0x%08p");
  stat_reg_addr(sdb, "ld_environ_base",
		"program environment base address address",
		&ld_environ_base, ld_environ_base, "0x%08p");
  stat_reg_int(sdb, "ld_target_big_endian",
	       "target executable endian-ness, non-zero if big endian",
	       &ld_target_big_endian, ld_target_big_endian, NULL);
}


/* load program text and initialized data into simulated virtual memory
   space and initialize program segment range variables */
void
ld_load_prog(char *fname,		/* program to load */
	     int argc, char **argv,	/* simulated program cmd line args */
	     char **envp,		/* simulated program environment */
	     struct regs_t *regs,	/* registers to initialize for load */
	     struct mem_t *mem,		/* memory space to load prog into */
	     int zero_bss_segs)		/* zero uninit data segment? */
{
  int i;
  word_t temp;
  md_addr_t sp, data_break = 0, null_ptr = 0, argv_addr, envp_addr;
  FILE *fobj;
  struct elf_filehdr fhdr;
  struct elf_scnhdr shdr;
  byte_t *strtab, *buffer;

  if (eio_valid(fname))
    {
      if (argc != 1)
	{
	  fprintf(stderr, "error: EIO file has arguments\n");
	  exit(1);
	}

      fprintf(stderr, "sim: loading EIO file: %s\n", fname);

      sim_eio_fname = mystrdup(fname);

      /* open the EIO file stream */
      sim_eio_fd = eio_open(fname);

      /* load initial state checkpoint */
      if (eio_read_chkpt(regs, mem, sim_eio_fd) != -1)
	fatal("bad initial checkpoint in EIO file");

      /* load checkpoint? */
      if (sim_chkpt_fname != NULL)
	{
	  counter_t restore_icnt;

	  FILE *chkpt_fd;

	  fprintf(stderr, "sim: loading checkpoint file: %s\n",
		  sim_chkpt_fname);

	  if (!eio_valid(sim_chkpt_fname))
	    fatal("file `%s' does not appear to be a checkpoint file",
		  sim_chkpt_fname);

	  /* open the checkpoint file */
	  chkpt_fd = eio_open(sim_chkpt_fname);

	  /* load the state image */
	  restore_icnt = eio_read_chkpt(regs, mem, chkpt_fd);

	  /* fast forward the baseline EIO trace to checkpoint location */
	  myfprintf(stderr, "sim: fast forwarding to instruction %n\n",
		    restore_icnt);
	  eio_fast_forward(sim_eio_fd, restore_icnt);
	}

      /* computed state... */
      ld_environ_base = regs->regs_R[MD_REG_SP];
      ld_prog_entry = regs->regs_PC;

      /* fini... */
      return;
    }
#ifdef MD_CROSS_ENDIAN
  else
    {
      warn("endian of `%s' does not match host", fname);
      warn("running with experimental cross-endian execution support");
      warn("****************************************");
      warn("**>> please check results carefully <<**");
      warn("****************************************");
#if 0
      fatal("SimpleScalar/ARM only supports binary execution on\n"
	    "       little-endian hosts, use EIO files on big-endian hosts");
#endif
    }
#endif /* MD_CROSS_ENDIAN */

  if (sim_chkpt_fname != NULL)
    fatal("checkpoints only supported while EIO tracing");

  /* record profile file name */
  ld_prog_fname = argv[0];

  /* load the program into memory, try both endians */
#if defined(__CYGWIN32__) || defined(_MSC_VER)
  fobj = fopen(argv[0], "rb");
#else
  fobj = fopen(argv[0], "r");
#endif
  if (!fobj)
    fatal("cannot open executable `%s'", argv[0]);

  if (fread(&fhdr, sizeof(struct elf_filehdr), 1, fobj) < 1)
    fatal("cannot read header from executable `%s'", argv[0]);

  /* check if it is a valid ELF file */
  if (!(fhdr.e_ident[EI_MAG0] == 0x7f &&
	fhdr.e_ident[EI_MAG1] == 'E' &&
	fhdr.e_ident[EI_MAG2] == 'L' &&
	fhdr.e_ident[EI_MAG3] == 'F'))
    fatal("bad magic number in executable `%s' (not an executable)", argv[0]);

  /* check if the file is executable */
  if (fhdr.e_type != ET_EXEC)
    fatal("object file `%s' is not executable", argv[0]);

  /* check if the file is for ARM architecture */
  if (fhdr.e_machine != EM_ARM)
    fatal("executable file `%s' is not for the ARM architecture", argv[0]);

  ld_prog_entry = fhdr.e_entry;

  debug("number of sections in executable = %d\n", fhdr.e_shnum);
  debug("offset to section header table = %d", fhdr.e_shoff);

  /* seek to the beginning of the first section header, the file header comes
     first, followed by the optional header (this is the aouthdr), the size
     of the aouthdr is given in Fdhr.f_opthdr */
  fseek(fobj, fhdr.e_shoff + (fhdr.e_shstrndx * fhdr.e_shentsize), SEEK_SET);
  if (fread(&shdr, sizeof(struct elf_scnhdr), 1, fobj) < 1)
    fatal("error reading string section header from `%s'", argv[0]);

  /* allocate space for string table */
  strtab = (char *)calloc(shdr.sh_size, sizeof(char));
  if (!strtab)
    fatal("out of virtual memory");

  /* read the string table */
  if (fseek(fobj, shdr.sh_offset, SEEK_SET) < 0)
    fatal("cannot seek to string table section");
  if (fread(strtab, shdr.sh_size, 1, fobj) < 0)
    fatal("cannot read string table section");

  debug("size of string table = %d", shdr.sh_size);
  debug("type of section = %d", shdr.sh_type);
  debug("offset of string table in file = %d", shdr.sh_offset);

  debug("processing %d sections in `%s'...", fhdr.e_shnum, argv[0]);

  /* loop through the section headers */
  for (i=0; i < fhdr.e_shnum; i++)
    {
      buffer = NULL;

      if (fseek(fobj, fhdr.e_shoff + (i * fhdr.e_shentsize), SEEK_SET) < 0)
	fatal("could not reset location in executable");
      if (fread(&shdr, sizeof(struct elf_scnhdr), 1, fobj) < 1)
	fatal("could not read section %d from executable", i);

      /* make sure the file is static */
      if (shdr.sh_type == SHT_DYNAMIC || shdr.sh_type == SHT_DYNSYM)
	fatal("file is dynamically linked, compile with `-static'");

      debug("processing section `%s'...", &strtab[shdr.sh_name]);
      debug("  section flags = 0x%08x", shdr.sh_flags);	
      debug("  section type = %d", shdr.sh_type);
      debug("  section address = 0x%08x", shdr.sh_addr);
      debug("  section offset = %d", shdr.sh_offset);
      debug("  section size = %d", shdr.sh_size);
      debug("  section link = %d", shdr.sh_link);
      debug("  section extra info = %d", shdr.sh_info);
      debug("  section entry size = %d", shdr.sh_entsize);

      if (shdr.sh_addr != 0
	  && shdr.sh_size != 0
	  && (shdr.sh_type == SHT_PROGBITS || shdr.sh_type == SHT_NOBITS))
	{
	  /* if the ELF format designates an address for the section then
	     load it into memory */
	  debug("  processing section `%s'...", &strtab[shdr.sh_name]);

	  if ((shdr.sh_flags & SHF_ALLOC) != 0 && shdr.sh_type != SHT_NOBITS)
	    {
	      debug("  loading section `%s'...", &strtab[shdr.sh_name]);
	      /* go to the offset in file where section is located */
	      if (fseek(fobj, shdr.sh_offset, SEEK_SET) < 0)
		fatal("cannot file pointer");

	      /* allocate memory for the section contents */
	      buffer = (char *)calloc(shdr.sh_size, sizeof(byte_t));
	      if (!buffer)
		fatal("out of virtual memory");

	      /* read section into buffer */	
	      if (fread(buffer, shdr.sh_size, 1, fobj) < 1)
		fatal("could not read all the contents of section");

	      /* copy the section contents into simulator target memory */
	      mem_bcopy(mem_access, mem, Write, shdr.sh_addr,
			buffer, shdr.sh_size);
	    }

	  /* update program space limits */
	  if ((shdr.sh_flags & SHF_EXECINSTR) != 0)
	    {
	      debug("updating text size...");
	      if (shdr.sh_addr < ld_text_base)
		ld_text_base = shdr.sh_addr;
	      if ((shdr.sh_addr + shdr.sh_size) > ld_text_bound)
		ld_text_bound = shdr.sh_addr + shdr.sh_size;
	    }
	  else
	    {
	      debug("updating data size...");
	      if (shdr.sh_addr < ld_data_base)
		ld_data_base = shdr.sh_addr;
	      if ((shdr.sh_addr + shdr.sh_size) > ld_data_bound)
		ld_data_bound = shdr.sh_addr + shdr.sh_size;
	    }
	  ld_text_size = ld_text_bound - ld_text_base;
	  ld_data_size = ld_data_bound - ld_data_base;

	  /* release buffer storage */
	  if (buffer != NULL)
	    free(buffer);	
	}
    }

  /* release string table storage */
  free(strtab);

  /* perform sanity checks on segment ranges */
  if (!ld_text_base || !ld_text_size)
    fatal("executable is missing a `.text' section");
  if (!ld_data_base || !ld_data_size)
    fatal("executable is missing a `.data' section");
  if (!ld_prog_entry)
    fatal("program entry point not specified");

  /* determine byte/words swapping required to execute on this host */
  sim_swap_bytes = (endian_host_byte_order() != endian_target_byte_order());
  if (sim_swap_bytes)
    {
#if 0 /* FIXME: disabled until further notice... */
      /* cross-endian is never reliable, why this is so is beyond the scope
	 of this comment, e-mail me for details... */
      fprintf(stderr, "sim: *WARNING*: swapping bytes to match host...\n");
      fprintf(stderr, "sim: *WARNING*: swapping may break your program!\n");
      /* #else */
      fatal("binary endian does not match host endian");
#endif
    }
  sim_swap_words = (endian_host_word_order() != endian_target_word_order());
  if (sim_swap_words)
    {
#if 0 /* FIXME: disabled until further notice... */
      /* cross-endian is never reliable, why this is so is beyond the scope
	 of this comment, e-mail me for details... */
      fprintf(stderr, "sim: *WARNING*: swapping words to match host...\n");
      fprintf(stderr, "sim: *WARNING*: swapping may break your program!\n");
      /* #else */
      fatal("binary endian does not match host endian");
#endif
    }

  /* set up a local stack pointer, this is where the argv and envp
     data is written into program memory */
  /* ld_stack_base = ld_text_base - (409600+4096); */

  ld_stack_base = 0xc0000000;
#if 0
  sp = ROUND_DOWN(ld_stack_base - MD_MAX_ENVIRON, sizeof(MD_DOUBLE_TYPE));
#endif
  sp = ld_stack_base - MD_MAX_ENVIRON;
  ld_stack_size = ld_stack_base - sp;

  /* initial stack pointer value */
  ld_environ_base = sp;

  /* write [argc] to stack */
  temp = argc;
  mem_access(mem, Write, sp, &temp, sizeof(word_t));
  regs->regs_R[MD_REG_R1] = temp;
  sp += sizeof(word_t);

  /* skip past argv array and NULL */
  argv_addr = sp;
  regs->regs_R[MD_REG_R2] = argv_addr;
  sp = sp + (argc + 1) * sizeof(md_addr_t);

  /* save space for envp array and NULL */
  envp_addr = sp;
  for (i=0; envp[i]; i++)
    sp += sizeof(md_addr_t);
  sp += sizeof(md_addr_t);

  /* fill in the argv pointer array and data */
  for (i=0; i<argc; i++)
    {
      /* write the argv pointer array entry */
      temp = sp;
      mem_access(mem, Write, argv_addr + i*sizeof(md_addr_t),
		 &temp, sizeof(md_addr_t));
      /* and the data */
      mem_strcpy(mem_access, mem, Write, sp, argv[i]);
      sp += strlen(argv[i])+1;
    }
  /* terminate argv array with a NULL */
  mem_access(mem, Write, argv_addr + i*sizeof(md_addr_t),
	     &null_ptr, sizeof(md_addr_t));

  /* write envp pointer array and data to stack */
  for (i = 0; envp[i]; i++)
    {
      /* write the envp pointer array entry */
      temp = sp;
      mem_access(mem, Write, envp_addr + i*sizeof(md_addr_t),
		 &temp, sizeof(md_addr_t));
      /* and the data */
      mem_strcpy(mem_access, mem, Write, sp, envp[i]);
      sp += strlen(envp[i]) + 1;
    }
  /* terminate the envp array with a NULL */
  mem_access(mem, Write, envp_addr + i*sizeof(md_addr_t),
	     &null_ptr, sizeof(md_addr_t));

  /* did we tromp off the stop of the stack? */
  if (sp > ld_stack_base)
    {
      /* we did, indicate to the user that MD_MAX_ENVIRON must be increased,
	 alternatively, you can use a smaller environment, or fewer
	 command line arguments */
      fatal("environment overflow, increase MD_MAX_ENVIRON in arm.h");
    }

  /* initialize the bottom of heap to top of data segment */
  ld_brk_point = ROUND_UP(ld_data_base + ld_data_size, MD_PAGE_SIZE);

  /* set initial minimum stack pointer value to initial stack value */
  ld_stack_min = regs->regs_R[MD_REG_SP];

  regs->regs_R[MD_REG_SP] = ld_environ_base;
  regs->regs_PC = ld_prog_entry;

  debug("ld_text_base: 0x%08x  ld_text_size: 0x%08x",
	ld_text_base, ld_text_size);
  debug("ld_data_base: 0x%08x  ld_data_size: 0x%08x",
	ld_data_base, ld_data_size);
  debug("ld_stack_base: 0x%08x  ld_stack_size: 0x%08x",
	ld_stack_base, ld_stack_size);
  debug("ld_prog_entry: 0x%08x", ld_prog_entry);
}








#if 0

XXX
    if (fread(&ahdr, sizeof(struct ecoff_aouthdr), 1, fobj) < 1)
      fatal("cannot read AOUT header from executable `%s'", argv[0]);

    ld_text_base = ahdr.text_start;
    ld_text_size = ahdr.tsize;
    ld_prog_entry = ahdr.entry;
    ld_data_base = ahdr.data_start;
    ld_data_size = ahdr.dsize + ahdr.bsize;
    regs->regs_R[MD_REG_GP] = ahdr.gp_value;

    /* compute data segment size from data break point */
    data_break = ld_data_base + ld_data_size;
XXX

	switch (shdr.s_flags)
	  {
	  case ECOFF_STYP_TEXT:
	    p = calloc(shdr.s_size, sizeof(char));
	    if (!p)
	      fatal("out of virtual memory");

	    if (fseek(fobj, shdr.s_scnptr, 0) == -1)
	      fatal("could not read `.text' from executable", i);
	    if (fread(p, shdr.s_size, 1, fobj) < 1)
	      fatal("could not read text section from executable");

	    /* copy program section into simulator target memory */
	    mem_bcopy(mem_access, mem, Write,
		      shdr.s_vaddr, p, shdr.s_size);

#if 0
	    /* create tail padding and copy into simulator target memory */
	    mem_bzero(mem_access, mem, shdr.s_vaddr + shdr.s_size,
		      TEXT_TAIL_PADDING);
#endif

	    /* release the section buffer */
	    free(p);

#if 0
	    Text_seek = shdr.s_scnptr;
	    Text_start = shdr.s_vaddr;
	    Text_size = shdr.s_size / 4;
	    /* there is a null routine after the supposed end of text */
	    Text_size += 10;
	    Text_end = Text_start + Text_size * 4;
	    /* create_text_reloc(shdr.s_relptr, shdr.s_nreloc); */
#endif
	    break;

	  case ECOFF_STYP_INIT:
	  case ECOFF_STYP_FINI:
	    if (shdr.s_size > 0)
	      {
		p = calloc(shdr.s_size, sizeof(char));
		if (!p)
		  fatal("out of virtual memory");
		
		if (fseek(fobj, shdr.s_scnptr, 0) == -1)
		  fatal("could not read `.text' from executable", i);
		if (fread(p, shdr.s_size, 1, fobj) < 1)
		  fatal("could not read text section from executable");
		
		/* copy program section into simulator target memory */
		mem_bcopy(mem_access, mem, Write, shdr.s_vaddr,
			  p, shdr.s_size);
		
		/* release the section buffer */
		free(p);
	      }
	    else
	      warn("section `%s' is empty...", shdr.s_name);
	    break;

	  case ECOFF_STYP_LITA:
	  case ECOFF_STYP_LIT8:
	  case ECOFF_STYP_LIT4:
	  case ECOFF_STYP_XDATA:
	  case ECOFF_STYP_PDATA:
	  case ECOFF_STYP_RCONST:
	    /* fall through */

	  case ECOFF_STYP_RDATA:
	    /* The .rdata section is sometimes placed before the text
	     * section instead of being contiguous with the .data section.
	     */
#if 0
	    Rdata_start = shdr.s_vaddr;
	    Rdata_size = shdr.s_size;
	    Rdata_seek = shdr.s_scnptr;
#endif
	    /* fall through */
	  case ECOFF_STYP_DATA:
#if 0
	    Data_seek = shdr.s_scnptr;
#endif
	    /* fall through */
	  case ECOFF_STYP_SDATA:
#if 0
	    Sdata_seek = shdr.s_scnptr;
#endif
	    if (shdr.s_size > 0)
	      {
		p = calloc(shdr.s_size, sizeof(char));
		if (!p)
		  fatal("out of virtual memory");

		if (fseek(fobj, shdr.s_scnptr, 0) == -1)
		  fatal("could not read `.text' from executable", i);
		if (fread(p, shdr.s_size, 1, fobj) < 1)
		  fatal("could not read text section from executable");

		/* copy program section it into simulator target memory */
		mem_bcopy(mem_access, mem, Write, shdr.s_vaddr,
			  p, shdr.s_size);

		/* release the section buffer */
		free(p);
	      }
	    else
	      warn("section `%s' is empty...", shdr.s_name);
	  break;

	  case ECOFF_STYP_BSS:
	  case ECOFF_STYP_SBSS:
	    /* no data to read... */
	    break;

	  default:
	    warn("section `%s' ignored...", shdr.s_name);
	  }
      }

    /* done with the executable, close it */
    if (fclose(fobj))
      fatal("could not close executable `%s'", argv[0]);
#endif

/*
 * syscall.c - proxy system call handler routines
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
 * $Id: syscall.c,v 1.1.1.1 2000/11/29 14:53:54 cu-cs Exp $
 *
 * $Log: syscall.c,v $
 * Revision 1.1.1.1  2000/11/29 14:53:54  cu-cs
 * Grand unification of arm sources.
 *
 *
 * Revision 1.1.2.11  2000/11/08 20:30:23  kimns
 * Files necessary to compile Wattch Model ....
 * SimpleScalar/ARM
 *
 * Revision 1.1.2.10  2000/09/11 18:30:24  chriswea
 * finished CODING sockets for arm linux.. now debug
 *
 * Revision 1.1.2.9  2000/09/11 01:13:42  chriswea
 * added code for socket,bind and connect and stubs for other socket
 * functions under socketcall
 *
 * Revision 1.1.2.8  2000/09/10 22:14:38  chriswea
 * now compiles without the min_syscall_mode flag
 * syscall not in arm linux are still present but
 * an && 0 is added to allow it to compile all
 * socket calls are now unsupported because they is
 * actually only socketcall system call that in
 * turn calls the correct socket function
 *
 * Revision 1.1.2.7  2000/08/31 03:34:56  taustin
 * Fixed a bug in open() flag translation (messed up value for O_TRUNC).
 *
 * Revision 1.1.2.6  2000/08/29 14:18:28  taustin
 * Fixed word and qword size fp register accesses.
 * Addes time() and times() system call support (for Perl).
 *
 * Revision 1.1.2.5  2000/08/28 15:44:47  taustin
 * Added O_ASYNC open() support.
 *
 * Revision 1.1.2.4  2000/08/25 18:40:16  taustin
 * Started implementing funky PC semantics for ARM.
 *
 * Revision 1.1.2.3  2000/08/02 09:44:58  taustin
 * Fixed stat system call emulation.
 *
 * Revision 1.1.1.1  2000/05/26 15:22:27  taustin
 * SimpleScalar Tool Set
 *
 *
 * Revision 1.6  1999/12/31 19:00:40  taustin
 * quad_t naming conflicts removed
 * cross-endian execution support added (w/ limited syscall support)
 * extensive extensions to the Alpha OSF system call model
 *
 * Revision 1.5  1999/12/13 18:59:54  taustin
 * cross endian execution support added
 *
 * Revision 1.4  1999/03/08 06:40:09  taustin
 * added SVR4 host support
 *
 * Revision 1.3  1998/09/03 22:20:59  taustin
 * iov_len field in osf_iov fixed (was qword_t changed to word_t)
 * added portable padding to osf_iov type definition
 * fixed sigprocmask implementation (from Klauser)
 * usleep_thread() implementation added
 *
 * Revision 1.2  1998/08/31 17:19:44  taustin
 * ported to MS VC++
 * added FIONREAD ioctl() support
 * added support for socket(), select(), writev(), readv(), setregid()
 *     setreuid(), connect(), setsockopt(), getsockname(), getpeername(),
 *     setgid(), setuid(), getpriority(), setpriority(), shutdown(), poll()
 * change invalid system call from panic() to fatal()
 *
 * Revision 1.1  1998/08/27 16:54:53  taustin
 * Initial revision
 *
 * Revision 1.1  1998/05/06  01:08:39  calder
 * Initial revision
 *
 * Revision 1.5  1997/04/16  22:12:17  taustin
 * added Ultrix host support
 *
 * Revision 1.4  1997/03/11  01:37:37  taustin
 * updated copyright
 * long/int tweaks made for ALPHA target support
 * syscall structures are now more portable across platforms
 * various target supports added
 *
 * Revision 1.3  1996/12/27  15:56:09  taustin
 * updated comments
 * removed system prototypes
 *
 * Revision 1.1  1996/12/05  18:52:32  taustin
 * Initial revision
 *
 *
 */

#include <stdio.h>
#include <stdlib.h>

/* only enable a minimal set of systen call proxies if on limited
   hosts or if in cross endian live execution mode */
#ifndef MIN_SYSCALL_MODE
#if defined(_MSC_VER) || defined(__CYGWIN32__) || defined(MD_CROSS_ENDIAN)
#define MIN_SYSCALL_MODE
#endif
#endif /* !MIN_SYSCALL_MODE */

/* live execution only support on same-endian hosts... */
#ifdef _MSC_VER
#include <io.h>
#else /* !_MSC_VER */
#include <unistd.h>
#endif
#include <fcntl.h>
#include <sys/types.h>
#ifndef _MSC_VER
#include <sys/param.h>
#endif
#include <errno.h>
#include <time.h>
#ifndef _MSC_VER
#include <sys/time.h>
#endif
#ifndef _MSC_VER
#include <sys/resource.h>
#endif
#include <signal.h>
#ifndef _MSC_VER
#include <sys/file.h>
#endif
#include <sys/stat.h>
#ifndef _MSC_VER
#include <sys/uio.h>
#endif
#include <setjmp.h>
#ifndef _MSC_VER
#include <sys/times.h>
#endif
#include <limits.h>
#ifndef _MSC_VER
#include <sys/ioctl.h>
#endif
#if defined(linux)
#include <utime.h>
#include <dirent.h>
#include <sys/vfs.h>
#endif
#if defined(_AIX)
#include <sys/statfs.h>
#else /* !_AIX */
#ifndef _MSC_VER
#include <sys/mount.h>
#endif
#endif /* !_AIX */
#if !defined(linux) && !defined(sparc) && !defined(hpux) && !defined(__hpux) && !defined(__CYGWIN32__) && !defined(ultrix)
#ifndef _MSC_VER
#include <sys/select.h>
#endif
#endif
#ifdef linux
#include <sgtty.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <netinet/in.h>
#endif /* linux */

#if defined(__svr4__)
#include <sys/dirent.h>
#include <sys/filio.h>
#elif defined(__osf__)
#include <dirent.h>
/* -- For some weird reason, getdirentries() is not declared in any
 * -- header file under /usr/include on the Alpha boxen that I tried
 * -- SS-Alpha on. But the function exists in the libraries.
 */
int getdirentries(int fd, char *buf, int nbytes, long *basep);
#endif

#if defined(__svr4__) || defined(__osf__)
#include <sys/statvfs.h>
#define statfs statvfs
#include <sys/time.h>
#include <utime.h>
#include <sgtty.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#endif

#if defined(sparc) && defined(__unix__)
#if defined(__svr4__) || defined(__USLC__)
#include <dirent.h>
#else
#include <sys/dir.h>
#endif

/* dorks */
#undef NL0
#undef NL1
#undef CR0
#undef CR1
#undef CR2
#undef CR3
#undef TAB0
#undef TAB1
#undef TAB2
#undef XTABS
#undef BS0
#undef BS1
#undef FF0
#undef FF1
#undef ECHO
#undef NOFLSH
#undef TOSTOP
#undef FLUSHO
#undef PENDIN
#endif

#if defined(hpux) || defined(__hpux)
#undef CR0
#endif

#ifdef __FreeBSD__
#include <sys/ioctl_compat.h>
#else
#ifndef _MSC_VER
#include <termio.h>
#endif
#endif

#if defined(hpux) || defined(__hpux)
/* et tu, dorks! */
#undef HUPCL
#undef ECHO
#undef B50
#undef B75
#undef B110
#undef B134
#undef B150
#undef B200
#undef B300
#undef B600
#undef B1200
#undef B1800
#undef B2400
#undef B4800
#undef B9600
#undef B19200
#undef B38400
#undef NL0
#undef NL1
#undef CR0
#undef CR1
#undef CR2
#undef CR3
#undef TAB0
#undef TAB1
#undef BS0
#undef BS1
#undef FF0
#undef FF1
#undef EXTA
#undef EXTB
#undef B900
#undef B3600
#undef B7200
#undef XTABS
#include <sgtty.h>
#include <utime.h>
#endif

#ifdef __CYGWIN32__
#include <sys/unistd.h>
#include <sys/vfs.h>
#endif

#include <sys/socket.h>
#include <sys/poll.h>

#ifdef _MSC_VER
#define access		_access
#define chmod		_chmod
#define chdir		_chdir
#define unlink		_unlink
#define open		_open
#define creat		_creat
#define pipe		_pipe
#define dup		_dup
#define dup2		_dup2
#define stat		_stat
#define fstat		_fstat
#define lseek		_lseek
#define read		_read
#define write		_write
#define close		_close
#define getpid		_getpid
#define utime		_utime
#include <sys/utime.h>
#endif /* _MSC_VER */

#include "host.h"
#include "misc.h"
#include "machine.h"
#include "regs.h"
#include "memory.h"
#include "loader.h"
#include "sim.h"
#include "endian.h"
#include "eio.h"
#include "syscall.h"

/* Syscall numbers
	Got these numbers from /kernel/arch/arm/kernel/calls.S 
	Need to sync with kernel/include/asm-arm/unistd.h (Done!)
	Some system calls seem to be out of kernel V2.4 */

#define ARM_SYS_ni_syscall	0
#define ARM_SYS_exit			1
#define ARM_SYS_fork			2
#define ARM_SYS_read			3
#define ARM_SYS_write		4
#define ARM_SYS_open			5
#define ARM_SYS_close		6
#define ARM_SYS_waitpid		7
#define ARM_SYS_creat		8
#define ARM_SYS_link			9
#define ARM_SYS_unlink		10
#define ARM_SYS_execve		11
#define ARM_SYS_chdir		12
#define ARM_SYS_time			13
#define ARM_SYS_mknod		14
#define ARM_SYS_chmod		15
#define ARM_SYS_lchown16	16
/* 17 and 18 not defined???  in Kernel v2.4 */
#define ARM_SYS_break		17

/*#define ARM_SYS_oldstat		18*/

#define ARM_SYS_lseek		19
#define ARM_SYS_getpid		20
#define ARM_SYS_mount		21
#define ARM_SYS_umount		22
#define ARM_SYS_setuid		23
#define ARM_SYS_getuid		24
#define ARM_SYS_stime		25
#define ARM_SYS_ptrace		26
#define ARM_SYS_alarm		27

#define ARM_SYS_pause		29
#define ARM_SYS_utime		30
#define ARM_SYS_stty			31
#define ARM_SYS_gtty			32
#define ARM_SYS_access		33
#define ARM_SYS_nice			34
#define ARM_SYS_ftime		35
#define ARM_SYS_sync			36
#define ARM_SYS_kill			37
#define ARM_SYS_rename		38
#define ARM_SYS_mkdir		39
#define ARM_SYS_rmdir		40
#define ARM_SYS_dup			41
#define ARM_SYS_pipe			42
#define ARM_SYS_times		43
#define ARM_SYS_prof			44
#define ARM_SYS_brk			45
#define ARM_SYS_setgid		46
#define ARM_SYS_getgid		47
#define ARM_SYS_signal		48
#define ARM_SYS_geteuid		49
#define ARM_SYS_getegid		50
#define ARM_SYS_acct			51
#define ARM_SYS_umount2		52
#define ARM_SYS_lock			53
#define ARM_SYS_ioctl		54
#define ARM_SYS_fcntl		55
#define ARM_SYS_mpx			56
#define ARM_SYS_setpgid		57
#define ARM_SYS_ulimit		58

#define ARM_SYS_umask		60
#define ARM_SYS_chroot		61
#define ARM_SYS_ustat		62
#define ARM_SYS_dup2			63
#define ARM_SYS_getppid		64
#define ARM_SYS_getpgrp		65
#define ARM_SYS_setsid		66
#define ARM_SYS_sigaction	67
#define ARM_SYS_sgetmask	68
#define ARM_SYS_ssetmask	69
#define ARM_SYS_setreuid	70
#define ARM_SYS_setregid	71
#define ARM_SYS_sigsuspend	72
#define ARM_SYS_sigpending	73
#define ARM_SYS_sethostname	74
#define ARM_SYS_setrlimit	75
#define ARM_SYS_getrlimit	76
#define ARM_SYS_getrusage	77
#define ARM_SYS_gettimeofday	78
#define ARM_SYS_settimeofday	79
#define ARM_SYS_getgroups16	80
#define ARM_SYS_setgroups16	81
#define ARM_SYS_oldselect	82
#define ARM_SYS_symlink		83

#define ARM_SYS_readlink	85
#define ARM_SYS_uselib		86
#define ARM_SYS_swapon		87
#define ARM_SYS_reboot		88
#define ARM_SYS_readdir		89
#define ARM_SYS_old_mmap	90
#define ARM_SYS_munmap		91
#define ARM_SYS_truncate	92
#define ARM_SYS_ftruncate	93
#define ARM_SYS_fchmod		94
#define ARM_SYS_fchown16	95
#define ARM_SYS_getpriority	96
#define ARM_SYS_setpriority	97
#define ARM_SYS_profil		98
#define ARM_SYS_statfs		99
#define ARM_SYS_fstatfs		100
#define ARM_SYS_ioperm		101
#define ARM_SYS_socketcall	102
#define ARM_SYS_syslog		103
#define ARM_SYS_setitimer	104
#define ARM_SYS_getitimer	105
#define ARM_SYS_newstat		106
#define ARM_SYS_newlstat	107
#define ARM_SYS_newfstat	108


#define ARM_SYS_vhangup		111
#define ARM_SYS_idle			112
#define ARM_SYS_syscall		113
#define ARM_SYS_wait4		114
#define ARM_SYS_swapoff		115
#define ARM_SYS_sysinfo		116
#define ARM_SYS_ipc			117
#define ARM_SYS_fsync		118
#define ARM_SYS_sigreturn	119
#define ARM_SYS_clone		120
#define ARM_SYS_setdomainname	121
#define ARM_SYS_newuname	122
#define ARM_SYS_adjtimex	124
#define ARM_SYS_mprotect	125
#define ARM_SYS_sigprocmask	126
#define ARM_SYS_create_module	127
#define ARM_SYS_init_module	128
#define ARM_SYS_delete_module	129
#define ARM_SYS_get_kernel_syms	130
#define ARM_SYS_quotactl	131
#define ARM_SYS_getpgid		132
#define ARM_SYS_fchdir		133
#define ARM_SYS_bdflush		134
#define ARM_SYS_sysfs		135
#define ARM_SYS_personality	136
#define ARM_SYS_afs_syscall	137
#define ARM_SYS_setfsuid	138
#define ARM_SYS_setfsgid	139
#define ARM_SYS_llseek		140
#define ARM_SYS_getdents	141
#define ARM_SYS_select		142
#define ARM_SYS_flock		143
#define ARM_SYS_msync		144
#define ARM_SYS_readv		145
#define ARM_SYS_writev		146
#define ARM_SYS_getsid		147
#define ARM_SYS_fdatasync	148
#define ARM_SYS_sysctl		149
#define ARM_SYS_mlock		150
#define ARM_SYS_munlock		151
#define ARM_SYS_mlockall	152
#define ARM_SYS_munlockall	153
#define ARM_SYS_sched_setparam	154
#define ARM_SYS_sched_getparam	155
#define ARM_SYS_sched_setscheduler	156
#define ARM_SYS_sched_getscheduler	157
#define ARM_SYS_sched_yield	158
#define ARM_SYS_sched_get_priority_max	159
#define ARM_SYS_sched_get_priority_min	160
#define ARM_SYS_sched_rr_get_interval	161
#define ARM_SYS_nanosleep	162
#define ARM_SYS_mremap		163
#define ARM_SYS_setresuid16	164
#define ARM_SYS_getresuid16	165
#define ARM_SYS_vm86				166
#define ARM_SYS_query_module	167
#define ARM_SYS_poll			168
#define ARM_SYS_nfsservctl	169
#define ARM_SYS_setresgid16	170
#define ARM_SYS_getresgid16	171
#define ARM_SYS_prctl		172
#define ARM_SYS_rt_sigreturn	173
#define ARM_SYS_rt_sigaction	174
#define ARM_SYS_rt_sigprocmask	175
#define ARM_SYS_rt_sigpending	176
#define ARM_SYS_rt_sigtimedwait	177
#define ARM_SYS_rt_sigqueueinfo	178
#define ARM_SYS_rt_sigsuspend	179
#define ARM_SYS_pread		180
#define ARM_SYS_pwrite		181
#define ARM_SYS_chown16		182
#define ARM_SYS_getcwd		183
#define ARM_SYS_capget		184
#define ARM_SYS_capset		185
#define ARM_SYS_sigaltstack	186
#define ARM_SYS_sendfile	187


#define ARM_SYS_vfork		190
/* #define ARM_SYS_getrlimit	191 */
#define ARM_SYS_mmap2		192
#define ARM_SYS_truncate64	193
#define ARM_SYS_ftruncate64	194
#define ARM_SYS_stat64		195
#define ARM_SYS_lstat64		196
#define ARM_SYS_fstat64		197
#define ARM_SYS_lchown32	198
#define ARM_SYS_getuid32	199
#define ARM_SYS_getgid32	200
#define ARM_SYS_geteuid32	201
#define ARM_SYS_getegid32	202
#define ARM_SYS_setreuid32	203
#define ARM_SYS_setregid32	204
#define ARM_SYS_getgroups32	205
#define ARM_SYS_setgroups32	206
#define ARM_SYS_fchown32	207
#define ARM_SYS_setresuid32	208
#define ARM_SYS_getresuid32	209
#define ARM_SYS_setresgid32	210
#define ARM_SYS_getresgid32	211
#define ARM_SYS_chown32		212
#define ARM_SYS_setuid32	213
#define ARM_SYS_setgid32	214
#define ARM_SYS_setfsuid32	215
#define ARM_SYS_setfsgid32	216


/* These are the numbers for the socket function on a socketcall */
/* these were defined at /usr/include/sys/socketcall.h */
#define ARM_SYS_SOCKET      1
#define ARM_SYS_BIND        2
#define ARM_SYS_CONNECT     3
#define ARM_SYS_LISTEN      4
#define ARM_SYS_ACCEPT      5
#define ARM_SYS_GETSOCKNAME 6
#define ARM_SYS_GETPEERNAME 7
#define ARM_SYS_SOCKETPAIR  8
#define ARM_SYS_SEND        9
#define ARM_SYS_RECV        10
#define ARM_SYS_SENDTO      11
#define ARM_SYS_RECVFROM    12
#define ARM_SYS_SHUTDOWN    13
#define ARM_SYS_SETSOCKOPT  14
#define ARM_SYS_GETSOCKOPT  15
#define ARM_SYS_SENDMSG     16
#define ARM_SYS_RECVMSG     17

/* translate system call arguments */
struct xlate_table_t
{
  int target_val;
  int host_val;
};

int
xlate_arg(int target_val, struct xlate_table_t *map, int map_sz, char *name)
{
  int i;

  for (i=0; i < map_sz; i++)
    {
      if (target_val == map[i].target_val)
	return map[i].host_val;
    }

  /* not found, issue warning and return target_val */
  warn("could not translate argument for `%s': %d", name, target_val);
  return target_val;
}

/* internal system call buffer size, used primarily for file name arguments,
   argument larger than this will be truncated */
#define MAXBUFSIZE 		1024

/* total bytes to copy from a valid pointer argument for ioctl() calls,
   syscall.c does not decode ioctl() calls to determine the size of the
   arguments that reside in memory, instead, the ioctl() proxy simply copies
   NUM_IOCTL_BYTES bytes from the pointer argument to host memory */
#define NUM_IOCTL_BYTES		128

/* OSF ioctl() requests */
#define OSF_TIOCGETP		0x40067408
#define OSF_FIONREAD		0x4004667f

/* target stat() buffer definition, the host stat buffer format is
   automagically mapped to/from this format in syscall.c */
struct  linux_statbuf
{
  half_t linux_st_dev;
  half_t pad0;			/* to match Linux padding... */
  word_t linux_st_ino;
  half_t linux_st_mode;
  half_t linux_st_nlink;
  half_t linux_st_uid;
  half_t linux_st_gid;
  half_t linux_st_rdev;
  half_t pad1;
  word_t linux_st_size;
  word_t linux_st_blksize;
  word_t linux_st_blocks;
  word_t linux_st_atime;
  word_t pad2;
  word_t linux_st_mtime;
  word_t pad3;
  word_t linux_st_ctime;
  word_t pad4;
  word_t pad5;
  word_t pad6;
};

struct osf_sgttyb {
  byte_t sg_ispeed;	/* input speed */
  byte_t sg_ospeed;	/* output speed */
  byte_t sg_erase;	/* erase character */
  byte_t sg_kill;	/* kill character */
  shalf_t sg_flags;	/* mode flags */
};

#define OSF_NSIG		32

#define OSF_SIG_BLOCK		1
#define OSF_SIG_UNBLOCK		2
#define OSF_SIG_SETMASK		3

struct osf_sigcontext {
  qword_t sc_onstack;              /* sigstack state to restore */
  qword_t sc_mask;                 /* signal mask to restore */
  qword_t sc_pc;                   /* pc at time of signal */
  qword_t sc_ps;                   /* psl to retore */
  qword_t sc_regs[32];             /* processor regs 0 to 31 */
  qword_t sc_ownedfp;              /* fp has been used */
  qword_t sc_fpregs[32];           /* fp regs 0 to 31 */
  qword_t sc_fpcr;                 /* floating point control register */
  qword_t sc_fp_control;           /* software fpcr */
};

struct osf_statfs {
  shalf_t f_type;		/* type of filesystem (see below) */
  shalf_t f_flags;		/* copy of mount flags */
  word_t f_fsize;		/* fundamental filesystem block size */
  word_t f_bsize;		/* optimal transfer block size */
  word_t f_blocks;		/* total data blocks in file system, */
  /* note: may not represent fs size. */
  word_t f_bfree;		/* free blocks in fs */
  word_t f_bavail;		/* free blocks avail to non-su */
  word_t f_files;		/* total file nodes in file system */
  word_t f_ffree;		/* free file nodes in fs */
  qword_t f_fsid;		/* file system id */
  word_t f_spare[9];		/* spare for later */
};

struct osf_timeval
{
  sword_t osf_tv_sec;		/* seconds */
  sword_t osf_tv_usec;		/* microseconds */
};

struct osf_timezone
{
  sword_t osf_tz_minuteswest;	/* minutes west of Greenwich */
  sword_t osf_tz_dsttime;	/* type of dst correction */
};

/* target getrusage() buffer definition, the host stat buffer format is
   automagically mapped to/from this format in syscall.c */
struct osf_rusage
{
  struct osf_timeval osf_ru_utime;
  struct osf_timeval osf_ru_stime;
  sword_t osf_ru_maxrss;
  sword_t osf_ru_ixrss;
  sword_t osf_ru_idrss;
  sword_t osf_ru_isrss;
  sword_t osf_ru_minflt;
  sword_t osf_ru_majflt;
  sword_t osf_ru_nswap;
  sword_t osf_ru_inblock;
  sword_t osf_ru_oublock;
  sword_t osf_ru_msgsnd;
  sword_t osf_ru_msgrcv;
  sword_t osf_ru_nsignals;
  sword_t osf_ru_nvcsw;
  sword_t osf_ru_nivcsw;
};

struct osf_rlimit
{
  qword_t osf_rlim_cur;		/* current (soft) limit */
  qword_t osf_rlim_max;		/* maximum value for rlim_cur */
};

struct osf_sockaddr
{
  half_t sa_family;		/* address family, AF_xxx */
  byte_t sa_data[24];		/* 14 bytes of protocol address */
};

struct osf_iovec
{
  md_addr_t iov_base;		/* starting address */
  word_t iov_len;		/* length in bytes */
  word_t pad;
};

#if 0
/* returns size of DIRENT structure */
#define OSF_DIRENT_SZ(STR)						\
  (sizeof(word_t) + 2*sizeof(half_t) + strlen(STR) + 1)
#endif

struct osf_dirent
{
  word_t d_ino;			/* file number of entry */
  half_t d_reclen;		/* length of this record */
  half_t d_namlen;		/* length of string in d_name */
  char d_name[256];		/* DUMMY NAME LENGTH */
				/* the real maximum length is */
				/* returned by pathconf() */
                                /* At this time, this MUST */
                                /* be 256 -- the kernel */
                                /* requires it */
};

/* open(2) flags for Alpha/AXP OSF target, syscall.c automagically maps
   between these codes to/from host open(2) flags */
#define LINUX_O_RDONLY		00
#define LINUX_O_WRONLY		01
#define LINUX_O_RDWR		02
#define LINUX_O_CREAT		0100
#define LINUX_O_EXCL		0200
#define LINUX_O_NOCTTY		0400
#define LINUX_O_TRUNC		01000
#define LINUX_O_APPEND		02000
#define LINUX_O_NONBLOCK	04000
#define LINUX_O_SYNC		010000
#define LINUX_O_ASYNC		020000

/* open(2) flags translation table for SimpleScalar target */
struct {
  int linux_flag;
  int local_flag;
} linux_flag_table[] = {
  /* target flag */	/* host flag */
#ifdef _MSC_VER
  { LINUX_O_RDONLY,	_O_RDONLY },
  { LINUX_O_WRONLY,	_O_WRONLY },
  { LINUX_O_RDWR,	_O_RDWR },
  { LINUX_O_APPEND,	_O_APPEND },
  { LINUX_O_CREAT,	_O_CREAT },
  { LINUX_O_TRUNC,	_O_TRUNC },
  { LINUX_O_EXCL,	_O_EXCL },
#ifdef _O_NONBLOCK
  { LINUX_O_NONBLOCK,	_O_NONBLOCK },
#endif
#ifdef _O_NOCTTY
  { LINUX_O_NOCTTY,	_O_NOCTTY },
#endif
#ifdef _O_SYNC
  { LINUX_O_SYNC,	_O_SYNC },
#endif
#else /* !_MSC_VER */
  { LINUX_O_RDONLY,	O_RDONLY },
  { LINUX_O_WRONLY,	O_WRONLY },
  { LINUX_O_RDWR,	O_RDWR },
  { LINUX_O_APPEND,	O_APPEND },
  { LINUX_O_CREAT,	O_CREAT },
  { LINUX_O_TRUNC,	O_TRUNC },
  { LINUX_O_EXCL,	O_EXCL },
  { LINUX_O_NONBLOCK,	O_NONBLOCK },
  { LINUX_O_NOCTTY,	O_NOCTTY },
#ifdef O_SYNC
  { LINUX_O_SYNC,		O_SYNC },
#endif
#ifdef O_ASYNC
  { LINUX_O_ASYNC,	O_ASYNC },
#endif
#endif /* _MSC_VER */
};
#define LINUX_NFLAGS	(sizeof(linux_flag_table)/sizeof(linux_flag_table[0]))

qword_t sigmask = 0;

qword_t sigaction_array[OSF_NSIG] =
 {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

/* setsockopt option names */
#define OSF_SO_DEBUG		0x0001
#define OSF_SO_ACCEPTCONN	0x0002
#define OSF_SO_REUSEADDR	0x0004
#define OSF_SO_KEEPALIVE	0x0008
#define OSF_SO_DONTROUTE	0x0010
#define OSF_SO_BROADCAST	0x0020
#define OSF_SO_USELOOPBACK	0x0040
#define OSF_SO_LINGER		0x0080
#define OSF_SO_OOBINLINE	0x0100
#define OSF_SO_REUSEPORT	0x0200

struct xlate_table_t sockopt_map[] =
{
  { OSF_SO_DEBUG,	SO_DEBUG },
#ifdef SO_ACCEPTCONN
  { OSF_SO_ACCEPTCONN,	SO_ACCEPTCONN },
#endif
  { OSF_SO_REUSEADDR,	SO_REUSEADDR },
  { OSF_SO_KEEPALIVE,	SO_KEEPALIVE },
  { OSF_SO_DONTROUTE,	SO_DONTROUTE },
  { OSF_SO_BROADCAST,	SO_BROADCAST },
#ifdef SO_USELOOPBACK
  { OSF_SO_USELOOPBACK,	SO_USELOOPBACK },
#endif
  { OSF_SO_LINGER,	SO_LINGER },
  { OSF_SO_OOBINLINE,	SO_OOBINLINE },
#ifdef SO_REUSEPORT
  { OSF_SO_REUSEPORT,	SO_REUSEPORT }
#endif
};

/* setsockopt TCP options */
#define OSF_TCP_NODELAY		0x01 /* don't delay send to coalesce packets */
#define OSF_TCP_MAXSEG		0x02 /* maximum segment size */
#define OSF_TCP_RPTR2RXT	0x03 /* set repeat count for R2 RXT timer */
#define OSF_TCP_KEEPIDLE	0x04 /* secs before initial keepalive probe */
#define OSF_TCP_KEEPINTVL	0x05 /* seconds between keepalive probes */
#define OSF_TCP_KEEPCNT		0x06 /* num of keepalive probes before drop */
#define OSF_TCP_KEEPINIT	0x07 /* initial connect timeout (seconds) */
#define OSF_TCP_PUSH		0x08 /* set push bit in outbnd data packets */
#define OSF_TCP_NODELACK	0x09 /* don't delay send to coalesce packets */

struct xlate_table_t tcpopt_map[] =
{
  { OSF_TCP_NODELAY,	TCP_NODELAY },
  { OSF_TCP_MAXSEG,	TCP_MAXSEG },
#ifdef TCP_RPTR2RXT
  { OSF_TCP_RPTR2RXT,	TCP_RPTR2RXT },
#endif
#ifdef TCP_KEEPIDLE
  { OSF_TCP_KEEPIDLE,	TCP_KEEPIDLE },
#endif
#ifdef TCP_KEEPINTVL
  { OSF_TCP_KEEPINTVL,	TCP_KEEPINTVL },
#endif
#ifdef TCP_KEEPCNT
  { OSF_TCP_KEEPCNT,	TCP_KEEPCNT },
#endif
#ifdef TCP_KEEPINIT
  { OSF_TCP_KEEPINIT,	TCP_KEEPINIT },
#endif
#ifdef TCP_PUSH
  { OSF_TCP_PUSH,	TCP_PUSH },
#endif
#ifdef TCP_NODELACK
  { OSF_TCP_NODELACK,	TCP_NODELACK }
#endif
};

/* setsockopt level names */
#define OSF_SOL_SOCKET		0xffff	/* options for socket level */
#define OSF_SOL_IP		0	/* dummy for IP */
#define OSF_SOL_TCP		6	/* tcp */
#define OSF_SOL_UDP		17	/* user datagram protocol */

struct xlate_table_t socklevel_map[] =
{
#if defined(__svr4__) || defined(__osf__)
  { OSF_SOL_SOCKET,	SOL_SOCKET },
  { OSF_SOL_IP,		IPPROTO_IP },
  { OSF_SOL_TCP,	IPPROTO_TCP },
  { OSF_SOL_UDP,	IPPROTO_UDP }
#else
  { OSF_SOL_SOCKET,	SOL_SOCKET },
  { OSF_SOL_IP,		SOL_IP },
  { OSF_SOL_TCP,	SOL_TCP },
  { OSF_SOL_UDP,	SOL_UDP }
#endif
};

/* socket() address families */
#define OSF_AF_UNSPEC		0
#define OSF_AF_UNIX		1	/* Unix domain sockets */
#define OSF_AF_INET		2	/* internet IP protocol */
#define OSF_AF_IMPLINK		3	/* arpanet imp addresses */
#define OSF_AF_PUP		4	/* pup protocols: e.g. BSP */
#define OSF_AF_CHAOS		5	/* mit CHAOS protocols */
#define OSF_AF_NS		6	/* XEROX NS protocols */
#define OSF_AF_ISO		7	/* ISO protocols */

struct xlate_table_t family_map[] =
{
  { OSF_AF_UNSPEC,	AF_UNSPEC },
  { OSF_AF_UNIX,	AF_UNIX },
  { OSF_AF_INET,	AF_INET },
#ifdef AF_IMPLINK
  { OSF_AF_IMPLINK,	AF_IMPLINK },
#endif
#ifdef AF_PUP
  { OSF_AF_PUP,		AF_PUP },
#endif
#ifdef AF_CHAOS
  { OSF_AF_CHAOS,	AF_CHAOS },
#endif
#ifdef AF_NS
  { OSF_AF_NS,		AF_NS },
#endif
#ifdef AF_ISO
  { OSF_AF_ISO,		AF_ISO }
#endif
};

/* socket() socket types */
#define OSF_SOCK_STREAM		1	/* stream (connection) socket */
#define OSF_SOCK_DGRAM		2	/* datagram (conn.less) socket */
#define OSF_SOCK_RAW		3	/* raw socket */
#define OSF_SOCK_RDM		4	/* reliably-delivered message */
#define OSF_SOCK_SEQPACKET	5	/* sequential packet socket */

struct xlate_table_t socktype_map[] =
{
  { OSF_SOCK_STREAM,	SOCK_STREAM },
  { OSF_SOCK_DGRAM,	SOCK_DGRAM },
  { OSF_SOCK_RAW,	SOCK_RAW },
  { OSF_SOCK_RDM,	SOCK_RDM },
  { OSF_SOCK_SEQPACKET,	SOCK_SEQPACKET }
};

/* OSF table() call. Right now, we only support TBL_SYSINFO queries */
#define OSF_TBL_SYSINFO		12
struct osf_tbl_sysinfo 
{
  long si_user;		/* user time */
  long si_nice;		/* nice time */
  long si_sys;		/* system time */
  long si_idle;		/* idle time */
  long si_hz;
  long si_phz;
  long si_boottime;	/* boot time in seconds */
  long wait;		/* wait time */
};

struct linux_tms
{
  word_t tms_utime;		/* user CPU time */
  word_t tms_stime;		/* system CPU time */

  word_t tms_cutime;		/* user CPU time of dead children */
  word_t tms_cstime;		/* system CPU time of dead children */

};


/* ARM SYSTEM CALL CONVENTIONS

	System call conventions as taken from unistd.h of arm architecture
	Depending on the call type r0, r1, r2, r3, r4 will contain the arguments
	of the system call.
	The actual system call number is found out using the link register after 
	an swi call is made. How this is done is shown in 
	kernel/arch/arm/kernel/entry-common.S file
	We need to decode the instruction to find out the system call number.
	
	Return value is returned in register 0. If the return value is between -1
	and -125 inclusive then there's an error.
*/

/* syscall proxy handler, architect registers and memory are assumed to be
   precise when this function is called, register and memory are updated with
   the results of the sustem call */

void
sys_syscall(struct regs_t *regs,	/* registers to access */
	    mem_access_fn mem_fn,	/* generic memory accessor */
	    struct mem_t *mem,		/* memory space to access */
	    md_inst_t inst,		/* system call inst */
	    int traceable)		/* traceable system call? */
{
  /* Figure out the system call number from the swi instruction
	  Last 24 bits gives us this number */

  qword_t syscode = inst & 0x000fffff;


  /* first, check if an EIO trace is being consumed... */
  /* Left from Alpha code should be the same in ARM though */

  if (traceable && sim_eio_fd != NULL)
    {
      eio_read_trace(sim_eio_fd, sim_num_insn, regs, mem_fn, mem, inst);

#if XXX
      /* kludge fix for sigreturn(), it modifies all registers */
      if (syscode == ARM_SYS_sigreturn)
	{
	  int i;
	  struct osf_sigcontext sc;
	  md_addr_t sc_addr = regs->regs_R[MD_REG_R0]; /* change the register name to r0 (first arg) */

	  mem_bcopy(mem_fn, mem, Read, sc_addr, 
		    &sc, sizeof(struct osf_sigcontext));
	  regs->regs_NPC = sc.sc_pc;
	  for (i=0; i < 32; ++i)
	    regs->regs_R[i] = sc.sc_regs[i];
	  for (i=0; i < 32; ++i)
	    regs->regs_F.q[i] = sc.sc_fpregs[i];
	  regs->regs_C.fpcr = sc.sc_fpcr;
	}
#endif

      /* fini... */
      return;
    }

  /* no, OK execute the live system call... */

  switch (syscode)
    {
    case ARM_SYS_exit:
      /* exit jumps to the target set in main() */
      longjmp(sim_exit_buf,
	      /* exitcode + fudge */(regs->regs_R[MD_REG_R0] & 0xff) + 1);
      break;

    case ARM_SYS_personality:
      regs->regs_R[MD_REG_R0] = 0x1000000;
      break;

    case ARM_SYS_old_mmap:
      {
	static md_addr_t mmap_brk_point = 0xd0000000;

	if (/* flags */regs->regs_R[MD_REG_R3] !=
	    0x22 /* (MAP_PRIVATE|MAP_ANONYMOUS) */)
	  fatal("non-anonymous MMAP's not yet implemented");

	regs->regs_R[MD_REG_R0] = mmap_brk_point;
	mmap_brk_point += regs->regs_R[MD_REG_R1];

	if (verbose)
	  fprintf(stderr, "MMAP: 0x%08x -> 0x%08x, %d bytes...\n",
		  regs->regs_R[MD_REG_R0], mmap_brk_point,
		  regs->regs_R[MD_REG_R1]);
      }
      break;

    case ARM_SYS_read:
      {
	char *buf;
	int error_return;

	/* allocate same-sized input buffer in host memory */
	if (!(buf =
	      (char *)calloc(/*nbytes*/regs->regs_R[MD_REG_R2], sizeof(char))))
	  fatal("out of memory in SYS_read");

	/* read data from file */
	do {
	  /*nread*/error_return =
	    read(/*fd*/regs->regs_R[MD_REG_R0], buf,
	         /*nbytes*/regs->regs_R[MD_REG_R2]);
	    if (error_return <= -1 && error_return >= -125)
		 regs->regs_R[MD_REG_R0] = -errno;
	    else
		 regs->regs_R[MD_REG_R0] = error_return;
	} while (/*nread*/error_return == -1
	         && errno == EAGAIN);

	/* check for error condition, is not necessary to do this but lets keep it for now 
	   it is taken care of in the do-while loop */
	
	if (error_return != -1) {
		regs->regs_R[MD_REG_R0] = error_return;
	} /* no error */
	else {
	  	regs->regs_R[MD_REG_R0] = -(errno); /* negative of the error number is returned in r0 */
	}

	/* copy results back into host memory */
	mem_bcopy(mem_fn, mem, Write,
		  /*buf*/regs->regs_R[MD_REG_R1], buf, /*nread*/regs->regs_R[MD_REG_R2]);

	/* done with input buffer */
	free(buf);
      }
      break;

    case ARM_SYS_write:
      {
	char *buf;
	int error_return;

	/* allocate same-sized output buffer in host memory */
	if (!(buf =
	      (char *)calloc(/*nbytes*/regs->regs_R[MD_REG_R2], sizeof(char))))
	  fatal("out of memory in SYS_write");

	/* copy inputs into host memory */
	mem_bcopy(mem_fn, mem, Read, /*buf*/regs->regs_R[MD_REG_R1], buf,
		  /*nbytes*/regs->regs_R[MD_REG_R2]);

	/* write data to file */
	if (sim_progfd && MD_OUTPUT_SYSCALL(regs))
	  {
	    /* redirect program output to file */

	    /*nwritten*/error_return =
	      fwrite(buf, 1, /*nbytes*/regs->regs_R[MD_REG_R2], sim_progfd);
	  }
	else
	  {
	    /* perform program output request */
	    do {
	      /*nwritten*/error_return =
	        write(/*fd*/regs->regs_R[MD_REG_R0],
		      buf, /*nbytes*/regs->regs_R[MD_REG_R2]);
	    } while (/*nwritten*/error_return == -1 && errno == EAGAIN);
	  }

	/* check for an error condition */
	if (error_return <= -1)
	  regs->regs_R[MD_REG_R0] = -errno;
	else
	  regs->regs_R[MD_REG_R0] = error_return;

	/* done with output buffer */
	free(buf);
      }
      break;

      /* I can't find this system call in linux ?? ctw */
#if !defined(MIN_SYSCALL_MODE) && 0
      /* ADDED BY CALDER 10/27/99 */
    case ARM_SYS_getdomainname:
      /* get program scheduling priority */
      {
	char *buf;
	int error_return;

	buf = malloc(/* len */(size_t)regs->regs_R[MD_REG_R1]);
	if (!buf)
	  fatal("out of virtual memory in gethostname()");

	/* result */error_return =
	  getdomainname(/* name */buf,
		      /* len */(size_t)regs->regs_R[MD_REG_R1]);

	/* copy string back to simulated memory */
	mem_bcopy(mem_fn, mem, Write,
		  /* name */regs->regs_R[MD_REG_R0],
		  buf, /* len */regs->regs_R[MD_REG_R1]);
      }

	/* check for an error condition */
	if (error_return != -1) {
		regs->regs_R[MD_REG_R0] = error_return;
	} /* no error */
	else {
	  	regs->regs_R[MD_REG_R0] = -(errno); /* negative of the error number is returned in r0 */
	}
      break;
#endif

#if !defined(MIN_SYSCALL_MODE)
      /* ADDED BY CALDER 10/27/99 */
    case ARM_SYS_flock:
      /* get flock() information on the file */
      {
	int error_return;
	
	error_return =
	  flock(/*fd*/(int)regs->regs_R[MD_REG_R0],
		/*cmd*/(int)regs->regs_R[MD_REG_R1]);

	/* check for an error condition */
	if (error_return != -1) {
		regs->regs_R[MD_REG_R0] = error_return;
	} /* no error */
	else {
	  	regs->regs_R[MD_REG_R0] = -(errno); /* negative of the error number is returned in r0 */
	}
	}
      
      break;
#endif

      /*-------------------------------Is there a bind in arm-linux??----------------------*/
#if !defined(MIN_SYSCALL_MODE) && 0
      /* ADDED BY CALDER 10/27/99 */
    case OSF_SYS_bind:
      {
	const struct sockaddr a_sock;

	mem_bcopy(mem_fn, mem, Read, /* serv_addr */regs->regs_R[MD_REG_A1],
		  &a_sock, /* addrlen */(int)regs->regs_R[MD_REG_A2]);

      regs->regs_R[MD_REG_V0] =
	bind((int) regs->regs_R[MD_REG_A0],
	     &a_sock,(int) regs->regs_R[MD_REG_A2]);

      /* check for an error condition */
      if (regs->regs_R[MD_REG_V0] != (word_t)-1)
	regs->regs_R[MD_REG_A3] = 0;
      else /* got an error, return details */
	{
	  regs->regs_R[MD_REG_A3] = -1;
	  regs->regs_R[MD_REG_V0] = errno;
	}
      }
      break;
#endif
      /*----------------------------------------------------------------------------------------*/

      /*-------------------------------Is there a sendto in arm-linux??----------------------*/
#if !defined(MIN_SYSCALL_MODE) && 0
      /* ADDED BY CALDER 10/27/99 */
    case OSF_SYS_sendto:
      {
	char *buf = NULL;
	struct sockaddr d_sock;
	int buf_len = 0;

	buf_len = regs->regs_R[MD_REG_A2];

	if (buf_len > 0)
	  buf = (char *) malloc(buf_len*sizeof(char));

	mem_bcopy(mem_fn, mem, Read, /* serv_addr */regs->regs_R[MD_REG_A1],
		  buf, /* addrlen */(int)regs->regs_R[MD_REG_A2]);

	if (regs->regs_R[MD_REG_A5] > 0) 
	  mem_bcopy(mem_fn, mem, Read, regs->regs_R[MD_REG_A4],
		    &d_sock, (int)regs->regs_R[MD_REG_A5]);

	regs->regs_R[MD_REG_V0] =
	  sendto((int) regs->regs_R[MD_REG_A0],
		 buf,(int) regs->regs_R[MD_REG_A2],
		 (int) regs->regs_R[MD_REG_A3],
		 &d_sock,(int) regs->regs_R[MD_REG_A5]);

	mem_bcopy(mem_fn, mem, Write, /* serv_addr */regs->regs_R[MD_REG_A1],
		  buf, /* addrlen */(int)regs->regs_R[MD_REG_A2]);

	/* maybe copy back whole size of sockaddr */
	if (regs->regs_R[MD_REG_A5] > 0)
	  mem_bcopy(mem_fn, mem, Write, regs->regs_R[MD_REG_A4],
		    &d_sock, (int)regs->regs_R[MD_REG_A5]);

	/* check for an error condition */
	if (regs->regs_R[MD_REG_V0] != (word_t)-1)
	  regs->regs_R[MD_REG_A3] = 0;
	else /* got an error, return details */
	  {
	    regs->regs_R[MD_REG_A3] = -1;
	    regs->regs_R[MD_REG_V0] = errno;
	  }

	if (buf != NULL) 
	  free(buf);
      }
      break;
#endif
      /*----------------------------------------------------------------------------------------*/

      /*-------------------------------Is there a recvfrom in arm-linux??----------------------*/
#if !defined(MIN_SYSCALL_MODE) && 0
      /* ADDED BY CALDER 10/27/99 */
    case OSF_SYS_old_recvfrom:
    case OSF_SYS_recvfrom:
      {
	int addr_len;
	char *buf;
	struct sockaddr *a_sock;
      
	buf = (char *) malloc(sizeof(char)*regs->regs_R[MD_REG_A2]);

	mem_bcopy(mem_fn, mem, Read, /* serv_addr */regs->regs_R[MD_REG_A1],
		  buf, /* addrlen */(int)regs->regs_R[MD_REG_A2]);

	mem_bcopy(mem_fn, mem, Read, /* serv_addr */regs->regs_R[MD_REG_A5],
		  &addr_len, sizeof(int));

	a_sock = (struct sockaddr *)malloc(addr_len);

	mem_bcopy(mem_fn, mem, Read, regs->regs_R[MD_REG_A4],
		  a_sock, addr_len);

	regs->regs_R[MD_REG_V0] =
	  recvfrom((int) regs->regs_R[MD_REG_A0],
		   buf,(int) regs->regs_R[MD_REG_A2],
		   (int) regs->regs_R[MD_REG_A3], a_sock,&addr_len);

	mem_bcopy(mem_fn, mem, Write, regs->regs_R[MD_REG_A1],
		  buf, (int) regs->regs_R[MD_REG_V0]);

	mem_bcopy(mem_fn, mem, Write, /* serv_addr */regs->regs_R[MD_REG_A5],
		  &addr_len, sizeof(int));

	mem_bcopy(mem_fn, mem, Write, regs->regs_R[MD_REG_A4],
		  a_sock, addr_len);

	/* check for an error condition */
	if (regs->regs_R[MD_REG_V0] != (word_t)-1)
	  regs->regs_R[MD_REG_A3] = 0;
	else /* got an error, return details */
	  {
	    regs->regs_R[MD_REG_A3] = -1;
	    regs->regs_R[MD_REG_V0] = errno;
	  }
	if (buf != NULL)
	  free(buf);
      }
      break;
#endif
      /*----------------------------------------------------------------------------------------*/

    case ARM_SYS_open:
      {
	char buf[MAXBUFSIZE];
	unsigned int i;
	int error_return;
	int linux_flags = regs->regs_R[MD_REG_R1], local_flags = 0;

	/* Need to make sure if these flags are the same in ARM */
	/* translate open(2) flags */
	for (i=0; i < LINUX_NFLAGS; i++)
	  {
	    if (linux_flags & linux_flag_table[i].linux_flag)
	      {
		linux_flags &= ~linux_flag_table[i].linux_flag;
		local_flags |= linux_flag_table[i].local_flag;
	      }
	  }
	/* any target flags left? */
	if (linux_flags != 0)
	  fatal("syscall: open: cannot decode flags: 0x%08x", linux_flags);

	/* copy filename to host memory */
	mem_strcpy(mem_fn, mem, Read, /*fname*/regs->regs_R[MD_REG_R0], buf);

	/* open the file */
#ifdef __CYGWIN32__
	/*fd*/error_return =
	  open(buf, local_flags|O_BINARY, /*mode*/regs->regs_R[MD_REG_R2]);
#else /* !__CYGWIN32__ */
	/*fd*/error_return =
	  open(buf, local_flags, /*mode*/regs->regs_R[MD_REG_R2]);
#endif /* __CYGWIN32__ */
	
	/* check for an error condition */
	if (error_return != -1) {
		regs->regs_R[MD_REG_R0] = error_return;
	  } /* no error */
	else {
	  	regs->regs_R[MD_REG_R0] = -(errno); /* negative of the error number is returned in r0 */
	  }
      }
      break;

    case ARM_SYS_close:
	{
	int error_return;
      /* don't close stdin, stdout, or stderr as this messes up sim logs */
      if (/*fd*/regs->regs_R[MD_REG_R0] == 0
	  || /*fd*/regs->regs_R[MD_REG_R0] == 1
	  || /*fd*/regs->regs_R[MD_REG_R0] == 2)
	{
	  regs->regs_R[MD_REG_R0] = 0;
	  break;
	}

      /* close the file */
      error_return = close(/*fd*/regs->regs_R[MD_REG_R0]);

      /* check for an error condition */
      if (error_return != -1) {
		regs->regs_R[MD_REG_R0] = error_return;
	} /* no error */
	else {
	  	regs->regs_R[MD_REG_R0] = -(errno); /* negative of the error number is returned in r0 */
	}
	}
      break;

#if 0
    case ARM_SYS_creat:
      {
	char buf[MAXBUFSIZE];
	int error_return;

	/* copy filename to host memory */
	mem_strcpy(mem_fn, Read, /*fname*/regs->regs_R[MD_REG_R0], buf);

	/* create the file */
	/*fd*/error_return =
	  creat(buf, /*mode*/regs->regs_R[MD_REG_R1]);

	/* check for an error condition */
	if (error_return != -1) {
		regs->regs_R[MD_REG_R0] = error_return;
	  } /* no error */
	else {
	  	regs->regs_R[MD_REG_R0] = -(errno); /* negative of the error number is returned in r0 */
	  }
      }
      break;
#endif

    case ARM_SYS_unlink:
      {
	char buf[MAXBUFSIZE];
	int error_return;

	/* copy filename to host memory */
	mem_strcpy(mem_fn, mem, Read, /*fname*/regs->regs_R[MD_REG_R0], buf);

	/* delete the file */
	/*result*/error_return = unlink(buf);

	/* check for an error condition */
	if (error_return != -1) {
		regs->regs_R[MD_REG_R0] = error_return;
	  } /* no error */
	else {
	  	regs->regs_R[MD_REG_R0] = -(errno); /* negative of the error number is returned in r0 */
	  }
      }
      break;

    case ARM_SYS_chdir:
      {
	char buf[MAXBUFSIZE];
	int error_return;

	/* copy filename to host memory */
	mem_strcpy(mem_fn, mem, Read, /*fname*/regs->regs_R[MD_REG_R0], buf);

	/* change the working directory */
	/*result*/error_return = chdir(buf);

	/* check for an error condition */
	if (error_return != -1) {
		regs->regs_R[MD_REG_R0] = error_return; /* normal return value */
	  } /* no error */
	else {
	  	regs->regs_R[MD_REG_R0] = -(errno); /* negative of the error number is returned in r0 */
	  }
      }
      break;

    case ARM_SYS_time:
      {
	time_t intime = regs->regs_R[MD_REG_R0];
	word_t tval = (word_t)time(&intime);
	fprintf(stderr, "time(%d) = %d\n", (int)intime, tval);

	/* check for an error condition */
	if (tval != (word_t)-1)
	  regs->regs_R[MD_REG_R0] = tval; /* normal return value */
	else
	  regs->regs_R[MD_REG_R0] = -(errno); /* negative of the errnum */
      }
      break;

    case ARM_SYS_chmod:
      {
	char buf[MAXBUFSIZE];

	/* copy filename to host memory */
	mem_strcpy(mem_fn, mem, Read, /*fname*/regs->regs_R[MD_REG_R0], buf);

	/* chmod the file */
	/*result*/regs->regs_R[MD_REG_R0] =
	  chmod(buf, /*mode*/regs->regs_R[MD_REG_A1]);

	/* check for an error condition */
	if (regs->regs_R[MD_REG_R0] == (word_t)-1)
	  /* got an error, return details */
	    regs->regs_R[MD_REG_R0] = -errno;
	  
      }
      break;

#if XXX
    case ARM_SYS_chown:
#ifdef _MSC_VER
      warn("syscall chown() not yet implemented for MSC...");
      regs->regs_R[MD_REG_R0] = 0;
#else /* !_MSC_VER */
      {
	char buf[MAXBUFSIZE];

	/* copy filename to host memory */
	mem_strcpy(mem_fn, mem,Read, /*fname*/regs->regs_R[MD_REG_R0], buf);

	/* chown the file */
	/*result*/regs->regs_R[MD_REG_R0] =
	  chown(buf, /*owner*/regs->regs_R[MD_REG_R1],
		/*group*/regs->regs_R[MD_REG_R2]);

	/* check for an error condition */
	if (regs->regs_R[MD_REG_R0] == (word_t)-1)
	  /* got an error, return details */
	    regs->regs_R[MD_REG_R0] = -errno;
      }
#endif /* _MSC_VER */
      break;
#endif

    case ARM_SYS_brk:
      {
	md_addr_t addr = regs->regs_R[MD_REG_R0];

	if (verbose)
	  myfprintf(stderr, "SYS_sbrk: addr: 0x%08p\n", addr);

	if (addr != 0)
	  {
	    ld_brk_point = addr;
	    regs->regs_R[MD_REG_V0] = ld_brk_point;

	    /* check whether heap area has merged with stack area */
	    if (addr >= regs->regs_R[MD_REG_SP])
	      {
		/* out of address space, indicate error */
		regs->regs_R[MD_REG_R0] = -ENOMEM;
	      }
	  }
	else /* just return break point */
	  regs->regs_R[MD_REG_V0] = ld_brk_point;

	if (verbose)
	  myfprintf(stderr, "ld_brk_point: 0x%08p\n", ld_brk_point);
      }
      break;

      /*-------------------------------Is there a obreak in arm-linux??----------------------*/
#if XXX
    case OSF_SYS_obreak:
      {
        md_addr_t addr;

        /* round the new heap pointer to the its page boundary */
#if 0
        addr = ROUND_UP(/*base*/regs->regs_R[MD_REG_A0], MD_PAGE_SIZE);
#endif
        addr = /*base*/regs->regs_R[MD_REG_A0];

	if (verbose)
	  myfprintf(stderr, "SYS_obreak: addr: 0x%012p\n", addr);

	ld_brk_point = addr;
	regs->regs_R[MD_REG_V0] = ld_brk_point;
	regs->regs_R[MD_REG_A3] = 0;

	if (verbose)
	  myfprintf(stderr, "ld_brk_point: 0x%012p\n", ld_brk_point);
      }
      break;
      /*----------------------------------------------------------------------------------------*/
#endif

    case ARM_SYS_lseek:
      /* seek into file */
      regs->regs_R[MD_REG_R0] =
	lseek(/*fd*/regs->regs_R[MD_REG_R0],
	      /*off*/regs->regs_R[MD_REG_R1], /*dir*/regs->regs_R[MD_REG_R2]);

      /* check for an error condition */
      if (regs->regs_R[MD_REG_R0] == (word_t)-1)
	  /* got an error, return details */
	    regs->regs_R[MD_REG_R0] = -errno;

      break;

    case ARM_SYS_getpid:
      /* get the simulator process id */
      /*result*/regs->regs_R[MD_REG_R0] = debugging ? 2500 : getpid();

      /* check for an error condition */
      if (regs->regs_R[MD_REG_R0] == (word_t)-1)
	  /* got an error, return details */
	    regs->regs_R[MD_REG_R0] = -errno;

      break;

    case ARM_SYS_getuid:
#ifdef _MSC_VER
      warn("syscall getuid() not yet implemented for MSC...");
      regs->regs_R[MD_REG_R0] = 0;
#else /* !_MSC_VER */
      /* get current user id */
      /*first result*/regs->regs_R[MD_REG_R0] = getuid();

      /* check for an error condition */
      if (regs->regs_R[MD_REG_R0] == (word_t)-1)
	  /* got an error, return details */
	    regs->regs_R[MD_REG_R0] = -errno;
#endif /* _MSC_VER */
      break;


    case ARM_SYS_geteuid:
#ifdef _MSC_VER
      warn("syscall getuid() not yet implemented for MSC...");
      regs->regs_R[MD_REG_R0] = 0;
#else /* !_MSC_VER */
      /* get current user id */
      /*first result*/regs->regs_R[MD_REG_R0] = geteuid();

      /* check for an error condition */
      if (regs->regs_R[MD_REG_R0] == (word_t)-1)
	  /* got an error, return details */
	    regs->regs_R[MD_REG_R0] = -errno;
#endif /* _MSC_VER */
      break;


    case ARM_SYS_access:
      {
	char buf[MAXBUFSIZE];

	/* copy filename to host memory */
	mem_strcpy(mem_fn, mem, Read, /*fName*/regs->regs_R[MD_REG_R0], buf);

	/* check access on the file */
	/*result*/regs->regs_R[MD_REG_R0] =
	  access(buf, /*mode*/regs->regs_R[MD_REG_R1]);

	/* check for an error condition */
	if (regs->regs_R[MD_REG_R0] == (word_t)-1)
	  /* got an error, return details */
	    regs->regs_R[MD_REG_R0] = -errno;
      }
      break;

    case ARM_SYS_newstat:
    case ARM_SYS_newlstat:
      {
	char buf[MAXBUFSIZE];
	struct linux_statbuf linux_sbuf;
	struct stat sbuf;

	/* copy filename to host memory */
	mem_strcpy(mem_fn, mem, Read, /*fName*/regs->regs_R[MD_REG_R0], buf);

	/* stat() the file */
	if (syscode == ARM_SYS_newstat)
	  /*result*/regs->regs_R[MD_REG_R0] = stat(buf, &sbuf);
	else /* syscode == ARM_SYS_lstat */
	  /*result*/regs->regs_R[MD_REG_R0] = lstat(buf, &sbuf);

	/* check for an error condition */
	if (regs->regs_R[MD_REG_R0] == (word_t)-1)
	  /* got an error, return details */
	    regs->regs_R[MD_REG_R0] = -errno;

	/* translate from host stat structure to target format */
	linux_sbuf.linux_st_dev = MD_SWAPH(sbuf.st_dev);
	linux_sbuf.linux_st_ino = MD_SWAPW(sbuf.st_ino);
	linux_sbuf.linux_st_mode = MD_SWAPH(sbuf.st_mode);
	linux_sbuf.linux_st_nlink = MD_SWAPH(sbuf.st_nlink);
	linux_sbuf.linux_st_uid = MD_SWAPH(sbuf.st_uid);
	linux_sbuf.linux_st_gid = MD_SWAPH(sbuf.st_gid);
	linux_sbuf.linux_st_rdev = MD_SWAPH(sbuf.st_rdev);
	linux_sbuf.linux_st_size = MD_SWAPW(sbuf.st_size);
	linux_sbuf.linux_st_blksize = MD_SWAPW(sbuf.st_blksize);
	linux_sbuf.linux_st_blocks = MD_SWAPW(sbuf.st_blocks);
	linux_sbuf.linux_st_atime = MD_SWAPW(sbuf.st_atime);
	linux_sbuf.linux_st_mtime = MD_SWAPW(sbuf.st_mtime);
	linux_sbuf.linux_st_ctime = MD_SWAPW(sbuf.st_ctime);

	/* copy stat() results to simulator memory */
	mem_bcopy(mem_fn, mem, Write, /*sbuf*/regs->regs_R[MD_REG_R1],
		  &linux_sbuf, sizeof(struct linux_statbuf));
      }
      break;

#if XXX
    case OSF_SYS_dup:
      /* dup() the file descriptor */
      /*fd*/regs->regs_R[MD_REG_R0] = dup(/*fd*/regs->regs_R[MD_REG_R0]);

      /* check for an error condition */
      if (regs->regs_R[MD_REG_R0] == (word_t)-1)
	  /* got an error, return details */
	    regs->regs_R[MD_REG_R0] = -errno;

      break;
#endif

#if 0
    case ARM_SYS_pipe: /* Be careful with this one */
      {
	int fd[2];

	/* copy pipe descriptors to host memory */;
	mem_bcopy(mem_fn, mem, Read, /*fd's*/regs->regs_R[MD_REG_R0],
		  fd, sizeof(fd));

	/* create a pipe */
	/*result*/regs->regs_R[MD_REG_R7] = pipe(fd);

	/* copy descriptor results to result registers */
	/*pipe1*/regs->regs_R[MD_REG_R0] = fd[0];
	/*pipe 2*/regs->regs_R[MD_REG_R1] = fd[1];

	/* check for an error condition */
	 if (regs->regs_R[MD_REG_R7] == (word_t)-1)
	  /* got an error, return details */
	    regs->regs_R[MD_REG_R0] = -errno;
      }
      break;
#endif

    case ARM_SYS_times:
      {
	struct linux_tms tms;
	struct tms ltms;
	clock_t result;

	result = times(&ltms);
	tms.tms_utime = ltms.tms_utime;
	tms.tms_stime = ltms.tms_stime;
	tms.tms_cutime = ltms.tms_cutime;
	tms.tms_cstime = ltms.tms_cstime;

	mem_bcopy(mem_fn, mem, Write,
		  /* buf */regs->regs_R[MD_REG_R0],
		  &tms, sizeof(struct linux_tms));

	if (result != (word_t)-1)
	  regs->regs_R[MD_REG_R0] = result;
	else
	  regs->regs_R[MD_REG_R0] = -errno; /* got an error, return details */
      }
      break;

    case ARM_SYS_getgid:
#ifdef _MSC_VER
      warn("syscall getgid() not yet implemented for MSC...");
      regs->regs_R[MD_REG_R0] = 0;
#else /* !_MSC_VER */
      /* get current group id */
      /*first result*/regs->regs_R[MD_REG_R0] = getgid();

      /* check for an error condition */
      if (regs->regs_R[MD_REG_R0] == (word_t)-1)
	  /* got an error, return details */
	    regs->regs_R[MD_REG_R0] = -errno;
#endif /* _MSC_VER */
      break;

    case ARM_SYS_getegid:
#ifdef _MSC_VER
      warn("syscall getgid() not yet implemented for MSC...");
      regs->regs_R[MD_REG_R0] = 0;
#else /* !_MSC_VER */
      /* get current group id */
      /*first result*/regs->regs_R[MD_REG_R0] = getegid();

      /* check for an error condition */
      if (regs->regs_R[MD_REG_R0] == (word_t)-1)
	  /* got an error, return details */
	    regs->regs_R[MD_REG_R0] = -errno;
#endif /* _MSC_VER */
      break;

    case ARM_SYS_ioctl:
      switch (/* req */regs->regs_R[MD_REG_R1])
	{
#if !defined(TIOCGETP) && defined(linux)
	case OSF_TIOCGETP: /* <Out,TIOCGETP,6> */
	  {
	    struct termios lbuf;
	    struct osf_sgttyb buf;

	    /* result */regs->regs_R[MD_REG_R0] =
			  tcgetattr(/* fd */(int)regs->regs_R[MD_REG_R0],
				    &lbuf);

	    /* translate results */
	    buf.sg_ispeed = lbuf.c_ispeed;
	    buf.sg_ospeed = lbuf.c_ospeed;
	    buf.sg_erase = lbuf.c_cc[VERASE];
	    buf.sg_kill = lbuf.c_cc[VKILL];
	    buf.sg_flags = 0;	/* FIXME: this is wrong... */

	    mem_bcopy(mem_fn, mem, Write,
		      /* buf */regs->regs_R[MD_REG_R2], &buf,
		      sizeof(struct osf_sgttyb));

	    if (regs->regs_R[MD_REG_R0] == (word_t)-1)
	  /* got an error, return details */
		    regs->regs_R[MD_REG_R0] = -errno;

	  }
	  break;
#endif
#ifdef TIOCGETP
	case OSF_TIOCGETP: /* <Out,TIOCGETP,6> */
	  {
	    struct sgttyb lbuf;
	    struct osf_sgttyb buf;

	    /* result */regs->regs_R[MD_REG_R0] =
	      ioctl(/* fd */(int)regs->regs_R[MD_REG_R0],
		    /* req */TIOCGETP,
		    &lbuf);

	    /* translate results */
	    buf.sg_ispeed = lbuf.sg_ispeed;
	    buf.sg_ospeed = lbuf.sg_ospeed;
	    buf.sg_erase = lbuf.sg_erase;
	    buf.sg_kill = lbuf.sg_kill;
	    buf.sg_flags = lbuf.sg_flags;
	    mem_bcopy(mem_fn, mem, Write,
		      /* buf */regs->regs_R[MD_REG_R2], &buf,
		      sizeof(struct osf_sgttyb));

	    if (regs->regs_R[MD_REG_R0] == (word_t)-1)
	  /* got an error, return details */
	    regs->regs_R[MD_REG_R0] = -errno;
	  }
	  break;
#endif
#ifdef FIONREAD
	case OSF_FIONREAD:
	  {
	    int nread;

	    /* result */regs->regs_R[MD_REG_R0] =
	      ioctl(/* fd */(int)regs->regs_R[MD_REG_R0],
		    /* req */FIONREAD,
		    /* arg */&nread);

	    mem_bcopy(mem_fn, mem, Write,
		      /* arg */regs->regs_R[MD_REG_R2],
		      &nread, sizeof(nread));

	    if (regs->regs_R[MD_REG_R0] == (word_t)-1)
	  /* got an error, return details */
	    regs->regs_R[MD_REG_R0] = -errno;
	  }
	  break;
#endif
#ifdef FIONBIO
	case /*FIXME*/FIONBIO:
	  {
	    int arg = 0;

	    if (regs->regs_R[MD_REG_R2])
	      mem_bcopy(mem_fn, mem, Read,
		      /* arg */regs->regs_R[MD_REG_R2],
		      &arg, sizeof(arg));

#ifdef NOTNOW
	    fprintf(stderr, "FIONBIO: %d, %d\n",
		    (int)regs->regs_R[MD_REG_R0],
		    arg);
#endif
	    /* result */regs->regs_R[MD_REG_R0] =
	      ioctl(/* fd */(int)regs->regs_R[MD_REG_R0],
		    /* req */FIONBIO,
		    /* arg */&arg);

	    if (regs->regs_R[MD_REG_R2])
	      mem_bcopy(mem_fn, mem, Write,
		      /* arg */regs->regs_R[MD_REG_R2],
		      &arg, sizeof(arg));

	    if (regs->regs_R[MD_REG_R0] == (word_t)-1)
	  /* got an error, return details */
	    regs->regs_R[MD_REG_R0] = -errno;

	  }
	  break;
#endif
	default:
	  warn("unsupported ioctl call: ioctl(%ld, ...)",
	       regs->regs_R[MD_REG_R1]);
	  regs->regs_R[MD_REG_R0] = 0;
	  break;
	}
      break;

#if 0
      {
	char buf[NUM_IOCTL_BYTES];
	int local_req = 0;

	/* convert target ioctl() request to host ioctl() request values */
	switch (/*req*/regs->regs_R[MD_REG_R1]) {
/* #if !defined(__CYGWIN32__) */
	case SS_IOCTL_TIOCGETP:
	  local_req = TIOCGETP;
	  break;
	case SS_IOCTL_TIOCSETP:
	  local_req = TIOCSETP;
	  break;
	case SS_IOCTL_TCGETP:
	  local_req = TIOCGETP;
	  break;
/* #endif */
#ifdef TCGETA
	case SS_IOCTL_TCGETA:
	  local_req = TCGETA;
	  break;
#endif
#ifdef TIOCGLTC
	case SS_IOCTL_TIOCGLTC:
	  local_req = TIOCGLTC;
	  break;
#endif
#ifdef TIOCSLTC
	case SS_IOCTL_TIOCSLTC:
	  local_req = TIOCSLTC;
	  break;
#endif
	case SS_IOCTL_TIOCGWINSZ:
	  local_req = TIOCGWINSZ;
	  break;
#ifdef TCSETAW
	case SS_IOCTL_TCSETAW:
	  local_req = TCSETAW;
	  break;
#endif
#ifdef TIOCGETC
	case SS_IOCTL_TIOCGETC:
	  local_req = TIOCGETC;
	  break;
#endif
#ifdef TIOCSETC
	case SS_IOCTL_TIOCSETC:
	  local_req = TIOCSETC;
	  break;
#endif
#ifdef TIOCLBIC
	case SS_IOCTL_TIOCLBIC:
	  local_req = TIOCLBIC;
	  break;
#endif
#ifdef TIOCLBIS
	case SS_IOCTL_TIOCLBIS:
	  local_req = TIOCLBIS;
	  break;
#endif
#ifdef TIOCLGET
	case SS_IOCTL_TIOCLGET:
	  local_req = TIOCLGET;
	  break;
#endif
#ifdef TIOCLSET
	case SS_IOCTL_TIOCLSET:
	  local_req = TIOCLSET;
	  break;
#endif
	}

	if (!local_req)
	  {
	    /* FIXME: could not translate the ioctl() request, just warn user
	       and ignore the request */
	    warn("syscall: ioctl: ioctl code not supported d=%d, req=%d",
		 regs->regs_R[MD_REG_R0], regs->regs_R[MD_REG_R1]);
	    regs->regs_R[MD_REG_R0] = 0;
	    /*regs->regs_R[7] = 0;*/
	  }
	else
	  {
	    /* ioctl() code was successfully translated to a host code */

	    /* if arg ptr exists, copy NUM_IOCTL_BYTES bytes to host mem */
	    if (/*argp*/regs->regs_R[MD_REG_R2] != 0)
	      mem_bcopy(mem_fn, mem, Read, /*argp*/regs->regs_R[MD_REG_R2],
			buf, NUM_IOCTL_BYTES);

	    /* perform the ioctl() call */
	    /*result*/regs->regs_R[MD_REG_R0] =
	      ioctl(/*fd*/regs->regs_R[MD_REG_R0], local_req, buf);

	    /* if arg ptr exists, copy NUM_IOCTL_BYTES bytes from host mem */
	    if (/*argp*/regs->regs_R[MD_REG_R2] != 0)
	      mem_bcopy(mem_fn, mem, Write, regs->regs_R[MD_REG_R2],
			buf, NUM_IOCTL_BYTES);

	    /* check for an error condition */
	    if (regs->regs_R[MD_REG_R0] == (word_t)-1)
	  /* got an error, return details */
	    regs->regs_R[MD_REG_R0] = -errno;
	  }
      }
      break;
#endif

    case ARM_SYS_newfstat:
      {
	struct linux_statbuf linux_sbuf;
	struct stat sbuf;

	/* fstat() the file */
	/*result*/regs->regs_R[MD_REG_R0] =
	  fstat(/*fd*/regs->regs_R[MD_REG_R0], &sbuf);

	/* check for an error condition */
	if (regs->regs_R[MD_REG_R0] == (word_t)-1)
	  /* got an error, return details */
	    regs->regs_R[MD_REG_R0] = -errno;

	/* translate the stat structure to host format */
	linux_sbuf.linux_st_dev = MD_SWAPH(sbuf.st_dev);
	linux_sbuf.linux_st_ino = MD_SWAPW(sbuf.st_ino);
	linux_sbuf.linux_st_mode = MD_SWAPH(sbuf.st_mode);
	linux_sbuf.linux_st_nlink = MD_SWAPH(sbuf.st_nlink);
	linux_sbuf.linux_st_uid = MD_SWAPH(sbuf.st_uid);
	linux_sbuf.linux_st_gid = MD_SWAPH(sbuf.st_gid);
	linux_sbuf.linux_st_rdev = MD_SWAPH(sbuf.st_rdev);
	linux_sbuf.linux_st_size = MD_SWAPW(sbuf.st_size);
	linux_sbuf.linux_st_blksize = MD_SWAPW(sbuf.st_blksize);
	linux_sbuf.linux_st_blocks = MD_SWAPW(sbuf.st_blocks);
	linux_sbuf.linux_st_atime = MD_SWAPW(sbuf.st_atime);
	linux_sbuf.linux_st_mtime = MD_SWAPW(sbuf.st_mtime);
	linux_sbuf.linux_st_ctime = MD_SWAPW(sbuf.st_ctime);

	/* copy fstat() results to simulator memory */
	mem_bcopy(mem_fn, mem, Write, /*sbuf*/regs->regs_R[MD_REG_R1],
		  &linux_sbuf, sizeof(struct linux_statbuf));
      }
      break;

#if XXX
/*-------------------------------Is there a getpagesize in arm-linux??----------------------*/
    case OSF_SYS_getpagesize:
      /* get target pagesize */
      regs->regs_R[MD_REG_V0] = /* was: getpagesize() */MD_PAGE_SIZE;

      /* check for an error condition */
      if (regs->regs_R[MD_REG_V0] != (word_t)-1)
	regs->regs_R[MD_REG_A3] = 0;
      else /* got an error, return details */
	{
	  regs->regs_R[MD_REG_A3] = -1;
	  regs->regs_R[MD_REG_V0] = errno;
	}
      break;
/*----------------------------------------------------------------------------------------*/
#endif

    case ARM_SYS_setitimer:
      /* FIXME: the sigvec system call is ignored */
      warn("syscall: setitimer ignored");
      regs->regs_R[MD_REG_R0] = 0;
      break;

#if XXX
/*-------------------------------Is there a table in arm-linux??----------------------*/
    case OSF_SYS_table:
      {
	qword_t table_id, table_index, buf_addr, num_elem, size_elem;
	struct osf_tbl_sysinfo sysinfo;
	
	table_id = regs->regs_R[MD_REG_A1];
	table_index = regs->regs_R[MD_REG_A2];
	buf_addr = regs->regs_R[MD_REG_A3];
	num_elem = regs->regs_R[MD_REG_A4];
	size_elem = regs->regs_R[MD_REG_A5];
	
	switch(table_id)
	{
	case OSF_TBL_SYSINFO:
	  if (table_index != 0)
	    {
	      panic("table: table id TBL_SYSINFO requires 0 index, got %08d",
		    table_index );
	    }
	  else if (num_elem != 1)
	    {
	      panic("table: table id TBL_SYSINFO requires 1 elts, got %08d",
		    num_elem );
	    }
	  else
	    {
	      struct rusage rusage_info;
	      
	      /* use getrusage() to determine user & system time */
	      if (getrusage(RUSAGE_SELF, &rusage_info) < 0)
		{
		  /* abort the system call */
		  regs->regs_R[MD_REG_A3] = -1;
		  /* not kosher to pass off errno of getrusage() as errno
		     of table(), but what the heck... */
		  regs->regs_R[MD_REG_V0] = errno;
		  break;
		}
	      
	      /* use sysconf() to determine clock tick frequency */
	      sysinfo.si_hz = sysconf(_SC_CLK_TCK);

	      /* convert user and system time into clock ticks */
	      sysinfo.si_user = rusage_info.ru_utime.tv_sec * sysinfo.si_hz + 
		(rusage_info.ru_utime.tv_usec * sysinfo.si_hz) / 1000000UL;
	      sysinfo.si_sys = rusage_info.ru_stime.tv_sec * sysinfo.si_hz + 
		(rusage_info.ru_stime.tv_usec * sysinfo.si_hz) / 1000000UL;

	      /* following can't be determined in a portable manner and
		 are ignored */
	      sysinfo.si_nice = 0;
	      sysinfo.si_idle = 0;
	      sysinfo.si_phz = 0;
	      sysinfo.si_boottime = 0;
	      sysinfo.wait = 0;

	      /* copy structure into simulator memory */
	      mem_bcopy(mem_fn, mem, Write, buf_addr,
			&sysinfo, sizeof(struct osf_tbl_sysinfo));

	      /* return success */
	      regs->regs_R[MD_REG_A3] = 0;
	    }
	  break;

	default:
	  warn("table: unsupported table id %d requested, ignored", table_id);
	  regs->regs_R[MD_REG_A3] = 0;
	}
      }
      break;
/*----------------------------------------------------------------------------------------*/
#endif


#if XXX
/*-------------------------------Is there a getdtablesize in arm-linux??----------------------*/
    case OSF_SYS_getdtablesize:
#if defined(_AIX) || defined(__alpha)
      /* get descriptor table size */
      regs->regs_R[MD_REG_V0] = getdtablesize();

      /* check for an error condition */
      if (regs->regs_R[MD_REG_V0] != (word_t)-1)
	regs->regs_R[MD_REG_A3] = 0;
      else /* got an error, return details */
	{
	  regs->regs_R[MD_REG_A3] = -1;
	  regs->regs_R[MD_REG_V0] = errno;
	}
#elif defined(ultrix)
      {
	/* no comparable system call found, try some reasonable defaults */
	warn("syscall: called getdtablesize\n");
	regs->regs_R[MD_REG_V0] = 16;
	regs->regs_R[MD_REG_A3] = 0;
      }
#elif defined(MIN_SYSCALL_MODE)
      {
	/* no comparable system call found, try some reasonable defaults */
	warn("syscall: called getdtablesize\n");
	regs->regs_R[MD_REG_V0] = 16;
	regs->regs_R[MD_REG_A3] = 0;
      }
#else
      {
	struct rlimit rl;

	/* get descriptor table size in rlimit structure */
	if (getrlimit(RLIMIT_NOFILE, &rl) != (word_t)-1)
	  {
	    regs->regs_R[MD_REG_V0] = rl.rlim_cur;
	    regs->regs_R[MD_REG_A3] = 0;
	  }
	else /* got an error, return details */
	  {
	    regs->regs_R[MD_REG_A3] = -1;
	    regs->regs_R[MD_REG_V0] = errno;
	  }
      }
#endif
      break;
/*----------------------------------------------------------------------------------------*/
#endif

    case ARM_SYS_dup2:
      /* dup2() the file descriptor */
      regs->regs_R[MD_REG_R0] =
	dup2(/*fd1*/regs->regs_R[MD_REG_R0], /*fd2*/regs->regs_R[MD_REG_R1]);

      /* check for an error condition */
      if (regs->regs_R[MD_REG_R0] == (word_t)-1)
	  /* got an error, return details */
	    regs->regs_R[MD_REG_R0] = -errno;
      break;

    case ARM_SYS_fcntl:
#ifdef _MSC_VER
      warn("syscall fcntl() not yet implemented for MSC...");
      regs->regs_R[MD_REG_R0] = 0;
#else /* !_MSC_VER */
      /* get fcntl() information on the file */
      regs->regs_R[MD_REG_R0] =
	fcntl(/*fd*/regs->regs_R[MD_REG_R0],
	      /*cmd*/regs->regs_R[MD_REG_R1], /*arg*/regs->regs_R[MD_REG_R2]);

      /* check for an error condition */
      if (regs->regs_R[MD_REG_R0] == (word_t)-1)
	  /* got an error, return details */
	    regs->regs_R[MD_REG_R0] = -errno;
#endif /* _MSC_VER */
      break;

	    /*-------------------------------Out of ARM-------------------------------------*/
#if 0
    case OSF_SYS_sigvec:
      /* FIXME: the sigvec system call is ignored */
      warn("syscall: sigvec ignored");
      regs->regs_R[MD_REG_A3] = 0;
      break;
#endif

#if 0
    case OSF_SYS_sigblock:
      /* FIXME: the sigblock system call is ignored */
      warn("syscall: sigblock ignored");
      regs->regs_R[MD_REG_A3] = 0;
      break;
#endif

#if 0
    case OSF_SYS_sigsetmask:
      /* FIXME: the sigsetmask system call is ignored */
      warn("syscall: sigsetmask ignored");
      regs->regs_R[MD_REG_A3] = 0;
      break;
#endif
/*----------------------------------------------------------------------------------*/

#if XXX
    case OSF_SYS_gettimeofday:
#ifdef _MSC_VER
      warn("syscall gettimeofday() not yet implemented for MSC...");
      regs->regs_R[MD_REG_R0] = 0;
#else /* _MSC_VER */
      {
	struct osf_timeval osf_tv;
	struct timeval tv, *tvp;
	struct osf_timezone osf_tz;
	struct timezone tz, *tzp;

	if (/*timeval*/regs->regs_R[MD_REG_R0] != 0)
	  {
	    /* copy timeval into host memory */
	    mem_bcopy(mem_fn, mem, Read, /*timeval*/regs->regs_R[MD_REG_R0],
		      &osf_tv, sizeof(struct osf_timeval));

	    /* convert target timeval structure to host format */
	    tv.tv_sec = MD_SWAPW(osf_tv.osf_tv_sec);
	    tv.tv_usec = MD_SWAPW(osf_tv.osf_tv_usec);
	    tvp = &tv;
	  }
	else
	  tvp = NULL;

	if (/*timezone*/regs->regs_R[MD_REG_R1] != 0)
	  {
	    /* copy timezone into host memory */
	    mem_bcopy(mem_fn, mem, Read, /*timezone*/regs->regs_R[MD_REG_R1],
		      &osf_tz, sizeof(struct osf_timezone));

	    /* convert target timezone structure to host format */
	    tz.tz_minuteswest = MD_SWAPW(osf_tz.osf_tz_minuteswest);
	    tz.tz_dsttime = MD_SWAPW(osf_tz.osf_tz_dsttime);
	    tzp = &tz;
	  }
	else
	  tzp = NULL;

	/* get time of day */
	/*result*/regs->regs_R[MD_REG_R0] = gettimeofday(tvp, tzp);

	/* check for an error condition */
	if (regs->regs_R[MD_REG_R0] == (word_t)-1)
	  /* got an error, return details */
	    regs->regs_R[MD_REG_R0] = -errno;

	if (/*timeval*/regs->regs_R[MD_REG_R0] != 0)
	  {
	    /* convert host timeval structure to target format */
	    osf_tv.osf_tv_sec = MD_SWAPW(tv.tv_sec);
	    osf_tv.osf_tv_usec = MD_SWAPW(tv.tv_usec);

	    /* copy timeval to target memory */
	    mem_bcopy(mem_fn, mem, Write, /*timeval*/regs->regs_R[MD_REG_R0],
		      &osf_tv, sizeof(struct osf_timeval));
	  }

	if (/*timezone*/regs->regs_R[MD_REG_R1] != 0)
	  {
	    /* convert host timezone structure to target format */
	    osf_tz.osf_tz_minuteswest = MD_SWAPW(tz.tz_minuteswest);
	    osf_tz.osf_tz_dsttime = MD_SWAPW(tz.tz_dsttime);

	    /* copy timezone to target memory */
	    mem_bcopy(mem_fn, mem, Write, /*timezone*/regs->regs_R[MD_REG_R1],
		      &osf_tz, sizeof(struct osf_timezone));
	  }
      }
#endif /* !_MSC_VER */
      break;
#endif

    case ARM_SYS_getrusage:
#if defined(__svr4__) || defined(__USLC__) || defined(hpux) || defined(__hpux) || defined(_AIX)
      {
	struct tms tms_buf;
	struct osf_rusage rusage;

	/* get user and system times */
	if (times(&tms_buf) != (word_t)-1)
	  {
	    /* no error */
	    regs->regs_R[MD_REG_R0] = 0;
	  }
	else /* got an error, indicate result */
	  {
	    regs->regs_R[MD_REG_R0] = -errno;
	  }

	/* initialize target rusage result structure */
#if defined(__svr4__)
	memset(&rusage, '\0', sizeof(struct osf_rusage));
#else /* !defined(__svr4__) */
	bzero(&rusage, sizeof(struct osf_rusage));
#endif

	/* convert from host rusage structure to target format */
	rusage.osf_ru_utime.osf_tv_sec = MD_SWAPW(tms_buf.tms_utime/CLK_TCK);
	rusage.osf_ru_utime.osf_tv_sec =
	  MD_SWAPW(rusage.osf_ru_utime.osf_tv_sec);
	rusage.osf_ru_utime.osf_tv_usec = 0;
	rusage.osf_ru_stime.osf_tv_sec = MD_SWAPW(tms_buf.tms_stime/CLK_TCK);
	rusage.osf_ru_stime.osf_tv_sec =
	  MD_SWAPW(rusage.osf_ru_stime.osf_tv_sec);
	rusage.osf_ru_stime.osf_tv_usec = 0;

	/* copy rusage results into target memory */
	mem_bcopy(mem_fn, mem, Write, /*rusage*/regs->regs_R[MD_REG_A1],
		  &rusage, sizeof(struct osf_rusage));
      }
#elif defined(__unix__)
      {
	struct rusage local_rusage;
	struct osf_rusage rusage;

	/* get rusage information */
	/*result*/regs->regs_R[MD_REG_R0] =
	  getrusage(/*who*/regs->regs_R[MD_REG_R0], &local_rusage);

	/* check for an error condition */
	if (regs->regs_R[MD_REG_R0] == (word_t)-1)
	  /* got an error, return details */
	    regs->regs_R[MD_REG_R0] = -errno;

	/* convert from host rusage structure to target format */
	rusage.osf_ru_utime.osf_tv_sec =
	  MD_SWAPW(local_rusage.ru_utime.tv_sec);
	rusage.osf_ru_utime.osf_tv_usec =
	  MD_SWAPW(local_rusage.ru_utime.tv_usec);
	rusage.osf_ru_utime.osf_tv_sec =
	  MD_SWAPW(local_rusage.ru_utime.tv_sec);
	rusage.osf_ru_utime.osf_tv_usec =
	  MD_SWAPW(local_rusage.ru_utime.tv_usec);
	rusage.osf_ru_stime.osf_tv_sec =
	  MD_SWAPW(local_rusage.ru_stime.tv_sec);
	rusage.osf_ru_stime.osf_tv_usec =
	  MD_SWAPW(local_rusage.ru_stime.tv_usec);
	rusage.osf_ru_stime.osf_tv_sec =
	  MD_SWAPW(local_rusage.ru_stime.tv_sec);
	rusage.osf_ru_stime.osf_tv_usec =
	  MD_SWAPW(local_rusage.ru_stime.tv_usec);
	rusage.osf_ru_maxrss = MD_SWAPW(local_rusage.ru_maxrss);
	rusage.osf_ru_ixrss = MD_SWAPW(local_rusage.ru_ixrss);
	rusage.osf_ru_idrss = MD_SWAPW(local_rusage.ru_idrss);
	rusage.osf_ru_isrss = MD_SWAPW(local_rusage.ru_isrss);
	rusage.osf_ru_minflt = MD_SWAPW(local_rusage.ru_minflt);
	rusage.osf_ru_majflt = MD_SWAPW(local_rusage.ru_majflt);
	rusage.osf_ru_nswap = MD_SWAPW(local_rusage.ru_nswap);
	rusage.osf_ru_inblock = MD_SWAPW(local_rusage.ru_inblock);
	rusage.osf_ru_oublock = MD_SWAPW(local_rusage.ru_oublock);
	rusage.osf_ru_msgsnd = MD_SWAPW(local_rusage.ru_msgsnd);
	rusage.osf_ru_msgrcv = MD_SWAPW(local_rusage.ru_msgrcv);
	rusage.osf_ru_nsignals = MD_SWAPW(local_rusage.ru_nsignals);
	rusage.osf_ru_nvcsw = MD_SWAPW(local_rusage.ru_nvcsw);
	rusage.osf_ru_nivcsw = MD_SWAPW(local_rusage.ru_nivcsw);

	/* copy rusage results into target memory */
	mem_bcopy(mem_fn, mem, Write, /*rusage*/regs->regs_R[MD_REG_R1],
		  &rusage, sizeof(struct osf_rusage));
      }
#elif defined(__CYGWIN32__) || defined(_MSC_VER)
	    warn("syscall: called getrusage\n");
            regs->regs_R[7] = 0;
#else
#error No getrusage() implementation!
#endif
      break;

#if XXX
/*-------------------------------Is there a utimes in arm-linux??----------------------*/
    case OSF_SYS_utimes:
      {
	char buf[MAXBUFSIZE];

	/* copy filename to host memory */
	mem_strcpy(mem_fn, mem, Read, /*fname*/regs->regs_R[MD_REG_A0], buf);

	if (/*timeval*/regs->regs_R[MD_REG_A1] == 0)
	  {
#if defined(hpux) || defined(__hpux) || defined(__i386__)
	    /* no utimes() in hpux, use utime() instead */
	    /*result*/regs->regs_R[MD_REG_V0] = utime(buf, NULL);
#elif defined(_MSC_VER)
            /* no utimes() in MSC, use utime() instead */
	    /*result*/regs->regs_R[MD_REG_V0] = utime(buf, NULL);
#elif defined(__svr4__) || defined(__USLC__) || defined(unix) || defined(_AIX) || defined(__alpha)
	    /*result*/regs->regs_R[MD_REG_V0] = utimes(buf, NULL);
#elif defined(__CYGWIN32__)
	    warn("syscall: called utimes\n");
#else
#error No utimes() implementation!
#endif
	  }
	else
	  {
	    struct osf_timeval osf_tval[2];
#ifndef _MSC_VER
	    struct timeval tval[2];
#endif /* !_MSC_VER */

	    /* copy timeval structure to host memory */
	    mem_bcopy(mem_fn, mem, Read, /*timeout*/regs->regs_R[MD_REG_A1],
		      osf_tval, 2*sizeof(struct osf_timeval));

#ifndef _MSC_VER
	    /* convert timeval structure to host format */
	    tval[0].tv_sec = MD_SWAPW(osf_tval[0].osf_tv_sec);
	    tval[0].tv_usec = MD_SWAPW(osf_tval[0].osf_tv_usec);
	    tval[1].tv_sec = MD_SWAPW(osf_tval[1].osf_tv_sec);
	    tval[1].tv_usec = MD_SWAPW(osf_tval[1].osf_tv_usec);
#endif /* !_MSC_VER */

#if defined(hpux) || defined(__hpux) || defined(__svr4__)
	    /* no utimes() in hpux, use utime() instead */
	    {
	      struct utimbuf ubuf;

	      ubuf.actime = MD_SWAPW(tval[0].tv_sec);
	      ubuf.modtime = MD_SWAPW(tval[1].tv_sec);

	      /* result */regs->regs_R[MD_REG_V0] = utime(buf, &ubuf);
	    }
#elif defined(_MSC_VER)
            /* no utimes() in hpux, use utime() instead */
            {
              struct _utimbuf ubuf;

              ubuf.actime = MD_SWAPW(osf_tval[0].osf_tv_sec);
              ubuf.modtime = MD_SWAPW(osf_tval[1].osf_tv_sec);

              /* result */regs->regs_R[MD_REG_V0] = utime(buf, &ubuf);
            }
#elif defined(__USLC__) || defined(unix) || defined(_AIX) || defined(__alpha)
	    /* result */regs->regs_R[MD_REG_V0] = utimes(buf, tval);
#elif defined(__CYGWIN32__)
	    warn("syscall: called utimes\n");
#else
#error No utimes() implementation!
#endif
	  }

	/* check for an error condition */
	if (regs->regs_R[MD_REG_V0] != (word_t)-1)
	  regs->regs_R[MD_REG_A3] = 0;
	else /* got an error, return details */
	  {
	    regs->regs_R[MD_REG_A3] = -1;
	    regs->regs_R[MD_REG_V0] = errno;
	  }
      }
      break;
/*----------------------------------------------------------------------------------*/
#endif

#if XXX
    case ARM_SYS_getrlimit:
    case ARM_SYS_setrlimit:
#ifdef _MSC_VER
      warn("syscall get/setrlimit() not yet implemented for MSC...");
      regs->regs_R[MD_REG_R0] = 0;
#elif defined(__CYGWIN32__)
      {
	warn("syscall: called get/setrlimit\n");
	regs->regs_R[MD_REG_R0] = 0;
      }
#else
      {
	struct osf_rlimit osf_rl;
	struct rlimit rl;

	/* copy rlimit structure to host memory */
	mem_bcopy(mem_fn, mem, Read, /*rlimit*/regs->regs_R[MD_REG_R1],
		  &osf_rl, sizeof(struct osf_rlimit));

	/* convert rlimit structure to host format */
	rl.rlim_cur = MD_SWAPQ(osf_rl.osf_rlim_cur);
	rl.rlim_max = MD_SWAPQ(osf_rl.osf_rlim_max);

	/* get rlimit information */
	if (syscode == OSF_SYS_getrlimit)
	  /*result*/regs->regs_R[MD_REG_R0] =
	    getrlimit(regs->regs_R[MD_REG_R0], &rl);
	else /* syscode == OSF_SYS_setrlimit */
	  /*result*/regs->regs_R[MD_REG_R0] =
	    setrlimit(regs->regs_R[MD_REG_R0], &rl);

	/* check for an error condition */
	if (regs->regs_R[MD_REG_R0] == (word_t)-1)
	  /* got an error, return details */
	    regs->regs_R[MD_REG_R0] = -errno;

	/* convert rlimit structure to target format */
	osf_rl.osf_rlim_cur = MD_SWAPQ(rl.rlim_cur);
	osf_rl.osf_rlim_max = MD_SWAPQ(rl.rlim_max);

	/* copy rlimit structure to target memory */
	mem_bcopy(mem_fn, mem, Write, /*rlimit*/regs->regs_R[MD_REG_R1],
		  &osf_rl, sizeof(struct osf_rlimit));
      }
#endif
      break;
#endif

    case ARM_SYS_sigprocmask:
      {
	static int first = TRUE;

	if (first)
	  {
	    warn("partially supported sigprocmask() call...");
	    first = FALSE;
	  }

	/* from klauser@cs.colorado.edu: there are a couple bugs in the
	   sigprocmask implementation; here is a fix: the problem comes from an
	   impedance mismatch between application/libc interface and
	   libc/syscall interface, the former of which you read about in the
	   manpage, the latter of which you actually intercept here.  The
	   following is mostly correct, but does not capture some minor
	   details, which you only get right if you really let the kernel
	   handle it. (e.g. you can't really ever block sigkill etc.) */

        /*regs->regs_R[MD_REG_V0] = sigmask;*/

        switch (regs->regs_R[MD_REG_R0])
	  {
          case OSF_SIG_BLOCK:
            sigmask |= regs->regs_R[MD_REG_R1];
            break;
          case OSF_SIG_UNBLOCK:
            sigmask &= ~regs->regs_R[MD_REG_R1];
            break;
          case OSF_SIG_SETMASK:
            sigmask = regs->regs_R[MD_REG_R1];
            break;
          default:
            regs->regs_R[MD_REG_R0] = -EINVAL;

	  }

#if 0 /* FIXME: obsolete... */
	if (regs->regs_R[MD_REG_R2] > /* FIXME: why? */0x10000000)
	  mem_bcopy(mem_fn, mem, Write, regs->regs_R[MD_REG_R2],
		    &sigmask, sizeof(sigmask));

	if (regs->regs_R[MD_REG_R1] != 0)
	  {
	    switch (regs->regs_R[MD_REG_R0])
	      {
	      case OSF_SIG_BLOCK:
		sigmask |= regs->regs_R[MD_REG_R1];
		break;
	      case OSF_SIG_UNBLOCK:
		sigmask &= regs->regs_R[MD_REG_R1]; /* I think */
	      break;
	      case OSF_SIG_SETMASK:
		sigmask = regs->regs_R[MD_REG_R1]; /* I think */
		break;
	      default:
		panic("illegal how value to sigprocmask()");
	      }
	  }
	regs->regs_R[MD_REG_R0] = 0;

#endif
      }
      break;

    case ARM_SYS_sigaction:
      {
	int signum;
	static int first = TRUE;

	if (first)
	  {
	    warn("partially supported sigaction() call...");
	    first = FALSE;
	  }

	signum = regs->regs_R[MD_REG_R0];
	if (regs->regs_R[MD_REG_R1] != 0)
	  sigaction_array[signum] = regs->regs_R[MD_REG_R1];

	if (regs->regs_R[MD_REG_R2])
	  regs->regs_R[MD_REG_R2] = sigaction_array[signum];

	regs->regs_R[MD_REG_R0] = 0;

	/* for some reason, __sigaction expects A3 to have a 0 return value 
	regs->regs_R[MD_REG_A3] = 0; */
  
	/* FIXME: still need to add code so that on a signal, the 
	   correct action is actually taken. */

	/* FIXME: still need to add support for returning the correct
	   error messages (EFAULT, EINVAL) */
      }
      break;

#if XXX
    case OSF_SYS_sigstack:
      warn("unsupported sigstack() call...");
      regs->regs_R[MD_REG_R0] = 0;
      break;
#endif

#if XXX
    case ARM_SYS_sigreturn:
      {
	int i;
	struct osf_sigcontext sc;
	static int first = TRUE;

	if (first)
	  {
	    warn("partially supported sigreturn() call...");
	    first = FALSE;
	  }

	mem_bcopy(mem_fn, mem, Read, /* sc */regs->regs_R[MD_REG_R0],
		  &sc, sizeof(struct osf_sigcontext));

	sigmask = MD_SWAPQ(sc.sc_mask); /* was: prog_sigmask */
	regs->regs_NPC = MD_SWAPQ(sc.sc_pc);

	/* FIXME: should check for the branch delay bit */
	/* FIXME: current->nextpc = current->pc + 4; not sure about this... */
	for (i=0; i < 32; ++i)
	  regs->regs_R[i] = sc.sc_regs[i];
	for (i=0; i < 32; ++i)
	  regs->regs_F.q[i] = sc.sc_fpregs[i];
	regs->regs_C.fpcr = sc.sc_fpcr;
      }
      break;
#endif

#if XXX
/*------------------------------------Out of ARM???? -------------------------------*/
    case OSF_SYS_uswitch:
      warn("unsupported uswitch() call...");
      regs->regs_R[MD_REG_V0] = regs->regs_R[MD_REG_A1]; 
      break;
#endif

#if XXX
    case OSF_SYS_setsysinfo:
      warn("unsupported setsysinfo() call...");
      regs->regs_R[MD_REG_V0] = 0; 
      break;
#endif

/* Is this in arm??? ----*/
#if !defined(MIN_SYSCALL_MODE) && 0 
    case OSF_SYS_getdirentries:
      {
	int i, cnt, osf_cnt;
	struct dirent *p;
	sword_t fd = regs->regs_R[MD_REG_A0];
	md_addr_t osf_buf = regs->regs_R[MD_REG_A1];
	char *buf;
	sword_t osf_nbytes = regs->regs_R[MD_REG_A2];
	md_addr_t osf_pbase = regs->regs_R[MD_REG_A3];
	sqword_t osf_base;
	long base = 0;

	/* number of entries in simulated memory */
	if (!osf_nbytes)
	  warn("attempting to get 0 directory entries...");

	/* allocate local memory, whatever fits */
	buf = calloc(1, osf_nbytes);
	if (!buf)
	  fatal("out of virtual memory");

	/* get directory entries */
#if defined(__svr4__)
	base = lseek ((int)fd, (off_t)0, SEEK_CUR);
	regs->regs_R[MD_REG_V0] =
	  getdents((int)fd, (struct dirent *)buf, (size_t)osf_nbytes);
#else /* !__svr4__ */
	regs->regs_R[MD_REG_V0] =
	  getdirentries((int)fd, buf, (size_t)osf_nbytes, &base);
#endif

	/* check for an error condition */
	if (regs->regs_R[MD_REG_V0] != (word_t)-1)
	  {
	    regs->regs_R[MD_REG_A3] = 0;

	    /* anything to copy back? */
	    if (regs->regs_R[MD_REG_V0] > 0)
	      {
		/* copy all possible results to simulated space */
		for (i=0, cnt=0, osf_cnt=0, p=(struct dirent *)buf;
		     cnt < regs->regs_R[MD_REG_V0] && p->d_reclen > 0;
		     i++, cnt += p->d_reclen, p=(struct dirent *)(buf+cnt))
		  {
		    struct osf_dirent osf_dirent;

		    osf_dirent.d_ino = MD_SWAPW(p->d_ino);
		    osf_dirent.d_namlen = MD_SWAPH(strlen(p->d_name));
		    strcpy(osf_dirent.d_name, p->d_name);
		    osf_dirent.d_reclen = MD_SWAPH(OSF_DIRENT_SZ(p->d_name));

		    mem_bcopy(mem_fn, mem, Write,
			      osf_buf + osf_cnt,
			      &osf_dirent, OSF_DIRENT_SZ(p->d_name));
		    osf_cnt += OSF_DIRENT_SZ(p->d_name);
		  }

		if (osf_pbase != 0)
		  {
		    osf_base = (sqword_t)base;
		    mem_bcopy(mem_fn, mem, Write, osf_pbase,
			      &osf_base, sizeof(osf_base));
		  }

		/* update V0 to indicate translated read length */
		regs->regs_R[MD_REG_V0] = osf_cnt;
	      }
	  }
	else /* got an error, return details */
	  {
	    regs->regs_R[MD_REG_A3] = -1;
	    regs->regs_R[MD_REG_V0] = errno;
	  }

	free(buf);
      }
      break;
#endif
/*-----------------------------------------------------------------------------------------*/


#if !defined(MIN_SYSCALL_MODE)
    case ARM_SYS_truncate: /*find out what the 64 is for */
      {
	char buf[MAXBUFSIZE];

	/* copy filename to host memory */
	mem_strcpy(mem_fn, mem, Read, /*fname*/regs->regs_R[MD_REG_R0], buf);

	/* truncate the file */
	/*result*/regs->regs_R[MD_REG_R0] =
	  truncate(buf, /* length */(size_t)regs->regs_R[MD_REG_R1]);

	/* check for an error condition */
	if (regs->regs_R[MD_REG_R0] == (word_t)-1)
	  /* got an error, return details */
	    regs->regs_R[MD_REG_R0] = -errno;
      }
      break;
#endif

#if !defined(__CYGWIN32__) && !defined(_MSC_VER)
    case ARM_SYS_ftruncate:
      /* truncate the file */
      /*result*/regs->regs_R[MD_REG_R0] =
	ftruncate(/* fd */(int)regs->regs_R[MD_REG_R0],
		 /* length */(size_t)regs->regs_R[MD_REG_R1]);

      /* check for an error condition */
      if (regs->regs_R[MD_REG_R0] == (word_t)-1)
	  /* got an error, return details */
	    regs->regs_R[MD_REG_R0] = -errno;
      break;
#endif

#if !defined(MIN_SYSCALL_MODE)
    case ARM_SYS_statfs:
      {
	char buf[MAXBUFSIZE];
	struct osf_statfs osf_sbuf;
	struct statfs sbuf;

	/* copy filename to host memory */
	mem_strcpy(mem_fn, mem, Read, /*fName*/regs->regs_R[MD_REG_R0], buf);

	/* statfs() the fs */
	/*result*/regs->regs_R[MD_REG_R0] = statfs(buf, &sbuf);

	/* check for an error condition */
	if (regs->regs_R[MD_REG_R0] == (word_t)-1)
	  /* got an error, return details */
	    regs->regs_R[MD_REG_R0] = -errno;

	/* translate from host stat structure to target format */
#if defined(__svr4__) || defined(__osf__)
	osf_sbuf.f_type = MD_SWAPH(0x6969) /* NFS, whatever... */;
#else /* !__svr4__ */
	osf_sbuf.f_type = MD_SWAPH(sbuf.f_type);
#endif
	osf_sbuf.f_fsize = MD_SWAPW(sbuf.f_bsize);
	osf_sbuf.f_blocks = MD_SWAPW(sbuf.f_blocks);
	osf_sbuf.f_bfree = MD_SWAPW(sbuf.f_bfree);
	osf_sbuf.f_bavail = MD_SWAPW(sbuf.f_bavail);
	osf_sbuf.f_files = MD_SWAPW(sbuf.f_files);
	osf_sbuf.f_ffree = MD_SWAPW(sbuf.f_ffree);
	/* osf_sbuf.f_fsid = MD_SWAPW(sbuf.f_fsid); */
      
          
	/* copy stat() results to simulator memory */

	mem_bcopy(mem_fn, mem, Write, /*sbuf*/regs->regs_R[MD_REG_R1],		           &osf_sbuf, sizeof(struct osf_statfs)); 

        /*changed osf_statbuf to osf_statfs for arm? */

      }
      break;
#endif



#if !defined(MIN_SYSCALL_MODE)
    case ARM_SYS_setregid:
      /* set real and effective group ID */

      /*result*/regs->regs_R[MD_REG_R0] =
	setregid(/* rgid */(gid_t)regs->regs_R[MD_REG_R0],
		 /* egid */(gid_t)regs->regs_R[MD_REG_R1]);

      fprintf(stderr,"Why??");
      /* check for an error condition */
      if (regs->regs_R[MD_REG_R0] == (word_t)-1)
	  /* got an error, return details */
	    regs->regs_R[MD_REG_R0] = -errno;
      break;
#endif

#if !defined(MIN_SYSCALL_MODE)
	    //    case OSF_SYS_setreuid:
    case ARM_SYS_setreuid:
      /* set real and effective user ID */

      /*result*/regs->regs_R[MD_REG_R0] =
	setreuid(/* ruid */(uid_t)regs->regs_R[MD_REG_R0],
		 /* euid */(uid_t)regs->regs_R[MD_REG_R1]);

      /* check for an error condition */
      if (regs->regs_R[MD_REG_R0] == (word_t)-1)
	  /* got an error, return details */
	    regs->regs_R[MD_REG_R0] = -errno;
      break;
#endif

	    /* this is a subclass of the socketcall in arm */
#if !defined(MIN_SYSCALL_MODE) && 0
    case ARM_SYS_socket:
        int __domain,__type,__protocol;

        /* grab the socket call arguments from simulated memory */
        mem_bcopy(mem_fn, mem, Read, regs->regs_R[MD_REG_R1], &__domain,
		  /*nbytes*/sizeof(int));
        mem_bcopy(mem_fn, mem, Read, (regs->regs_R[MD_REG_R1]+sizeof(int)), &__type,
		  /*nbytes*/sizeof(int));
        mem_bcopy(mem_fn, mem, Read, (regs->regs_R[MD_REG_R1]+2*sizeof(int)), &__protocol,
		  /*nbytes*/sizeof(int));

      /* create an endpoint for communication */

      /* result */regs->regs_R[MD_REG_R0] =
	socket(/* domain */xlate_arg(__domain,
				     family_map, N_ELT(family_map),
				     "socket(family)"),
	       /* type */xlate_arg(__type,
				   socktype_map, N_ELT(socktype_map),
				   "socket(type)"),
	       /* protocol */xlate_arg(__protocol,
				       family_map, N_ELT(family_map),
				       "socket(proto)"));

      /* check for an error condition */
      if (regs->regs_R[MD_REG_R0] == (word_t)-1)
	  /* got an error, return details */
	    regs->regs_R[MD_REG_R0] = -errno;
      break;
#endif
	    /* This is a subclass of the socketcall in arm linux */
#if !defined(MIN_SYSCALL_MODE) && 0
    case ARM_SYS_connect:
      {
	struct osf_sockaddr osf_sa;

	/* initiate a connection on a socket */

	/* get the socket address */
	if (regs->regs_R[MD_REG_R2] > sizeof(struct osf_sockaddr))
	  {
	    fatal("sockaddr size overflow: addrlen = %d",
		  regs->regs_R[MD_REG_R2]);
	  }
	/* copy sockaddr structure to host memory */
	mem_bcopy(mem_fn, mem, Read, /* serv_addr */regs->regs_R[MD_REG_R1],
		  &osf_sa, /* addrlen */(int)regs->regs_R[MD_REG_R2]);
#if 0
	int i;
	sa.sa_family = osf_sa.sa_family;
	for (i=0; i < regs->regs_R[MD_REG_R2]; i++)
	  sa.sa_data[i] = osf_sa.sa_data[i];
#endif
	/* result */regs->regs_R[MD_REG_R0] =
	  connect(/* sockfd */(int)regs->regs_R[MD_REG_R0],
		  (void *)&osf_sa, /* addrlen */(int)regs->regs_R[MD_REG_R2]);

	/* check for an error condition */
	if (regs->regs_R[MD_REG_R0] == (word_t)-1)
	  /* got an error, return details */
	    regs->regs_R[MD_REG_R0] = -errno;
      }
      break;
#endif

/* Should olduname and oldolduname be supported ?? ctw*/
#if !defined(MIN_SYSCALL_MODE)
    case ARM_SYS_newuname:
      /* get name and information about current kernel */

      regs->regs_R[MD_REG_R0] = -EPERM;
      break;
#endif

#if !defined(MIN_SYSCALL_MODE)
    case ARM_SYS_writev:
      {
	int i;
	int error_return;
	char *buf;
	struct iovec *iov;

	/* allocate host side I/O vectors */
	iov =
	  (struct iovec *)malloc(/* iovcnt */regs->regs_R[MD_REG_R2]
				 * sizeof(struct iovec));
	if (!iov)
	  fatal("out of virtual memory in SYS_writev");

	/* copy target side I/O vector buffers to host memory */
	for (i=0; i < /* iovcnt */regs->regs_R[MD_REG_R2]; i++)
	  {
	    struct osf_iovec osf_iov;

	    /* copy target side pointer data into host side vector */
	    mem_bcopy(mem_fn, mem, Read,
		      (/*iov*/regs->regs_R[MD_REG_R1]
		       + i*sizeof(struct osf_iovec)),
		      &osf_iov, sizeof(struct osf_iovec));

	    iov[i].iov_len = MD_SWAPW(osf_iov.iov_len);
	    if (osf_iov.iov_base != 0 && osf_iov.iov_len != 0)
	      {
		buf = (char *)calloc(MD_SWAPW(osf_iov.iov_len), sizeof(char));
		if (!buf)
		  fatal("out of virtual memory in SYS_writev");
		mem_bcopy(mem_fn, mem, Read, MD_SWAPQ(osf_iov.iov_base),
			  buf, MD_SWAPW(osf_iov.iov_len));
		iov[i].iov_base = buf;
	      }
	    else
	      iov[i].iov_base = NULL;
	  }

	/* perform the vector'ed write */
	do {
	  /*result*/error_return =
	    writev(/* fd */(int)regs->regs_R[MD_REG_R0], iov,
		   /* iovcnt */(size_t)regs->regs_R[MD_REG_R2]);
	} while (/*result*/error_return == -1
	         && errno == EAGAIN);

	/* check for an error condition */
	if (error_return != -1) {
		regs->regs_R[MD_REG_R0] = error_return;
	  } /* no error */
	else {
	  	regs->regs_R[MD_REG_R0] = -(errno); /* negative of the error number is returned in r0 */
	  }

	/* free all the allocated memory */
	for (i=0; i < /* iovcnt */regs->regs_R[MD_REG_R2]; i++)
	  {
	    if (iov[i].iov_base)
	      {
		free(iov[i].iov_base);
		iov[i].iov_base = NULL;
	      }
	  }
	free(iov);
      }
      break;
#endif

#if !defined(MIN_SYSCALL_MODE)
    case ARM_SYS_readv:
      {
	int i;
	char *buf = NULL;
	struct osf_iovec *osf_iov;
	struct iovec *iov;
	int error_return;

	/* allocate host side I/O vectors */
	osf_iov =
	  calloc(/* iovcnt */regs->regs_R[MD_REG_R2],
		 sizeof(struct osf_iovec));
	if (!osf_iov)
	  fatal("out of virtual memory in SYS_readv");

	iov =
	  calloc(/* iovcnt */regs->regs_R[MD_REG_R2], sizeof(struct iovec));
	if (!iov)
	  fatal("out of virtual memory in SYS_readv");

	/* copy host side I/O vector buffers */
	for (i=0; i < /* iovcnt */regs->regs_R[MD_REG_R2]; i++)
	  {
	    /* copy target side pointer data into host side vector */
	    mem_bcopy(mem_fn, mem, Read,
		      (/*iov*/regs->regs_R[MD_REG_R1]
		       + i*sizeof(struct osf_iovec)),
		      &osf_iov[i], sizeof(struct osf_iovec));

	    iov[i].iov_len = MD_SWAPW(osf_iov[i].iov_len);
	    if (osf_iov[i].iov_base != 0 && osf_iov[i].iov_len != 0)
	      {
		buf =
		  (char *)calloc(MD_SWAPW(osf_iov[i].iov_len), sizeof(char));
		if (!buf)
		  fatal("out of virtual memory in SYS_readv");
		iov[i].iov_base = buf;
	      }
	    else
	      iov[i].iov_base = NULL;
	  }

	/* perform the vector'ed read */
	do {
	  /*result*/error_return =
	    readv(/* fd */(int)regs->regs_R[MD_REG_R0], iov,
		  /* iovcnt */(size_t)regs->regs_R[MD_REG_R2]);
	} while (/*result*/error_return == -1
		 && errno == EAGAIN);

	/* copy target side I/O vector buffers to host memory */
	for (i=0; i < /* iovcnt */regs->regs_R[MD_REG_R2]; i++)
	  {
	    if (osf_iov[i].iov_base != 0)
	      {
		mem_bcopy(mem_fn, mem, Write, MD_SWAPQ(osf_iov[i].iov_base),
			  iov[i].iov_base, MD_SWAPW(osf_iov[i].iov_len));
	      }
	  }

	/* check for an error condition */
	if (error_return != -1) {
		regs->regs_R[MD_REG_R0] = error_return;
	  } /* no error */
	else {
	  	regs->regs_R[MD_REG_R0] = -(errno); /* negative of the error number is returned in r0 */
	  }

	/* free all the allocated memory */
	for (i=0; i < /* iovcnt */regs->regs_R[MD_REG_R2]; i++)
	  {
	    if (iov[i].iov_base)
	      {
		free(iov[i].iov_base);
		iov[i].iov_base = NULL;
	      }
	  }

	if (osf_iov)
	  free(osf_iov);
	if (iov)
	  free(iov);
      }
      break;
#endif

/*------------Subcall of socketcall-------------------------------*/
#if !defined(MIN_SYSCALL_MODE) && 0
    case OSF_SYS_setsockopt:
      {
	char *buf;
	struct xlate_table_t *map;
	int nmap;

 	/* set options on sockets */

	/* copy optval to host memory */
	if (/* optval */regs->regs_R[MD_REG_A3] != 0
	    && /* optlen */regs->regs_R[MD_REG_A4] != 0)
	  {
	    buf = calloc(1, /* optlen */(size_t)regs->regs_R[MD_REG_A4]);
	    if (!buf)
	      fatal("cannot allocate memory in OSF_SYS_setsockopt");
	    
	    /* copy target side pointer data into host side vector */
	    mem_bcopy(mem_fn, mem, Read,
		      /* optval */regs->regs_R[MD_REG_A3],
		      buf, /* optlen */(int)regs->regs_R[MD_REG_A4]);
	  }
	else
	  buf = NULL;

	/* pick the correct translation table */
	if ((int)regs->regs_R[MD_REG_A1] == OSF_SOL_SOCKET)
	  {
	    map = sockopt_map;
	    nmap = N_ELT(sockopt_map);
	  }
	else if ((int)regs->regs_R[MD_REG_A1] == OSF_SOL_TCP)
	  {
	    map = tcpopt_map;
	    nmap = N_ELT(tcpopt_map);
	  }
	else
	  {
	    warn("no translation map available for `setsockopt()': %d",
		 (int)regs->regs_R[MD_REG_A1]);
	    map = sockopt_map;
	    nmap = N_ELT(sockopt_map);
	  }

	/* result */regs->regs_R[MD_REG_V0] =
	  setsockopt(/* sock */(int)regs->regs_R[MD_REG_A0],
		     /* level */xlate_arg((int)regs->regs_R[MD_REG_A1],
					  socklevel_map, N_ELT(socklevel_map),
					  "setsockopt(level)"),
		     /* optname */xlate_arg((int)regs->regs_R[MD_REG_A2],
					    map, nmap,
					    "setsockopt(opt)"),
		     /* optval */buf,
		     /* optlen */regs->regs_R[MD_REG_A4]);

	/* check for an error condition */
	if (regs->regs_R[MD_REG_V0] != (word_t)-1)
	  regs->regs_R[MD_REG_A3] = 0;
	else /* got an error, return details */
	  {
	    regs->regs_R[MD_REG_A3] = -1;
	    regs->regs_R[MD_REG_V0] = errno;
	  }

	if (buf != NULL)
	  free(buf);
      }
      break;
#endif

/* subcall of socketcall in arm linux */
#if !defined(MIN_SYSCALL_MODE) && 0
    case OSF_SYS_old_getsockname:
      {
	/* get socket name */
	char *buf;
	word_t osf_addrlen;
	int addrlen;

	/* get simulator memory parameters to host memory */
	mem_bcopy(mem_fn, mem, Read,
		  /* paddrlen */regs->regs_R[MD_REG_A2],
		  &osf_addrlen, sizeof(osf_addrlen));
	addrlen = (int)osf_addrlen;
	if (addrlen != 0)
	  {
	    buf = calloc(1, addrlen);
	    if (!buf)
	      fatal("cannot allocate memory in OSF_SYS_old_getsockname");
	  }
	else
	  buf = NULL;
	
	/* result */regs->regs_R[MD_REG_V0] =
	  getsockname(/* sock */(int)regs->regs_R[MD_REG_A0],
		      /* name */(struct sockaddr *)buf,
		      /* namelen */&addrlen);

	/* check for an error condition */
	if (regs->regs_R[MD_REG_V0] != (word_t)-1)
	  regs->regs_R[MD_REG_A3] = 0;
	else /* got an error, return details */
	  {
	    regs->regs_R[MD_REG_A3] = -1;
	    regs->regs_R[MD_REG_V0] = errno;
	  }

	/* copy results to simulator memory */
	if (addrlen != 0)
	  mem_bcopy(mem_fn, mem, Write,
		    /* addr */regs->regs_R[MD_REG_A1],
		    buf, addrlen);
	osf_addrlen = (qword_t)addrlen;
	mem_bcopy(mem_fn, mem, Write,
		  /* paddrlen */regs->regs_R[MD_REG_A2],
		  &osf_addrlen, sizeof(osf_addrlen));

	if (buf != NULL)
	  free(buf);
      }
      break;
#endif

/* part socketcall in arm linux */
#if !defined(MIN_SYSCALL_MODE) && 0
    case OSF_SYS_old_getpeername:
      {
	/* get socket name */
	char *buf;
	word_t osf_addrlen;
	int addrlen;

	/* get simulator memory parameters to host memory */
	mem_bcopy(mem_fn, mem, Read,
		  /* paddrlen */regs->regs_R[MD_REG_A2],
		  &osf_addrlen, sizeof(osf_addrlen));
	addrlen = (int)osf_addrlen;
	if (addrlen != 0)
	  {
	    buf = calloc(1, addrlen);
	    if (!buf)
	      fatal("cannot allocate memory in OSF_SYS_old_getsockname");
	  }
	else
	  buf = NULL;
	
	/* result */regs->regs_R[MD_REG_V0] =
	  getpeername(/* sock */(int)regs->regs_R[MD_REG_A0],
		      /* name */(struct sockaddr *)buf,
		      /* namelen */&addrlen);

	/* check for an error condition */
	if (regs->regs_R[MD_REG_V0] != (word_t)-1)
	  regs->regs_R[MD_REG_A3] = 0;
	else /* got an error, return details */
	  {
	    regs->regs_R[MD_REG_A3] = -1;
	    regs->regs_R[MD_REG_V0] = errno;
	  }

	/* copy results to simulator memory */
	if (addrlen != 0)
	  mem_bcopy(mem_fn, mem, Write,
		    /* addr */regs->regs_R[MD_REG_A1],
		    buf, addrlen);
	osf_addrlen = (qword_t)addrlen;
	mem_bcopy(mem_fn, mem, Write,
		  /* paddrlen */regs->regs_R[MD_REG_A2],
		  &osf_addrlen, sizeof(osf_addrlen));

	if (buf != NULL)
	  free(buf);
      }
      break;
#endif
/*-----------------------------------------------------------------------------------*/

#if !defined(MIN_SYSCALL_MODE)
    case ARM_SYS_setgid:
      /* set group ID */

      /*result*/regs->regs_R[MD_REG_R0] =
	setgid(/* gid */(gid_t)regs->regs_R[MD_REG_R0]);

      /* check for an error condition */
      if (regs->regs_R[MD_REG_R0] == (word_t)-1)
	  /* got an error, return details */
	    regs->regs_R[MD_REG_R0] = -errno;
      break;
#endif

#if !defined(MIN_SYSCALL_MODE)
    case ARM_SYS_setuid:
      /* set user ID */

      /*result*/regs->regs_R[MD_REG_R0] =
	setuid(/* uid */(uid_t)regs->regs_R[MD_REG_R0]);

      /* check for an error condition */
      if (regs->regs_R[MD_REG_R0] == (word_t)-1)
	  /* got an error, return details */
	    regs->regs_R[MD_REG_R0] = -errno;
      break;
#endif

#if !defined(MIN_SYSCALL_MODE)
    case ARM_SYS_getpriority:
      /* get program scheduling priority */

      /*result*/regs->regs_R[MD_REG_R0] =
	getpriority(/* which */(int)regs->regs_R[MD_REG_R0],
		    /* who */(int)regs->regs_R[MD_REG_R1]);

      /* check for an error condition */
      if (regs->regs_R[MD_REG_R0] == (word_t)-1)
	  /* got an error, return details */
	    regs->regs_R[MD_REG_R0] = -errno;
      break;
#endif

#if !defined(MIN_SYSCALL_MODE)
    case ARM_SYS_setpriority:
      /* set program scheduling priority */

      /*result*/regs->regs_R[MD_REG_R0] =
	setpriority(/* which */(int)regs->regs_R[MD_REG_R0],
		    /* who */(int)regs->regs_R[MD_REG_R1],
		    /* prio */(int)regs->regs_R[MD_REG_R2]);

      /* check for an error condition */
      if (regs->regs_R[MD_REG_R0] == (word_t)-1)
	  /* got an error, return details */
	    regs->regs_R[MD_REG_R0] = -errno;
      break;
#endif

#if !defined(MIN_SYSCALL_MODE)
    case ARM_SYS_select:
      {
	fd_set readfd, writefd, exceptfd;
	fd_set *readfdp, *writefdp, *exceptfdp;
	struct timeval timeout, *timeoutp;

	/* copy read file descriptor set into host memory */
	if (/* readfds */regs->regs_R[MD_REG_R1] != 0)
	  {
	    mem_bcopy(mem_fn, mem, Read,
		      /* readfds */regs->regs_R[MD_REG_R1],
		      &readfd, sizeof(fd_set));
	    readfdp = &readfd;
	  }
	else
	  readfdp = NULL;

	/* copy write file descriptor set into host memory */
	if (/* writefds */regs->regs_R[MD_REG_R2] != 0)
	  {
	    mem_bcopy(mem_fn, mem, Read,
		      /* writefds */regs->regs_R[MD_REG_R2],
		      &writefd, sizeof(fd_set));
	    writefdp = &writefd;
	  }
	else
	  writefdp = NULL;

	/* copy exception file descriptor set into host memory */
	if (/* exceptfds */regs->regs_R[MD_REG_R3] != 0)
	  {
	    mem_bcopy(mem_fn, mem, Read,
		      /* exceptfds */regs->regs_R[MD_REG_R3],
		      &exceptfd, sizeof(fd_set));
	    exceptfdp = &exceptfd;
	  }
	else
	  exceptfdp = NULL;

	/* copy timeout value into host memory */
	if (/* timeout */regs->regs_R[MD_REG_R4] != 0)
	  {
	    mem_bcopy(mem_fn, mem, Read,
		      /* timeout */regs->regs_R[MD_REG_R4],
		      &timeout, sizeof(struct timeval));
	    timeoutp = &timeout;
	  }
	else
	  timeoutp = NULL;

#if defined(hpux) || defined(__hpux)
	/* select() on the specified file descriptors */
	/* result */regs->regs_R[MD_REG_R0] =
	  select(/* nfds */regs->regs_R[MD_REG_R0],
		 (int *)readfdp, (int *)writefdp, (int *)exceptfdp, timeoutp);
#else
	/* select() on the specified file descriptors */
	/* result */regs->regs_R[MD_REG_R0] =
	  select(/* nfds */regs->regs_R[MD_REG_R0],
		 readfdp, writefdp, exceptfdp, timeoutp);
#endif

	/* check for an error condition */
	if (regs->regs_R[MD_REG_R0] == (word_t)-1)
	  /* got an error, return details */
	    regs->regs_R[MD_REG_R0] = -errno;

	/* copy read file descriptor set to target memory */
	if (/* readfds */regs->regs_R[MD_REG_R1] != 0)
	  mem_bcopy(mem_fn, mem, Write,
		    /* readfds */regs->regs_R[MD_REG_R1],
		    &readfd, sizeof(fd_set));

	/* copy write file descriptor set to target memory */
	if (/* writefds */regs->regs_R[MD_REG_R2] != 0)
	  mem_bcopy(mem_fn, mem, Write,
		    /* writefds */regs->regs_R[MD_REG_R2],
		    &writefd, sizeof(fd_set));

	/* copy exception file descriptor set to target memory */
	if (/* exceptfds */regs->regs_R[MD_REG_R3] != 0)
	  mem_bcopy(mem_fn, mem, Write,
		    /* exceptfds */regs->regs_R[MD_REG_R3],
		    &exceptfd, sizeof(fd_set));

	/* copy timeout value result to target memory */
	if (/* timeout */regs->regs_R[MD_REG_R4] != 0)
	  mem_bcopy(mem_fn, mem, Write,
		    /* timeout */regs->regs_R[MD_REG_R4],
		    &timeout, sizeof(struct timeval));
      }
      break;
#endif

/* part of socketcall in arm linux */
#if !defined(MIN_SYSCALL_MODE) && 0
    case ARM_SYS_shutdown:
      /* shuts down socket send and receive operations */

      /*result*/regs->regs_R[MD_REG_R0] =
	shutdown(/* sock */(int)regs->regs_R[MD_REG_R0],
		 /* how */(int)regs->regs_R[MD_REG_R1]);

      /* check for an error condition */
      if (regs->regs_R[MD_REG_R0] == (word_t)-1)
	  /* got an error, return details */
	    regs->regs_R[MD_REG_R0] = -errno;
      break;
#endif

#if !defined(MIN_SYSCALL_MODE)
    case ARM_SYS_poll:
      {
	int i;
	int error_return;
	struct pollfd *fds;

	/* allocate host side I/O vectors */
	fds = calloc(/* nfds */regs->regs_R[MD_REG_R1], sizeof(struct pollfd));
	if (!fds)
	  fatal("out of virtual memory in SYS_poll");

	/* copy target side I/O vector buffers to host memory */
	for (i=0; i < /* nfds */regs->regs_R[MD_REG_R1]; i++)
	  {
	    /* copy target side pointer data into host side vector */
	    mem_bcopy(mem_fn, mem, Read,
		      (/* fds */regs->regs_R[MD_REG_R0]
		       + i*sizeof(struct pollfd)),
		      &fds[i], sizeof(struct pollfd));
	  }

	/* perform the vector'ed write */
	/* result */error_return =
	  poll(/* fds */fds,
	       /* nfds */(unsigned long)regs->regs_R[MD_REG_R1],
	       /* timeout */(int)regs->regs_R[MD_REG_R2]);

	/* copy target side I/O vector buffers to host memory */
	for (i=0; i < /* nfds */regs->regs_R[MD_REG_R1]; i++)
	  {
	    /* copy target side pointer data into host side vector */
	    mem_bcopy(mem_fn, mem, Write,
		      (/* fds */regs->regs_R[MD_REG_A0]
		       + i*sizeof(struct pollfd)),
		      &fds[i], sizeof(struct pollfd));
	  }

	/* check for an error condition */
	if (error_return != -1) {
		regs->regs_R[MD_REG_R0] = error_return;
	} /* no error */
	else {
	  	regs->regs_R[MD_REG_R0] = -(errno); /* negative of the error number is returned in r0 */
	}

	

	/* free all the allocated memory */
	free(fds);
      }
      break;
#endif

#if XXX
    case OSF_SYS_usleep_thread:
#if 0
      fprintf(stderr, "usleep(%d)\n", (unsigned int)regs->regs_R[MD_REG_A0]);
#endif
#ifdef alpha
      regs->regs_R[MD_REG_V0] = usleep((unsigned int)regs->regs_R[MD_REG_A0]);
#else
      usleep((unsigned int)regs->regs_R[MD_REG_A0]);
      regs->regs_R[MD_REG_V0] = 0;
#endif
      /* check for an error condition */
      if (regs->regs_R[MD_REG_V0] != (word_t)-1)
        regs->regs_R[MD_REG_A3] = 0;
      else /* got an error, return details */
        {
          regs->regs_R[MD_REG_A3] = -1;
          regs->regs_R[MD_REG_V0] = errno;
        }
#if 0
      warn("unsupported usleep_thread() call...");
      regs->regs_R[MD_REG_V0] = 0; 
#endif
      break;
#endif
      
/* part of socketcall in arm linux */
#if !defined(MIN_SYSCALL_MODE) && 0
    case OSF_SYS_gethostname:
      /* get program scheduling priority */
      {
	char *buf;

	buf = malloc(/* len */(size_t)regs->regs_R[MD_REG_A1]);
	if (!buf)
	  fatal("out of virtual memory in gethostname()");

	/* result */regs->regs_R[MD_REG_V0] =
	  gethostname(/* name */buf,
		      /* len */(size_t)regs->regs_R[MD_REG_A1]);

	/* check for an error condition */
	if (regs->regs_R[MD_REG_V0] != (word_t)-1)
	  regs->regs_R[MD_REG_A3] = 0;
	else /* got an error, return details */
	  {
	    regs->regs_R[MD_REG_A3] = -1;
	    regs->regs_R[MD_REG_V0] = errno;
	  }

	/* copy string back to simulated memory */
	mem_bcopy(mem_fn, mem, Write,
		  /* name */regs->regs_R[MD_REG_A0],
		  buf, /* len */regs->regs_R[MD_REG_A1]);
      }
      break;
#endif

#if XXX
    case OSF_SYS_madvise:
      warn("unsupported madvise() call ignored...");
      regs->regs_R[MD_REG_V0] = 0;
      break;
#endif
/* The entry into the sockets function calls!!! */
#if !defined(MIN_SYSCALL_MODE)
    case ARM_SYS_socketcall:
    /* the first argument is the socket call function type */
    switch((int)regs->regs_R[MD_REG_R0]){
          case ARM_SYS_SOCKET:
	  {
          /* create an endpoint for communication */
          int __domain,__type,__protocol;
             /* grab the socket call arguments from simulated memory */
             mem_bcopy(mem_fn, mem, Read, regs->regs_R[MD_REG_R1], &__domain,
		  /*nbytes*/sizeof(int));
             mem_bcopy(mem_fn, mem, Read, (regs->regs_R[MD_REG_R1]+sizeof(int)), &__type,
		  /*nbytes*/sizeof(int));
             mem_bcopy(mem_fn, mem, Read, (regs->regs_R[MD_REG_R1]+2*sizeof(int)), &__protocol,
		  /*nbytes*/sizeof(int));

 
           /* result */regs->regs_R[MD_REG_R0] =
	   socket(/* domain */xlate_arg(__domain,
				     family_map, N_ELT(family_map),
				     "socket(family)"),
	       /* type */xlate_arg(__type,
				   socktype_map, N_ELT(socktype_map),
				   "socket(type)"),
	       /* protocol */xlate_arg(__protocol,
				       family_map, N_ELT(family_map),
				       "socket(proto)"));


          /* check for an error condition */
              if (regs->regs_R[MD_REG_R0] == (word_t)-1)
	  /* got an error, return details */
	         regs->regs_R[MD_REG_R0] = -errno;
          }
          break;

          case ARM_SYS_BIND:
          {
	     const struct sockaddr a_sock;
             int __sockfd,__addrlen,__sim_a_sock;

             /* grab the function call arguments from memory */
             mem_bcopy(mem_fn, mem, Read,regs->regs_R[MD_REG_R0], 
                    &__sockfd,sizeof(int));
	     mem_bcopy(mem_fn, mem, Read,(regs->regs_R[MD_REG_R0]+sizeof(int)),
		    &__sim_a_sock, sizeof(int));
	     mem_bcopy(mem_fn, mem, Read,
                    (regs->regs_R[MD_REG_R0]+2*sizeof(int)),
		    &__addrlen, sizeof(int));

             /* copy the sockadd to real memory */
             mem_bcopy(mem_fn, mem, Read,__sim_a_sock,
		    &a_sock, sizeof(struct sockaddr));
             
             regs->regs_R[MD_REG_R0] =
	        bind(__sockfd, &a_sock,__addrlen);

             /* check for an error condition */
             
             /*NOT sure if the commented code is done since it is a subcall?*/
             //if (regs->regs_R[MD_REG_R0] != (word_t)-1)
	     //regs->regs_R[MD_REG_R3] = 0;
             //else /* got an error, return details */
	     //{
	     //   regs->regs_R[MD_REG_R3] = -1;
	     //   regs->regs_R[MD_REG_R0] = errno;
	     //}
          }
          break;

          case ARM_SYS_CONNECT:
	  {
 	     struct osf_sockaddr osf_sa;
             int __sockfd,__addrlen,__sim_addr;
	  /* initiate a connection on a socket */

	  /*copy the arguments from simulated memory */ 
             mem_bcopy(mem_fn, mem, Read,regs->regs_R[MD_REG_R0], 
                    &__sockfd,sizeof(int));
	     mem_bcopy(mem_fn, mem, Read,(regs->regs_R[MD_REG_R0]+sizeof(int)),
		    &__sim_addr, sizeof(int));
	     mem_bcopy(mem_fn, mem, Read,(regs->regs_R[MD_REG_R0]+2*sizeof(int)),
		    &__addrlen, sizeof(int));

          
	/* copy sockaddr structure to host memory */
	mem_bcopy(mem_fn, mem, Read, /* serv_addr */__sim_addr,
		  &osf_sa, sizeof(struct osf_sockaddr));

#if 0
	int i;
	sa.sa_family = osf_sa.sa_family;
	for (i=0; i < regs->regs_R[MD_REG_R2]; i++)
	  sa.sa_data[i] = osf_sa.sa_data[i];
#endif
	/* result */regs->regs_R[MD_REG_R0] =
	  connect(/* sockfd */__sockfd,
		  (void *)&osf_sa, __addrlen);

	  /* check for an error condition */
	  if (regs->regs_R[MD_REG_R0] == (word_t)-1)
	     /* got an error, return details */
	       regs->regs_R[MD_REG_R0] = -errno;
          }
          break;

          
          case ARM_SYS_LISTEN:
          {
             warn("invalid/unimplemented socket function, PC=0x%08p, winging it"
                , regs->regs_PC);
             abort();
          }
          break;
          case ARM_SYS_ACCEPT:
	  {
            warn("invalid/unimplemented socket function, PC=0x%08p, winging it"
                , regs->regs_PC);
             abort();
          }
          break;
          case ARM_SYS_GETSOCKNAME:
          {
	    /* get socket name */
	    char *buf;
	    word_t osf_addrlen;
	    int addrlen;
            int __s, __name, __namelen;
	    /*copy the arguments from simulated memory */ 
            mem_bcopy(mem_fn, mem, Read,regs->regs_R[MD_REG_R0], 
                    &__s,sizeof(int));
	    mem_bcopy(mem_fn, mem, Read,(regs->regs_R[MD_REG_R0]+sizeof(int)),
		    &__name, sizeof(int));
	    mem_bcopy(mem_fn, mem, Read,(regs->regs_R[MD_REG_R0]+2*sizeof(int)),
		    &__namelen, sizeof(int));

	    /* get simulator memory parameters to host memory */
	    mem_bcopy(mem_fn, mem, Read,
		      /* paddrlen */__namelen,
		  &osf_addrlen, sizeof(osf_addrlen));
	    addrlen = (int)osf_addrlen;
	    if (addrlen != 0)
	    {
	      buf = calloc(1, addrlen);
	      if (!buf)
	        fatal("cannot allocate memory in OSF_SYS_old_getsockname");
	    }
	    else
	      buf = NULL;

            /* do the actual system call on the bative machine */	
	    /* result */regs->regs_R[MD_REG_V0] =
	    getsockname(/* sock */__s,
		      /* name */(struct sockaddr *)buf,
		      /* namelen */&addrlen);

	    /* check for an error condition */
	    if (regs->regs_R[MD_REG_R0] != (word_t)-1)
              ;
	      //	      regs->regs_R[MD_REG_A3] = 0;
	    else /* got an error, return details */
	    {
	      //regs->regs_R[MD_REG_A3] = -1;
	      regs->regs_R[MD_REG_V0] = errno;
	    }

	    /* copy results to simulator memory */
	    if (addrlen != 0)
	       mem_bcopy(mem_fn, mem, Write,
		    /* addr */regs->regs_R[MD_REG_A1],
		    buf, addrlen);

	    osf_addrlen = (qword_t)addrlen;
	    mem_bcopy(mem_fn, mem, Write,
		  /* paddrlen */__namelen,
		  &osf_addrlen, sizeof(osf_addrlen));

	    if (buf != NULL)
	      free(buf);
          }	  
          break;

          case ARM_SYS_GETPEERNAME:
          {
	    /* get socket name */
	    char *buf;
	    word_t osf_addrlen;
	    int addrlen;
            int __s, __name, __namelen;
            /*grab the function call arguments from sim memory*/
	    mem_bcopy(mem_fn, mem, Read,
		  (regs->regs_R[MD_REG_R1]),
		  &__s, sizeof(int));

            mem_bcopy(mem_fn, mem, Read,
		  (regs->regs_R[MD_REG_R1]+sizeof(int)),
		  &__name, sizeof(int));

            mem_bcopy(mem_fn, mem, Read,
		  (regs->regs_R[MD_REG_R1]+2*sizeof(int)),
		  &__namelen, sizeof(int));


	    /* get simulator memory parameters to host memory */
	    mem_bcopy(mem_fn, mem, Read,
		  /* paddrlen */__namelen,
		  &osf_addrlen, sizeof(osf_addrlen));
	    addrlen = (int)osf_addrlen;

            /* allocate host memory for system call result */
	    if (addrlen != 0)
	    {
	      buf = calloc(1, addrlen);
	      if (!buf)
	        fatal("cannot allocate memory in OSF_SYS_old_getsockname");
	    }
	    else
	      buf = NULL;
	
	  /* result */regs->regs_R[MD_REG_R0] =
	    getpeername(/* sock */__s,
		      /* name */(struct sockaddr *)buf,
		      /* namelen */&addrlen);

	  /* check for an error condition */
	    /* NOT sure how to handle this yet ??
               do we set memory?? in arm*/
	    if (regs->regs_R[MD_REG_R0] != (word_t)-1);
	    //regs->regs_R[MD_REG_A3] = 0;
	    else /* got an error, return details */
	    //  {
	    //    regs->regs_R[MD_REG_A3] = -1;
	        regs->regs_R[MD_REG_R0] = errno;
	    //  }

	  /* copy results to simulator memory */
	  if (addrlen != 0)
	    mem_bcopy(mem_fn, mem, Write,
		    /* addr */__name,
		    buf, addrlen);

	    osf_addrlen = (qword_t)addrlen;
	    mem_bcopy(mem_fn, mem, Write,
		  /* paddrlen */__namelen,
		  &osf_addrlen, sizeof(osf_addrlen));

	     if (buf != NULL)
	      free(buf);
          }
          break;

          case ARM_SYS_SOCKETPAIR:
	  {
            warn("invalid/unimplemented socket function, PC=0x%08p, winging it"
                , regs->regs_PC);
             abort();
          }
          break;          
  
          case ARM_SYS_SEND:
	  {

            warn("invalid/unimplemented socket function, PC=0x%08p, winging it"
                , regs->regs_PC);
             abort();
          }
          break;

          case ARM_SYS_RECV:
	  {
            warn("invalid/unimplemented socket function, PC=0x%08p, winging it"
                , regs->regs_PC);
             abort();
          }
          break;
 
          case ARM_SYS_SENDTO:
          {
	     char *buf = NULL;
	     struct sockaddr d_sock;
	     int buf_len = 0;
             int __s, __msg, __len, __flags, __to, __tolen;
            /*grab the function call arguments from sim memory*/
	    mem_bcopy(mem_fn, mem, Read,
		  (regs->regs_R[MD_REG_R1]),
		  &__s, sizeof(int));

            mem_bcopy(mem_fn, mem, Read,
		  (regs->regs_R[MD_REG_R1]+sizeof(int)),
		  &__msg, sizeof(int));

            mem_bcopy(mem_fn, mem, Read,
		  (regs->regs_R[MD_REG_R1]+2*sizeof(int)),
		  &__len, sizeof(int));

            mem_bcopy(mem_fn, mem, Read,
		  (regs->regs_R[MD_REG_R1]+3*sizeof(int)),
		  &__flags, sizeof(int));

            mem_bcopy(mem_fn, mem, Read,
		  (regs->regs_R[MD_REG_R1]+4*sizeof(int)),
		  &__to, sizeof(int));

            mem_bcopy(mem_fn, mem, Read,
		  (regs->regs_R[MD_REG_R1]+5*sizeof(int)),
		  &__tolen, sizeof(int));


	     buf_len = __len;
             /* make a buffer in host memory for system call */
	     if (buf_len > 0)
	       buf = (char *) malloc(buf_len*sizeof(char));

	     /* copy the message from simualted memory to host memory */
	      mem_bcopy(mem_fn, mem, Read, /* serv_addr */__msg,
		  buf, /* addrlen */__len);

	     if (__tolen > 0) 
	       mem_bcopy(mem_fn, mem, Read, __to,
		    &d_sock, __tolen);

             /* make the actual system call */
	     regs->regs_R[MD_REG_R0] =
	     sendto(__s,buf,__len,__flags,&d_sock,__tolen);

	     mem_bcopy(mem_fn, mem, Write, /* serv_addr*/__msg,
		  buf, /* addrlen */__len);

	   /* maybe copy back whole size of sockaddr */
	   if (__tolen > 0)
	      mem_bcopy(mem_fn, mem, Write, __to,
		    &d_sock, __tolen);

	   /* Not sure what to do with the error conditions yet */
	    /* check for an error condition */
	    if (regs->regs_R[MD_REG_R0] != (word_t)-1)
              ;
             
	      //  regs->regs_R[MD_REG_A3] = 0;
	    else /* got an error, return details */
	    {
	      //regs->regs_R[MD_REG_A3] = -1;
	      regs->regs_R[MD_REG_R0] = errno;
	    }

	    if (buf != NULL) 
	      free(buf);
          }	 
          break;

          case ARM_SYS_RECVFROM:
          {
	    int addr_len;
	    char *buf;
	    struct sockaddr *a_sock;
            int __s, __buf, __len, __flags, __from, __fromlen;          
             /*grab the function call arguments from sim memory*/
	    mem_bcopy(mem_fn, mem, Read,
		  (regs->regs_R[MD_REG_R1]),
		  &__s, sizeof(int));

            mem_bcopy(mem_fn, mem, Read,
		  (regs->regs_R[MD_REG_R1]+sizeof(int)),
		  &__buf, sizeof(int));

            mem_bcopy(mem_fn, mem, Read,
		  (regs->regs_R[MD_REG_R1]+2*sizeof(int)),
		  &__len, sizeof(int));

            mem_bcopy(mem_fn, mem, Read,
		  (regs->regs_R[MD_REG_R1]+3*sizeof(int)),
		  &__flags, sizeof(int));

            mem_bcopy(mem_fn, mem, Read,
		  (regs->regs_R[MD_REG_R1]+4*sizeof(int)),
		  &__from, sizeof(int));

            mem_bcopy(mem_fn, mem, Read,
		  (regs->regs_R[MD_REG_R1]+5*sizeof(int)),
		  &__fromlen, sizeof(int));

	    buf = (char *) malloc((__len));

	    mem_bcopy(mem_fn, mem, Read, /* serv_addr */__buf,
		  buf, /* addrlen */__len);

	    mem_bcopy(mem_fn, mem, Read, /* serv_addr */__fromlen,
		  &addr_len, sizeof(int));

            /* make a buffer in host memory for the socket address */
	    a_sock = (struct sockaddr *)malloc(addr_len);

	    mem_bcopy(mem_fn, mem, Read, __from,
		  a_sock, addr_len);

            /* make the actual system call */
	    regs->regs_R[MD_REG_R0] =
	    recvfrom(__s, buf,__len,__flags, a_sock,&addr_len);

	    mem_bcopy(mem_fn, mem, Write, __buf,
		  buf, (int) regs->regs_R[MD_REG_R0]);

	    mem_bcopy(mem_fn, mem, Write, /* serv_addr */__fromlen,
		  &addr_len, sizeof(int));

	    mem_bcopy(mem_fn, mem, Write, __from,
		  a_sock, addr_len);

	    /* check for an error condition */
	    if (regs->regs_R[MD_REG_R0] != (word_t)-1)
               ;
	    //regs->regs_R[MD_REG_A3] = 0;
	    else /* got an error, return details */
	       {
		 // regs->regs_R[MD_REG_A3] = -1;
	       regs->regs_R[MD_REG_V0] = errno;
	       }
	    if (buf != NULL)
	      free(buf);
          }
          break;

          case ARM_SYS_SHUTDOWN:
	  {
            /* can't find docs on this winging it!! */
            int __arg1, __arg2;
	    mem_bcopy(mem_fn, mem, Read,
		  (regs->regs_R[MD_REG_R1]),
		  &__arg1, sizeof(int));

            mem_bcopy(mem_fn, mem, Read,
		  (regs->regs_R[MD_REG_R1]+sizeof(int)),
		  &__arg2, sizeof(int));
            /* shuts down socket send and receive operations */

            /*result*/regs->regs_R[MD_REG_R0] =
	    shutdown(/* sock */__arg1,
		   /* how */__arg2);

            /* check for an error condition */
            if (regs->regs_R[MD_REG_R0] == (word_t)-1)
	    /* got an error, return details */
	       regs->regs_R[MD_REG_R0] = -errno;
          }
          break;

          case ARM_SYS_SETSOCKOPT:
          {
	    char *buf;
	    struct xlate_table_t *map;
	    int nmap;
            int __s, __level, __optname, __optval, __optlen;
	    mem_bcopy(mem_fn, mem, Read,
		  (regs->regs_R[MD_REG_R1]),
		  &__s, sizeof(int));

            mem_bcopy(mem_fn, mem, Read,
		  (regs->regs_R[MD_REG_R1]+sizeof(int)),
		  &__level, sizeof(int));

            mem_bcopy(mem_fn, mem, Read,
		  (regs->regs_R[MD_REG_R1]+2*sizeof(int)),
		  &__optname, sizeof(int));

            mem_bcopy(mem_fn, mem, Read,
		  (regs->regs_R[MD_REG_R1]+3*sizeof(int)),
		  &__optval, sizeof(int));

            mem_bcopy(mem_fn, mem, Read,
		  (regs->regs_R[MD_REG_R1]+4*sizeof(int)),
		  &__optlen, sizeof(int));


 	    /* set options on sockets */

	    /* copy optval to host memory */
	    if (/* optval */__optval != 0
	       && /* optlen */__optlen != 0)
	    {
	      buf = calloc(1, /* optlen */(size_t)__optlen);
	      if (!buf)
	        fatal("cannot allocate memory in OSF_SYS_setsockopt");
	    
	      /* copy target side pointer data into host side vector */
	      mem_bcopy(mem_fn, mem, Read,
		      /* optval */__optval,
		      buf, /* optlen */__optlen);
	    }
	    else
	      buf = NULL;

	    /* pick the correct translation table */
	    if (__level == OSF_SOL_SOCKET)
	    {
	      map = sockopt_map;
	      nmap = N_ELT(sockopt_map);
	    }
	    else if (__level == OSF_SOL_TCP)
	    {
	      map = tcpopt_map;
	      nmap = N_ELT(tcpopt_map);
	    }
	    else
	    {
	      warn("no translation map available for `setsockopt()': %d",
		 __level);
	      map = sockopt_map;
	      nmap = N_ELT(sockopt_map);
	    }

	    /* result */regs->regs_R[MD_REG_R0] =
	    setsockopt(/* sock */__s,
		     /* level */xlate_arg(__level,
					  socklevel_map, N_ELT(socklevel_map),
					  "setsockopt(level)"),
		     /* optname */xlate_arg(__optname,
					    map, nmap,
					    "setsockopt(opt)"),
		     /* optval */buf,
		     /* optlen */__optlen);

            /*not sure how to handle errors yet */        
	    /* check for an error condition */
	    if (regs->regs_R[MD_REG_R0] != (word_t)-1)
              ;
	      //  regs->regs_R[MD_REG_A3] = 0;
	    else /* got an error, return details */
	    {
	      //  regs->regs_R[MD_REG_A3] = -1;
	      regs->regs_R[MD_REG_R0] = errno;
	    }

	    if (buf != NULL)
	      free(buf);
          }	 
          break;

          case ARM_SYS_GETSOCKOPT:
	  {
            warn("invalid/unimplemented socket function, PC=0x%08p, winging it"
                , regs->regs_PC);
             abort();
          }
          break;          

          case ARM_SYS_SENDMSG:
	  {
            warn("invalid/unimplemented socket function, PC=0x%08p, winging it"
                , regs->regs_PC);
             abort();
          }
          break;

          case ARM_SYS_RECVMSG:
	  {
            warn("invalid/unimplemented socket function, PC=0x%08p, winging it"
                , regs->regs_PC);
             abort();
          }
          break;
          default:
             warn("invalid/unimplemented socket function, PC=0x%08p, winging it"
                , regs->regs_PC);
             abort();
     }
     break;
#endif


    default:
      warn("invalid/unimplemented syscall %d, PC=0x%08p, winging it",
	   (int)syscode, regs->regs_PC);
      /* declare an error condition */
      regs->regs_R[MD_REG_R0] = -EINVAL;
    }

  if (verbose)
    fprintf(stderr, "syscall(%d): returned 0x%08x(%d)...\n",
      (int)syscode, regs->regs_R[MD_REG_R0], regs->regs_R[MD_REG_R0]);
}

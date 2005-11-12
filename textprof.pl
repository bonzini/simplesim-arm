#!/usr/intel/bin/perl

#
# textprof - text segment profile viewer
#
# This file is a part of the SimpleScalar tool suite written by
# Todd M. Austin as a part of the Multiscalar Research Project.
#  
# The tool suite is currently maintained by Doug Burger and Todd M. Austin.
# 
# Copyright (C) 1994, 1995, 1996, 1997 by Todd M. Austin
#
# This source file is distributed "as is" in the hope that it will be
# useful.  It is distributed with no warranty, and no author or
# distributor accepts any responsibility for the consequences of its
# use. 
#
# Everyone is granted permission to copy, modify and redistribute
# this source file under the following conditions:
#
#    This tool set is distributed for non-commercial use only. 
#    Please contact the maintainer for restrictions applying to 
#    commercial use of these tools.
#
#    Permission is granted to anyone to make or distribute copies
#    of this source code, either as received or modified, in any
#    medium, provided that all copyright notices, permission and
#    nonwarranty notices are preserved, and that the distributor
#    grants the recipient permission for further redistribution as
#    permitted by this document.
#
#    Permission is granted to distribute this file in compiled
#    or executable form under the same conditions that apply for
#    source code, provided that either:
#
#    A. it is accompanied by the corresponding machine-readable
#       source code,
#    B. it is accompanied by a written offer, with no time limit,
#       to give anyone a machine-readable copy of the corresponding
#       source code in return for reimbursement of the cost of
#       distribution.  This written offer must permit verbatim
#       duplication by anyone, or
#    C. it is distributed by someone who received only the
#       executable form, and is accompanied by a copy of the
#       written offer of source code that they received concurrently.
#
# In other words, you are welcome to use, share and improve this
# source file.  You are forbidden to forbid anyone else to use, share
# and improve what you give them.
#
# INTERNET: dburger@cs.wisc.edu
# US Mail:  1210 W. Dayton Street, Madison, WI 53706
#
# $Id: textprof.pl,v 1.1.1.1 2000/11/29 14:53:54 cu-cs Exp $
#
# $Log: textprof.pl,v $
# Revision 1.1.1.1  2000/11/29 14:53:54  cu-cs
# Grand unification of arm sources.
#
#
# Revision 1.1.1.1  2000/05/26 15:18:59  taustin
# SimpleScalar Tool Set
#
#
# Revision 1.1  1998/08/27 16:41:31  taustin
# Initial revision
#
# 

#
# config parms
#
$gzip_cmd = "gzip -dc";

#
# parse commands
#
if (@ARGV != 3)
  {
     print STDERR
"Usage: textprof <disassembly_file> <simulator_output> <stat_name>\n".
"\n".
"         where <disassembly_file> is a disassembly file, generated with\n".
"         the command \"objdump -d <binary>\", <simulator_output> is the\n".
"         simulator output containing a text-based profile, and <stat_name>\n".
"         is the name of the text-based profile to be viewed.  Inputs may\n".
"         be gzip\'ed (.gz) or compress\'ed (.Z).\n".
"\n".
"         Example usage:\n".
"\n".
"           objdump -d test-math >&! test-math.dis\n".
"           sim-profile -pcstat sim_num_insn test-math >&! test-math.out\n".
"           textprof test-math.dis test-math.out sim_num_insn\n".
"\n";
     exit -1;
  }

$dis_file = $ARGV[0];
$sim_output = $ARGV[1];
$stat_name = $ARGV[2];

#
# parse out the <stat_name> profile from <sim_output>
#
if (grep(/.gz$/, $sim_output) || grep(/.Z$/, $sim_output) )
  {
    open(SIM_OUTPUT, "$gzip_cmd $sim_output |")
	|| die "Cannot open simulator output file: $sim_output\n";
  }
else
  {
    open(SIM_OUTPUT, $sim_output)
	|| die "Cannot open simulator output file: $sim_output\n";
  }
$parse_mode = "FIND_START";
while (<SIM_OUTPUT>)
  {
    # detect start of stats
    if ($parse_mode eq "FIND_START" && (/^$stat_name\.start_dist$/))
      {
        $parse_mode = "FIND_END";
	next;
      }
    if ($parse_mode eq "FIND_END" && (/^$stat_name\.end_dist$/))
      {
	$parse_mode = "FOUND_STAT";
	last;
      }
    if ($parse_mode eq "FIND_END")
      {
	# parse a real histo line
	if (/^(0x[0-9a-fA-F]+)\s+(\d+)\s+([0-9.]+)$/)
	  {
	    $histo_exists{hex($1)} = 1;
	    $histo_freq{hex($1)} = $2;
	    $histo_frac{hex($1)} = $3;
	  }
	else
	  {
	    print "** WARNING ** could not parse line: `$_'\n";
	  }
      }
  }
if ($parse_mode ne "FOUND_STAT")
  {
    print STDERR "** FATAL ** couldn't find histogram `$stat_name' in `$sim_output'\n";
    exit -1;
  }
close(SIM_OUTPUT);

#
# consume the disassembly file, spew back out with simulator stats
#
if (grep(/.gz$/, $dis_file) || grep(/.Z$/, $dis_file) )
  {
    open(DIS_FILE, "$gzip_cmd $dis_file |")
	|| die "Cannot open disassembly file: $dis_file\n";
  }
else
  {
    open(DIS_FILE, $dis_file)
	|| die "Cannot open disassembly file: $dis_file\n";
  }

# print text profile header
print "Text profile view for statistic `$stat_name'.\n";
print "Statistics extracted from `$sim_output'.\n";
print "Statistics extracted bound to disassembly file `$dis_file'.\n";
print "Text statistics are shown in the form (<count>, <% of total>).\n";
print "\n";

while (<DIS_FILE>)
  {
    if (/^([0-9a-fA-F]+)\s+(.*)$/)
      {
	if ($histo_exists{hex($1)})
	  {
	    printf "%s:  (%11d, %6.2f): %s\n",
	      $1, $histo_freq{hex($1)}, $histo_frac{hex($1)}, $2;
	    $histo_probed{hex($1)} = 1;
	  }
	else
	  {
	    printf "%s:                       : %s\n", $1, $2;
	  }
      }
    else
      {
	# various other disassembly lines, just echo them out as is
	print;
      }
  }
close(DIS_FILE);

#
# check for unprobed EIP stats
#
$cnt = 0;
$unbnd = 0;
foreach $addr (keys %histo_exists)
  {
    if (!$histo_probed{$addr})
      {
	$cnt++;
	if ($cnt > 5)
	  {
	    $unbnd++;
	  }
	else
	  {
	    printf "** WARNING ** address 0x%08x was not bound to disassembly output.\n", $addr;
	  }
      }
  }
if ($unbnd)
  {
    print "** WARNING ** $unbnd more unbound addresses were detected.\n";
  }

exit 0;

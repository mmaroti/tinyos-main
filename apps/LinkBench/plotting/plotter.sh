#!/bin/bash

PROGNAME=${0##*/} 
LOGFILE=${0%.*}.log

# supported statistics
STAT_LIST="TC BC RC SC SSC SFC SDC SDSC SDFC WAC NAC RCC EXC WC FC DRC MC REMC";
PROFILE_LIST="MAT MINT MLAT RXTX RST MSGC RXB TXB DBG";

SHORTOPTS="hb:e:x:y:o:t:" 
LONGOPTS="min,max,info,help"
# default values
BENCH_IDX=""
EXPR=""
XLABEL=""
YLABEL=""
TITLE=""
RESULTDIR=.
DISPMIN="#"
DISPMAX="#"
DISPINFO="#"

usage()
{ 
  echo "
  Usage: $PROGNAME [options] [ XML files ]
         
    Creates statistics based on XML files, and creates plots based on them.
 
    Options: 
       -h               display this help message
       -b <num>         set the benchmark filter to num
       -e 'EXPRESSION'  what to compute?
          Arithmetic expressions are required. Variables can be used:
          ex: TC_0 : TC stat value on the 0th edge.
              SSC_1 / TC_0 : 1st edge's SSC stat ratio compared to 0th edge's TC stat
          List of available statistics:
           (edge)    $STAT_LIST
           (profile) $PROFILE_LIST              
       -o <dir>         output dir
       -x <title>       X ordinate title of the plot
       -y <title>       Y ordinate title of the plot
       -t <title>       Graph title
  " 
}

#
# PARSE COMMAND LINE
# ##################################################
ARGS=$(getopt -s bash --options $SHORTOPTS --longoptions $LONGOPTS --name $PROGNAME -- "$@" ) 
eval set -- "$ARGS"

while true; do
   case $1 in 
      -h|--help) 
          usage; exit 0;; 
      -b) shift;
          BENCH_IDX=$1;;
      -e) shift
          EXPR=$1;;
      -x) shift
          XLABEL=$1;;
      -y) shift
          YLABEL=$1;;
      -o) shift
          RESULTDIR=$1;;
      --min) 
          DISPMIN="";;
      --max) 
          DISPMAX="";;
      --info) 
          DISPINFO="";; 
      *)  shift ; break;;
   esac
   shift
done

#
# HANDLE ERRORS
# ##################################################
if [ $# -eq 0 ]; then
  echo "No XML files specified! See help (-h)!"
  exit 1
fi
if [ -z "$EXPR" ]; then
  echo "No expression specified! See help (-h)!"
  exit 1
fi
if [ -z $BENCH_IDX ]; then
  echo "No benchmark specified! See help (-h)!"
  exit 1
fi
if [ ! -d $RESULTDIR ]; then
  mkdir -p $RESULTDIR
elif [[ ! -w $RESULTDIR ]]; then
  echo "Cannot create files in '$RESULTDIR'!"
  exit 1
fi

# Create statistics based on the provided XML file
# Using AWK
create_stat() {
(
echo "
function line_filter() { 
  gsub(/[^0-9]/,\" \"); 
  gsub(/ +/,\" \"); 
  sub(/^ /,\"\");
}
function compute(data,num) {
  if (num != 0) {
    asort(data,sdata);
    median = sdata[int(num/2)+1]; min = sdata[1]; max = sdata[num]; sum = 0;
    for(i=1; i <= num; i++) { sum += sdata[i]; }
    mean = sum/num;
  }
  for(i=1;i<=num;i++) { 
    print i\" \"data[i]\" \"min\" \"max\" \"mean\" \"median; 
  }
}
BEGIN { FS=\"[<|>| ]\"; skip=1; num=0; }
/<benchidx>/ { bidx=\$3; if (bidx == $1) skip = 0; else skip = 1;}
"

for sidx in {0..3}; do
  echo -n "/<stat idx=\"$sidx\">/ { "
  fidx=2;
  echo -n "if (!skip) { line_filter(); "
  for stat in $STAT_LIST; do
    echo -n "${stat}_${sidx}=\$$fidx; "
    ((fidx = $fidx + 1));
  done
  echo "}}"
done

for pidx in {0..3}; do
  echo -n "/<profile idx=\"$pidx\">/ { "
  fidx=2;
  echo -n "if (!skip) { line_filter(); "
  for prof in $PROFILE_LIST; do
    echo -n "${prof}_${pidx}=\$$fidx; "
    ((fidx = $fidx + 1));
  done
  echo "}}"
done

echo "/<\/testresult>/ { if (!skip) raw[++num] = $2 }"
echo "END {
  compute(raw,num);   
}"
) | gawk -f- $3

}

get_plotting_values() {
  # get the min,max,mean,median values into env variables
  eval $(head -1 $1 | awk '{ printf "MIN=%s;MAX=%s;MEAN=%s;MED=%s;", $3,$4,$5,$6 }')
  eval $(echo "BEGIN { 
    yd=($MAX-$MIN)/10;
    ymin=($MIN-($MAX-$MIN)/10);
    ymax=($MAX+($MAX-$MIN)/10);
    lpos=($MIN+($MAX-$MIN)/10);
    printf \"YDELTA=%s;YMIN=%s;YMAX=%s;LPOS=%s;\", yd,ymin,ymax,lpos }" | awk -f-)
  YTICS=$(echo "$MIN,$YDELTA")
  XMAX=$(( $(tail -1 $1 | awk '{print $1}') + 1))
}

gen_sed_script() {
  for s in $1; do
    echo "s:_${s}_:${!s}:g"
  done
}

mkdir -p .stats
rm -rf .stats/*
# Make statistics for all files separately
for f in $@; do
  if [[ ! -e $f || ! -f $f || ! -r $f ]]; then
    echo "$f does not exist, is not a file, or not readable!"
    exit 1
  fi
  create_stat $BENCH_IDX $EXPR $f > .stats/${f%.*}.stat
done

# If only one file is provided, make a simpler plot
if [ $# -eq 1 ]; then

  FILEBASE=${1%.*}
  PLOTEPS=$RESULTDIR/$FILEBASE.eps
  INPUT=.stats/${f%.*}.stat
  
  # get handy values from the statistics for plotting
  get_plotting_values $INPUT
 
  # apply the substitutions
  SUBS="DISPMIN DISPMAX INPUT PLOTEPS DISPINFO XLABEL YLABEL TITLE YMIN YMAX XMAX MEAN MED LPOS"
  for s in $SUBS; do
    echo "s:_${s}_:${!s}:g"
  done | sed -f - plot_single.plt | gnuplot
  
# If multiple input files are present
else
  INPUT=`tempfile`
  for f in .stats/*.stat; do
    # we need only the aggregates
    head -1 $f | cut -d ' ' -f2-
  done | cat -n > $INPUT
  
  PLOTEPS=$RESULTDIR/composite.eps
  
  # get handy values from the statistics for plotting
  get_plotting_values $INPUT

  # apply the substitutions
  SUBS="DISPMIN DISPMAX INPUT PLOTEPS DISPINFO XLABEL YLABEL TITLE YMIN YMAX XMAX MEAN MED LPOS"
  for s in $SUBS; do
    echo "s:_${s}_:${!s}:g"
  done | sed -f - plot_composite.plt | gnuplot
  
fi

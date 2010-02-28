#!/bin/bash
# Benchmark all RadioTests with different configurations

FIRSTTEST=0;
LASTTEST=10;
LASTTRIGGERTEST=3;
OUTPUT=results.xml;
LOG=runtime.log;
NULL="/dev/null";

# Check parameters
if [ $# -eq 1 ]; then
  OUTPUT=$1
fi
if [ $# -ge 2 ]; then
  FIRSTTEST=$1;
  LASTTEST=$2;
fi
if [ $# -ge 3 ]; then
  OUTPUT=$3
fi


# Parameter sets for trigger tests
RUNTIMES=( 1000 );
TRIGGERS=( 8 10 12 100 );
# LPL : value 0 means no LPL!
LPL=( 0 );

RUNTIMESCNT=`expr ${#RUNTIMES[*]} - 1`;
TRIGGERSCNT=`expr ${#TRIGGERS[*]} - 1`;
LPLCNT=`expr ${#LPL[*]} - 1`;

# XML header
echo "<?xml version=\"1.0\" encoding=\"ISO-8859-1\" standalone=\"no\"?>" > $OUTPUT
echo "<?xml-stylesheet type=\"text/xsl\" href=\"testresults.xsl\"?>" >> $OUTPUT
echo "<resultset date=\"`date`\">" >> $OUTPUT

# First, be sure all motes are reset
java RadioTest -r 1>$LOG 2>$NULL

# Iterations
for p in `seq $FIRSTTEST $LASTTEST`; do  
  for l in `seq 0 $LPLCNT`; do
    LPLIV=${LPL[$l]};
      
    # If it's a trigger-type test
    if [ $p -le $LASTTRIGGERTEST ]; then      
      for t in `seq 0 $RUNTIMESCNT`; do
        TIME=${RUNTIMES[$t]};

        for tr in `seq 0 $TRIGGERSCNT`; do
          TRIGGER=${TRIGGERS[$tr]};

          # no ACK, no DADDR
          java RadioTest -p $p -t $TIME -tr $TRIGGER -r -lpl $LPLIV -xml $OUTPUT 1>>$LOG 2>$NULL;
          # no ACK, DADDR
          java RadioTest -p $p -t $TIME -tr $TRIGGER -r -lpl $LPLIV -daddr -xml $OUTPUT 1>>$LOG 2>$NULL;
          # ACK, DADDR
          java RadioTest -p $p -t $TIME -tr $TRIGGER -r -lpl $LPLIV -daddr -ack -xml $OUTPUT 1>>$LOG 2>$NULL;
        done
      done
    else
      # no ACK, no DADDR
      java RadioTest -p $p -t 1000 -lpl $LPLIV -r -xml $OUTPUT 1>>$LOG 2>$NULL;
      # no ACK, DADDR
      java RadioTest -p $p -t 1000 -lpl $LPLIV -r -daddr -xml $OUTPUT 1>>$LOG 2>$NULL;
      # ACK, DADDR
      java RadioTest -p $p -t 1000 -lpl $LPLIV -r -daddr -ack -xml $OUTPUT 1>>$LOG 2>$NULL;
    fi
  done
done

echo "</resultset>" >> $OUTPUT


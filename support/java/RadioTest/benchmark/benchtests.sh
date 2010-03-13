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

if [ -e $OUTPUT ]; then
  mv $OUTPUT $OUTPUT.bckp
fi

function write_header() {
  # XML header
  echo "<?xml version=\"1.0\" encoding=\"ISO-8859-1\" standalone=\"no\"?>" > $OUTPUT
  echo "<?xml-stylesheet type=\"text/xsl\" href=\"testresults.xsl\"?>" >> $OUTPUT
  echo "<resultset date=\"`date`\">" >> $OUTPUT
}

function write_footer() {
  echo "</resultset>" >> $OUTPUT
}

# First, be sure all motes are reset
java RadioTest -r 1>$LOG 2>$NULL
if [ $? -ne 0 ]; then exit 1; fi

write_header;

# Parameter sets for trigger tests
RUNTIMES=( 1000 );
TRIGGERS=( 8 10 12 100 );
# LPL : value 0 means no LPL!
LPL=( 0 );

RUNTIMESCNT=`expr ${#RUNTIMES[*]} - 1`;
TRIGGERSCNT=`expr ${#TRIGGERS[*]} - 1`;
LPLCNT=`expr ${#LPL[*]} - 1`;

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
          if [ $? -ne 0 ]; then write_footer; exit 1; fi
          # no ACK, DADDR
          java RadioTest -p $p -t $TIME -tr $TRIGGER -r -lpl $LPLIV -daddr -xml $OUTPUT 1>>$LOG 2>$NULL;
          if [ $? -ne 0 ]; then write_footer; exit 1; fi
          # ACK, DADDR
          java RadioTest -p $p -t $TIME -tr $TRIGGER -r -lpl $LPLIV -daddr -ack -xml $OUTPUT 1>>$LOG 2>$NULL;
          if [ $? -ne 0 ]; then write_footer; exit 1; fi
        done
      done
    else
      # no ACK, no DADDR
      java RadioTest -p $p -t 1000 -lpl $LPLIV -r -xml $OUTPUT 1>>$LOG 2>$NULL;
      if [ $? -ne 0 ]; then write_footer; exit 1; fi
      # no ACK, DADDR
      java RadioTest -p $p -t 1000 -lpl $LPLIV -r -daddr -xml $OUTPUT 1>>$LOG 2>$NULL;
      if [ $? -ne 0 ]; then write_footer; exit 1; fi
      # ACK, DADDR
      java RadioTest -p $p -t 1000 -lpl $LPLIV -r -daddr -ack -xml $OUTPUT 1>>$LOG 2>$NULL;
      if [ $? -ne 0 ]; then write_footer; exit 1; fi
    fi
  done
done

write_footer;

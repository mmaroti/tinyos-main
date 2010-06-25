#!/bin/bash
# Benchmark all RadioTests with different configurations

FIRSTTEST=0;
LASTTEST=12;
TRIGGERTESTS=(1 1 1 1 0 0 0 0 0 0 0 1 1);

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

function write_header() {
  # XML header
  echo "<?xml version=\"1.0\" encoding=\"ISO-8859-1\" standalone=\"no\"?>" > $OUTPUT
  echo "<?xml-stylesheet type=\"text/xsl\" href=\"testresults.xsl\"?>" >> $OUTPUT
  echo "<resultset date=\"`date`\">" >> $OUTPUT
}

function write_footer() {
  echo "</resultset>" >> $OUTPUT
}

function echotest() {
  echo -e "Running test : $1 @ $2 $3 $4 $5 - \t `date +%T`"
}

# First, be sure all motes are reset
java RadioTest -r 1>$LOG 2>$NULL
if [ $? -ne 0 ]; then exit 1; fi

write_header;

# Parameter sets for tests
RUNTIMES=( 1000 );
TRIGGERS=( 5 9 10 11 12 50 75 100 );
LPL=( 0 ); 
# LPL : value 0 means no LPL!


RUNTIMESCNT=`expr ${#RUNTIMES[*]} - 1`;
TRIGGERSCNT=`expr ${#TRIGGERS[*]} - 1`;
LPLCNT=`expr ${#LPL[*]} - 1`;

# Iterations
for p in `seq $FIRSTTEST $LASTTEST`; do  
     
  # If it's a trigger-type test
  if [ ${TRIGGERTESTS[$p]} -eq 1 ]; then  
      
    for t in `seq 0 $RUNTIMESCNT`; do
      TIME=${RUNTIMES[$t]};

      for tr in `seq 0 $TRIGGERSCNT`; do
        TRIGGER=${TRIGGERS[$tr]};

        for l in `seq 0 $LPLCNT`; do
          LPLIV=${LPL[$l]};
          
          # no ACK, no DADDR
          echotest $p $TIME $TRIGGER $LPLIV "bcast"
          java RadioTest -p $p -t $TIME -tr $TRIGGER -r -lpl $LPLIV -xml $OUTPUT 1>>$LOG 2>$NULL;
          if [ $? -ne 0 ]; then write_footer; exit 1; fi
          # no ACK, DADDR
          echotest $p $TIME $TRIGGER $LPLIV "daddr"
          java RadioTest -p $p -t $TIME -tr $TRIGGER -r -lpl $LPLIV -daddr -xml $OUTPUT 1>>$LOG 2>$NULL;
          if [ $? -ne 0 ]; then write_footer; exit 1; fi
          # ACK, DADDR
          echotest $p $TIME $TRIGGER $LPLIV "ack"
          java RadioTest -p $p -t $TIME -tr $TRIGGER -r -lpl $LPLIV -ack -xml $OUTPUT 1>>$LOG 2>$NULL;
          if [ $? -ne 0 ]; then write_footer; exit 1; fi

        done
      done
    done
  # not trigger tests
  else
    for t in `seq 0 $RUNTIMESCNT`; do
      TIME=${RUNTIMES[$t]};

      for l in `seq 0 $LPLCNT`; do
        LPLIV=${LPL[$l]};
     
        # no ACK, no DADDR
        echotest $p $TIME $LPLIV "bcast"
        java RadioTest -p $p -t $TIME -lpl $LPLIV -r -xml $OUTPUT 1>>$LOG 2>$NULL;
        if [ $? -ne 0 ]; then write_footer; exit 1; fi
          
        # no ACK, DADDR
        echotest $p $TIME $LPLIV "daddr"        
        java RadioTest -p $p -t $TIME -lpl $LPLIV -r -daddr -xml $OUTPUT 1>>$LOG 2>$NULL;
        if [ $? -ne 0 ]; then write_footer; exit 1; fi
         
        # ACK, DADDR
        echotest $p $TIME $LPLIV "ack"        
        java RadioTest -p $p -t $TIME -lpl $LPLIV -r -ack -xml $OUTPUT 1>>$LOG 2>$NULL;
        if [ $? -ne 0 ]; then write_footer; exit 1; fi
          
      done
    done
  fi
done
write_footer;

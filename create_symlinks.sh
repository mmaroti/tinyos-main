#!/bin/sh
SOURCE=$SZTETOSROOT
DEST=$TOSROOT
DIRS="/tos/chips/
/tos/platforms/
/tos/sensorboards/
/support/make/
/support/make/avr/
"
for d in $DIRS
do
	for f in `ls $SZTETOSROOT$d`
	do
 		if [ -e "$TOSROOT$d$f" ]
 		then
 			echo "$TOSROOT$d$f exists!"
 		else
 			echo "$TOSROOT$d$f created."
 			ln -s $SZTETOSROOT$d$f $TOSROOT$d$f
 		fi
	done
done

#!/bin/bash
EXEC="Benchmark.jar"
if [ -f $EXEC ]; then
  java -jar $EXEC $@
else
  echo "No $EXEC file found, please compile the Java sources!";
fi

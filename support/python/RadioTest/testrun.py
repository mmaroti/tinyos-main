#!/usr/bin/env python

from TOSSIM import *;
import sys;

nodenum = 2;

t = Tossim([]);
t.addChannel("Debug",sys.stdout);

# Setup Radio topology and noise
r = t.radio();
f = open("topo.txt", "r")
lines = f.readlines()
for line in lines:
  s = line.split()
  if (len(s) > 0):
    r.add(int(s[0]), int(s[1]), float(s[2]))

print "Establishing noise models";
noise = open("meyer-heavy.txt", "r")
lines = noise.readlines()
for line in lines:
  str = line.strip()
  if (str != ""):
    val = int(str)
    for i in range(1, nodenum+1):
      t.getNode(i).addNoiseTraceReading(val)

for i in range(1, nodenum+1):
  print "Creating noise model for ",i;
  t.getNode(i).createNoiseModel()

# Boot motes
for i in range(1, nodenum+1):
  t.getNode(i).bootAtTime(21213);

for i in range(0, 10000):
  t.runNextEvent()

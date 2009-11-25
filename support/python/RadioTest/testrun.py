#!/usr/bin/env python

from TOSSIM import *
from setup_t import *
import sys

nodenum = 3

t = Tossim([])
r = t.radio();

t.addChannel("Debug",sys.stdout)

# Boot motes
for i in range(0, nodenum):
  t.getNode(i).bootAtTime((31 + t.ticksPerSecond() / 10) * i + 1);

# Setup Radio topology and noise
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
    for i in range(0, nodenum):
      t.getNode(i).addNoiseTraceReading(val)

for i in range(0, nodenum):
  print "Creating noise model for ",i;
  t.getNode(i).createNoiseModel()

for i in range(0, 10):
  t.runNextEvent();

# Inject control message
msg = setup_t()
msg.set_problem_idx(0)
msg.set_runtime_msec(1000)
msg.set_sendtrig_msec(100)
msg.set_flags(0)

pkt = t.newPacket()
pkt.setData(msg.data)
pkt.setType(msg.get_amType())
pkt.setDestination(0xffff);
pkt.setStrength(1000);

print "Injecting CTRL packets ";
for i in range(0, nodenum):
  pkt.deliver(i, t.time() + 3)

for i in range(0, 1000):
  t.runNextEvent()

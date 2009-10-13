#!/usr/bin/env python

from TOSSIM import *;
#from test_message_t import *;
#from setup_t import *;

import sys;

t = Tossim([]);
t.addChannel("Debug",sys.stdout);
t.addChannel("Pending",sys.stdout);
t.addChannel("Radio",sys.stdout);

# Setup Radio topology and noise
"""r = t.radio();
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
    for i in range(1, 6):
      t.getNode(i).addNoiseTraceReading(val)

for i in range(1, 6):
  print "Creating noise model for ",i;
  t.getNode(i).createNoiseModel()
"""
# Boot motes
t.getNode(1).bootAtTime(21213);
t.getNode(5).bootAtTime(21213);
t.getNode(3).bootAtTime(21213);
t.getNode(2).bootAtTime(21213);
t.getNode(4).bootAtTime(21213);

# Construct an injectable packet
"""msg = test_message_t();
msg.set_type(1);

setup = setup_t();
setup.set_nodenum(10);
setup.set_msgsize(30);
setup.set_radiopolicy(4+32);

setup.set_lpl_localSleepInterval(0);
setup.set_lpl_localDutyCycle(0);
setup.set_lpl_rxSleepInterval(0);
setup.set_lpl_rxDutyCycle(0);
setup.set_lpl_dutyCycleToSleep(0);

msg.set_data(setup);
# packet ready

pkt = t.newPacket();
pkt.setData(msg.data)
pkt.setType(msg.get_amType())
pkt.setDestination(2)"""

for i in range(0, 10000):
  t.runNextEvent()

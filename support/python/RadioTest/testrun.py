#!/usr/bin/env python

import sys, getopt
from TOSSIM import *
from ctrlmsg_t import *
from time import gmtime, strftime, timezone, time

def tossim_run(motenum,policy,platform):

  if motenum < 0 or policy < 0 or policy > 11:
    sys.exit()
  
  print "Running TOSSIM Simulation ",strftime("%a, %d %b %Y %H:%M:%S", gmtime(time()-timezone))
  print "[ Motenum :",motenum," Policy :",policy," Platform :",platform," ]"

  t = Tossim([])
  r = t.radio();

  t.addChannel("Debug",sys.stdout)

  # Boot motes
  for i in range(0, motenum):
    t.getNode(i+1).bootAtTime((31 + t.ticksPerSecond() / 10) * i + 1);

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
      for i in range(0, motenum):
        t.getNode(i+1).addNoiseTraceReading(val)

  for i in range(0, motenum):
    print "Creating noise model for ",i+1;
    t.getNode(i+1).createNoiseModel()

  for i in range(0, 10):
    t.runNextEvent();

  # PACKET INJECTIONS
  # --------------------------------------------
  pkt = t.newPacket()
  msg = ctrlmsg_t();
  pkt.setDestination(0xffff);
  pkt.setStrength(1000);
  pkt.setType(msg.get_amType())

  # Inject control message ( SETUP )
  msg.set_type(0);
  msg.set_config_problem_idx(policy)
  msg.set_config_runtime_msec(1000)
  msg.set_config_sendtrig_msec(100)
  msg.set_config_flags(0)
  pkt.setData(msg.data)

  print "Injecting CTRL CONFIG packets ";
  for i in range(0, motenum):
    pkt.deliver(i+1, t.time() + 3)
  for i in range(0, 10000):
    t.runNextEvent()

  # Inject control messages ( REQUEST )
  msg.set_type(2)
  for i in range(0, motenum):
    for j in range(0,motenum*2):
      msg.set_idx(j)
      pkt.setData(msg.data)
      print "Injecting CTRL REQUEST packet for ",i+1," requesting stat ",j;
      pkt.deliver(i+1, t.time() + 3)

      for k in range(0, 1000):
        t.runNextEvent()

  # Inject control message ( RESET )
  msg.set_type(1)
  pkt.setData(msg.data)
  
  print "Injecting CTRL RESET packets ";
  for i in range(0, motenum):
    pkt.deliver(i+1, t.time() + 3)
  for i in range(0, 1000):
    t.runNextEvent()


def usage():
    print sys.argv[0]," <-m|--motenum=> <int> <-p|--policy=> <int> [-h|--help] [-w|--hardware= <WSN platform>]"

def main():
    try:
        opts, args = getopt.getopt(sys.argv[1:], "m:p:h:w:", ["motenum=", "policy=","help","hardware"])
    except getopt.GetoptError, err:
        print str(err)
        usage()
        sys.exit(2)
    valid = 0;
    mnum = 0
    plcy = -1
    platform = "Unspecified"
    for o, a in opts:
        if o in ("-h", "--help"):
            usage()
            sys.exit()
        elif o in ("-m", "--motenum"):
            mnum = a;
            valid = valid + 1;
        elif o in ("-p", "--policy"):
            plcy = a;
            valid = valid + 1;
        elif o in ("-w", "--hardware"):
            platform = a;
        else:
            assert False, "unhandled option"

    if valid < 2:
      usage()
      sys.exit()
    else:
      tossim_run(int(mnum),int(plcy),platform);

if __name__ == "__main__":
    main()

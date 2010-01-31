/** Copyright (c) 2009, University of Szeged
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions
* are met:
*
* - Redistributions of source code must retain the above copyright
* notice, this list of conditions and the following disclaimer.
* - Redistributions in binary form must reproduce the above
* copyright notice, this list of conditions and the following
* disclaimer in the documentation and/or other materials provided
* with the distribution.
* - Neither the name of University of Szeged nor the names of its
* contributors may be used to endorse or promote products derived
* from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
* "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
* FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
* COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
* OF THE POSSIBILITY OF SUCH DAMAGE.
*
* Author: Veress Krisztian
*         veresskrisztian@gmail.com
*/

import java.io.*;
import java.util.concurrent.locks.*;
import java.util.concurrent.TimeUnit;
import java.util.Vector;
import net.tinyos.message.*;
import net.tinyos.util.*;

public class RadioTestController implements MessageListener {
	
	private MoteIF mif;
  private short motecount,edgecount,stage;
  private Vector< StatT > stats;
  private boolean[] handshake;

  final Lock lock = new ReentrantLock();
  final Condition answered = lock.newCondition(); 
  		
  private static short currentMote = 0;
  private static short currentStat = 0;

  public static int MAXPROBES=3;
  public static int MAXTIMEOUT=1000;

	public RadioTestController(final short mc)
	{
		mif = new MoteIF();
    mif.registerListener(new CtrlMsgT(),this);

    motecount = mc;
    edgecount = (short)(mc*(mc-1));

    stats = new Vector<StatT>();
    for( int i=0; i< edgecount; ++i )
      stats.add(new StatT());

    handshake = new boolean[motecount];
    for( int i=0; i< motecount; ++i )
      handshake[i]=false;
    
    stage = 0;
	}

	public void resetMotes()  
  {
    CtrlMsgT cmsg = new CtrlMsgT();
    // refer to RadioTest.h CTRL_RESET
    cmsg.set_type((short)20);
		try {
			mif.send(MoteIF.TOS_BCAST_ADDR,cmsg);
      // We should go to setup stage
      stage = 0;
		} catch(IOException e) {
		  System.out.println("Cannot RESET motes!");
    }
	}

  public boolean setupMotes(final SetupT config) {
    CtrlMsgT cmsg = new CtrlMsgT();
    // refer to RadioTest.h CTRL_SETUP
    cmsg.set_type((short)0);
    cmsg.set_data_config_problem_idx(config.get_problem_idx());
    cmsg.set_data_config_runtime_msec(config.get_runtime_msec());
    cmsg.set_data_config_sendtrig_msec(config.get_sendtrig_msec());
    cmsg.set_data_config_flags(config.get_flags());
    lock.lock();

    System.out.println(" Configuring motes ... ");
    System.out.println(" ---------------------------------");
    CtrlMsgT cmsg2 = new CtrlMsgT();
		try {
			mif.send(MoteIF.TOS_BCAST_ADDR,cmsg);
      Thread.sleep((int)(500));

      // refer to RadioTest.h CTRL_SETUP_SYN
      cmsg2.set_type((short)1);
      for ( currentMote = 0; currentMote < motecount ; ++currentMote ) {
        System.out.print(" [M" + (currentMote+1) + "] Setup ACK : ");

        // send CTRL_SETUP_SYN and wait for CTRL_SETUP_ACK at most 1 sec at most 3 times
        for( short probe = 0; !handshake[currentMote] && probe < MAXPROBES ; ++probe ) {
          mif.send(currentMote+1,cmsg2);
          if ( !answered.await(MAXTIMEOUT,TimeUnit.MILLISECONDS) )
            System.out.print('.');
        }
        if ( !handshake[currentMote] ) {
          System.out.println(" Timeout, giving up.");
          return false;
        }
        System.out.println("OK");
      }
      return true;
		} catch(IOException e) {
		  System.out.println("Cannot accurately SETUP motes!");
    } catch ( InterruptedException e ) {
      System.err.println("The thread waiting for setup completion is interrupted!");
    } finally {
      lock.unlock();
    }
    return true;
	}

	public void run(final short runtimemsec)
	{
    // We are now on running stage
    stage = 1;
    System.out.print(" Running the loaded configuration ... ");
    CtrlMsgT cmsg = new CtrlMsgT();
    // refer to RadioTest.h CTRL_START
    cmsg.set_type((short)10);
		try {
			mif.send(MoteIF.TOS_BCAST_ADDR,cmsg);
      // wait for test completion
      Thread.sleep((int)(runtimemsec*1.2));
      System.out.println("DONE");
		} catch(IOException e) {
		  System.out.println("Cannot send START message!");
    } catch ( InterruptedException e ) {
      System.err.println("The thread waiting for test completion is interrupted!");
    }
	}

  public boolean collect()
	{
    // We are now on downloading stage
    stage = 2;
    System.out.println(" Downloading statistics ... ");
    System.out.println(" ---------------------------------");
    lock.lock();
    try {
      for ( currentMote = 0; currentMote < motecount ; ++currentMote ) {
        System.out.print(" [M" + (currentMote+1) + "] :");
        for ( currentStat = 0; currentStat < edgecount; ++currentStat ) {
          System.out.print(" " + currentStat);
          handshake[currentMote] = false;
          for( short probe = 0; !handshake[currentMote] && probe < MAXPROBES; ++probe ) {
            requestStat();
            if ( !answered.await(MAXTIMEOUT,TimeUnit.MILLISECONDS) )
              System.out.print(".");
          }
          if ( !handshake[currentMote] ) {
            System.out.println(" Timeout, giving up.");
            return false;
          }          
        }
        System.out.println(" OK");
      }
    } catch ( InterruptedException e ) {
      System.err.println("The thread waiting for stat collection is interrupted!");
    } finally {
      lock.unlock();
    }
    return true;
	}

  private boolean requestStat() {
      
    CtrlMsgT cmsg = new CtrlMsgT();
    // refer to RadioTest.h CTRL_STAT_REQ
    cmsg.set_type((short)30);
    cmsg.set_data_stat_statidx(currentStat);
    try {
 		  mif.send(currentMote+1,cmsg);
      return true;
 	  } catch(IOException e) {
 	    System.out.println("Cannot collect stat from mote " + currentMote );
      return false;
    }
  }
  
  public void messageReceived(int dest_addr,Message msg)
	{
    if ( msg instanceof CtrlMsgT ) {
      CtrlMsgT cmsg = (CtrlMsgT)msg;
      lock.lock();

      // refer to RadioTest.h CTRL_SETUP_ACK
      if ( cmsg.get_type() == 2 && stage == 0 ) {
        handshake[currentMote] = true;
        answered.signal();

      // refer to RadioTest.h CTRL_STAT_OK
      } else if ( cmsg.get_type() == 31 && 
                  stage == 2  &&
                  currentStat == cmsg.get_data_stat_statidx() ) {

        //System.out.println(cmsg.toString());

        StatT s = stats.get(cmsg.get_data_stat_statidx());
        s.set_sendSuccessCount(     s.get_sendSuccessCount()    +   cmsg.get_data_stat_statpayload_sendSuccessCount());
        s.set_sendFailCount(        s.get_sendFailCount()       +   cmsg.get_data_stat_statpayload_sendFailCount());
        s.set_sendDoneSuccessCount( s.get_sendDoneSuccessCount()+   cmsg.get_data_stat_statpayload_sendDoneSuccessCount());
        s.set_sendDoneFailCount(    s.get_sendDoneFailCount()   +   cmsg.get_data_stat_statpayload_sendDoneFailCount());
        s.set_wasAckedCount(        s.get_wasAckedCount()       +   cmsg.get_data_stat_statpayload_wasAckedCount());
        s.set_resendCount(          s.get_resendCount()         +   cmsg.get_data_stat_statpayload_resendCount());
        s.set_receiveCount(         s.get_receiveCount()        +   cmsg.get_data_stat_statpayload_receiveCount());
        s.set_duplicateReceiveCount(s.get_duplicateReceiveCount()+  cmsg.get_data_stat_statpayload_duplicateReceiveCount());
        s.set_missedCount(          s.get_missedCount()         +   cmsg.get_data_stat_statpayload_missedCount());
        s.set_wouldBacklogCount(    s.get_wouldBacklogCount()   +   cmsg.get_data_stat_statpayload_wouldBacklogCount());
  
        stats.set(cmsg.get_data_stat_statidx(),s);

        handshake[currentMote] = true;
        answered.signal();

      // refer to RadioTest.h CTRL_STAT_NEXIST
      } else if ( cmsg.get_type() == 32 && stage == 2 ) {
        System.out.print("L");
        edgecount = (short)(cmsg.get_data_stat_statidx());
        handshake[currentMote] = true;
        answered.signal();
      }
      lock.unlock();
    }
	}

  public void printStats() {
    System.out.println(" Statistics :");
    System.out.println(" ---------------------------------");
    for(int j = 0; j < edgecount; ++j ) {
      System.out.print(" E"+ j + ":");
      System.out.print(" " + stats.get(j).get_sendSuccessCount());
      System.out.print(" " + stats.get(j).get_sendFailCount());
      System.out.print(" " + stats.get(j).get_sendDoneSuccessCount());
      System.out.print(" " + stats.get(j).get_sendDoneFailCount());
      System.out.print(" " + stats.get(j).get_wasAckedCount());
      System.out.print(" " + stats.get(j).get_resendCount());
      System.out.print(" " + stats.get(j).get_receiveCount());
      System.out.print(" " + stats.get(j).get_duplicateReceiveCount());
      System.out.print(" " + stats.get(j).get_missedCount());
      System.out.println(" " + stats.get(j).get_wouldBacklogCount());

    }
  }
}

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
import java.util.Vector;
import org.apache.commons.cli.*;
import net.tinyos.message.*;
import net.tinyos.util.*;

public class RadioTestController implements MessageListener {
	
	private MoteIF mif;
  private short motecount;
  private short edgecount;
  private Vector< StatT > stats;

  // for stat collection
  private short nextMoteID = 1;
  final Lock lock = new ReentrantLock();
	final Condition collected = lock.newCondition(); 
  		
	public RadioTestController(final short mc)
	{
		mif = new MoteIF();
    mif.registerListener(new CtrlMsgT(),this);

    motecount = mc;
    edgecount = (short)(mc*(mc-1));

    stats = new Vector<StatT>();
    for( int i=0; i< edgecount; ++i )
      stats.add(new StatT());
	}

	public void resetMote(final int moteID)  
  {
    CtrlMsgT cmsg = new CtrlMsgT();
    // refer to RadioTest.h CTRL_RESET
    cmsg.set_type((short)4);
		try {
			mif.send(moteID,cmsg);
		} catch(IOException e) {
		  System.out.println("Cannot reset mote " + moteID );
    }
	}

  public void setupMote(final int moteID, final SetupT config) {
    CtrlMsgT cmsg = new CtrlMsgT();
    // refer to RadioTest.h CTRL_SETUP
    cmsg.set_type((short)0);
    cmsg.set_data_config_problem_idx(config.get_problem_idx());
    cmsg.set_data_config_runtime_msec(config.get_runtime_msec());
    cmsg.set_data_config_sendtrig_msec(config.get_sendtrig_msec());
    cmsg.set_data_config_flags(config.get_flags());
		try {
			mif.send(moteID,cmsg);
		} catch(IOException e) {
		  System.out.println("Cannot setup mote " + moteID );
    }
	}

  private void requestStat() {
    CtrlMsgT cmsg = new CtrlMsgT();
    // refer to RadioTest.h CTRL_REQ_STAT
    cmsg.set_type((short)1);
    cmsg.set_idx((short)0);
    try {
 		  mif.send(nextMoteID,cmsg);
 	  } catch(IOException e) {
 	    System.out.println("Cannot initiate collection of stats from mote " + nextMoteID );
    }
  }
  
  public void messageReceived(int dest_addr,Message msg)
	{
    if ( msg instanceof CtrlMsgT ) {
      CtrlMsgT cmsg = (CtrlMsgT)msg;
      // refer to RadioTest.h CTRL_UPL_END
      if ( cmsg.get_type() == 3 ) {
        System.out.println("CTRL_UPL_END received.");
        lock.lock();
        if ( ++nextMoteID > motecount )
          collected.signal();
        else
          requestStat();
        lock.unlock();
      // refer to RadioTest.h CTRL_UPL_STAT
      } else if ( cmsg.get_type() == 2 ) {
        System.out.println("CTRL_UPL_STAT received.");

        StatT s = stats.get(cmsg.get_idx());
        s.set_sendSuccessCount(     s.get_sendSuccessCount()    +   cmsg.get_data_stat_sendSuccessCount());
        s.set_sendFailCount(        s.get_sendFailCount()       +   cmsg.get_data_stat_sendFailCount());
        s.set_sendDoneSuccessCount( s.get_sendDoneSuccessCount()+   cmsg.get_data_stat_sendDoneSuccessCount());
        s.set_sendDoneFailCount(    s.get_sendDoneFailCount()   +   cmsg.get_data_stat_sendDoneFailCount());
        s.set_wasAckedCount(        s.get_wasAckedCount()       +   cmsg.get_data_stat_wasAckedCount());
        s.set_resendCount(          s.get_resendCount()         +   cmsg.get_data_stat_resendCount());
        s.set_receiveCount(         s.get_receiveCount()        +   cmsg.get_data_stat_receiveCount());
        s.set_duplicateReceiveCount(s.get_duplicateReceiveCount()+  cmsg.get_data_stat_duplicateReceiveCount());
        s.set_missedCount(          s.get_missedCount()         +   cmsg.get_data_stat_missedCount());
        s.set_wouldBacklogCount(    s.get_wouldBacklogCount()   +   cmsg.get_data_stat_wouldBacklogCount());
  
        stats.set(cmsg.get_idx(),s);
      }
    }
	}

  public void printStats() {
    System.out.println("Statistics :");
    for(int j = 0; j < edgecount; ++j ) {
      System.out.println();
      System.out.println("Edge #" + j);
      System.out.println("--------------------------------------------");
      System.out.println(stats.get(j).toString());
    }
  }

	public void run(final SetupT config)
	{
		for(int i = 0; i < motecount; ++i )
      resetMote(i+1);
        
    for(int i = 0; i < motecount; ++i )
      setupMote(i+1,config);

    lock.lock();
    try {
      // wait for test completion
      Thread.sleep((int)(config.get_runtime_msec()*1.1));

      requestStat();
      // wait until all statistics are collected
      collected.await();
      printStats();
    } catch ( InterruptedException e ) {
      System.err.println("Stats collection interrupted!");
    } finally {
      lock.unlock();
    }
	}
}


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
import org.apache.commons.cli.*;

public class RadioTestController implements MessageListener {
	
	private MoteIF mif;
  private short stage, pidx;

  // Memory for downloaded data
  private Vector< StatT > stats;
  private long[][]  finalMsgIds;
  private long[]    debuglines;

  // Needed for proper downloading
  final Lock lock = new ReentrantLock();
  final Condition answered = lock.newCondition(); 
  private boolean handshake;  		
  private static short currentMote = 0, currentData = 0;
  private static final short  MAXPROBES   = 3;
  private static final int    MAXTIMEOUT  = 1000;

  // Conformance to the NesC code
  // refer to RadioTestCases.h
  public static final short PROBLEMSET_COUNT = 11;
  private static short[] trproblem = new short[] { 1,1,1,1,0,0,0,0,0,0,0 };
  private static short[] motecount = new short[] { 2,2,3,3,2,2,3,3,2,3,3 };
  private static short[] edgecount = new short[] { 1,2,3,6,1,2,3,6,2,3,6 };

	public RadioTestController(final short p)
	{
		mif = new MoteIF();
    mif.registerListener(new CtrlMsgT(),this);
    mif.registerListener(new ResponseMsgT(),this);
    stage = 0;
    pidx = p;
 	}

	public void resetMotes()  
  {
    System.out.print("> Reset all motes  ... ");
    CtrlMsgT cmsg = new CtrlMsgT();
    // refer to RadioTest.h CTRL_RESET
    cmsg.set_type((short)20);
		try {
			mif.send(MoteIF.TOS_BCAST_ADDR,cmsg);
      // We should go to pre-setup stage
      stage = 0;
      System.out.println("OK");
		} catch(IOException e) {
		  System.out.println("FAIL");
    }
	}

  public boolean setupMotes(final SetupT config) throws MissingOptionException {

    if ( trproblem[pidx] != 0 && config.get_timer_msec() == 0 )
      throw new MissingOptionException("Network policy " + pidx + " needs trigger timer specified!");
    
    CtrlMsgT cmsg = new CtrlMsgT();

    // refer to RadioTest.h CTRL_SETUP
    cmsg.set_type((short)0);
    cmsg.set_config_problem_idx(config.get_problem_idx());
    cmsg.set_config_runtime_msec(config.get_runtime_msec());
    cmsg.set_config_timer_msec( (trproblem[pidx] != 0) ? config.get_timer_msec() : 0);
    cmsg.set_config_lastchance_msec(config.get_lastchance_msec());
    cmsg.set_config_lplwakeupintval(config.get_lplwakeupintval());
    cmsg.set_config_flags(config.get_flags());
    lock.lock();

    System.out.print("> Setting up motes ... ");

    CtrlMsgT cmsg2 = new CtrlMsgT();
		try {
			mif.send(MoteIF.TOS_BCAST_ADDR,cmsg);
      Thread.sleep((int)(500));

      // refer to RadioTest.h CTRL_SETUP_SYN
      cmsg2.set_type((short)1);
      for ( currentMote = 0; currentMote < motecount[pidx] ; ++currentMote ) {
        handshake=false;

        // send CTRL_SETUP_SYN and wait for RESP_SETUP_ACK at most MAXTIMEOUT secs at most MAXPROBES times
        for( short probe = 0; !handshake && probe < MAXPROBES ; ++probe ) {
          mif.send(currentMote+1,cmsg2);
          answered.await(MAXTIMEOUT,TimeUnit.MILLISECONDS);
        }
        if ( !handshake ) {
          System.out.println("TIMEOUT, MOTE :" + (currentMote+1));
          return false;
        }
      }
      System.out.println("OK");
      return true;
		} catch(IOException e) {
		  System.out.println("FAIL");
    } catch ( InterruptedException e ) {
      System.err.println("FAIL");
    } finally {
      lock.unlock();
    }
    return true;
	}

	public boolean run(final int fullruntime)
	{
    // We are now on running stage
    stage = 1;
    System.out.print("> Running testcase ... ");
    CtrlMsgT cmsg = new CtrlMsgT();
    // refer to RadioTest.h CTRL_START
    cmsg.set_type((short)10);
		try {
			mif.send(MoteIF.TOS_BCAST_ADDR,cmsg);
      // wait for test completion + 100 msecs
      Thread.sleep((int)(fullruntime + 100));
      System.out.println("OK");
      return true;
		} catch(IOException e) {
		  System.out.println("FAIL");
      return false;
    } catch ( InterruptedException e ) {
      System.err.println("FAIL");
      return false;
    }
	}

  public boolean collect()
	{
    // We are now on downloading stage
    stage = 2;  
    stats = new Vector<StatT>();
 
    debuglines = new long[motecount[pidx]];
    finalMsgIds = new long[edgecount[pidx]][2];

    System.out.print("> Downloading data ... "); 

    lock.lock();
    try {
      for ( currentMote = 0; currentMote < motecount[pidx] ; ++currentMote ) {
        for ( currentData = 0; currentData < edgecount[pidx]; ++currentData ) {

          stats.add(new StatT());
  
          // Download stat
          handshake = false;
          for( short probe = 0; !handshake && probe < MAXPROBES; ++probe ) {
            // refer to RadioTest.h CTRL_DATA_REQ
            requestData((short)30);
            answered.await(MAXTIMEOUT,TimeUnit.MILLISECONDS);
          }
          if ( !handshake ) {
            System.out.println("TIMEOUT, MOTE : " + (currentMote+1) + " STAT : " + currentData);
            return false;
          }

          // Download debug info
          handshake = false;
          for( short probe = 0; !handshake && probe < MAXPROBES; ++probe ) {
            // refer to RadioTest.h CTRL_DBG_REQ
            requestData((short)40);
            answered.await(MAXTIMEOUT,TimeUnit.MILLISECONDS);
          }
          if ( !handshake ) {
            System.out.println("TIMEOUT, MOTE : " + (currentMote+1) + " DEBUG : " + currentData);
            return false;
          }
        }
      }
      System.out.println("OK");
      return true;
    } catch ( InterruptedException e ) {
      System.err.println("FAIL");
      return false;
    } finally {
      lock.unlock();
    }
	}

  private boolean requestData(final short type) {
      
    CtrlMsgT cmsg = new CtrlMsgT();
    cmsg.set_type(type);
    cmsg.set_reqidx(currentData);
    try {
 		  mif.send(currentMote+1,cmsg);
      return true;
 	  } catch(IOException e) {
      return false;
    }
  }
  
  public void messageReceived(int dest_addr,Message msg)
	{
    if ( msg instanceof ResponseMsgT ) {
      ResponseMsgT rmsg = (ResponseMsgT)msg;
      lock.lock();

      // refer to RadioTest.h RESP_SETUP_ACK
      if ( rmsg.get_type() == 2 && stage == 0 ) {
        handshake = true;
        answered.signal();

      // refer to RadioTest.h RESP_DATA_OK
      } else if ( rmsg.get_type() == 31 && 
                  stage == 2  &&
                  currentData == rmsg.get_respidx() ) {

        // Save stat
        StatT s = stats.get(currentData);
        s.set_triggerCount(         s.get_triggerCount()          +   rmsg.get_payload_stat_triggerCount());
        s.set_backlogCount(         s.get_backlogCount()          +   rmsg.get_payload_stat_backlogCount());
        s.set_resendCount(          s.get_resendCount()           +   rmsg.get_payload_stat_resendCount());
        s.set_sendCount(            s.get_sendCount()             +   rmsg.get_payload_stat_sendCount());
        s.set_sendSuccessCount(     s.get_sendSuccessCount()      +   rmsg.get_payload_stat_sendSuccessCount());
        s.set_sendFailCount(        s.get_sendFailCount()         +   rmsg.get_payload_stat_sendFailCount());

        s.set_sendDoneCount(        s.get_sendDoneCount()         +   rmsg.get_payload_stat_sendDoneCount());
        s.set_sendDoneSuccessCount( s.get_sendDoneSuccessCount()  +   rmsg.get_payload_stat_sendDoneSuccessCount());
        s.set_sendDoneFailCount(    s.get_sendDoneFailCount()     +   rmsg.get_payload_stat_sendDoneFailCount());

        s.set_wasAckedCount(        s.get_wasAckedCount()         +   rmsg.get_payload_stat_wasAckedCount());
        s.set_notAckedCount(        s.get_notAckedCount()         +   rmsg.get_payload_stat_notAckedCount());

        s.set_receiveCount(         s.get_receiveCount()          +   rmsg.get_payload_stat_receiveCount());
        s.set_expectedCount(        s.get_expectedCount()         +   rmsg.get_payload_stat_expectedCount());
        s.set_duplicateCount(       s.get_duplicateCount()        +   rmsg.get_payload_stat_duplicateCount());
        s.set_missedCount(          s.get_missedCount()           +   rmsg.get_payload_stat_missedCount());

        s.set_remainedCount(        s.get_remainedCount()         +   rmsg.get_payload_stat_remainedCount());
        stats.set(currentData,s);

        handshake = true;
        answered.signal();
  
      // refer to RadioTest.h RESP_DBG_OK
      } else if ( rmsg.get_type() == 41 && 
                  stage == 2  &&
                  currentData == rmsg.get_respidx() ) {

        debuglines[currentMote] = rmsg.get_payload_debug_dbgLINE();

        int offset = ( rmsg.get_payload_debug_endtype() == 3 ) 
                     ? -1
                     : ( ( rmsg.get_payload_debug_endtype() == 1 ) ? 0 : 1 );
        
        if ( offset != - 1 )
          finalMsgIds[currentData][offset] = rmsg.get_payload_debug_nextmsgid();

        handshake = true;
        answered.signal();

      // refer to RadioTest.h RESP_DATA_NEXISTS
      } else if ( rmsg.get_type() == 50 && stage == 2 ) {
        handshake = true;
        answered.signal();
      }
      lock.unlock();
    }
	}

  private String statAsString(final short idx) {
    StatT s = stats.get(idx);
    String ret = " Stat(" + idx + "): ";

    ret += " " + s.get_triggerCount();
    ret += " " + s.get_backlogCount();
    ret += " " + s.get_resendCount();

    ret += " | " + s.get_sendCount();
    ret += " " + s.get_sendSuccessCount();
    ret += " " + s.get_sendFailCount();

    ret += " | " + s.get_sendDoneCount();
    ret += " " + s.get_sendDoneSuccessCount();
    ret += " " + s.get_sendDoneFailCount();
      
    ret += " | " + s.get_wasAckedCount();
    ret += " " + s.get_notAckedCount();

    ret += " | " + s.get_receiveCount();
    ret += " " + s.get_expectedCount();
    ret += " " + s.get_duplicateCount();
    ret += " " + s.get_missedCount();

    ret += " | " + s.get_remainedCount();
    ret += " | ";

    return ret;
  }

  private String statAsXml(final short idx) {
    StatT s = stats.get(idx);
    String ret = "<stat idx=\"" + idx + "\">";

    ret += "<TC>" + s.get_triggerCount() + "</TC>";
    ret += "<BC>" + s.get_backlogCount() + "</BC>";
    ret += "<RC>" + s.get_resendCount() + "</RC>";

    ret += "<SC>" + s.get_sendCount() + "</SC>";
    ret += "<SSC>" + s.get_sendSuccessCount() + "</SSC>";
    ret += "<SFC>" + s.get_sendFailCount() + "</SFC>";

    ret += "<SDC>" + s.get_sendDoneCount() + "</SDC>";
    ret += "<SDSC>" + s.get_sendDoneSuccessCount() + "</SDSC>";
    ret += "<SDFC>" + s.get_sendDoneFailCount() + "</SDFC>";

    ret += "<WAC>" + s.get_wasAckedCount() + "</WAC>";
    ret += "<NAC>" + s.get_notAckedCount() + "</NAC>";

    ret += "<RCC>" + s.get_receiveCount() + "</RCC>";
    ret += "<EXC>" + s.get_expectedCount() + "</EXC>";
    ret += "<DRC>" + s.get_duplicateCount() + "</DRC>";
    ret += "<MC>" + s.get_missedCount() + "</MC>";

    ret += "<REMC>" + s.get_remainedCount() + "</REMC>";
    ret += "</stat>";
    return ret;
  }

  private String finalMsgIdsAsString(final int idx) {
    String ret = "Final MsgIds : ";
    ret += " " + finalMsgIds[idx][0];
    ret += " " + finalMsgIds[idx][1];
    return ret;
  }

  private String finalMsgIdsAsXml(final int idx) {
    String ret = "<finaledgestate idx=\"" + idx + "\">";
    ret += "<SNM>" + finalMsgIds[idx][0] + "</SNM>";
    ret += "<RNM>" + finalMsgIds[idx][1] + "</RNM>";
    ret += "</finaledgestate>";
    return ret;
  }

  public void printResults(final SetupT config, final String output) {
    if ( output == "" ) {

      System.out.println();

      String out = " [ Problem: " + config.get_problem_idx();
      out += " | Runtime: " + config.get_runtime_msec() + " ms";
      out += " | Trigger: " + ((trproblem[pidx] != 0) ? config.get_timer_msec() : 0)  + " ms";
      out += " | LCTime: " + config.get_lastchance_msec() + " ms";
      out += " | ACK/DAddr/LPL: ";
      out += ( config.get_flags() & 0x1 ) > 0 ? "On/" : "Off/";
      out += ( config.get_flags() & 0x2 ) > 0 ? "On/" : "Off/";
      out += ( config.get_flags() & 0x4 ) > 0 ? config.get_lplwakeupintval() + " ms ]" : "Off ]";
      System.out.println(out); 

      // Dump out debug info
      System.out.print(" Mote DebugLines (1-" + motecount[pidx] + "): " );
      for( currentMote = 0; currentMote < motecount[pidx]; ++currentMote ) {
        System.out.print( debuglines[currentMote] + " " );
      }      
      System.out.println();

      // Dump out the stats
      for( currentData = 0; currentData < edgecount[pidx]; ++currentData ) {
        System.out.print( statAsString(currentData) );
        System.out.println( finalMsgIdsAsString(currentData) );
      }
      System.out.println();
      System.out.println("> Test completed.");

    } else {
      try {
        PrintStream pstream = new PrintStream(new FileOutputStream(output,true));
        // Dump out the test's configuration
        pstream.println("<testresult>");
        pstream.println("  <testcase>");
        pstream.println("    <idx>" + pidx + "</idx>");
        pstream.println("    <runtime>" + config.get_runtime_msec() + "</runtime>");
        pstream.println("    <trigger>" + ((trproblem[pidx] != 0) ? config.get_timer_msec() : 0) + "</trigger>");
        pstream.println("    <lastchance>" + config.get_lastchance_msec() + "</lastchance>");
        pstream.println("    <ack>" + (((config.get_flags() & 0x1) > 0 )? "On" : "Off") + "</ack>");
        pstream.println("    <daddr>" + (((config.get_flags() & 0x2) > 0 ) ? "On" : "Off") + "</daddr>");
        pstream.println("    <lpl>" + ((config.get_lplwakeupintval() > 0 ) ? "On" : "Off") + "</lpl>");
        pstream.println("    <lplintval>" + config.get_lplwakeupintval() + "</lplintval>");
        pstream.println("  </testcase>");

        // Dump out statistics
        pstream.println("  <statlist>");
        for( currentData = 0; currentData < edgecount[pidx]; ++currentData )
          pstream.println( "    " + statAsXml(currentData) );
        pstream.println("  </statlist>");      

        // Dump out last msg ids
        pstream.println("  <finallist>");
        for( currentData = 0; currentData < edgecount[pidx]; ++currentData )
          pstream.println( "    " + finalMsgIdsAsXml(currentData) );
        pstream.println("  </finallist>");      
        pstream.println("  <debuglist>");
        for( currentMote = 0; currentMote < motecount[pidx]; ++currentMote ) 
          pstream.println( "    <debug idx=\"" + (currentMote+1) + "\">" + debuglines[currentMote] + "</debug>" );
        pstream.println("  </debuglist>");
        pstream.println("</testresult>");
        pstream.close();
      } catch( FileNotFoundException e ) {
        System.err.println("Invalid XML output file specified!");
      }
    }
  }

}

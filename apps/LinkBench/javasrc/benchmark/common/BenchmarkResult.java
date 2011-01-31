/*
* Copyright (c) 2010, University of Szeged
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
* Author: Krisztian Veress
*         veresskrisztian@gmail.com
*/
package benchmark.common;

import java.io.PrintStream;
import java.util.Calendar;
import java.util.Vector;

/**
 * This class holds all the results of a benchmark run along with the computed
 * statistics and any error which may occur during the test run.
 *
 */
public class BenchmarkResult {

  private SetupT            config;
  private Vector<StatT>     stats;
  private long[]            debuginfo;
  private String            error;

  /**
   * Basic constuctor of a result holder.
   */
  public BenchmarkResult() {
    this.init(null);
  }

  /**
   * Construct a result holder with a known configuration.
   * @param config the configuration
   */
  public BenchmarkResult(final SetupT config) {
    this.init(config);
  }

  /**
   * Helper function for constructors
   * @param config the configuration
   */
  private void init(final SetupT config) {
    this.config = config;
    this.stats = null;
    this.debuginfo = null;
    this.error = "";
  }

  /**
   * Clean the result holder object and make it ready for a new benchmark.
   *
   * @param motecount the new benchmark's motecount
   * @param edgecount the new benchmark's edgecount
   */
  public void cleanResize(final int motecount, final int edgecount) {
    if ( this.stats != null )
      this.stats.clear();
    
    this.stats = new Vector(edgecount);
    this.debuginfo = new long[motecount];
    // Initialize the elements
    for (int i = 0; i< edgecount; ++i ) {
      this.stats.add(new StatT());
    }
    for (int i = 0; i< motecount; ++i ) {
      this.debuginfo[i] = 0;
    }

    this.error = "";
  }

  /**
   * Set the configuration of the benchmark for the results holder object.
   * @param config the configuration
   */
  public void setConfig(final SetupT config) {
    this.config = config;
  }

  /**
   * Get the actual benchmark result's configuration
   * @return the configuration
   */
  public SetupT getConfig() {
    return config;
  }

  /**
   * Set an error indicating that an error occured durint the test run.
   * @param error the string representation of the error
   */
  public void setError(final String error) {
    this.error = error;
  }

  /**
   * Get the error which may have occured.
   * @return the string representation of the error
   */
  public String getError() {
    return this.error;
  }

  /**
   * Append new debug information to existing ones from a message.
   * Note that the message does not contain the sender's id, that is why
   * we must explicitly specify it for this function.
   *
   * @param idx the mote's id to which this information belongs
   * @param msg the message holding the information
   */
  public void appendDebugInfoFromMessage(final int idx, final DataMsgT msg) {

    this.debuginfo[idx-1] = msg.get_payload_debug();
  }

  /**
   * Append new statistics to existing ones from a message.
   * Note that the message does not contain the sender's id, that is why
   * we must explicitly specify it for this function.
   *
   * @param idx the mote's id to which this information belongs
   * @param msg the message holding the information
   */
  public void appendStatFromMessage(final int idx, final DataMsgT msg) {

    // Get the new Stat from the message
    StatT s = new StatT();
    s.set_triggerCount(msg.get_payload_stat_triggerCount());
    s.set_backlogCount(msg.get_payload_stat_backlogCount());
    s.set_resendCount(msg.get_payload_stat_resendCount());
    s.set_sendCount(msg.get_payload_stat_sendCount());
    s.set_sendSuccessCount(msg.get_payload_stat_sendSuccessCount());
    s.set_sendFailCount(msg.get_payload_stat_sendFailCount());

    s.set_sendDoneCount(msg.get_payload_stat_sendDoneCount());
    s.set_sendDoneSuccessCount(msg.get_payload_stat_sendDoneSuccessCount());
    s.set_sendDoneFailCount(msg.get_payload_stat_sendDoneFailCount());

    s.set_wasAckedCount(msg.get_payload_stat_wasAckedCount());
    s.set_notAckedCount(msg.get_payload_stat_notAckedCount());

    s.set_receiveCount(msg.get_payload_stat_receiveCount());
    s.set_expectedCount(msg.get_payload_stat_expectedCount());
    s.set_wrongCount(msg.get_payload_stat_wrongCount());
    s.set_duplicateCount(msg.get_payload_stat_duplicateCount());
    s.set_missedCount(msg.get_payload_stat_missedCount());
    s.set_forwardCount(msg.get_payload_stat_forwardCount());

    s.set_remainedCount(msg.get_payload_stat_remainedCount());

    // Get the current stat
    StatT s1 = this.stats.get(idx);
    
    // Make a new stat - this will be the merginf of the current and the new one.
    StatT news = new StatT();

    news.set_triggerCount(         s1.get_triggerCount()          +   s.get_triggerCount());
    news.set_backlogCount(         s1.get_backlogCount()          +   s.get_backlogCount());
    news.set_resendCount(          s1.get_resendCount()           +   s.get_resendCount());
    news.set_sendCount(            s1.get_sendCount()             +   s.get_sendCount());
    news.set_sendSuccessCount(     s1.get_sendSuccessCount()      +   s.get_sendSuccessCount());
    news.set_sendFailCount(        s1.get_sendFailCount()         +   s.get_sendFailCount());

    news.set_sendDoneCount(        s1.get_sendDoneCount()         +   s.get_sendDoneCount());
    news.set_sendDoneSuccessCount( s1.get_sendDoneSuccessCount()  +   s.get_sendDoneSuccessCount());
    news.set_sendDoneFailCount(    s1.get_sendDoneFailCount()     +   s.get_sendDoneFailCount());

    news.set_wasAckedCount(        s1.get_wasAckedCount()         +   s.get_wasAckedCount());
    news.set_notAckedCount(        s1.get_notAckedCount()         +   s.get_notAckedCount());

    news.set_receiveCount(         s1.get_receiveCount()          +   s.get_receiveCount());
    news.set_expectedCount(        s1.get_expectedCount()         +   s.get_expectedCount());
    news.set_wrongCount(           s1.get_wrongCount()            +   s.get_wrongCount());
    news.set_duplicateCount(       s1.get_duplicateCount()        +   s.get_duplicateCount());
    news.set_missedCount(          s1.get_missedCount()           +   s.get_missedCount());
    news.set_forwardCount(         s1.get_forwardCount()          +   s.get_forwardCount());

    news.set_remainedCount((short)(s1.get_remainedCount()         +   s.get_remainedCount()));

    this.stats.set(idx, news);
  }

  /**
   * Print the current result's configuration into a character stream.
   *
   * @param stream the stream
   */
  public void printConfig(PrintStream stream) {
    stream.println(BenchmarkCommons.setupAsString(this.config));
  }

  /**
   * Print the current results with debug and error information into a character
   * stream.
   *
   * @param stream the stream
   */
  public void print(PrintStream stream) {
    stream.println(BenchmarkCommons.statsAsString(this.stats));
    stream.println(BenchmarkCommons.debugsAsString(this.debuginfo));
    stream.println(BenchmarkCommons.errorAsString(this.error));
  }

  /**
   * Print the current results with debug and error information into a character
   * stream with XML formatting.
   *
   * @param stream the stream
   */
  public void printXml(PrintStream stream) {
    Calendar calendar = Calendar.getInstance();
    stream.println("<testresult date=\"" + calendar.getTime().toString() + "\">");
    stream.println(BenchmarkCommons.setupAsXml(config));
    stream.println(BenchmarkCommons.statsAsXml(stats));
    stream.println(BenchmarkCommons.debugsAsXml(debuginfo));
    stream.println(BenchmarkCommons.errorAsXml(error));
    stream.println("</testresult>");
  }

}

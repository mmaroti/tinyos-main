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

import java.util.Vector;

public class BenchmarkCommons {

  public static final short   DEF_RANDSTART     = 0;
  public static final short   DEF_RUNTIME       = 1000;
  public static final short   DEF_LASTCHANCE    = 20;

  public static final boolean DEF_TIMER_ONESHOT = false;
  public static final short   DEF_TIMER_DELAY   = 0;
  public static final short   DEF_TIMER_PERIOD  = 100;

  /**
   * Compute the overall running time of the benchmark defined by the argument
   * @param config The benchmark configuration
   * @return the overall running time in msecs
   */
  public static long getRuntime(final SetupT config) {
    return config.get_pre_run_msec() +
           config.get_runtime_msec() +
           config.get_post_run_msec();
  }

  /**
   * Get the XML header for results generation
   * @return the XML header as one string
   */
  public static String xmlHeader() {
    return "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>" +
           "<?xml-stylesheet type=\"text/xsl\" href=\"assets/benchmark.xsl\"?>" +
           "<resultset>";
  }

  /**
   * Get the XML footer for results generation
   * @return the XML footer as one string
   */
  public static String xmlFooter() {
    return "</resultset>";
  }


  /**
   * Generates a multi-line String representation of a benchmark
   * configuration passed as an argument.
   *
   * @param config The configuration
   * @return the string representation
   */
  public static String setupAsString(final SetupT config) {
  
    String newline = System.getProperty("line.separator");
    String out = "> Problem: \t" + config.get_problem_idx() + newline;
    out += "  Runtime: \t[max (" + config.get_pre_run_msec() + "ms) + " 
           + config.get_runtime_msec() + "ms + " 
           + config.get_post_run_msec() + " ms]" + newline;
    out += "  F.Ack/Bcast: \t";
    out += ( config.get_flags() & 0x1 ) > 0 ? "On/" : "Off/";
    out += ( config.get_flags() & 0x2 ) > 0 ? "On" : "Off";
    out += newline;
    out += "  LPL: \t\t";
    out += (config.get_lplwakeup() == 0) ? "Off" : config.get_lplwakeup() + " ms";
    out += newline;
    
    out += "  Timers: \t[";
    short ios[] = config.get_timers_isoneshot();
    long delay[] = config.get_timers_delay();
    long period[] = config.get_timers_period_msec(); 
               
    for (int i=0; i< BenchmarkStatic.MAX_TIMER_COUNT; ++i) {
      out += (ios[i] == 1 ) ? "1shot " : "period ";
      out += delay[i] + "ms/";
      out += period[i] + "ms";
      if (i != BenchmarkStatic.MAX_TIMER_COUNT-1)
        out += " | ";
    }
    out += "]";
    return out;
  }

  /**
   * Generates an XML representation of a benchmark
   * configuration passed as an argument.
   *
   * @param config The configuration
   * @return the XML representation
   */
  public static String setupAsXml(final SetupT config) {
    String out = "<configuration>";
    out +="    <benchidx>" + config.get_problem_idx() + "</benchidx>";
    out +="    <pre_runtime>" + config.get_pre_run_msec() + "</pre_runtime>";
    out +="    <runtime>" + config.get_runtime_msec() + "</runtime>";
    out +="    <post_runtime>" + config.get_post_run_msec() + "</post_runtime>";
    out +="    <ack>" + (((config.get_flags() & 0x1) > 0 )? "On" : "Off") + "</ack>";
    out +="    <bcast>" + (((config.get_flags() & 0x2) > 0 ) ? "On" : "Off") + "</bcast>";
    out +="    <lpl>" + ((config.get_lplwakeup() == 0) ? "Off" : config.get_lplwakeup() + " ms") + "</lpl>";
  
    short ios[] = config.get_timers_isoneshot();
    long delay[] = config.get_timers_delay();
    long period[] = config.get_timers_period_msec();        
    for (int i=0; i< BenchmarkStatic.MAX_TIMER_COUNT; ++i) {
      out += "<timer idx=\"" + i + "\" ";
      out += (ios[i] == 1 )? "oneshot=\"yes\" " : "oneshot=\"no\" ";
      out += "delay=\"" + delay[i] + "\" ";
      out += "period=\"" + period[i] + "\"/>";
    }
    out += "</configuration>";
    return out;
  }

  /**
   * Generates a multi-line String representation of the results of a benchmark
   *
   * @param stats the collection of the results
   * @return the string representation
   */
  public static String statsAsString(final Vector<StatT> stats) {
    String newline = System.getProperty("line.separator");
    String hdr = "  Statistics :\t[ Tri Blg Res | send Succ Fail | sDone Succ Fail | Ack NAck | Recv  Exp Wrng Dupl Frwd Miss | Rem ]";
    String ret = "";
    for (int i = 0; i < stats.size(); ++i) {
      StatT s = stats.get(i);
      String str = String.format("       E(%2d) :\t[ %2$3d %3$3d %4$3d | %5$4d %6$4d %7$4d | %8$5d %9$4d %10$4d | %11$3d %12$4d | %13$4d %14$4d %15$4d %16$4d %17$4d %18$4d | %19$3d ]",
              i,
              s.get_triggerCount(),
              s.get_backlogCount(),
              s.get_resendCount(),
              s.get_sendCount(),
              s.get_sendSuccessCount(),
              s.get_sendFailCount(),
              s.get_sendDoneCount(),
              s.get_sendDoneSuccessCount(),
              s.get_sendDoneFailCount(),
              s.get_wasAckedCount(),
              s.get_notAckedCount(),
              s.get_receiveCount(),
              s.get_expectedCount(),
              s.get_wrongCount(),
              s.get_duplicateCount(),
              s.get_forwardCount(),
              s.get_missedCount(),
              s.get_remainedCount());
       ret += str + newline;
    }
    return hdr + newline + ret;
  }

   /**
   * Generates an XML representation of the results of a benchmark.
   *
   * @param stats the collection of the results
   * @return the XML representation
   */
  public static String statsAsXml(final Vector<StatT> stats) {
    String ret = "<statlist>";
    for ( int i = 0; i< stats.size(); ++i ) {
      StatT s = stats.get(i);
      ret += "<stat idx=\"" + i + "\">";

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
      ret += "<WC>" + s.get_wrongCount() + "</WC>";
      ret += "<FC>" + s.get_forwardCount() + "</FC>";
      ret += "<DRC>" + s.get_duplicateCount() + "</DRC>";
      ret += "<MC>" + s.get_missedCount() + "</MC>";

      ret += "<REMC>" + s.get_remainedCount() + "</REMC>";
      ret += "</stat>";
    }  
    ret += "</statlist>";
    return ret;
  }

  /**
   * Generates a String representation of the debug information of a benchmark
   *
   * @param debuglines the debug information
   * @return the string representation
   */
  public static String debugsAsString(final long[] debuglines) {
    String ret = "  Mote debug :\t";
    for( int i = 0; i < debuglines.length; ++i ){
      ret += debuglines[i] + " ";
    }
    return ret;
  }

  /**
   * Generates an XML representation of the debug information of a benchmark
   *
   * @param debuglines the debug information
   * @return the XML representation
   */
  public static String debugsAsXml(final long[] debuglines) {
    String ret = "<debuglist>";
    for( int i = 0; i < debuglines.length; ++i ){
      ret +=  "<debug idx=\"" + (i+1) + "\">" + debuglines[i] + "</debug>";
    }
    ret += "</debuglist>";
    return ret;
  }

  /**
   * Generates a String from the error which may have occured during the benchmark
   * @param s the error
   * @return the ready-for-output representation
   */
  public static String errorAsString(final String s) {
    return "  Error :\t" + s;
  }

  /**
   * Generates an XML tag from the error which may have occured during the benchmark
   * @param s the error
   * @return the ready-for-output XML representation
   */
  public static String errorAsXml(final String s) {
    return "<error>" + s + "</error>";
  }

}
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

import org.apache.commons.cli.*;

public class RadioTest {

  public static Options opt;
  private static int default_runtime_msec = 1000;
  private static int default_sendtrig_msec = 100;
  private static int default_lastchance_msec = 20;

  public static void printHelp() {
    HelpFormatter f = new HelpFormatter();
    f.printHelp("RadioTest", opt, true);
  }

	public static void main (String[] args)
	{
    opt = new Options();
    try {

      Option policy = OptionBuilder.withArgName( "number" )
                                .hasArg()
                                .withDescription( "The network policy to be used [0-" + (RadioTestController.PROBLEMSET_COUNT-1) + "]." )
                                .withLongOpt("policy")
                                .create( "p" );

      Option runtime = OptionBuilder.withArgName( "number" )
                                .hasArg()
                                .withDescription( "The test running time in millisecs. [default: " + default_runtime_msec + " msec]" )
                                .withLongOpt("time")
                                .create( "t" );

      Option trigger = OptionBuilder.withArgName( "number" )
                                .hasArg()
                                .withDescription( "The sending trigger time (it is ignored in some policies).  [default : " + default_sendtrig_msec + " msec]" )
                                .withLongOpt("trigger")
                                .create( "tr" );

      Option lastchance = OptionBuilder.withArgName( "number" )
                                .hasArg()
                                .withDescription( "The grace time period after test completion for last-chance reception. [default : " + default_lastchance_msec + " msec]" )
                                .withLongOpt("lastchance")
                                .create( "lc" );

      Option lpl = OptionBuilder.withArgName( "number" )
                                .hasArg()
                                .withDescription( "The Low Power Listening wakeup interval in millisecs). [default: 0 msec]" )
                                .create( "lpl" );

      Option xml = OptionBuilder.withArgName( "filename" )
                                .hasArg()
                                .withDescription( "Write statistics to XML output" )
                                .create( "xml" );

      opt.addOption(policy);
      opt.addOption(runtime);
      opt.addOption(trigger);
      opt.addOption(lastchance);
      opt.addOption(lpl);
      opt.addOption(xml);

      opt.addOption("ack", false, "Use acknowledgements or not. [default : false]");
      opt.addOption("daddr", "direct_addr", false, "Use direct-sending rather than broadcasting. [default : false]");
      opt.addOption("r", "reset", false, "Reset all motes");
      opt.addOption("h", "help", false, "Print help for this application");

      BasicParser parser = new BasicParser();
      CommandLine cl = parser.parse(opt, args);

      if ( cl.hasOption('h') ) {
        printHelp();

      } else if ( cl.hasOption('p') ) {
        short problemidx = (short)Integer.parseInt(cl.getOptionValue('p'));
        if ( problemidx >= RadioTestController.PROBLEMSET_COUNT || problemidx < 0 )
          throw new MissingOptionException("Invalid policy specified!");
  
        int runtimemsec = cl.hasOption('t') 
                                ? Integer.parseInt(cl.getOptionValue("t")) 
                                : default_runtime_msec;
        if ( runtimemsec <= 0 )
          throw new MissingOptionException("Invalid runtime specified!");
      
        int triggermsec = cl.hasOption("tr") 
                                ? Integer.parseInt(cl.getOptionValue("tr")) 
                                : default_sendtrig_msec;
        if ( triggermsec <= 0 )
          throw new MissingOptionException("Invalid trigger time specified!");

        int lchance = cl.hasOption("lc") 
                                ? Integer.parseInt(cl.getOptionValue("lc")) 
                                : default_lastchance_msec;
        if ( lchance <= 0 )
          throw new MissingOptionException("Invalid last chance time specified!");        

        int lplwui = cl.hasOption("lpl") ? Integer.parseInt(cl.getOptionValue("lpl")) : 0;
        if ( lplwui < 0 )
          throw new MissingOptionException("Invalid LPL wakeup interval specified!");

        short flags = 0;
        // Acknowledgements : set USE_ACK !AND! USE_DADDR bits
        if ( cl.hasOption("ack") )
          flags |= 0x3; 
        // Direct addressing : set USE_DADDR bit
        if ( cl.hasOption("daddr") )
          flags |= 0x2;
        // Low-Power Listening : set USE_LPL bit if and only the wakeup interval > 0
        // Note : In debug mode, the mote verifies this predicate!
        if ( lplwui > 0 )
          flags |= 0x4;

        SetupT st = new SetupT();
        st.set_problem_idx(problemidx);
        st.set_runtime_msec(runtimemsec);
        st.set_timer_msec(triggermsec);
        st.set_lastchance_msec(lchance);
        st.set_lplwakeupintval((short)lplwui);
        st.set_flags(flags);

        RadioTestController rtc = new RadioTestController(problemidx);
        
        // Run the test
        if ( rtc.setupMotes(st) && rtc.run(runtimemsec+lchance) && rtc.collect() ) {
          String output = cl.hasOption("xml") ? cl.getOptionValue("xml") : "";
          rtc.printResults(st,output);
        }        
        // If reset is also requested
        if ( cl.hasOption('r') )
          rtc.resetMotes();

      // Only reset is requested
      } else if ( cl.hasOption('r') ) {
        RadioTestController rtc = new RadioTestController((short)0);
        rtc.resetMotes();
      } else {
        throw new MissingOptionException("Invalid arguments specified!");
      }
    } catch (Exception e) {
      System.err.println();
      System.err.println("Error : " + e.getMessage());
      System.err.println();
      printHelp();

    } finally {
      System.exit(0);
    }
  }
}

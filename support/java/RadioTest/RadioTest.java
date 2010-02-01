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

	public static void main (String[] args)
	{
    Options opt = new Options();
    try {

      Option policy = OptionBuilder.withArgName( "number" )
                                .hasArg()
                                .withDescription( "The network policy to be used [0-" + (RadioTestController.PROBLEMSET_COUNT-1) + "]." )
                                .withLongOpt("policy")
                                .create( "p" );

      Option runtime = OptionBuilder.withArgName( "number" )
                                .hasArg()
                                .withDescription( "The test running time in millisecs." )
                                .withLongOpt("time")
                                .create( "t" );

      Option trigger = OptionBuilder.withArgName( "number" )
                                .hasArg()
                                .withDescription( "The sending trigger time (it is ignored in some policies). [default : 0]" )
                                .withLongOpt("trigger")
                                .create( "tr" );

      opt.addOption(policy);
      opt.addOption(runtime);
      opt.addOption(trigger);

      opt.addOption("ack", false, "Use acknowledgements or not. [default : false]");
      opt.addOption("ds", "direct_send", false, "Use direct-sending rather than broadcasting. [default : false]");
      opt.addOption("lpl", false, "Use Low-Power Listening. [default : false]");
      opt.addOption("r", "reset", false, "Reset all motes");
      opt.addOption("h", "help", false, "Print help for this application");
      opt.addOption("dl", "download", false, "Download the statistics from all motes");

      BasicParser parser = new BasicParser();
      CommandLine cl = parser.parse(opt, args);

      if ( cl.hasOption('h') ) {
        HelpFormatter f = new HelpFormatter();
        f.printHelp("RadioTest", opt, true);

      // Downloading
      } else if ( cl.hasOption('p') && cl.hasOption("dl") ) {
          short problemidx = (short)Integer.parseInt(cl.getOptionValue('p'));
          RadioTestController rtc = new RadioTestController(problemidx);
          if ( rtc.collect() )
            rtc.printStats();

      // Option checks, test running
      } else if ( cl.hasOption('p') && cl.hasOption('t') ) {

        short problemidx = (short)Integer.parseInt(cl.getOptionValue('p'));
        if ( problemidx >= RadioTestController.PROBLEMSET_COUNT )
          throw new MissingOptionException("Invalid policy specified!");

        short runtimemsec = (short)Integer.parseInt(cl.getOptionValue("t"));
        int triggermsec = cl.hasOption("tr") ? Integer.parseInt(cl.getOptionValue("tr")) : 0;
        short flags = 0;
        if ( cl.hasOption("ack") )
          flags |= 0x1;
        if ( cl.hasOption("ds") )
          flags |= 0x2;
        if ( cl.hasOption("lpl") )
          flags |= 0x4;

        SetupT st = new SetupT();
        st.set_problem_idx(problemidx);
        st.set_runtime_msec(runtimemsec);
        st.set_sendtrig_msec(triggermsec);
        st.set_flags(flags);

        RadioTestController rtc = new RadioTestController(problemidx);
        if ( rtc.setupMotes(st) ) {
          rtc.run(runtimemsec);
          if ( rtc.collect() )
            rtc.printStats();
        }
      } else if ( !cl.hasOption('r') ){
          throw new MissingOptionException("Invalid arguments!");
      }

      // Resetting if requested
      if ( cl.hasOption('r') ) {
          RadioTestController rtc = new RadioTestController((short)0);
          rtc.resetMotes();
      }

    } catch (NumberFormatException ex) {
      System.err.println("Invalid arguments specified!");
      HelpFormatter f = new HelpFormatter();
      f.printHelp("RadioTest", opt, true);

    } catch (MissingOptionException e) {
      System.err.println(e.getMessage());
      System.err.println();
      HelpFormatter f = new HelpFormatter();
      f.printHelp("RadioTest", opt, true);

    } catch (ParseException e) {
      e.printStackTrace();
    } finally {
      System.exit(0);
    }
  }
}

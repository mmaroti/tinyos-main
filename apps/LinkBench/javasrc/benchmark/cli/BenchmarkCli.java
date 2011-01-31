/** Copyright (c) 2010, University of Szeged
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

package benchmark.cli;

import benchmark.common.*;
import java.io.FileNotFoundException;
import java.io.PrintStream;

import java.util.regex.Pattern;
import java.util.regex.Matcher;
import org.apache.commons.cli.*;

public class BenchmarkCli {

  private BenchmarkController ctrl;

  public BenchmarkCli() {
    ctrl = new BenchmarkController();
  }

  /**
   * Print out the help information
   */
  public static void printHelp(final Options opt) {
    
    HelpFormatter f = new HelpFormatter();
    System.out.println("Usage scenarios:");

    Options opt0 = new Options();
    opt0.addOption(opt.getOption("h"));
    System.out.println();
    System.out.println("1. Print help information.");
    System.out.println("--------------------------------------------------------------------");
    f.printHelp(150, "Benchmark", "", opt0, "", true);

    // Batch - usage
    Options opt1 = new Options();
    opt1.addOption(opt.getOption("F"));
    opt1.addOption(opt.getOption("o"));    
    System.out.println();
    System.out.println("2. Running benchmarks with pre-defined configurations in batch mode.");
    System.out.println("--------------------------------------------------------------------");
    f.printHelp(150, "Benchmark", "", opt1, "", true);
    
    // Reset - usage
    Options opt2 = new Options();
    opt2.addOption(opt.getOption("r"));
    System.out.println();
    System.out.println("3. Reset all motes.");
    System.out.println("--------------------------------------------------------------------");
    f.printHelp(150, "Benchmark", "", opt2, "", true);
    
    // Download - usage
    Options opt3 = new Options();
    opt3.addOption(opt.getOption("dload"));
    opt3.addOption(opt.getOption("dbg"));
    opt3.addOption(opt.getOption("mc"));
    opt3.addOption(opt.getOption("xml"));
    System.out.println();
    System.out.println("4. Only download data from the motes (if data available).");
    System.out.println("--------------------------------------------------------------------");
    f.printHelp(150, "Benchmark", "", opt3, "", true);
    
    // Command-line usage
    Options opt4 = new Options();
    opt4.addOption(opt.getOption("b"));
    opt4.addOption(opt.getOption("t"));
    opt4.addOption(opt.getOption("rs"));
    opt4.addOption(opt.getOption("lc"));
    opt4.addOption(opt.getOption("tr"));
    opt4.addOption(opt.getOption("ack"));
    opt4.addOption(opt.getOption("bcast"));
    opt4.addOption(opt.getOption("xml"));
    opt4.addOption(opt.getOption("lpl"));
    opt4.addOption(opt.getOption("mc"));
    opt4.addOption(opt.getOption("dbg"));
    System.out.println();
    System.out.println("5. Running a specific benchmark with command-line arguments");
    System.out.println("--------------------------------------------------------------------");
    f.printHelp(88, "Benchmark", "", opt4, "", true);
    
  }

  /**
   * Construct the Options opt appropriate for
   * the Apache CLI command-line interpreter.
   */
  private static void initOptions(Options opt) {

    // Batch related options
    Option batchfile = OptionBuilder
            .withArgName("file")
            .hasArg()
            .withDescription("The batch file with configuration parameters for multiple benchmark runs")
            .create("F");

    Option batchoutput = OptionBuilder
            .withArgName("file")
            .hasArg()
            .withDescription("The output XML file name. [default: results.xml]")
            .create("o");

    // Problem id option
    Option problem = OptionBuilder
            .withArgName("number")
            .hasArg()
            .withDescription("The benchmark to be used")
            .withLongOpt("benchmark")
            .create("b");

    // Time- related options
    Option randomstart = OptionBuilder
            .withArgName("number")
            .hasArg()
            .withDescription("Random start delay in millisecs. [default: " +
              BenchmarkCommons.DEF_RANDSTART + " msec]")
            .withLongOpt("randomstart")
            .create("rs");

    Option runtime = OptionBuilder
            .withArgName("normal")
            .hasArg()
            .withDescription("The benchmark running time in millisecs. " +
              "[default: " + BenchmarkCommons.DEF_RUNTIME + " msec]")
            .withLongOpt("time")
            .create("t");

    Option lastchance = OptionBuilder
            .withArgName("number")
            .hasArg()
            .withDescription("The grace time period after test completion for" +
              " last-chance reception. [default : " +
              BenchmarkCommons.DEF_LASTCHANCE + " msec]")
            .withLongOpt("lastchance")
            .create("lc");

    Option trtimers = OptionBuilder
            .withArgName("timer config list")
            .hasArg()
            .withDescription("Trigger timer configuration " +
              "index:isoneshot,maxrandomdelay,period.  [default : 1:" +
              (BenchmarkCommons.DEF_TIMER_ONESHOT ? "1" : "0") +
              BenchmarkCommons.DEF_TIMER_DELAY +
              BenchmarkCommons.DEF_TIMER_PERIOD + " ]")
            .withLongOpt("triggers")
            .create("tr");


    Option xml = OptionBuilder
            .withArgName("file")
            .hasArg()
            .withDescription("Produce xml output")
            .create("xml");

    Option lpl = OptionBuilder
            .withArgName("MAC param")
            .hasArg()
            .withDescription("Wakeup interval for LPL/LPP")
            .create("lpl");

    Option mcount = OptionBuilder
            .withArgName("number")
            .hasArg()
            .withDescription("How many motes are in the network.")
            .withLongOpt("motecount")
            .create("mc");


    opt.addOption(problem);
    opt.addOption(randomstart);
    opt.addOption(runtime);
    opt.addOption(lastchance);
    opt.addOption(xml);
    opt.addOption(lpl);
    opt.addOption(mcount);
    opt.addOption(trtimers);
    opt.addOption(batchfile);
    opt.addOption(batchoutput);

    opt.addOption("h", "help", false, "Print help for this application");
    opt.addOption("r", "reset", false, "Reset all motes");
    opt.addOption("ack", false, "Force acknowledgements. [default : false]");
    opt.addOption("bcast", "broadcast", false, "Force broadcasting. [default : false]");
    opt.addOption("dload", "download", false, "Only download data from motes.");
    opt.addOption("dbg", false, "Download also debug information. [default : false]");

  }

  public boolean doReset() {
    System.out.print("> Reset all motes   ... ");
    try {
      ctrl.reset();
      System.out.println("OK");
      return true;
    } catch (BenchmarkController.MessageSendException ex) {
      System.out.println("FAIL");
      return false;
    }
  }

  public boolean doSync() {
    System.out.print("> Synchronize motes ... ");
    try {
      ctrl.syncAll();
      System.out.println("OK");
      return true;
    } catch (BenchmarkController.CommunicationException ex) {
      System.out.println("FAIL");
      System.out.println("  " + ex.getMessage() );
      return false;
    }
  }

  public boolean doDownload(final int maxMoteId) {
    System.out.print("> Downloading data  ... ");
    try {
      ctrl.download();
      System.out.println("OK");
      return true;
    } catch (BenchmarkController.CommunicationException ex) {
      System.out.println("FAIL");
      System.out.println("  " + ex.getMessage() );
      return false;
    }
  }

  public boolean doDownloadDebug(final int maxMoteId) {
    System.out.print("> Downloading debug ... ");
    try {
      ctrl.download_debug();
      System.out.println("OK");
      return true;
    } catch (BenchmarkController.CommunicationException ex) {
      System.out.println("FAIL");
      System.out.println("  " + ex.getMessage() );
      return false;
    }
  }

  public boolean doSetup(final SetupT st) {
    System.out.print("> Setting up motes  ... ");
    try {
      ctrl.setup(st);
      System.out.println("OK");
      return true;
    } catch (BenchmarkController.MessageSendException ex) {
      System.out.println("FAIL");
      return false;
    }
  }

  public boolean doRun() {
    System.out.print("> Running benchmark ... ");
    try {
      ctrl.run();
      System.out.println("OK");
      return true;
    } catch (BenchmarkController.MessageSendException ex) {
      System.out.println("FAIL");
      return false;
    }
  }

  public void doPrintXml(final String filename) {
    PrintStream ps;
    try {
      ps = new PrintStream(filename);
      ps.println(BenchmarkCommons.xmlHeader());
      this.ctrl.getResults().printXml(ps);
      ps.println(BenchmarkCommons.xmlFooter());
      ps.close();
    } catch (FileNotFoundException ex) {
      System.out.println("Cannot open " + filename + " for writing!");
    } 
  }

  public void doPrint() {
    this.ctrl.getResults().print(System.out);
  }

	public static void main (String[] args)
  {
    try {
      // Make the options and parse it
      Options opt = new Options();
      BenchmarkCli.initOptions(opt);
      
      BasicParser parser = new BasicParser();
      CommandLine cl = parser.parse(opt, args);

      // Help request -- if present, do nothing else.
      // -----------------------------------------------------------------------
      if ( cl.hasOption('h') ) {
        BenchmarkCli.printHelp(opt);
      }
      // Reset request -- if present, do nothing else.
      // -----------------------------------------------------------------------
      else if ( cl. hasOption('r') ) {
        BenchmarkCli cli = new BenchmarkCli();
        cli.doReset();
      }
      // Download request
      // -----------------------------------------------------------------------
      else if ( cl.hasOption("dload") ) {
      
        int maxmoteid = cl.hasOption("mc")
                                ? Integer.parseInt(cl.getOptionValue("mc")) 
                                : 1;
        if ( maxmoteid < 1 )
          throw new MissingOptionException("Invalid number of motes specified!");

        // Do what needs to be done
        BenchmarkCli cli = new BenchmarkCli();
        if ( cli.doSync() &&
             cli.doDownload(maxmoteid) &&
             (cl.hasOption("dbg") ? cli.doDownloadDebug(maxmoteid) : true) )
        {
          // Dump results to XML or STDOUT
          if ( cl.hasOption("xml") )
            cli.doPrintXml(cl.getOptionValue("xml"));
          else
            cli.doPrint();
        }
      }
      // Batch request
      // -----------------------------------------------------------------------
      else if ( cl.hasOption('F') ) {
        String bfile = cl.getOptionValue('F');
        String ofile = cl.hasOption('o') ? cl.getOptionValue('o') : "results.xml";
        
        BenchmarkBatch rbb = new BenchmarkBatch(ofile);
        if ( rbb.parse(bfile) ) {
          rbb.run();
        }
      }
      // Command line control
      // -----------------------------------------------------------------------
      else if ( cl.hasOption('b') ) {
        
        short problemidx = (short)Integer.parseInt(cl.getOptionValue('b'));
        if ( problemidx < 0 )
          throw new MissingOptionException("Invalid problem specified!");
  
        int startdelay = cl.hasOption("rs") 
                                ? Integer.parseInt(cl.getOptionValue("rs")) 
                                : BenchmarkCommons.DEF_RANDSTART;
        if ( startdelay < 0 )
            throw new MissingOptionException("Invalid random start time specified!");

        int runtimemsec = cl.hasOption('t')
                                ? Integer.parseInt(cl.getOptionValue("t"))
                                : BenchmarkCommons.DEF_RUNTIME;
        if ( runtimemsec <= 0 )
          throw new MissingOptionException("Invalid runtime specified!");

        int lchance = cl.hasOption("lc") 
                                ? Integer.parseInt(cl.getOptionValue("lc")) 
                                : BenchmarkCommons.DEF_LASTCHANCE;
        if ( lchance < 0 )
          throw new MissingOptionException("Invalid last chance time specified!");        

        int lplwakeup = cl.hasOption("lpl") 
                                ? Integer.parseInt(cl.getOptionValue("lpl")) 
                                : 0;
        if ( lplwakeup < 0 )
          throw new MissingOptionException("Invalid wakeup interval specified!");
 
        int maxmoteid = cl.hasOption("mc")
                                ? Integer.parseInt(cl.getOptionValue("mc")) 
                                : 1;
        if ( maxmoteid < 1 )
          throw new MissingOptionException("Invalid number of motes specified!");
 
        // Trigger timer initialization values
        short ios[] =   new short[BenchmarkStatic.MAX_TIMER_COUNT];
        long delay[] =  new long[BenchmarkStatic.MAX_TIMER_COUNT];
        long period[] = new long[BenchmarkStatic.MAX_TIMER_COUNT];

        for( int i = 0; i < BenchmarkStatic.MAX_TIMER_COUNT; ++i ) {
          ios[i] = BenchmarkCommons.DEF_TIMER_ONESHOT ? 1 : 0;
          delay[i] = BenchmarkCommons.DEF_TIMER_DELAY;
          period[i] = BenchmarkCommons.DEF_TIMER_PERIOD;
        }
        
        if ( cl.hasOption("tr") ) {
          String[] triggers = cl.getOptionValues("tr");
          Pattern pattern = Pattern.compile("(\\d+):(\\d+),(\\d+),(\\d+)");

          for( int i = 0; i < triggers.length; ++i ) {
            Matcher matcher = pattern.matcher(triggers[i]);
            if (matcher.find()) {
              int trigidx = Integer.parseInt(matcher.group(1));
              if ( trigidx < 1 || trigidx > BenchmarkStatic.MAX_TIMER_COUNT)
                throw new MissingOptionException("Valid timer indexes are : [1.." + BenchmarkStatic.MAX_TIMER_COUNT + "], see help!");
              --trigidx;
              
              ios[trigidx] = (byte)Integer.parseInt(matcher.group(2));
              
              delay[trigidx] = Integer.parseInt(matcher.group(3));
              period[trigidx] = Integer.parseInt(matcher.group(4));
              
              if (period[trigidx] < 0 || delay[trigidx] < 0 || ios[trigidx] < 0 || ios[trigidx] > 1)
                throw new MissingOptionException("Trigger timer "+ (trigidx+1) +" is invalid, see help!");
              
              // at time 0, only one-shot timers are allowed to fire
              if (period[trigidx] == 0 && ios[trigidx] != 1)
                throw new MissingOptionException("Only one-shot timers are allowed with 0 ms period!");
              
            } else
              throw new MissingOptionException("Invalid trigger timer specification, see help!");
          }
          
        }
        
        short flags = 0;
        if ( cl.hasOption("ack") )
          flags |= BenchmarkStatic.GLOBAL_USE_ACK; 
        if ( cl.hasOption("bcast") )
          flags |= BenchmarkStatic.GLOBAL_USE_BCAST;
        if ( cl.hasOption("lpl") )
          flags |= BenchmarkStatic.GLOBAL_USE_EXTERNAL_MAC;
        
        // Create the setup structure
        SetupT st = new SetupT();
        st.set_problem_idx(problemidx);
        st.set_pre_run_msec(startdelay);
        st.set_runtime_msec(runtimemsec);
        st.set_post_run_msec(lchance);
        st.set_flags(flags);
        st.set_timers_isoneshot(ios);
        st.set_timers_delay(delay);
        st.set_timers_period_msec(period);
        st.set_lplwakeup(lplwakeup);

        // Do what needs to be done
        BenchmarkCli cli = new BenchmarkCli();
        if (cli.doReset()             &&
            cli.doSetup(st)           &&
            cli.doSync()              &&
            cli.doRun()               &&
            cli.doDownload(maxmoteid) &&
            (cl.hasOption("dbg") ? cli.doDownloadDebug(maxmoteid) : true) )
        {

          // Dump results to XML or STDOUT
          if ( cl.hasOption("xml") )
            cli.doPrintXml(cl.getOptionValue("xml"));
          else
            cli.doPrint();
        }

      } else {
        throw new MissingOptionException("Invalid program arguments, use --help for help!");
      }
    } catch (Exception e) {
      System.err.println();
      System.err.println("Error : " + e.getMessage());
      System.err.println();
    } finally {
      System.exit(0);
    }
  }
}
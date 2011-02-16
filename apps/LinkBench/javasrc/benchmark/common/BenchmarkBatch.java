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

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.PrintStream;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;
import org.yaml.snakeyaml.Yaml;

public class BenchmarkBatch {

  private List<SetupT>  setups;
  private List<Integer> motecounts;
  private String        outputfile;

  private final String S_CONFIG           = "config";
  private final String S_CONFIG_BMARK     = "bmark";
  private final String S_CONFIG_MOTEC     = "motes";
  private final String S_CONFIG_RANDSTART = "randstart";
  private final String S_CONFIG_TIME      = "time";
  private final String S_CONFIG_LC        = "lchance";
  private final String S_TIMERS           = "timers";
  private final String S_TIMER_PREFIX     = "t";
  private final String S_FORCES           = "forces";
  private final String S_FORCES_ACK       = "ack";
  private final String S_FORCES_BCAST     = "bcast";
  private final String S_WAKEUP           = "wakeup";


  class WrongFormatException extends Exception {
    public WrongFormatException(String message) {
      super(message);
    }
  }

  /**
   * Construct an object being able to conduct batch benchmark runs.
   * @param outputfile the file where we should output the results
   */
  public BenchmarkBatch(final String outputfile) {
    this.outputfile = outputfile;
    this.setups = new ArrayList<SetupT>();
    this.motecounts = new ArrayList<Integer>();
  }

  private void checkUnknownTags(final Set<String> reference, final Set<String> check) throws WrongFormatException {
    Set<String> difference = new HashSet<String>(check);
    difference.removeAll(reference);
    if ( ! difference.isEmpty() ) {
      throw new WrongFormatException("Unknown tags found : " + difference.toString());
    }
  }

  /**
   * Parse a YAML-formatted configuration file, and save it for further processing.
   *
   * @param configfile the file
   * @return TRUE if no error detected, FALSE otherwise
   * @throws WrongFormatException
   */
  public boolean parse(final String configfile) throws WrongFormatException {
    try {
      System.out.print("> Parsing configuration file ... ");
      int num = 0;
      for (Object doc : new Yaml().loadAll(new FileInputStream(new File(configfile)))) {
        ++num;
       
        // whole benchmark section
        // ---------------------------------------------------------------------
        Map<String, Object> bmark = (Map<String, Object>) doc;
        // Format checking
        if ( bmark == null )
          throw new WrongFormatException("Check your configuration file's format, it is incorrect!");
        else {
          String ref[] = { S_CONFIG, S_TIMERS, S_FORCES, S_WAKEUP };
          checkUnknownTags(new HashSet(Arrays.asList(ref)), bmark.keySet());
          
          if ( !bmark.containsKey(S_CONFIG) )
            throw new WrongFormatException("No '" + S_CONFIG + "' section found in benchmark description : " + num);
        }
        
        // config section - it MUST exist (previously we checked it!)
        // ---------------------------------------------------------------------
        Map<String, Integer> bconfig = (Map<String, Integer>) bmark.get(S_CONFIG);
        // Format checking
        {
          String ref[] = { S_CONFIG_BMARK, S_CONFIG_MOTEC, S_CONFIG_TIME, S_CONFIG_LC, S_CONFIG_RANDSTART };
          checkUnknownTags(new HashSet(Arrays.asList(ref)), bconfig.keySet());

          if ( !bconfig.containsKey(S_CONFIG_BMARK) || !bconfig.containsKey(S_CONFIG_TIME) )
            throw new WrongFormatException("The '" + S_CONFIG_BMARK + "' and '" + S_CONFIG_TIME + "' values are mandatory!");

          // Value check
          if ( bconfig.containsKey(S_CONFIG_BMARK) && bconfig.get(S_CONFIG_BMARK) < 0)
            throw new WrongFormatException("All '" + S_CONFIG_BMARK + "' values must be non-negative!");

          if ( bconfig.containsKey(S_CONFIG_TIME) && bconfig.get(S_CONFIG_TIME) <= 0)
            throw new WrongFormatException("All '" + S_CONFIG_TIME + "' values must be positive!");

          if ( bconfig.containsKey(S_CONFIG_MOTEC) && bconfig.get(S_CONFIG_MOTEC) < 1)
            throw new WrongFormatException("All '" + S_CONFIG_MOTEC + "' values must be positive!");

          if ( bconfig.containsKey(S_CONFIG_LC) && bconfig.get(S_CONFIG_LC) < 0)
            throw new WrongFormatException("All '" + S_CONFIG_LC + "' values must be non-negative!");

          if ( bconfig.containsKey(S_CONFIG_RANDSTART) && bconfig.get(S_CONFIG_RANDSTART) < 0)
            throw new WrongFormatException("All '" + S_CONFIG_RANDSTART + "' values must be non-negative!");

        }

        // timers section
        // ---------------------------------------------------------------------
        List< Map<String,List<Integer>> > btimers = bmark.containsKey(S_TIMERS)
                ? (List< Map<String,List<Integer>> >) bmark.get(S_TIMERS)
                : null;
        // Trigger timer initialization values
        short ios[] =   new short[BenchmarkStatic.MAX_TIMER_COUNT];
        long delay[] =  new long[BenchmarkStatic.MAX_TIMER_COUNT];
        long period[] = new long[BenchmarkStatic.MAX_TIMER_COUNT];

        for( int i = 0; i < BenchmarkStatic.MAX_TIMER_COUNT; ++i ) {
          ios[i] = BenchmarkCommons.DEF_TIMER_ONESHOT ? 1 : 0;
          delay[i] = BenchmarkCommons.DEF_TIMER_DELAY;
          period[i] = BenchmarkCommons.DEF_TIMER_PERIOD;
        }

        // Format checking
        if (btimers != null) {
          for (Map<String, List<Integer>> timerspec : btimers) {
            for (int i = 1; i <= BenchmarkStatic.MAX_TIMER_COUNT; ++i) {
              if (timerspec.containsKey(S_TIMER_PREFIX + i)) {
                List<Integer> timervalues = (List<Integer>) timerspec.get(S_TIMER_PREFIX + i);
                if (timervalues.size() != 3) {
                  throw new WrongFormatException("All timer specification must contain exactly 3 values!");
                }

                ios[i-1] = timervalues.get(0).shortValue();
                delay[i-1] = timervalues.get(1).longValue();
                period[i-1] = timervalues.get(2).longValue();
              }
            }
          }
        }

        // forces section
        // ---------------------------------------------------------------------
        List<String> forceopts = bmark.containsKey(S_FORCES)
                ? (List<String>) bmark.get(S_FORCES)
                : null;
        // Format + value checking
        if ( forceopts != null ) {
          String ref[] = { S_FORCES_ACK, S_FORCES_BCAST};
          checkUnknownTags(new HashSet(Arrays.asList(ref)), new HashSet<String>(forceopts));
        }
        
        // wakeup section
        // ---------------------------------------------------------------------
        Integer bwakeup = bmark.containsKey(S_WAKEUP)
                ? (Integer) bmark.get(S_WAKEUP)
                : (Integer)0;
        // Value checking
        if ( bwakeup < 0 )
          throw new WrongFormatException("All '" + S_WAKEUP + "' values must be non-negative!");

        // Create a SetupT for the current benchmark
        SetupT setup = new SetupT();
        setup.set_problem_idx(bconfig.get(S_CONFIG_BMARK).shortValue());

        setup.set_pre_run_msec(
                bconfig.containsKey(S_CONFIG_RANDSTART) ?
                bconfig.get(S_CONFIG_RANDSTART) :
                BenchmarkCommons.DEF_RANDSTART);

        setup.set_runtime_msec(bconfig.get(S_CONFIG_TIME).shortValue());
        setup.set_post_run_msec(
                bconfig.containsKey(S_CONFIG_LC) ?
                bconfig.get(S_CONFIG_LC) :
                BenchmarkCommons.DEF_LASTCHANCE);

        setup.set_lplwakeup(bwakeup);

        short flags = 0;
        if ( forceopts != null ) {
          if ( forceopts.contains(S_FORCES_ACK) )
            flags |= BenchmarkStatic.GLOBAL_USE_ACK;
          if ( forceopts.contains(S_FORCES_BCAST) )
            flags |= BenchmarkStatic.GLOBAL_USE_BCAST;
          if ( bmark.containsKey(S_WAKEUP) )
            flags |= BenchmarkStatic.GLOBAL_USE_EXTERNAL_MAC;
        }
        setup.set_flags(flags);

        setup.set_timers_isoneshot(ios);
        setup.set_timers_delay(delay);
        setup.set_timers_period_msec(period);

        // update our attributes
        this.setups.add(setup);
        this.motecounts.add(
                bconfig.containsKey(S_CONFIG_MOTEC) ?
                bconfig.get(S_CONFIG_MOTEC) : 1);

      }
      System.out.println("OK");
      System.out.println("   " + configfile + " : " + this.setups.size() + " benchmark(s) successfully parsed.");
      return true;

    } catch (FileNotFoundException ex) {
      System.out.println("FAIL");
      System.out.println("   File named " + configfile + " not found!");

    } catch (WrongFormatException ex) {
      System.out.println("FAIL");
      System.out.println("   " + ex.getMessage());
    } catch (Exception ex) {
      System.out.println("FAIL");
      System.out.println("   Wrong configuration file format! The file must be YAML-formatted.");
    }
    return false;
  }

  /**
   * Run the previously parsed benchmark configurations.
   */
  public boolean run() {
    PrintStream ps = null;
    try {
      ps = new PrintStream(this.outputfile);
      ps.println(BenchmarkCommons.xmlHeader());

      BenchmarkController ctrl = new BenchmarkController();
      ctrl.reset();

      int i = 0;
      int total = this.setups.size();
      int progress = 1;
      // Run each benchmark sequentially
      for (SetupT s : setups) {
        ctrl.updateMoteCount(this.motecounts.get(i++));
        
        System.out.print("\r> Progress : " + (progress * 100 / total ) + "% (" + progress + "/" + total + ")" );
        try {
          ctrl.reset();
          ctrl.setup(s);
          ctrl.syncAll();
          ctrl.run();
          ctrl.download();
          ctrl.download_debug();
        } catch (BenchmarkController.MessageSendException ex) {
          ctrl.getResults().setError(ex.getMessage());
        } catch (BenchmarkController.CommunicationException ex) {
          ctrl.getResults().setError(ex.getMessage());
        }
        ++progress;
        ctrl.getResults().printXml(ps);
      }
      ps.println(BenchmarkCommons.xmlFooter());
      ps.close();
      System.out.println();
      System.out.println("> Batch processing successfully finished!");
      return true;
    } catch (FileNotFoundException ex) {
      System.out.println("   File named " + outputfile + " cannot be created!");
      System.out.println("> Batch processing failed!");
      return false;
    } catch (Exception ex) {
      System.out.println("   " + ex.getMessage());
      System.out.println("> Batch processing failed!");
      return false;
    } 
  }

}

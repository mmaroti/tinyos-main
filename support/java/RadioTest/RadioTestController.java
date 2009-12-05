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

import org.apache.commons.cli.*;

import net.tinyos.message.*;
import net.tinyos.util.*;

class RadioTestController implements MessageListener {
	
	private MoteIF mif;
			
	public RadioTestController()
	{
		mif = new MoteIF(PrintStreamMessenger.err);
	}

	public void messageReceived(int dest_addr,Message msg)
	{
	}
	
	public void sendCtrlMessage(int nodeID,SetupT smsg)  
  {
		try {
			mif.send(nodeID,smsg);
		} catch(IOException e) {
		  System.out.println("Cannot send setup message to node " + nodeID );
    }
	}
	
	public void run(int moteNum,SetupT smsg)
	{
		for(int count = 0; count <= moteNum; ++count )
    	sendCtrlMessage(count,smsg);
		try {
			Thread.sleep(100);
 		} catch(InterruptedException e) {}
	}

	public static void main (String[] args)
	{
    Options opt = new Options();
    try {
      
      Option policy = OptionBuilder.withArgName( "number" )
                                .hasArg()
                                .isRequired()
                                .withDescription( "The network policy to be used [0-11]." )
                                .withLongOpt("policy")
                                .create( "p" );

      Option runtime = OptionBuilder.withArgName( "number" )
                                .hasArg()
                                .isRequired()
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
      opt.addOption("h", "help", false, "Print help for this application");

      BasicParser parser = new BasicParser();
      CommandLine cl = parser.parse(opt, args);

      if ( cl.hasOption('h') || 
           !(cl.hasOption("p") && cl.hasOption("t")) ) {
        HelpFormatter f = new HelpFormatter();
        f.printHelp("RadioTestController", opt, true);
      }
      else {
        short problemidx = (short)Integer.parseInt(cl.getOptionValue("p"));
        int runtimemsec = Integer.parseInt(cl.getOptionValue("t"));
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

  		  RadioTestController ctrl = new RadioTestController();
			  ctrl.run(2,st);
			  System.exit(0);
      }
    } catch (NumberFormatException ex) {
      System.err.println("Invalid arguments specified!");
      HelpFormatter f = new HelpFormatter();
      f.printHelp("RadioTestController", opt, true);
    } catch (MissingOptionException e) {
      System.err.println("Invalid arguments specified!");
      HelpFormatter f = new HelpFormatter();
      f.printHelp("RadioTestController", opt, true);
    } catch (ParseException e) {
      e.printStackTrace();
    }
  }
}


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
* Author: Ali Baharev
*/


package org.szte.wsn.accel;

import net.tinyos.util.*;
import net.tinyos.message.*;
import net.tinyos.packet.*;

final class Sender {

	private final MoteIF moteIF;
	private final CtrlMsg msg = new CtrlMsg();

	Sender(MoteIF mif) {
		moteIF = mif;
	}

	void command(int moteID, short command) {

		msg.set_cmd(command);
		try {
			moteIF.send(moteID, msg);
		} catch (Exception e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		System.out.println("Sending to mote "+moteID+", command "+command);
	}

}

final class Receiver implements MessageListener {
	
	private final Sender s;
	
	int counter = 0;
	
	Receiver(Sender s) {
		this.s = s;
	}

	@Override
	public void messageReceived(int to, Message m) {
//		if (counter%3==0) {
//			ReportMsg msg = (ReportMsg) m;
//			short mode = msg.get_mode()==0?(short)1:0;
//			s.switchMode(msg.get_id(), mode);
//		}
//		else {
//			System.out.println("Skipping message");
//		}
//		++counter;
	}
	
}

public final class AccelPC {

	private final Sender s;
	
	private final Receiver r;

	public AccelPC(MoteIF mif) {
		s = new Sender(mif);
		r = new Receiver(s);
		mif.registerListener(new ReportMsg(), r);
	}

	private static void exitFailure(String msg) {
		System.err.println("Error: " + msg);
		System.exit(1);		
	}

	public static void main(String[] args) {

		String source = "sf@localhost:9002";

		if (args.length == 2) {
			if (!args[0].equals("-comm")) {
				exitFailure("arguments -comm <source>");
			}
			source = args[1];
		}
		else if (args.length != 0) {
			exitFailure("arguments -comm <source>");
		}

		PhoenixSource phoenix = BuildSource.makePhoenix(source, PrintStreamMessenger.err);

		MoteIF mif = new MoteIF(phoenix);
		//TestSerial serial = new TestSerial(mif);		
		new AccelPC(mif).go();
	}

	private void go() {

		try {
			
			s.command(6, (short) 1);

			s.command(6, (short) 2);

			s.command(6, (short) 3);

			s.command(6, (short) 0);

//			while (true) {
//				//s.switchMode(6, mode);
//				Thread.sleep(5000);
//			}
		}
		catch (Exception e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
}

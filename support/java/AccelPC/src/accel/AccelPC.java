package accel;

import net.tinyos.util.*;
import net.tinyos.message.*;
import net.tinyos.packet.*;

final class Sender {

	private final MoteIF moteIF;
	private final CtrlMsg msg = new CtrlMsg();

	Sender(MoteIF mif) {
		moteIF = mif;
	}

	void switchMode(int moteID, short mode) {

		msg.set_cmd(mode);
		try {
			moteIF.send(moteID, msg);
		} catch (Exception e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		System.out.println("Setting mote "+moteID+" to mode "+mode);
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
		if (counter%3==0) {
			ReportMsg msg = (ReportMsg) m;
			short mode = msg.get_mode()==0?(short)1:0;
			s.switchMode(msg.get_id(), mode);
		}
		else {
			System.out.println("Skipping message");
		}
		++counter;
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

			while (true) {
				//s.switchMode(6, mode);
				Thread.sleep(5000);
			}

		}
		catch (InterruptedException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
}

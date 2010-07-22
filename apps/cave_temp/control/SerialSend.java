import static java.lang.System.out;
import net.tinyos.packet.*;
import net.tinyos.message.*;
import net.tinyos.util.PrintStreamMessenger;
import java.io.*;

class SerialSend{
	
	private MoteIF moteIF;
	
	public SerialSend(MoteIF moteIF){
		this.moteIF=moteIF;
	}
	
	public void send(String counter, String counter2)  
	{
		SerialMsg msg=new SerialMsg();
		try{
			msg.set_counter(Integer.parseInt(counter));
			msg.set_counter2(Integer.parseInt(counter2));
			moteIF.send(MoteIF.TOS_BCAST_ADDR,msg);
			out.println("Message sent to the mote ");
		}catch(IOException e)
		{
			out.println("Cannot send message to mote ");
		}
	}

	
	public static void main(String[] args) throws Exception 
	{
		PhoenixSource phoenix = null;
		MoteIF mif = null;
		String data=null;
		String data2=null;
				
		if( args.length == 1 ){
			phoenix = BuildSource.makePhoenix(PrintStreamMessenger.err);
			data=args[0];
		} else if ( args.length == 4 && args[0].equals("-comm") ) {
			phoenix = BuildSource.makePhoenix(args[1], PrintStreamMessenger.err);
			data=args[2];
			data2=args[3];
		} else {
			System.err.println("usage: java TestSerial [-comm <source>] [data to send] [ID to send]");
			System.exit(1);
		}
		mif = new MoteIF(phoenix);
		SerialSend app= new SerialSend(mif);
		app.send(data,data2);
		System.exit(0);
	}
}

	

	
	
	

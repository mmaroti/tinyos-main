import static java.lang.System.out;
import net.tinyos.packet.*;
import net.tinyos.message.*;
import net.tinyos.util.PrintStreamMessenger;
import java.io.*;

class SerialReceive implements MessageListener{
	
	private MoteIF moteIF;
	static PrintWriter fop;
	int last=-1;
	public SerialReceive(MoteIF moteIF){
		this.moteIF=moteIF;
		this.moteIF.registerListener(new SerialMsg(),this);
	}
	
	public void messageReceived(int dest_addr,Message msg){
		int tempmert;
		double tempkorr;
		int time;
		int node;
		if (msg instanceof SerialMsg) {
			SerialMsg serialData = (SerialMsg)msg;
			SerialPacket serPkt = msg.getSerialPacket();
			tempmert = serialData.get_temperature();
			tempkorr= (-39.6)+0.01*tempmert;
			time=serialData.get_time();
			node=serialData.get_nodeID();
			try{
		fop = new PrintWriter(new FileWriter("homerseklet"+node+".txt",true));
		}catch(IOException ex){}
			if (last==time){System.exit(1);}
			else {last=time;}
			out.println("Temperature: " + tempkorr +" Time "+time+" Node: "+node);
			fop.println("Temperature: " + tempkorr +" Time "+time+" Node: "+node);
			fop.close();
		}
	}
	
	public static void main(String[] args)throws IOException  
	{
		PhoenixSource phoenix = null;
		MoteIF mif = null;
		
		if( args.length == 0 ){
			phoenix = BuildSource.makePhoenix(PrintStreamMessenger.err);
		} else if( args.length == 2 && args[0].equals("-comm") ) {
			phoenix = BuildSource.makePhoenix(args[1], PrintStreamMessenger.err);
		} else {
			System.err.println("usage: java TestSerial [-comm <source>]");
			System.exit(1);
		}
		mif = new MoteIF(phoenix);
		SerialReceive app= new SerialReceive(mif);
		
		
	}
}

	

	
	
	

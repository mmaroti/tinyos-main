import static java.lang.System.out;
import static java.lang.System.in;
import java.io.*;
import net.tinyos.message.*;
import net.tinyos.util.*;
import net.tinyos.packet.*;

class Hyperterminal implements MessageListener{
	private PhoenixSource phoenix;
	private MoteIF mif;
	private InputStreamReader convert;
	private BufferedReader in;

	public Hyperterminal(final String source){
		phoenix=BuildSource.makePhoenix(source, PrintStreamMessenger.err);
		mif = new MoteIF(phoenix);
		mif.registerListener(new DataMsg(),this);
	}
	
	public void SendCommand(short[] asciiCodes){
		DataMsg command = new DataMsg();
		command.set_cmd(asciiCodes);
		command.set_length((short)asciiCodes.length);
		try{
			mif.send(MoteIF.TOS_BCAST_ADDR,command);
		}catch(IOException e){
			out.println("Cannot send message");
		}
	}
	
	public short[] readCommand(){
	    BufferedReader buff = new BufferedReader(new InputStreamReader(System.in));
	    short[] asciiCodes=null;
	    String str=null;
	    try{
	    	str = buff.readLine();
	    	str+=(char)13;
	    }catch(IOException e){}; 
   	    asciiCodes= new short[str.length()];
	   	for ( int i=0; i<str.length(); ++i ){
	    	asciiCodes[i] = (short)str.charAt(i);
	    }
	    return asciiCodes;
	}
	
	public void messageReceived(int dest_addr,Message msg){
		DataMsg recAnswer=(DataMsg)msg;
		out.print("The received answer is: ");
		for(int i=0;i<recAnswer.get_length();++i){
			out.print((char)recAnswer.getElement_cmd(i)+" ");
			//out.print(recAnswer.getElement_cmd(i)+" ");
		}
		out.println();
	}
	
	
	public static void main (String[] args) {
		int i=1;
		InputStreamReader converter = new InputStreamReader(System.in);
		if ( args.length == 2 && args[0].equals("-comm") ) {
			Hyperterminal hy = new Hyperterminal(args[1]);
			do{
				out.println("Please insert the "+(i++)+". command: ");
				hy.SendCommand(hy.readCommand());
			}while(true	);
		} else {
			System.err.println("usage: java Oscilloscope [-comm <source>]");
			System.exit(1);
		}
		
	}

}

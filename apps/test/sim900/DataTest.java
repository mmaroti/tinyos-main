import static java.lang.System.out;
import static java.lang.System.in;
import java.io.*;

import net.tinyos.message.*;
import net.tinyos.util.*;
import net.tinyos.packet.*;

class DataTest implements MessageListener{
	private PhoenixSource phoenix;
	private MoteIF mif;

	public DataTest(final String source){
		phoenix=BuildSource.makePhoenix(source, PrintStreamMessenger.err);
		mif = new MoteIF(phoenix);
		mif.registerListener(new GsmMsg(),this);
	}
	
	public void SendData(String cmd){
		GsmMsg data = new GsmMsg();
		data.setString_cmd(cmd);
		data.set_length(cmd.length());
		try{
			mif.send(13,data);
		}catch(IOException e){
			out.println("Cannot send message");
		}
	}
	
	public String readData(){
	    BufferedReader buff = new BufferedReader(new InputStreamReader(System.in));
	    out.println("Enter data");
	    String input = null;
		try {
			input = buff.readLine();
		} catch (IOException e) {
			e.printStackTrace();
		}
	    return input;
	}
	
	public void messageReceived(int dest_addr,Message msg){
		GsmMsg recAnswer=(GsmMsg)msg;
		out.print("The received Data is: ");
		for(int i=8;i<recAnswer.get_length();++i){
			out.print((char)recAnswer.getElement_cmd(i));
		}
		out.println();
	}	
	
	public static void main (String[] args) {
		int i=1;
		if ( args.length == 2 && args[0].equals("-comm") ) {
			DataTest hy = new DataTest(args[1]);
			do{
				out.println("Please enter the "+(i++)+". data: ");
				hy.SendData(hy.readData());
			}while(true	);
		} else {
			System.err.println("usage: java DataTest [-comm <source>]");
			System.exit(1);
		}
		
	}

}

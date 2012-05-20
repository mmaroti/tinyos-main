
import net.tinyos.message.*;
import net.tinyos.util.*;
import java.io.*;
import java.lang.*;


public class GreenHouse implements MessageListener
{
    MoteIF mif;
	
	protected java.text.SimpleDateFormat timestamp = new java.text.SimpleDateFormat("yyyy.MM.dd HH:mm:ss");

	public void run(){
		mif = new MoteIF(PrintStreamMessenger.err);
		mif.registerListener(new GreenHouseMsg(), this);
	}
	
	public /*synchronized */void messageReceived(int to, Message m){
		
		System.out.println("Message received.");
		if(m instanceof GreenHouseMsg)
		{
			GreenHouseMsg ghmsg = (GreenHouseMsg)(m);
			int nid = ghmsg.get_source();
			
			System.out.println("Correct message format. Sent by mote " + nid + ".");
			
			System.out.println("GHMsg: " + ghmsg);
			System.out.println("Message: " + m);
			
			File logfile;
			FileOutputStream fos = null;
			PrintWriter pw;
			
			final String delimiter = " ";
			
			logfile = new File("log/mote_" + Integer.toString(nid) + ".txt");
			/*
			if( !logfile.exists() )
			{
				try
				{
					logfile.createNewFile();
					FileWriter fw = new FileWriter( logfile );
					//fw.write("Log file of mote with ID" + Integer.toString(nid) );
					fw.write("Log file is created." );
				}
				catch(IOException e)
				{
					System.out.println("Unable to create "+logfile+": "+e.getMessage());
				}
			}
			*/

			//Létrehozunk egy FileWriter objektumot, hogy hozzáfûhessünk (2. paraméter = TRUE) a fájlhoz.
			/*
			try
			{
				FileWriter fw = new FileWriter( logfile, true );
				//fw.write( timestamp.format(new java.util.Date()) );
				fw.write( "2012. 01. 30. 11:30" );
			}
			catch(IOException e)
			{
				System.out.println("Error writing" + logfile +": "+e.getMessage());
			}
			*/
			if( !logfile.exists() )
			{
				try
				{
					logfile.createNewFile();
					fos = new FileOutputStream( logfile );
				}
				catch(FileNotFoundException e)
				{
					System.out.println("Error: " + e.getMessage());
				}
				catch(IOException e)
				{
					System.out.println("Unable to create " + logfile + ": "+e.getMessage());
				}
				
				pw = new PrintWriter( fos );
				pw.println("Log file of mote with ID " + Integer.toString(nid));
				pw.close();
			}
			try
			{
				fos = new FileOutputStream( logfile, true );
				pw = new PrintWriter( fos, true );
				/*
				int baseOffset = m.baseOffset();
				int dataLength = m.dataLength();
				byte[] rawData = m.dataGet();
				*/
				pw.println( "--------------------------------------" );
				pw.println( timestamp.format(new java.util.Date()) );
				/*
				pw.print("Raw data: ");
				for(int i = baseOffset; i < baseOffset + dataLength; i++)
					pw.print(Byte.toString(rawData[i]) + " ");
				pw.println();
				*/
				
				pw.println( "ID of transmitter: " + Integer.toString(ghmsg.get_source()) );
				pw.println( "Sequence number: " + Integer.toString(ghmsg.get_seqno()) );
				pw.println( "Parent: " + Integer.toString(ghmsg.get_parent()) );
				pw.println( "Metric: " + Integer.toString(ghmsg.get_metric()) );
				pw.print( "Data: " );
				int[] data = ghmsg.get_data();
				String dataString = "";
				for(int i = 0; i < data.length; i++)
				{
					dataString += Double.toString( data[i] ) + delimiter;
				}
				pw.println(dataString);
				
				//pw.println( ghmsg.toString() );
				
				pw.close();
			}
			catch(FileNotFoundException e)
			{
				System.out.println("Error: " + e.getMessage());
			}
		}
	}
	
    public static void main(String[] args) {
		GreenHouse gh = new GreenHouse();
		gh.run();
    }
}

import java.io.*;
import java.lang.*;
import net.tinyos.packet.*;
import net.tinyos.message.*;
import net.tinyos.util.*;


public class GreenHouse implements MessageListener
{
	//A szenzorlapka és a számítógép közötti kommunikációt megvalósító osztály
    private MoteIF mif;
	//Üzenetek érkezésének idõpontját regisztráló tag
	protected java.text.SimpleDateFormat timestamp = new java.text.SimpleDateFormat("yyyy.MM.dd HH:mm:ss");
	//Konstruktor
	public GreenHouse(MoteIF mif){
		this.mif = mif;
		//Várt üzenet típusának megadása
		this.mif.registerListener(new GreenHouseMsg(), this);
	}
	//mif.receiver.[msgTemplate].listener.messageReceived(int, Message)
	public /*synchronized */void messageReceived(int to, Message m){
		
		System.out.println("Message received.");
		System.out.println("Message's dataLength = " + m.dataLength());
		System.out.println("Message's baseOffset = " + m.baseOffset());
		
		if(m instanceof GreenHouseMsg)
		{
			GreenHouseMsg ghmsg = (GreenHouseMsg) m;
			
			int nid = ghmsg.get_source();
			
			System.out.println("Correct message format. Sent by mote " + nid + ".");
			//Fájlba írás
			File logfile;
			FileOutputStream fos = null;
			PrintWriter pw;
			
			final String delimiter = " ";
			//A szenzorlapkához tartozó logfájl nevének meghatározása
			logfile = new File("log/mote_" + Integer.toString(nid) + ".txt");
			//Ha nem létezik, megpróbáljuk létrehozni
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
			//Tényleges adatírás
			try
			{
				fos = new FileOutputStream( logfile, true );
				pw = new PrintWriter( fos, true );
				
				pw.println( "--------------------------------------" );
				pw.println( timestamp.format(new java.util.Date()) );				
				
				pw.println( "ID of transmitter: " + Integer.toString(nid) );
				pw.println( "Sequence number: " + Integer.toString(ghmsg.get_seqno()) );
				pw.println( "Parent: " + Integer.toString(ghmsg.get_parent()) );
				pw.println( "Metric: " + Integer.toString(ghmsg.get_metric()) );
				
				pw.print( "Data: " );
				int[] data = ghmsg.get_data();
				String dataString = "";
				for(int i = 0; i < data.length; i++)
					dataString += Double.toString( data[i] ) + delimiter;
				pw.println(dataString);
				
				pw.close();
			}
			catch(FileNotFoundException e)
			{
				System.out.println("Error: " + e.getMessage());
			}
		}
	}
	
    public static void main(String[] args) {
		PhoenixSource phoenix = null;
		MoteIF mif = null;
		//Parancssori paraméterek alapján adatforrás meghatározása
		if( args.length == 0 )
		{
			phoenix = BuildSource.makePhoenix( PrintStreamMessenger.err );
		}
		else if( args.length == 2 && args[0].equals("-comm") )
		{
			phoenix = BuildSource.makePhoenix( args[1], PrintStreamMessenger.err );
		}
		else
		{
			System.err.println("Usage: java GreenHouse [-comm <source>]");
			java.lang.System.exit(1);
		}
		mif = new MoteIF(phoenix);
		//Alkalmazás futtatása
		GreenHouse app = new GreenHouse(mif);
    }
}

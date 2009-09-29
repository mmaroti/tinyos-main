import static java.lang.System.out;
import java.io.IOException;
import java.util.ArrayList;
import net.tinyos.message.*;
import net.tinyos.util.*;

class TimeSync implements MessageListener{

	private MoteIF mif;
			
	public TimeSync()
	{
		mif = new MoteIF(PrintStreamMessenger.err);
		mif.registerListener(new TimeMsg(),this);
	}
	public void messageReceived(int dest_addr,Message msg)
	{
		if(msg instanceof TimeMsg)
		{
			TimeMsg timemsg=(TimeMsg)msg;
			out.print(timemsg.get_remNodeID());
			out.print("\t" + (timemsg.get_remTime()));
			out.print("\t" + timemsg.get_locNodeID());
			out.println("\t " + timemsg.get_locTime());
		}
	}
	public static void main (String[] args)
	{
		TimeSync timesync= new TimeSync();
		out.println("RID\tRemTime\tLID\tLocTime");
	}	
}

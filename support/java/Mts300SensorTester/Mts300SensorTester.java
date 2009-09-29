import static java.lang.System.out;
import java.io.IOException;
import java.util.ArrayList;
import net.tinyos.message.*;
import net.tinyos.util.*;

class Mts300SensorTester implements MessageListener{
	private MoteIF mif;
	private int recMsgNum=0;
	public ReceivedData[] recDataArray;
	
	public Mts300SensorTester(String from,String to)
	{
		mif = new MoteIF(PrintStreamMessenger.err);
		mif.registerListener(new ControlMsg(),this);
		mif.registerListener(new DataMsg(),this);
		recDataArray = new ReceivedData[Math.abs(Integer.parseInt(to)-Integer.parseInt(from))+1];
	}
	public void messageReceived(int dest_addr,Message msg)
	{
		if(msg instanceof DataMsg)
		{
			DataMsg datamsg=(DataMsg)msg;
			recDataArray[recMsgNum] = new ReceivedData();
			recDataArray[recMsgNum].setNodeID(datamsg.get_nodeID());
			recDataArray[recMsgNum].setMin(datamsg.get_min());
			recDataArray[recMsgNum].setMax(datamsg.get_max());
			recDataArray[recMsgNum].setAverage(datamsg.get_average());
			recDataArray[recMsgNum].setEnergy(datamsg.get_energy());
			recDataArray[recMsgNum].setsampleCnt(datamsg.get_sampleCnt());
			recDataArray[recMsgNum].setmicSampPer(datamsg.get_micSampPer());
			recDataArray[recMsgNum].setmicSampNum(datamsg.get_micSampNum());
			recMsgNum++;
		}
	}
	
	public void sendCtrlMessage(char instr,short nodeID)  
    {
    	ControlMsg smsg=new ControlMsg();
	
    	smsg.set_instr((short)instr);
		smsg.set_nodeID(nodeID);
		try{
			mif.send((int)nodeID,smsg);
		}catch(IOException e)
		{
			out.println("Cannot send message to mote " + nodeID);
		}
	}
	public void Display(String testType)
	{
		int nodeCount = 0;
		if(recMsgNum == 0)
		{
			out.println("No message received!");
		}
		else
		{
			out.println();
			out.println("ID\tMin\tAver\tMax\tEnergy\tFreq(HZ)");
			while(nodeCount!=recMsgNum)
			{
				
				out.print(recDataArray[nodeCount].getNodeID());
				out.print("\t"+recDataArray[nodeCount].getMin());
				if (testType.equals("mic"))
				{
					out.printf("\t%1.2f",(double)recDataArray[nodeCount].getAverage()/(double)recDataArray[nodeCount].getmicSampNum());	
				}
				else
				{
					out.printf("\t%1.2f",(double)recDataArray[nodeCount].getAverage()/(double)recDataArray[nodeCount].getsampleCnt());
				}	 
				out.print("\t"+recDataArray[nodeCount].getMax());
				if (testType.equals("mic"))
				{
					out.printf("\t%1.2f",(double)recDataArray[nodeCount].getEnergy()/(double)recDataArray[nodeCount].getmicSampNum());	
				}
				else
				{
					out.printf("\t%1.2f",(double)recDataArray[nodeCount].getEnergy()/(double)recDataArray[nodeCount].getsampleCnt());
				}	 
				if (testType.equals("mic"))
				{
					out.printf("\t%1.2f\n",((1/(double)recDataArray[nodeCount].getmicSampPer())*1000000));	
				}
				else
				{
					out.println("\t"+recDataArray[nodeCount].getsampleCnt());
				}			
				nodeCount++;
			}
		}
	}

	public void run(String testType,String from,String to)
	{
		String cache;
		if(Integer.parseInt(from)>Integer.parseInt(to))
		{
			cache = from;
			from = to;
			to = cache;
		}
		for(int nodeID=Integer.parseInt(from);nodeID<=Integer.parseInt(to);nodeID++)
		{
			if (testType.equals("beeper"))
			{
				sendCtrlMessage(testType.charAt(0),(short)nodeID);
				try{
					Thread.sleep(1000);
				}catch(InterruptedException e){}
			}
			else
			{
				sendCtrlMessage(testType.charAt(0),(short)nodeID);
			}
		}
		if (!testType.equals("beeper"))
		{
			try{
				Thread.sleep(3000);
				}catch(InterruptedException e){}
			for(int nodeID=Integer.parseInt(from);nodeID<=Integer.parseInt(to);nodeID++)
			{
				sendCtrlMessage('g',(short)nodeID);
			}
			try{
				Thread.sleep(100);
			}catch(InterruptedException e){}
		}
	}
	public static void main (String[] args)
	{
		String[] temp;
		if (args.length<2)
		{
			out.println("Usage: SensorTester [device test type] [motes which are used in the test (from-to)]");
			out.println("[device test type]: ");
			out.println("\tBeeper test:     beeper");
			out.println("\tLight  test:     light");
			out.println("\tMicrophone test: mic");
			out.println("\tVref:            vref");
			out.println("\tTemperature:     temp");		
		}
		else
		{
			temp = args[1].split("-");
			Mts300SensorTester tester= new Mts300SensorTester(temp[0],temp[1]);
			tester.run(args[0],temp[0],temp[1]);
			tester.Display(args[0]);
			System.exit(0);
		}
	}	
}

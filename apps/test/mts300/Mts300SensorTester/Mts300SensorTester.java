/** Copyright (c) 2009, University of Szeged
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
* Author: Zoltan Kincses
*/

import static java.lang.System.out;
import java.io.IOException;
import java.util.ArrayList;
import java.io.FileOutputStream;
import java.io.PrintStream;
import net.tinyos.message.*;
import net.tinyos.util.*;
import net.tinyos.packet.*; 

class Mts300SensorTester implements MessageListener{
	private MoteIF mif;
	private PhoenixSource phoenix; 
	private int recMsgNum=0;
	private FileOutputStream fileOutput;
	private PrintStream printToFile;
	public ReceivedData[] recDataArray;
	
	public Mts300SensorTester(String from,String to,String source)
	{
		phoenix=BuildSource.makePhoenix(source, PrintStreamMessenger.err);
		mif = new MoteIF(phoenix);
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
		try{
			int nodeCount = 0;
			if(!testType.equals("beeper")){
				fileOutput= new FileOutputStream(testType+"_test.txt");
				printToFile = new PrintStream(fileOutput);
			}
			if(recMsgNum == 0)
			{
				if(!testType.equals("beeper")){
					out.println("No message received!");
					printToFile.println("No message received!");
				}
			}else{
				out.println();
				out.printf("%-13s %-13s %-13s %-13s %-13s %-13s%n","NodeID","Min","Average","Max","Energy","Freqency(HZ)");
				printToFile.printf("%-13s %-13s %-13s %-13s %-13s %-13s%n","NodeID","Min","Average","Max","Energy","Freqency(HZ)");
				while(nodeCount!=recMsgNum)
				{
					if (testType.equals("mic"))
					{
						out.printf("%-13d %-13d %-13.2f %-13d %-13.2f %-13.2f%n",recDataArray[nodeCount].getNodeID(),recDataArray[nodeCount].getMin(),((double)recDataArray[nodeCount].getAverage()/(double)recDataArray[nodeCount].getmicSampNum()),recDataArray[nodeCount].getMax(),((double)recDataArray[nodeCount].getEnergy()/(double)recDataArray[nodeCount].getmicSampNum()),((1/(double)recDataArray[nodeCount].getmicSampPer())*1000000));	
						printToFile.printf("%-13d %-13d %-13.2f %-13d %-13.2f %-13.2f%n",recDataArray[nodeCount].getNodeID(),recDataArray[nodeCount].getMin(),((double)recDataArray[nodeCount].getAverage()/(double)recDataArray[nodeCount].getmicSampNum()),recDataArray[nodeCount].getMax(),((double)recDataArray[nodeCount].getEnergy()/((double)recDataArray[nodeCount].getmicSampNum()-1)),((1/(double)recDataArray[nodeCount].getmicSampPer())*1000000));	
					}
					else
					{
						out.printf("%-13d %-13d %-13.2f %-13d %-13.2f %-13d%n",recDataArray[nodeCount].getNodeID(),recDataArray[nodeCount].getMin(),((double)recDataArray[nodeCount].getAverage()/(double)recDataArray[nodeCount].getsampleCnt()),recDataArray[nodeCount].getMax(),((double)recDataArray[nodeCount].getEnergy()/(double)recDataArray[nodeCount].getsampleCnt()),recDataArray[nodeCount].getsampleCnt());	
						printToFile.printf("%-13d %-13d %-13.2f %-13d %-13.2f %-13d%n",recDataArray[nodeCount].getNodeID(),recDataArray[nodeCount].getMin(),((double)recDataArray[nodeCount].getAverage()/(double)recDataArray[nodeCount].getsampleCnt()),recDataArray[nodeCount].getMax(),((double)recDataArray[nodeCount].getEnergy()/((double)recDataArray[nodeCount].getsampleCnt()-1)),recDataArray[nodeCount].getsampleCnt());	
					}	 
					nodeCount++;
				}
			}
		}catch(IOException e){
			out.println("Cannot open the file");
		}
	}

	public void run(String testType,String from,String to)
	{
		String cache;
		try{
			if(Integer.parseInt(from)>Integer.parseInt(to))
			{
				cache = from;
				from = to;
				to = cache;
			}
			for(int nodeID=Integer.parseInt(from);nodeID<=Integer.parseInt(to);nodeID++)
			{
				if (testType.equals("beeper")) {
					sendCtrlMessage(testType.charAt(0),(short)nodeID);
					Thread.sleep(1000);
				}else{
					sendCtrlMessage(testType.charAt(0),(short)nodeID);
				}
			}
			if (!testType.equals("beeper")) {
				Thread.sleep(2000);
				for(int nodeID=Integer.parseInt(from);nodeID<=Integer.parseInt(to);nodeID++) {
					sendCtrlMessage('g',(short)nodeID);
					Thread.sleep(100);
				}
//				Thread.sleep(100);
			}
		}catch(InterruptedException e){}
	}
	public static void main (String[] args)
	{
		String[] temp;
		if (args.length<4)
		{
			out.println("Usage: SensorTester [device test type] [motes which are used in the test (from-to)] -comm <source>");
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
			Mts300SensorTester tester= new Mts300SensorTester(temp[0],temp[1],args[3]);
			tester.run(args[0],temp[0],temp[1]);
			tester.Display(args[0]);
			System.exit(0);
		}
	}	
}

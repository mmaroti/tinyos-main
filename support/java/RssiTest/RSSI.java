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
import java.lang.Math;
import net.tinyos.message.*;
import net.tinyos.util.*;

class RSSI implements MessageListener {
	
	private MoteIF mif;
	private int recMsgNum=0;
	public ReceivedData[] recDataArray;
			
	public RSSI(String from,String to)
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
			recDataArray[recMsgNum].setsenderNodeID(datamsg.get_senderNodeID());
			recDataArray[recMsgNum].setreceiverNodeID(datamsg.get_receiverNodeID());
			recDataArray[recMsgNum].setrssiMin(datamsg.get_rssiMin());
			recDataArray[recMsgNum].setrssiAver(datamsg.get_rssiAver());
			recDataArray[recMsgNum].setrssiEnergy(datamsg.get_rssiEnergy());
			recDataArray[recMsgNum].setrssiMax(datamsg.get_rssiMax());
			recDataArray[recMsgNum].setlqiMin(datamsg.get_lqiMin());
			recDataArray[recMsgNum].setlqiAver(datamsg.get_lqiAver());
			recDataArray[recMsgNum].setlqiEnergy(datamsg.get_lqiEnergy());
			recDataArray[recMsgNum].setlqiMax(datamsg.get_lqiMax());
			recDataArray[recMsgNum].setsampleCnt(datamsg.get_sampleCnt());
			recMsgNum++;
		}
	}
	
	public void sendCtrlMessage(short nodeID,char instr)  
    {
    	ControlMsg smsg=new ControlMsg();
	
    	smsg.set_nodeID(nodeID);
    	smsg.set_instr((short)instr);
		try{
			mif.send((int)nodeID,smsg);
		}catch(IOException e)
		{
			out.println("Cannot send message to node " + nodeID );
		}
	}
	public void Display()
	{
		int nodeCount = 0;
		if(recMsgNum == 0)
		{
			out.println("No message received!");
		}
		else
		{
			out.println();
			out.println("Rssi:");
			out.println();
			out.println("SID\tRID\tMin\tAver\tMax\tEnergy\tSamples");
			while(nodeCount!=recMsgNum)
			{
				
				out.print("  "+recDataArray[nodeCount].getsenderNodeID());
				out.print("\t"+recDataArray[nodeCount].getreceiverNodeID());
				out.print("\t"+recDataArray[nodeCount].getrssiMin());
				out.printf("\t%3.2f",((double)recDataArray[nodeCount].getrssiAver()/(double)recDataArray[nodeCount].getsampleCnt()));
				out.print("\t"+recDataArray[nodeCount].getrssiMax());
				out.printf("\t%3.2f",((double)recDataArray[nodeCount].getrssiEnergy()/(double)(recDataArray[nodeCount].getsampleCnt()-1)));
				out.println("\t"+recDataArray[nodeCount].getsampleCnt());
				nodeCount++;
			}
			nodeCount=0;
			out.println();
			out.println("Lqi:");
			out.println();
			out.println("SID\tRID\tMin\tAver\tMax\tEnergy\tSamples");
			while(nodeCount!=recMsgNum)
			{
				
				out.print("  "+recDataArray[nodeCount].getsenderNodeID());
				out.print("\t"+recDataArray[nodeCount].getreceiverNodeID());
				out.print("\t"+recDataArray[nodeCount].getlqiMin());
				out.printf("\t%3.2f",((double)recDataArray[nodeCount].getlqiAver()/(double)recDataArray[nodeCount].getsampleCnt()));
				out.print("\t"+recDataArray[nodeCount].getlqiMax());
				out.printf("\t%3.2f",((double)recDataArray[nodeCount].getlqiEnergy()/(double)(recDataArray[nodeCount].getsampleCnt()-1)));
				out.println("\t"+recDataArray[nodeCount].getsampleCnt());
				nodeCount++;
			}
			
		}
	}
	
	public void run(String nodeID,String from,String to)
	{
		String cache;
		sendCtrlMessage((short)Integer.parseInt(nodeID),'s');
		try
		{
			Thread.sleep(2000);
		}catch(InterruptedException e)
		{}
		if(Integer.parseInt(from)>Integer.parseInt(to))
		{
			cache = from;
			from = to;
			to = cache;
		}
		for(int count = Integer.parseInt(from); count<=Integer.parseInt(to);count++)
		{
			sendCtrlMessage((short)count,'g');
		}
		try
		{
			Thread.sleep(100);
		}catch(InterruptedException e)
		{}
	}
	public static void main (String[] args)
	{
		String[] temp;
		if (args.length<2)
		{
			out.println("Usage: RSSI [controller mote ID] [motes which measure data (from-to)]");
		}
		else 
		{
			temp=args[1].split("-");
			RSSI rssitester= new RSSI(temp[0],temp[1]);
			rssitester.run(args[0],temp[0],temp[1]);
			rssitester.Display();
			System.exit(0);
		}
	}	
}


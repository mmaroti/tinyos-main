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
import java.io.FileOutputStream;
import java.io.PrintStream;

class RSSI implements MessageListener {
	
	private MoteIF mif;
	private FileOutputStream fileOutput;
	private PrintStream printToFile;
	boolean rssiFirst=true,lqiFirst=true,vrefFirst=true,tempFirst=true,humFirst=true,srFirst=true,osrFirst=true;
						
	public RSSI()
	{
		mif = new MoteIF(PrintStreamMessenger.err);
		mif.registerListener(new ControlMsg(),this);
		mif.registerListener(new Int8Msg(),this);
		mif.registerListener(new Uint8Msg(),this);
		mif.registerListener(new Uint16Msg(),this);
	}
	public void messageReceived(int dest_addr,Message msg)
	{
		try {
			if (fileOutput==null && printToFile==null) {
				fileOutput= new FileOutputStream("TestResults.txt");
				printToFile = new PrintStream(fileOutput);
			}
			if(msg instanceof Int8Msg) {
				Int8Msg int8Msg = (Int8Msg)msg;
				if(int8Msg.get_dataType()==0)
				{
					out.println("The mote cannot receive the control messages");
					fileOutput.close();
					printToFile.close();
					System.exit(-1);
				}
				if(rssiFirst==true) {
					printToFile.println("The result of the RSSI measurement");
					printToFile.printf("%-13s %-13s %-13s %-13s %-13s %-13s%n","SenderID","ReceiveID","Minimum","Average","Maximum","Energy");	
					rssiFirst=false;
				}
				printToFile.printf("%-13d %-13d %-13d %-13.2f %-13d %-13.2f%n",int8Msg.get_senderNodeID(),int8Msg.get_receiverNodeID(),int8Msg.get_min(),((double)int8Msg.get_sum_a()/(double)int8Msg.get_sampleCnt()),int8Msg.get_max(),((double)int8Msg.get_sum_e()/(double)(int8Msg.get_sampleCnt()-1)));
			}
			else if (msg instanceof Uint8Msg) {
				Uint8Msg uint8Msg = (Uint8Msg)msg;
				if(uint8Msg.get_dataType()==0) {
					out.println("The mote cannot receive the control messages");
					fileOutput.close();
					printToFile.close();
					System.exit(-1);
				}
				if((uint8Msg.get_dataType()=='r') & (rssiFirst==true)) {
					printToFile.println("The result of the RSSI measurement");
					printToFile.printf("%-13s %-13s %-13s %-13s %-13s %-13s%n","SenderID","ReceiveID","Minimum","Average","Maximum","Energy");	
					rssiFirst=false;
				}
				else if ((uint8Msg.get_dataType()=='l') & (lqiFirst==true)) {
					printToFile.println("The result of the LQI measurement");
					printToFile.printf("%-13s %-13s %-13s %-13s %-13s %-13s%n","SenderID","ReceiveID","Minimum","Average","Maximum","Energy");	
					lqiFirst=false;
				}	
				printToFile.printf("%-13d %-13d %-13d %-13.2f %-13d %-13.2f%n",uint8Msg.get_senderNodeID(),uint8Msg.get_receiverNodeID(),uint8Msg.get_min(),((double)uint8Msg.get_sum_a()/(double)uint8Msg.get_sampleCnt()),uint8Msg.get_max(),((double)uint8Msg.get_sum_e()/(double)(uint8Msg.get_sampleCnt()-1)));
			} else if (msg instanceof Uint16Msg) {
				Uint16Msg uint16Msg = (Uint16Msg)msg;
				if(uint16Msg.get_dataType()=='v' & vrefFirst==true) {
					printToFile.println("The result of the Vref measurement");
					printToFile.printf("%-13s %-13s %-13s %-13s %-13s %-13s%n","SenderID","ReceiveID","Minimum","Average","Maximum","Energy");		
					vrefFirst=false;	
				} else if (uint16Msg.get_dataType()=='t' & tempFirst==true){
					printToFile.println("The result of the Temperature measurement");
					printToFile.printf("%-13s %-13s %-13s %-13s %-13s %-13s%n","SenderID","ReceiveID","Minimum","Average","Maximum","Energy");		
					tempFirst=false;
				} else if (uint16Msg.get_dataType()=='h' & humFirst==true){
					printToFile.println("The result of the Humidity measurement");
					printToFile.printf("%-13s %-13s %-13s %-13s %-13s %-13s%n","SenderID","ReceiveID","Minimum","Average","Maximum","Energy");		
					humFirst=false;
				} else if (uint16Msg.get_dataType()=='s' & srFirst==true){
					printToFile.println("The result of the Solar Radiation measurement");
					printToFile.printf("%-13s %-13s %-13s %-13s %-13s %-13s%n","SenderID","ReceiveID","Minimum","Average","Maximum","Energy");		
					srFirst=false;
				} else if (uint16Msg.get_dataType()=='o' & osrFirst==true){
					printToFile.println("The result of the Overal Solar Radiation measurement");
					printToFile.printf("%-13s %-13s %-13s %-13s %-13s %-13s%n","SenderID","ReceiveID","Minimum","Average","Maximum","Energy");		
					osrFirst=true;	
				}
				printToFile.printf("%-13d %-13d %-13d %-13.2f %-13d %-13.2f%n",uint16Msg.get_senderNodeID(),uint16Msg.get_receiverNodeID(),uint16Msg.get_min(),((double)uint16Msg.get_sum_a()/(double)uint16Msg.get_sampleCnt()),uint16Msg.get_max(),((double)uint16Msg.get_sum_e()/(double)(uint16Msg.get_sampleCnt()-1)));
			}
		} catch(IOException e) {
			out.println("Cannot open the file");
			}
	}

	public void sendCtrlMessage(short nodeID,short[] instr)  
    {
    	ControlMsg ctrlmsg=new ControlMsg();
	
    	ctrlmsg.set_nodeID(nodeID);
    	ctrlmsg.set_instr(instr);
		try{
			mif.send((int)nodeID,ctrlmsg);
		}catch(IOException e)
		{
			out.println("Cannot send message to node " + nodeID );
		}
	}
	
	public void run(String nodeType,String nodeID,String from,String to)
	{
		try	{
			String cache;
			short[] instr= new short[2];
			instr[0]='s';
			sendCtrlMessage(Short.parseShort(nodeID),instr);
			if(nodeType.equals("Iris")){
				Thread.sleep(4000);
			} else if (nodeType.equals("Telsob")) {
				Thread.sleep(10000);
			}
			if(Integer.parseInt(from)>Integer.parseInt(to)) {
				cache = from;
				from = to;
				to = cache;
			}
			for(short count = Short.parseShort(from); count<=Short.parseShort(to);count++) {
				instr[0]='g';
				instr[1]='r';
				sendCtrlMessage(count,instr);
				Thread.sleep(500);
			}
			for(short count = Short.parseShort(from); count<=Short.parseShort(to);count++) {	
			
				instr[1]='l';
				sendCtrlMessage(count,instr);
				Thread.sleep(500);
			}
			for(short count = Short.parseShort(from); count<=Short.parseShort(to);count++) {	
				instr[1]='v';
				sendCtrlMessage(count,instr);
				Thread.sleep(500);
			}
			if(nodeType.equals("Telosb")) {
				for(short count = Short.parseShort(from); count<=Short.parseShort(to);count++) {	
					instr[1]='t';
					sendCtrlMessage(count,instr);
					Thread.sleep(500);
				}
				for(short count = Short.parseShort(from); count<=Short.parseShort(to);count++) {	
					instr[1]='h';
					sendCtrlMessage(count,instr);
					Thread.sleep(500);
				}
				for(short count = Short.parseShort(from); count<=Short.parseShort(to);count++) {	
					instr[1]='s';
					sendCtrlMessage(count,instr);
					Thread.sleep(500);			
				}
				for(short count = Short.parseShort(from); count<=Short.parseShort(to);count++) {	
					instr[1]='o';
					sendCtrlMessage(count,instr);
					Thread.sleep(500);	
				}
			}
			fileOutput.close();
			printToFile.close();
			out.print("Ready!\n");
		}catch(InterruptedException e){
		}catch(IOException e){}
	}

	public static void main (String[] args)
	{
		String[] temp;
		if (args.length<3)
		{
			out.println("Usage: RSSI [nodeType][controller mote ID][motes which measure data (from-to)]");
		}
		else 
		{
			temp=args[2].split("-");
			RSSI rssitester= new RSSI();
			rssitester.run(args[0],args[1],temp[0],temp[1]);
			System.exit(0);
		}
	}	
}


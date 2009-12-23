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
import java.io.FileOutputStream;
import java.io.PrintStream;
import net.tinyos.message.*;
import net.tinyos.util.*;
import net.tinyos.packet.*;

public class MicStream implements MessageListener {

	private MoteIF moteIF;
	private FileOutputStream fileOutput;
	private FileOutputStream fileOutputBin;
	private PrintStream printToFile;
	private long startTime=0;
	private long endTime=0;
	private long sampleNum=0;
	private long missedPkt=0;
	private String sampleFrq;

		
	public MicStream(String sampleFrq,MoteIF moteIF)
	{
		this.moteIF=moteIF;
		this.moteIF.registerListener(new MicMsg(),this);
		this.moteIF.registerListener(new CtrlMsg(),this);
		this.moteIF.registerListener(new ReadyMsg(),this);
		this.sampleFrq=sampleFrq;
		
	}
	
/*		
	private void dataCapture(MicMsg micmsg,ReadyMsg readymsg,boolean ready)
	{
		
		try{
			if (!ready){
				if (fileOutput==null) {
					fileOutput= new FileOutputStream("micdata_"+sampleFrq+".txt");
				}
				for(int i=0;i<(micmsg.totalSize_micData()/micmsg.elementSize_micData());++i){
					if (micmsg.getElement_micData(i)<=255){
						fileOutput.write(0);
					}else{
						fileOutput.write(micmsg.getElement_micData(i)>>8);
					}
					fileOutput.write(micmsg.getElement_micData(i));
				}
				
			}else{
				out.print("Measurement finished\n");   // A futás vége
				endTime = System.currentTimeMillis();  // A futási idõ mérésének a vége
				out.printf("Frequency: %.2f Hz\n",((1/(double)(readymsg.get_usActualPeriod()))*1000000));
				out.printf("Ellapsed time: %d ms\n",(endTime-startTime));
				fileOutput.close();
			}
			
		}catch(IOException e){
			out.println("Cannot open the file");
		}
	}
*/
	private void dataCapture(MicMsg micmsg,ReadyMsg readymsg,boolean ready)
	{
		try{
			if (!ready){
				if (fileOutput==null && printToFile==null) {
					fileOutput= new FileOutputStream("micdata_"+sampleFrq+".txt");
					fileOutputBin = new FileOutputStream("micadata_"+sampleFrq+".dat");
					printToFile = new PrintStream(fileOutput);
				}
				///////////////////////////////////////////////////////////////////////////////////
				////////// Ellenõrzi hogy megérkezett-e a következõ üzenet ////////////////////////
				////////// Ha nem akkor beleírja a file-ba, hogy az adott  ////////////////////////
				//////////            üzenet nem érkezett meg              ////////////////////////
				///////////////////////////////////////////////////////////////////////////////////
				if(sampleNum==micmsg.get_sampleNum()){
					sampleNum++;
				} else {
					for(long i=sampleNum;i<micmsg.get_sampleNum();++i){
						printToFile.print(i+". packet is lost by the BaseStation"+"\n");
						missedPkt++;
					}
					sampleNum=micmsg.get_sampleNum()+1;
				}
				///////////////////////////////////////////////////////////////////////////////////
				////////// Ha a következõ üzenet érkezett meg akkor azt    ////////////////////////
				//////////              beleírja a file-ba                 ////////////////////////
				///////////////////////////////////////////////////////////////////////////////////
				printToFile.print(micmsg.get_sampleNum()+": ");				
				for(int i=0;i<(micmsg.totalSize_micData()/micmsg.elementSize_micData());++i){
					printToFile.print(micmsg.getElement_micData(i)+" ");
/*					if (micmsg.getElement_micData(i)<=255){
						fileOutputBin.write(0);
					}else{
						fileOutputBin.write(micmsg.getElement_micData(i)>>8);
					}
*/					
					fileOutputBin.write(micmsg.getElement_micData(i));
				}
				printToFile.println();
				
			}else{
				out.print("Measurement finished\n");   // A futás vége
				endTime = System.currentTimeMillis();  // A futási idõ mérésének a vége
				///////////////////////////////////////////////////////////////////////////////////
				////////// Megvizsgálja, hogy az utolsóként vett üzenet   /////////////////////////
				//////////      után volt-e még elveszett üzenet          /////////////////////////
				///////////////////////////////////////////////////////////////////////////////////
				if(readymsg.get_sampleNum()!=sampleNum){
					for(long i=sampleNum;i<=readymsg.get_sampleNum();i++){
						printToFile.print(i+". packet is lost by the BaseStation"+"\n");
						missedPkt++;
					}
				}
				printToFile.println();
//				printToFile.printf("Frequency: %.2f Hz\n",((1/(double)readymsg.get_usActualPeriod())*1000000));				
				printToFile.printf("Frequency: %.2f Hz\n",((1/(((double)readymsg.get_usActualPeriod()*8)/7.3728))*1000000));
				printToFile.printf("The number of packets lost by the BaseStation: %d\n",missedPkt);
				printToFile.printf("The total number of generated packets: %d\n",readymsg.get_bufferDoneNum());
				printToFile.printf("The number of errors under send: %d\n",readymsg.get_sendErrorNum());
				printToFile.printf("The number of errors under sendDone: %d\n",readymsg.get_sendDoneErrorNum());
				printToFile.printf("Ellapsed time: %d ms\n",(endTime-startTime));
				fileOutput.close();
				fileOutputBin.close();
				printToFile.close();
			}
			
		}catch(IOException e){
			out.println("Cannot open the file");
		}
	}

	public void sendCtrlMessage(char instr)  
    {
    	CtrlMsg ctrlmsg=new CtrlMsg();
	
    	ctrlmsg.set_instr((short)instr);
    	ctrlmsg.set_micPeriod(Integer.parseInt(sampleFrq));
		try{
			moteIF.send(MoteIF.TOS_BCAST_ADDR,ctrlmsg);
		}catch(IOException e)
		{
			out.println("Cannot send message to mote ");
		}
		out.print("Measurement started\n");     //A futás kezdete
		startTime = System.currentTimeMillis(); // A futási idõ mérésének a kezdete
	}

	public void messageReceived(int dest_addr,Message msg)
	{
		if (msg instanceof MicMsg) {
			MicMsg mic = (MicMsg)msg;
			dataCapture(mic,null,false);
		}
		else if(msg instanceof ReadyMsg){
			ReadyMsg ready = (ReadyMsg)msg;
			dataCapture(null,ready,true);
			System.exit(1);
		}
	}
	
	public static void main (String[] args) throws Exception {
		PhoenixSource phoenix=BuildSource.makePhoenix("serial@com24:iris", PrintStreamMessenger.err);
        MoteIF mif = new MoteIF(phoenix);
		MicStream mictester= new MicStream(args[0],mif);
		mictester.sendCtrlMessage('s');
	}	
    
    
}
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

import net.tinyos.message.*;
import net.tinyos.util.*;

public class MicStream implements MessageListener {

	private MoteIF mif;
	private FileOutputStream output;
		
	public MicStream()
	{
		mif = new MoteIF(PrintStreamMessenger.err);
		mif.registerListener(new MicMsg(),this);
		mif.registerListener(new CtrlMsg(),this);
		mif.registerListener(new ReadyMsg(),this);
		
	}
	
	protected void finalze()
	{
		mif.deregisterListener(new MicMsg(),this);
		mif.deregisterListener(new CtrlMsg(),this);
		mif.deregisterListener(new ReadyMsg(),this);
	}
		
	private void dataCapture(MicMsg msg,boolean ready)
	{
		
		try{
			if (!ready){
				if (output==null) {
					output= new FileOutputStream("micdata.txt");
				}
				for(int i=0;i<(msg.totalSize_micData()/msg.elementSize_micData());++i){
					if (msg.getElement_micData(i)<=255){
						output.write(0);
					}else{
						output.write(msg.getElement_micData(i)>>8);
					}
					output.write(msg.getElement_micData(i));
				}
				
			}else{
				output.close();
			}
			
		}catch(IOException e){
			out.println("Cannot open the file");
		}
	}

	public void sendCtrlMessage(char instr,int micPeriod)  
    {
    	CtrlMsg ctrlmsg=new CtrlMsg();
	
    	ctrlmsg.set_instr((short)instr);
    	ctrlmsg.set_micPeriod(micPeriod);
		try{
			mif.send(MoteIF.TOS_BCAST_ADDR,ctrlmsg);
		}catch(IOException e)
		{
			out.println("Cannot send message to mote ");
		}
	}

	public void messageReceived(int dest_addr,Message msg)
	{
		if (msg instanceof MicMsg) {
			MicMsg mic = (MicMsg)msg;
			dataCapture(mic,false);
		}
		else if(msg instanceof ReadyMsg){
			ReadyMsg ready = (ReadyMsg)msg;
			out.printf("Frequency: %.2f Hz\n",((1/(double)(ready.get_usActualPeriod()))*1000000));
			dataCapture(null,true);
			System.exit(1);
		}
	}
	
	public static void main (String[] args) throws Exception {
		MicStream mictester= new MicStream();
		mictester.sendCtrlMessage('s',Integer.parseInt(args[0]));
	}	
}
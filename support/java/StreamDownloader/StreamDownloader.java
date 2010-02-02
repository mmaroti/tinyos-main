/*
* Copyright (c) 2009, University of Szeged
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
* Author:Andras Biro
*/
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Date;
import java.util.Timer;
import java.util.TimerTask;
import net.tinyos.message.Message;
import net.tinyos.message.MessageListener;
import net.tinyos.message.MoteIF;
import net.tinyos.packet.BuildSource;
import net.tinyos.packet.PhoenixSource;
import net.tinyos.util.PrintStreamMessenger;


public class StreamDownloader  implements MessageListener{
	private MoteIF moteIF;
	private Timer beacon;
	private byte am_type;
	private ArrayList<dataFile> files=new ArrayList<dataFile>();
	private static final int MAX_PAYLOAD=28;
	  
	  public StreamDownloader(String source,byte amtype) {
		  	PhoenixSource phoenix;
		  	am_type=amtype;
			if (source == null) {
				phoenix = BuildSource.makePhoenix(PrintStreamMessenger.err);
			} else {
				phoenix = BuildSource.makePhoenix(source, PrintStreamMessenger.err);
			}
			this.moteIF = new MoteIF(phoenix);
			this.moteIF.registerListener(new dataMsg(amtype), this);
			this.moteIF.registerListener(new respMsg(amtype), this);
	  }

	  public void messageReceived(int to, Message message) {
		  System.out.println("message received");
		  if (message instanceof dataMsg&&message.dataLength()==MAX_PAYLOAD){
			  System.out.println("Data message received");
			  dataMsg msg = (dataMsg) message;
			  dataFile currentFile=null;
			  for(int i=0;i<files.size();i++){
				  if(files.get(i).nodeid==msg.getSerialPacket().get_header_src()){
					  currentFile=files.get(i);
					  break;
				  }
			  }
			  if(currentFile!=null){
				  try {
					FileOutputStream fout=new FileOutputStream(currentFile.file,true);
					fout.write(msg.dataGet());
					fout.flush();
					fout.close();
					currentFile.maxaddress+=msg.dataLength();
				} catch (FileNotFoundException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				} catch (IOException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
				  
			  }			  
		  } else if (message instanceof last_dataMsg&&message.dataLength()==MAX_PAYLOAD-1){
			  System.out.println("Last data message received");
			  last_dataMsg msg = (last_dataMsg) message;
			  dataFile currentFile=null;
			  for(int i=0;i<files.size();i++){
				  if(files.get(i).nodeid==msg.getSerialPacket().get_header_src()){
					  currentFile=files.get(i);
					  break;
				  }
			  }
			  if(currentFile!=null){
				  try {
					FileOutputStream fout=new FileOutputStream(currentFile.file,true);
					fout.write(msg.dataGet());
					fout.flush();
					fout.close();
					currentFile.maxaddress+=msg.get_len();
				} catch (FileNotFoundException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				} catch (IOException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
				  
			  }			  
		  } else if (message instanceof respMsg&&message.dataLength()==12){
			  System.out.println("Resp message received");
			  respMsg msg = (respMsg) message;
			  dataFile currentFile=null;
			  for(int i=0;i<files.size();i++){
				  if(files.get(i).nodeid==msg.get_src()){
					  currentFile=files.get(i);
					  break;
				  }
			  }
			  if(currentFile==null){
				  currentFile=new dataFile();
				  currentFile.maxaddress=0;
				  currentFile.nodeid=msg.get_src();
				  currentFile.file=new File(String.valueOf(currentFile.nodeid));	  
			  }
			  respMsg response = new respMsg(am_type);
			  response.set_dest(currentFile.nodeid);
			  response.set_src(0);
			  response.set_max_address(msg.get_max_address());
			  if(msg.get_min_address()<currentFile.maxaddress)
				  response.set_min_address(currentFile.maxaddress);
			  else
				  response.set_min_address(msg.get_min_address());
			  try {
				moteIF.send(currentFile.nodeid, response);
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		  } 
	  }
	  
	  public void startHello(long interval){
		  beacon = new Timer();
		  beacon.schedule(new sayHello(),new Date(), interval * 1000);
	  }
	  
	  public void stopHello(){
		  beacon.cancel();
	  }
	  
	  private class sayHello extends TimerTask {
		    public void run() {
		    	System.out.println("hello message sent");
		    	helloMsg hello=new helloMsg(am_type);
				
				try {
					moteIF.send(0xffff, hello);
				} catch (IOException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}	
		    }
	  }
	  
	  private class dataFile{
			File file;
			long minaddress,maxaddress;
			int nodeid;
	  }
	  
	  public static void main(String[] args) throws Exception {
	    String source = null;
	    if (args.length == 2) {
	      if (args[0].equals("-comm")) {
	    	  source = args[1];
	      }
	    }
	    
	    StreamDownloader sd=new StreamDownloader(source,(byte) 10);
	    sd.startHello(10);
	  }
	

}

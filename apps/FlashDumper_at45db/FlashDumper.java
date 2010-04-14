/*
* Copyright (c) 2010, University of Szeged
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
import java.io.IOException;
import java.util.ArrayList;
import java.util.Currency;

import net.tinyos.message.Message;
import net.tinyos.message.MessageListener;
import net.tinyos.message.MoteIF;
import net.tinyos.packet.BuildSource;
import net.tinyos.packet.PhoenixSource;
import net.tinyos.util.PrintStreamMessenger;


public class FlashDumper implements MessageListener{

	private MoteIF moteIF;
	private int endpage;
	private ArrayList<Page> pages = new ArrayList<Page>();

	public FlashDumper(String source,int startpage, int endpage){
		this.endpage=endpage;
		PhoenixSource phoenix;
		if (source == null) {
			phoenix = BuildSource.makePhoenix(PrintStreamMessenger.err);
		} else {
			phoenix = BuildSource.makePhoenix(source, PrintStreamMessenger.err);
		}
		this.moteIF = new MoteIF(phoenix);
		this.moteIF.registerListener(new dataMsg(), this);
		this.moteIF.registerListener(new ctrlMsg(), this);
		ctrlMsg req = new ctrlMsg();
		req.set_startpage(startpage);
		req.set_endpage(endpage);
		try {
			moteIF.send(MoteIF.TOS_BCAST_ADDR, req);
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
	
	public void messageReceived(int to, Message message) {
		if (message instanceof dataMsg && message.dataLength() == dataMsg.DEFAULT_MESSAGE_SIZE) {
			dataMsg msg = (dataMsg) message;
			Page currentPage=null;
			for(Page p:pages){
				if(p.number==msg.get_page()){
					currentPage=p;
					break;
				}
			}
			if(currentPage==null){
				currentPage=new Page(msg.get_page());
				pages.add(currentPage);
				
			}
			if(msg.get_offset()==65535){
				currentPage.print();
				if(msg.get_page()==endpage)
					System.exit(0);
			} else if(msg.get_offset()+dataMsg.numElements_data()>=currentPage.data.length){
				int j=0;
				for(int i=msg.get_offset();i<currentPage.data.length;i++){
					currentPage.data[i]=msg.getElement_data(j);
					j++;
				}
				
			} else{
				int j=0;
				for(int i=msg.get_offset();i<msg.get_offset()+dataMsg.numElements_data();i++){
					currentPage.data[i]=msg.getElement_data(j);
					j++;
				}
			}
		} else if (message instanceof ctrlMsg && message.dataLength() == ctrlMsg.DEFAULT_MESSAGE_SIZE) {
			ctrlMsg msg = (ctrlMsg) message;
			Page currentPage=null;
			for(Page p:pages){
				if(p.number==msg.get_startpage()){
					currentPage=p;
					break;
				}
			}
			if(currentPage==null){
				currentPage=new Page(msg.get_startpage());
				pages.add(currentPage);
			}
			currentPage.valid=true;
			currentPage.size=msg.get_endpage();
			currentPage.print();			
			if(msg.get_startpage()==endpage)
				System.exit(0);
			
		}
		
	}
	
	private class Page{
		short[] data;
		int number;
		boolean valid;
		short bytesinline=16; 
		int size;
		
		public Page(int number){
			this.number=number;
			data=new short[264];
			valid=false;
			size=data.length;
		}
		
		private String printInt(int sh,int len){
			String ret="";
			if(sh>Math.pow(16,len))
				return null;
			for(int i=16;i<Math.pow(16, len);i*=16){
				if(sh<i)
					ret+="0";
			}
			ret+=Integer.toHexString(sh);
			return ret;
		}
		public void print(){
			if(valid){
				System.out.println("Page Nr"+number);
				System.out.print(printInt(0,3)+"|");
				for(int i=0;i<size;i++){
					System.out.print(printInt(data[i],2)+" ");
					if((i+1)%8==0)
						System.out.print("| ");
					if((i+1)%bytesinline==0){
						System.out.println();
						System.out.print(printInt(i,3)+"|");
					}
				}
				System.out.println();
			} else {
				System.out.println("Page Nr"+number+" read error");
			}
		}
	}
	
	public static void main(String[] args) throws Exception {
		if (args.length==2) {
			new FlashDumper(null,Integer.parseInt(args[0]), Integer.parseInt(args[1]));
		} else	if (args.length==4) {
			if (args[0].equals("-comm")) {
				new FlashDumper(args[1],Integer.parseInt(args[2]), Integer.parseInt(args[3]));
			}
			if (args[2].equals("-comm")) {
				new FlashDumper(args[3],Integer.parseInt(args[0]), Integer.parseInt(args[1]));
			}
		} else
			System.err.println("Usage: FlashDumper <startpage> <endpage> [-comm MOTECOM]");
		
		
	}

}

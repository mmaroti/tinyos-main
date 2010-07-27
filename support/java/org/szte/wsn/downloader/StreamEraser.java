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

package org.szte.wsn.downloader;

import java.io.IOException;
import java.io.File;
import net.tinyos.message.Message;
import net.tinyos.message.MessageListener;
import net.tinyos.message.MoteIF;
import net.tinyos.packet.BuildSource;
import net.tinyos.packet.PhoenixSource;
import net.tinyos.util.PrintStreamMessenger;


public class StreamEraser implements MessageListener {
	private MoteIF moteIF;
	private int nodeid;
	public static final int ALL_NODE=0xffff;
	public static final int FIRST_NODE=0xffff+1;
	//private ArrayList<dataFile> files = new ArrayList<dataFile>();
	
	public StreamEraser(String source, int nodeid) {
		PhoenixSource phoenix;
		this.nodeid=nodeid;
		if (source == null) {
			phoenix = BuildSource.makePhoenix(PrintStreamMessenger.err);
		} else {
			phoenix = BuildSource.makePhoenix(source, PrintStreamMessenger.err);
		}
		this.moteIF = new MoteIF(phoenix);
		this.moteIF.registerListener(new ctrltsMsg(), this);
		if(nodeid!=FIRST_NODE&&nodeid!=ALL_NODE)
			System.out.println("Waiting for node #"+nodeid);
		else if(nodeid==FIRST_NODE)
			System.out.println("Waiting for first node");
		else if(nodeid==ALL_NODE)
			System.out.println("Waiting for nodes");
	}

	public void messageReceived(int to, Message message) {
		if (message instanceof ctrltsMsg && message.dataLength() == ctrltsMsg.DEFAULT_MESSAGE_SIZE) {
			ctrltsMsg msg = (ctrltsMsg) message;
			if(nodeid==FIRST_NODE||nodeid==ALL_NODE||msg.getSerialPacket().get_header_src()==nodeid){
				if(msg.get_max_address()-msg.get_min_address()>200){
					System.out.println("Found node #"+msg.getSerialPacket().get_header_src()+" data:"+ (msg.get_max_address()-msg.get_min_address())+" , sending erase command");
					if(nodeid==FIRST_NODE)
						nodeid=msg.getSerialPacket().get_header_src();
					ctrlMsg response = new ctrlMsg();
					response.set_min_address(0);
					response.set_max_address(0);
					try {
						moteIF.send(msg.getSerialPacket().get_header_src(), response);
						System.out.println("Deleting local files");
						File bin=new File(dataWriter.nodeidToPath(nodeid, ".bin"));
						File ts=new File(dataWriter.nodeidToPath(nodeid, ".gap"));
						File gap=new File(dataWriter.nodeidToPath(nodeid, ".ts"));
						if(bin.exists()){
							bin.delete();
							System.out.println(bin.getPath()+" deleted");
						} 
						if(gap.exists()){
							gap.delete();
							System.out.println(gap.getPath()+" deleted");
						} 
						if(ts.exists()){
							ts.delete();
							System.out.println(ts.getPath()+" deleted");
						} 
//						if(nodeid!=ALL_NODE)
//							cmdSent=true;
					} catch (IOException e) {
						// TODO Auto-generated catch block
						e.printStackTrace();
					}
				} else {
					System.out.print("New message from node#"+msg.getSerialPacket().get_header_src()+": ");
					System.out.println("MinAddress: " + msg.get_min_address()+" MaxAddress: "+msg.get_max_address());
					if(nodeid!=ALL_NODE)
						System.exit(0);
				}
			}
		}
	}
	
	private static void usage() {
		System.out.println("Usage: StreamEraser <NODEID> [-comm port]");
		System.out.println("Special NODEIDs:");
		System.out.println("	all:	Delete all nodes while the program is running");
		System.out.println("	first:	Delete the first node");
		System.exit(1);
	}
	
	public static void main(String[] args) throws Exception {
		String source = "sf@localhost:9002";
		if (args.length == 3) {
			if (args[1].equals("-comm")) {
				source = args[2];
			} else
				usage();
		}
		int node_id=0;
		try{
			node_id=Integer.valueOf(args[0]);
		}catch(NumberFormatException e){
			if(args[0].equals("all"))
				node_id=ALL_NODE;
			else if(args[0].equals("first"))
				node_id=FIRST_NODE;
			else{
				usage();
			}
		}

		new StreamEraser(source, node_id);
	}
}

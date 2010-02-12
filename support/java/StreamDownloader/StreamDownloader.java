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
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.channels.FileChannel;
import java.util.ArrayList;
import net.tinyos.message.Message;
import net.tinyos.message.MessageListener;
import net.tinyos.message.MoteIF;
import net.tinyos.packet.BuildSource;
import net.tinyos.packet.PhoenixSource;
import net.tinyos.util.PrintStreamMessenger;

public class StreamDownloader implements MessageListener {
	private MoteIF moteIF;
	private byte am_type;
	private ArrayList<dataFile> files = new ArrayList<dataFile>();
	
	

	public StreamDownloader(String source, byte amtype) {
		PhoenixSource phoenix;
		am_type = amtype;
		if (source == null) {
			phoenix = BuildSource.makePhoenix(PrintStreamMessenger.err);
		} else {
			phoenix = BuildSource.makePhoenix(source, PrintStreamMessenger.err);
		}
		this.moteIF = new MoteIF(phoenix);
		this.moteIF.registerListener(new dataMsg(am_type), this);
		this.moteIF.registerListener(new ctrlMsg(am_type), this);
	}

	public void messageReceived(int to, Message message) {
		//System.out.println("message received");
		if (message instanceof ctrlMsg && message.dataLength() == ctrlMsg.DEFAULT_MESSAGE_SIZE) {
			ctrlMsg msg = (ctrlMsg) message;
			System.out.println("Ctrl message received min:"+msg.get_min_address()+" max:"+msg.get_max_address());
			dataFile currentFile = null;
			for (int i = 0; i < files.size(); i++) {
				if (files.get(i).nodeid == msg.getSerialPacket().get_header_src()) {
					currentFile = files.get(i);
					break;
				}
			}
			if (currentFile == null) {
				currentFile = new dataFile(msg.getSerialPacket().get_header_src());
				
				files.add(currentFile);
			}
			ctrlMsg response = new ctrlMsg(am_type);
			response.set_max_address(msg.get_max_address());
			if (msg.get_min_address() <= currentFile.maxaddress+1)
				response.set_min_address(currentFile.maxaddress+1);
			else {
				response.set_min_address(msg.get_min_address());
				currentFile.addGap(currentFile.maxaddress + 1, msg
						.get_min_address() - 1);
			}
			try {
				moteIF.send(currentFile.nodeid, response);
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		} else if (message instanceof dataMsg&& message.dataLength() == dataMsg.DEFAULT_MESSAGE_SIZE) {
			dataMsg msg = (dataMsg) message;
			System.out.print("Data received from "+msg.getSerialPacket().get_header_src()+" address: "+msg.get_address()+"|");
			dataFile currentFile = null;
			for (int i = 0; i < files.size(); i++) {
				if (files.get(i).nodeid == msg.getSerialPacket().get_header_src()) {
					currentFile = files.get(i);
					break;
				}
			}
			if (currentFile != null) {
				try {
					FileOutputStream fout = new FileOutputStream(currentFile.dataFile, true);
					FileChannel fc=(FileChannel)fout.getChannel();
					ByteBuffer bb=ByteBuffer.wrap(msg.get_data());
					fc.position(msg.get_address());
					fc.write(bb);
					fout.flush();
					fout.close();
					if(msg.get_address()==currentFile.maxaddress+1){//the next bytes
						System.out.println("Data OK");
						currentFile.maxaddress += dataMsg.totalSize_data();
					} else if(msg.get_address()>currentFile.maxaddress+1){//we missed some data
						System.out.println("New gap: " + (currentFile.maxaddress+1) + "-" + (msg.get_address()-1));
						currentFile.addGap(currentFile.maxaddress+1, msg.get_address()-1);
						currentFile.maxaddress = msg.get_address() + dataMsg.totalSize_data();
					} else { //we fill a gap
						for(int i=0;i<currentFile.getGapNumber();i++){
							currentFile.getGap(i);
							if(!currentFile.getGap(i).unrepairable){
								if((currentFile.getGap(i).start<msg.get_address()+dataMsg.totalSize_data())||(currentFile.getGap(i).end>=msg.get_address())){
									long start,end;
									if(currentFile.getGap(i).start<msg.get_address()+dataMsg.totalSize_data()){
										start=msg.get_address()+dataMsg.totalSize_data()-1;
									} else
										start=currentFile.getGap(i).start;
									if(currentFile.getGap(i).end>=msg.get_address()){
										end=msg.get_address();
									} else
										end=currentFile.getGap(i).end;
									System.out.print("Remove gap: " + currentFile.getGap(i).start+"-"+currentFile.getGap(i).end);
									currentFile.removeGap(i);
									if(end>start){//we didn't fill the whole gap
										System.out.print("New gap: " + (currentFile.maxaddress+1) + "-" + (msg.get_address()-1));
										currentFile.addGap(start, end);
									}
									System.out.print("\n");
								}
							}
						}
					}
				} catch (FileNotFoundException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				} catch (IOException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
			} 
		} else if (message instanceof complMsg&& message.dataLength() == complMsg.DEFAULT_MESSAGE_SIZE) {
			complMsg msg = (complMsg) message;
			dataFile currentFile = null;
			for (int i = 0; i < files.size(); i++) {
				if (files.get(i).nodeid == msg.getSerialPacket().get_header_src()) {
					currentFile = files.get(i);
					break;
				}
			}
			if (currentFile != null){
				Long[] rep;
				rep = currentFile.repairGap(msg.get_newminaddress());
				if(rep[0]!=rep[1]){
					ctrlMsg response = new ctrlMsg(am_type);
					response.set_min_address(rep[0]);
					response.set_max_address(rep[1]);
					try {
						moteIF.send(currentFile.nodeid, response);
					} catch (IOException e) {
						// TODO Auto-generated catch block
						e.printStackTrace();
					}
				}
			}
		}
	}

	public static void main(String[] args) throws Exception {
		String source = null;
		if (args.length == 2) {
			if (args[0].equals("-comm")) {
				source = args[1];
			}
		}

		new StreamDownloader(source, (byte) 10);
	}

}

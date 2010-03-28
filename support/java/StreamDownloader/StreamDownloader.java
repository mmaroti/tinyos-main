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
import java.io.FileNotFoundException;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Date;

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
	private int listenonly;
	public static final int MIN_DOWNLOAD_SIZE=dataMsg.numElements_data()*4;
	
	

	public StreamDownloader(int listenonly, String source, byte amtype) {
		PhoenixSource phoenix;
		am_type = amtype;
		this.listenonly=listenonly;
		if (source == null) {
			phoenix = BuildSource.makePhoenix(PrintStreamMessenger.err);
		} else {
			phoenix = BuildSource.makePhoenix(source, PrintStreamMessenger.err);
		}
		this.moteIF = new MoteIF(phoenix);
		this.moteIF.registerListener(new dataMsg(am_type), this);
		this.moteIF.registerListener(new ctrlMsgTS((byte) (am_type+1)), this);
	}

	public void messageReceived(int to, Message message) {
		if(listenonly<0||message.getSerialPacket().get_header_src()==listenonly){
			if (message instanceof ctrlMsgTS && message.dataLength() == ctrlMsgTS.DEFAULT_MESSAGE_SIZE) {
				long received_t=(new Date()).getTime();
				ctrlMsgTS msg = (ctrlMsgTS) message;
				System.out.println("Ctrl message received from #"+msg.getSerialPacket().get_header_src()+" min:"+msg.get_min_address()+" max:"+msg.get_max_address()+" timestamp:"+(Long)(msg.get_localtime()+msg.get_timestamp()));
				dataFile currentFile = null;
				for (int i = 0; i < files.size(); i++) {
					if (files.get(i).nodeid == msg.getSerialPacket().get_header_src()) {
						currentFile = files.get(i);
						break;
					}
				}
				if (currentFile == null) {
					try {
						currentFile = new dataFile(msg.getSerialPacket().get_header_src());
						files.add(currentFile);
					} catch (IOException e) {
						System.err.println("Can't read gapfile for node #"+msg.getSerialPacket().get_header_src()+" data won't be downloaded from there");
						currentFile=null;
					}
				}
				currentFile.addTimeStamp(received_t, msg.get_localtime()+msg.get_timestamp());
				if(msg.get_max_address()-currentFile.maxaddress>=MIN_DOWNLOAD_SIZE){
					ctrlMsg response = new ctrlMsg(am_type);
					response.set_max_address(msg.get_max_address());
					if (msg.get_min_address() <= currentFile.maxaddress+1)
						response.set_min_address(currentFile.maxaddress+1);
					else {
						response.set_min_address(msg.get_min_address());
					}
					try {
						moteIF.send(currentFile.nodeid, response);
					} catch (IOException e) {
						// TODO Auto-generated catch block
						e.printStackTrace();
					}
				} else {//if we don't have to download new data, we try to fill a gap
					Long[] rep;
					rep = currentFile.repairGap(msg.get_min_address());
					if(rep[0]<rep[1]){
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
			} else if (message instanceof dataMsg && message.dataLength() == dataMsg.DEFAULT_MESSAGE_SIZE) {
				dataMsg msg = (dataMsg) message;
	//			if(msg.get_address()<100)
	//				return;
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
						currentFile.dataFile.seek(msg.get_address());
						currentFile.dataFile.write(msg.get_data(), 0, msg.get_length());
						if(msg.get_address()==currentFile.maxaddress+1){//the next bytes
							System.out.println("Data OK");
							currentFile.maxaddress += msg.get_length();
						} else if(msg.get_address()>currentFile.maxaddress+1){//we missed some data
							System.out.println("New gap: " + (currentFile.maxaddress+1) + "-" + (msg.get_address()-1));
							currentFile.addGap(currentFile.maxaddress+1, msg.get_address()-1);
							currentFile.maxaddress = msg.get_address() + msg.get_length()-1;
						} else { //we fill a gap
							for(int i=0;i<currentFile.getGapNumber();i++){
								currentFile.getGap(i);
								if(!currentFile.getGap(i).unrepairable){
									if(((currentFile.getGap(i).start<msg.get_address()+msg.get_length())&&(currentFile.getGap(i).start>=msg.get_address()))||
										((currentFile.getGap(i).end>=msg.get_address())&&(currentFile.getGap(i).end<msg.get_address()+msg.get_length()))){
										long start_bef,end_bef,start_aft,end_aft;
										start_bef=currentFile.getGap(i).start;
										end_bef=msg.get_address()-1;
										start_aft=msg.get_address()+msg.get_length();
										end_aft=currentFile.getGap(i).end;
										System.out.print("Remove gap: " + currentFile.getGap(i).start+"-"+currentFile.getGap(i).end+"|");
										currentFile.removeGap(i);
										if(end_bef>start_bef){//we didn't fill the whole gap
											System.out.print("New gap: " + start_bef + "-" + end_bef+"|");
											currentFile.addGap(start_bef, end_bef);
										}
										if(end_aft>start_aft){//we didn't fill the whole gap
											System.out.print("New gap: " + start_aft + "-" + end_aft+"|");
											currentFile.addGap(start_aft, end_aft);
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
			}
		}
	}

	public static void main(String[] args) throws Exception {
		String source = null;
		int listenonly=-1;
		if (args.length == 1) {
			listenonly=Integer.parseInt(args[0]);
		} else if (args.length == 2) {
			if (args[0].equals("-comm")) {
				source = args[1];
			}
		} else if (args.length == 2) {
			if (args[0].equals("-comm")) {
				source = args[1];
				listenonly=Integer.parseInt(args[2]);
			} else if (args[1].equals("-comm")) {
				source = args[2];
				listenonly=Integer.parseInt(args[0]);
			} 
		}

		new StreamDownloader(listenonly,source, (byte) 10);
	}

}

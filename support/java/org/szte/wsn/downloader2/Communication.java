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
package org.szte.wsn.downloader2;

import java.io.IOException;
import java.util.Date;
import java.util.HashSet;
import java.util.Timer;
import java.util.TimerTask;

import net.tinyos.message.Message;
import net.tinyos.message.MessageListener;
import net.tinyos.message.MoteIF;
import net.tinyos.packet.BuildSource;
import net.tinyos.packet.PhoenixSource;
import net.tinyos.util.PrintStreamMessenger;

public class Communication  implements MessageListener {

	private Timer timer=new Timer();
	//private TimerTask startdownload=null;
	//private TimerTask pingtask=null;
	
	private MoteIF moteIF;
	private PhoenixSource phoenix;
	private StreamDownloader sd;
	private short seqnum=0;
	private final int M_NOTHING=0;
	private final int M_DISCOVER=1;
	private final int M_AUTODOWNLOAD=2;
	private int mode=M_AUTODOWNLOAD;
	private HashSet<Integer> motes=new HashSet<Integer>();

	public HashSet<Integer> getMotes() {
		return motes;
	}

	private boolean verbose=true;
	
	private void exception(IOException e){
		System.err.println("Communication error, can't send message. Exiting");
		e.printStackTrace();
		System.exit(1);
	}
	
	public final class Discover extends TimerTask {

		@Override
		public void run() {
			mode=M_NOTHING;
			sd.discoveryComplate(motes);
		}
		
	}
	
	@Override
	public void messageReceived(int to, Message m) {
		if(m instanceof CtrlMsg){
			if(mode==M_DISCOVER){
				motes.add(m.getSerialPacket().get_header_src());
			}
		} else if(m instanceof DataMsg){
			DataMsg rec=(DataMsg)m;
			byte[] data=rec.get_payload();
			sd.newData(rec.get_source(),rec.get_address(),data);
		} else if(m instanceof TimeMsg){
			TimeMsg rec=(TimeMsg)m;
			sd.newTimeSync(rec.get_remoteTime()+rec.get_sendTime()-rec.get_localTime(),
					rec.get_bootCount(), new Date().getTime(),m.getSerialPacket().get_header_src());
		}
	}
	
	public void sendGet(int nodeID, long minaddress, long maxaddress) throws IOException{
		GetMsg get=new GetMsg();
		get.set_nodeid(nodeID);
		get.set_min_address(minaddress);
		get.set_max_address(maxaddress);
		get.set_seq_num(++seqnum);
		moteIF.send(MoteIF.TOS_BCAST_ADDR, get);
	}
	
	private void sendCommnad(short command){
		CommandMsg cmd=new CommandMsg();
		cmd.set_cmd(command);
		try {
			moteIF.send(MoteIF.TOS_BCAST_ADDR, cmd);
		} catch (IOException e) {
			exception(e);
		}
	}

	public void sendPing(){
		sendCommnad(CommandMsg.COMMAND_PING);
		if(verbose)
			System.out.println("Ping sent");
	}
	
	public void sendErase(){
		try {
			sendGet(MoteIF.TOS_BCAST_ADDR, 1, 1);
		} catch (IOException e) {
			exception(e);
		}
	}
	
	public void sendErase(int nodeid){
		try {
			sendGet(nodeid, 1, 1);
		} catch (IOException e) {
			exception(e);
		}
	}
	
	
	public Communication(StreamDownloader sd,String source){
		this.sd=sd;
		if (source == null) {
			phoenix = BuildSource.makePhoenix(PrintStreamMessenger.err);
		} else {
			phoenix = BuildSource.makePhoenix(source, PrintStreamMessenger.err);
		}
		this.moteIF = new MoteIF(phoenix);
		this.moteIF.registerListener(new DataMsg(), this);
		this.moteIF.registerListener(new CtrlMsg(), this);
		this.moteIF.registerListener(new TimeMsg(), this);
	}
	
	public void stop(){
		timer.cancel();
		phoenix.shutdown();
	}

	public void discover(HashSet<Integer> pastmotes){
		if(pastmotes==null)
			motes.clear();
		else
			motes=pastmotes;
		if(verbose)
			System.out.println("Starting discovery");
		mode=M_DISCOVER;
		timer.schedule(new Discover(), 5000);
		sendPing();	
	}

	public void stopSending(int nodeid) {
		// TODO Auto-generated method stub
		
	}

}

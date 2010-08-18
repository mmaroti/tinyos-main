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
import java.util.ArrayList;
import java.util.Date;
import java.util.HashSet;
import java.util.Timer;
import java.util.TimerTask;

import org.szte.wsn.dataprocess.file.Gap;


public class StreamDownloader{
	
	private static final int MIN_DOWNLOAD_SIZE=DataMsg.numElements_data()*4;
	
	private Communication communication;
	private int listenonly,maxnode,timeout,pongwait;
	private ArrayList<DataWriter> writers = new ArrayList<DataWriter>();
	private ArrayList<Pong> pongs=new ArrayList<Pong>();
	private HashSet<Integer> currently_handled = new HashSet<Integer>();
	private Timer timer=new Timer();
	private TimerTask startdownload=new StartDownload(pongs);
	
	private final class Pong{
		private long minaddress, maxaddress;
		private int nodeid;
		
		public Pong(int nodeid,long minaddress,long maxaddress) {
			this.minaddress=minaddress;
			this.maxaddress=maxaddress;
			this.nodeid=nodeid;
		}
		
		public long getMinAddress(){
			return minaddress;
		}
		
		public long getMaxAddress(){
			return maxaddress;
		}
		
		public int getNodeID(){
			return nodeid;
		}
	}
	
	public final class StartDownload extends TimerTask{
		ArrayList<Pong> pongs;
		
		public StartDownload(ArrayList<Pong> pongs){
			this.pongs=pongs;
		}

		@Override
		public void run() {
			Pong maxdownloadPong=null;
			long maxdownload=Long.MIN_VALUE;
			//TODO: gap javítás
			if(currently_handled.size()<=maxnode){
				for(Pong p:pongs){
					if(!currently_handled.contains(p.getNodeID())){
						try {
							DataWriter currentwriter=StreamDownloader.getWriter(p.getNodeID(), writers);
							if(currentwriter!=null){
								long download;
								Gap repair=currentwriter.repairGap(p.minaddress);
								if(repair!=null){
									download=Long.MAX_VALUE;
									maxdownloadPong=p;
									break;
								}
								if(p.getMinAddress()<currentwriter.getMaxAddress()){
									download=p.getMaxAddress()-currentwriter.getMaxAddress();
								}else{
									download=p.getMaxAddress()-p.getMinAddress();
								}
								if(download>maxdownload){
									maxdownload=download;
									maxdownloadPong=p;
								}
							} else {
								if(p.maxaddress-p.minaddress>maxdownload){
									maxdownload=p.maxaddress-p.minaddress;
									maxdownloadPong=p;
								}
							}
						} catch (IOException e) {
							System.err.println("Error: Can't read file "+DataWriter.nodeidToPath(p.getNodeID(),".bin"));
						}
					}
				}
				if(maxdownload>MIN_DOWNLOAD_SIZE&&maxdownloadPong!=null){
					DataWriter maxdownloadWriter=getWriter(maxdownloadPong.getNodeID(), writers);
					if(maxdownloadWriter==null){
						maxdownloadWriter=new DataWriter(maxdownloadPong.getNodeID());
						writers.add(maxdownloadWriter);
					}
					maxdownloadWriter.setLastModified();
					currently_handled.add(maxdownloadPong.getNodeID());
					if(maxdownload==Long.MAX_VALUE){
						Gap repair=maxdownloadWriter.repairGap(maxdownloadPong.minaddress);
						try{
							communication.sendGet(maxdownloadWriter.getNodeid(), repair.getStart(), repair.getEnd());
						} 
						catch(IOException e){
							System.err.println("Warning: Can't send get message");
						}
					} else {
						long minaddress;
						try {
							minaddress = (maxdownloadPong.minaddress>maxdownloadWriter.getMaxAddress())?maxdownloadPong.minaddress:maxdownloadWriter.getMaxAddress();
							try{
								communication.sendGet(maxdownloadWriter.getNodeid(), minaddress, maxdownloadPong.maxaddress);
							} 
							catch(IOException e){
								System.err.println("Warning: Can't send get message");
							}
						}catch (IOException e) {
							System.err.println("Error: Can't read file "+DataWriter.nodeidToPath(maxdownloadPong.getNodeID(),".bin"));
						}
					}
				}
			}
			pongs.clear();
		}
	}
	
	public final class ClearHandled extends TimerTask {
		public void run() {
			long now=(new Date()).getTime();
			HashSet<Integer> remove=new HashSet<Integer>();
			for(Integer i:currently_handled){
				if(writers.get(i).getLastModified()+timeout<now)
					remove.add(i);
			}
			currently_handled.removeAll(remove);
		}
	}
	
	public final class Ping extends TimerTask {
		public void run() {
			try {
				communication.sendPing();
				timer.schedule(startdownload, pongwait);
			} catch (IOException e) {
				System.err.println("Warning: Can't send ping");
			}
		}
	}
	
	public static DataWriter getWriter(int nodeid, ArrayList<DataWriter> datawriters ){
		for(int i=0;i<datawriters.size();i++){
			if(datawriters.get(i).getNodeid()==nodeid){
				return datawriters.get(i);
			}
		}
		return null;
		
	}
	public StreamDownloader(int listenonly, int maxnode,int pongwait, int pinginterval, int timeout, String source) {
		this.listenonly=listenonly;
		this.maxnode=maxnode;
		this.timeout=timeout;
		this.pongwait=pongwait;
		communication=new Communication(this, source);
		TimerTask ping  = new Ping();
		TimerTask ch  = new ClearHandled();
	    timer.scheduleAtFixedRate(ping, 0, pinginterval*1000);
	    timer.scheduleAtFixedRate(ch, timeout*1000, timeout*1000);
	}
	
	public class RunWhenShuttingDown extends Thread {
        public void run() {
            System.out.println("Closing files");
            for(DataWriter i:writers){
            	try {
					i.close();
				} catch (IOException e) {
					System.err.println("Error: Can't close file "+DataWriter.nodeidToPath(i.getNodeid(),".bin"));
				}
            }
        }
    }

	public void newData(int nodeid, long address, byte[] data) {
		DataWriter writer=getWriter(nodeid, writers);
		if(writer==null){//create a new file
			writer=new DataWriter(nodeid);
			writers.add(writer);
		}
		
		try {//write
			String output=writer.writeData(address, data);
			System.out.println(output);
		} catch (IOException e) {
			System.err.println("Error: Can't write file "+DataWriter.nodeidToPath(writer.getNodeid(),".bin"));
		}
	}

	public void newPong(int nodeid, long min_address, long max_address) {
		if(listenonly<0||listenonly==nodeid){
			if(new Date().getTime()-startdownload.scheduledExecutionTime()<=pongwait)
				pongs.add(new Pong(nodeid, min_address, max_address));
			else if(currently_handled.contains(nodeid)){
				pongs.add(new Pong(nodeid, min_address, max_address));
				startdownload.run();
			}else
				System.err.println("Warning: Unhandled pong from "+nodeid+" (timeout)");
		} else
			System.out.println("Unhandled pong from "+nodeid);
	}
	
	public static void usage(){
		System.out.println("Usage: StreamDownloader [options]");
		System.out.println("Options: ");
		System.out.println("-comm <port>: Listen on <port>. Default: MOTECOMM");
		System.out.println("-only <id>: Only download mote Nr. <id>");
		System.out.println("-max <number>: Maximum <number> motes will be handled together. Default: 3");
		System.out.println("-pinginterval <number>: Ping interval in seconds. Default: 10");
		System.out.println("-timeout <number>: Download timeout in seconds. Default: 10");
		System.out.println("-pongwait <number>: the program waits <number> seconds after ping for pongs. Default: 3");
	}

	public static void main(String[] args) throws Exception {
		String source = "sf@localhost:9002";
		int listenonly=-1, maxnode=1, timeout=10,pinginterval=10,pongwait=3;
		if (args.length == 0||args.length == 2||args.length == 4||args.length == 6) {
			for(int i=0;i<args.length;i+=2){
				if (args[i].equals("-comm")) {
					source = args[i+1];
				}
				if (args[i].equals("-only")) {
					listenonly = Integer.parseInt(args[i+1]);
				}
				if (args[i].equals("-max")) {
					maxnode = Integer.parseInt(args[i+1]);
				}
				if (args[i].equals("-timeout")) {
					timeout = Integer.parseInt(args[i+1]);
				}
				if (args[i].equals("-pinginterval")) {
					pinginterval = Integer.parseInt(args[i+1]);
				}
				if (args[i].equals("-pinginterval")) {
					pongwait = Integer.parseInt(args[i+1]);
				}
			}
		} else {
			StreamDownloader.usage();
		}
		new StreamDownloader(listenonly, maxnode, pinginterval,pongwait, timeout, source);
	}
	
}

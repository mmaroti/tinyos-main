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
import java.util.Timer;
import java.util.TimerTask;

import org.szte.wsn.dataprocess.file.Gap;


public class StreamDownloader{
	
	private static final int MIN_DOWNLOAD_SIZE=DataMsg.numElements_payload()*4;
	private static final int NONE=0xffff;
	private static final int ERASE_NO=-1;
	private static final int ERASE_ALL=0xffff;
	
	private Communication communication;
	private int listenonly,timeout,pongwait;
	private ArrayList<DataWriter> writers = new ArrayList<DataWriter>();
	private ArrayList<Pong> pongs=new ArrayList<Pong>();
	private Pong currently_handled;
	private Timer timer=new Timer();
	private TimerTask startdownload=null;
	private TimerTask pingtask=null;

	private int pinginterval;
	private boolean erase;
	
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
	
	/**
	 * This class handles the downloading. Will be scheduled delayed after ping, or run
	 * after a "download complete" pong to correct downloading errors  
	 */
	public final class StartDownload extends TimerTask{
		ArrayList<Pong> pongs;
		
		public StartDownload(ArrayList<Pong> pongs){
			this.pongs=pongs;
		}

		@Override
		public void run() {
			Pong maxdownloadPong=null;
			long maxdownload=Long.MIN_VALUE;
			for(Pong p:pongs){
				if(currently_handled.getNodeID()==p.getNodeID()||currently_handled.getNodeID()==NONE){
					try {
						DataWriter currentwriter=StreamDownloader.getWriter(p.getNodeID(), writers);
						if(currentwriter!=null){
							long download;
							Gap repair=currentwriter.repairGap(p.minaddress);
							if(repair!=null){
								maxdownload=Long.MAX_VALUE;
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
				if(pingtask!=null){
					pingtask.cancel();
					pingtask=null;
				}
				DataWriter maxdownloadWriter=getWriter(maxdownloadPong.getNodeID(), writers);
				maxdownloadWriter.setLastModified();
				
				if(maxdownload==Long.MAX_VALUE){
					Gap repair=maxdownloadWriter.repairGap(maxdownloadPong.minaddress);
					System.out.println("Download from #"+maxdownloadWriter.getNodeid()+" ("+repair.getStart()+"-"+repair.getEnd()+")");

					try{
						communication.sendGet(maxdownloadWriter.getNodeid(), repair.getStart(), repair.getEnd());
						lastpercent=-1;
						currently_handled=new Pong(maxdownloadWriter.getNodeid(), repair.getStart(), repair.getEnd());
					} 
					catch(IOException e){
						System.err.println("Warning: Can't send get message");
					}
				} else {
					long minaddress;
					try {
						minaddress = (maxdownloadPong.minaddress>maxdownloadWriter.getMaxAddress())?maxdownloadPong.minaddress:maxdownloadWriter.getMaxAddress();
						System.out.println("Download from #"+maxdownloadWriter.getNodeid()+" ("+minaddress+"-"+maxdownloadPong.maxaddress+")");

						try{
							communication.sendGet(maxdownloadWriter.getNodeid(), minaddress, maxdownloadPong.maxaddress);
							currently_handled=new Pong(maxdownloadWriter.getNodeid(), minaddress, maxdownloadPong.maxaddress);

						} 
						catch(IOException e){
							System.err.println("Warning: Can't send get message");
						}
					}catch (IOException e) {
						System.err.println("Error: Can't read file "+DataWriter.nodeidToPath(maxdownloadPong.getNodeID(),".bin"));
					}
				}
			}
			pongs.clear();
		}
	}
	
	/**
	 * This class handles download timeout. 
	 */
	//TODO this should schedule after download started, not fixedrate 
	public final class ClearHandled extends TimerTask {
		public void run() {
			long now=(new Date()).getTime();
			if(currently_handled.getNodeID()!=NONE&&getWriter(currently_handled.getNodeID(), writers).getLastModified()+timeout*1000<now){
				currently_handled=new Pong(NONE,0,0);
				if(pingtask==null){
					System.out.println("Download timed out");
					pingtask=new Ping();
					timer.scheduleAtFixedRate(pingtask, 0, pinginterval*1000);
				}
			}
		}
	}
	
	/**
	 * Sends ping periodically
	 */
	public final class Ping extends TimerTask {
		public void run() {
			System.out.println("Ping");
			try {
				communication.sendPing();
				startdownload=new StartDownload(pongs);
				timer.schedule(startdownload, pongwait*1000);
			} catch (IOException e) {
				System.err.println("Warning: Can't send ping");
			}
		}
	}
	
	/**
	 * This class handles erase timeout
	 */
	public final class EraseExit extends TimerTask {
		public void run() {
			System.out.println("Timeout: exiting");
			System.exit(0);
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
	
	public static int lastpercent=-1;
	public static String ProgressBar(long length, long current, long data ,float error, long errorcount){
		if(length<=0)
			return "";
		java.text.DecimalFormat floatformat = new java.text.DecimalFormat("###.##");
		int perct=(int)(100*current/length);
		if(perct%5==0&&perct!=lastpercent){
			lastpercent=perct;
			return perct+"% = "+floatformat.format((float)data/1024)+"KiB. Gaps: "+floatformat.format(error)+"% ("+errorcount+")\n";
		}else
			return "";
	}
	
	public StreamDownloader(int listenonly, int pinginterval, int pongwait, int timeout, String source) {
		this.listenonly=listenonly;
		this.timeout=timeout;
		this.pongwait=pongwait;
		this.pinginterval=pinginterval;
		this.erase=false;
		currently_handled=new Pong(NONE, 0, 0);
		communication=new Communication(this, source);
		pingtask  = new Ping();
	    timer.scheduleAtFixedRate(pingtask, 0, pinginterval*1000);
	    timer.scheduleAtFixedRate(new ClearHandled(), timeout*1000, timeout*1000);
	}
	
	public StreamDownloader(int listenonly,int timeout, String source){//eraser constructor
		this.listenonly=listenonly;
		this.erase=true;
		this.timeout=timeout;
		communication=new Communication(this, source);
		try{
			if(listenonly==NONE){
			    timer.schedule(new EraseExit(), timeout*1000);
				communication.sendErase();
				System.out.println("Erase command sent to every node");
			} else{
				communication.sendErase(listenonly);
				System.out.println("Erase command sent to node #"+listenonly);
			}
			
		} catch (IOException e){
			System.err.println("Can't send erase command; exiting");
			System.exit(1);
		}
	}

	public void newData(int nodeid, long address, byte[] data) {
		//System.out.print(".");
		if(currently_handled.getNodeID()==nodeid){
			DataWriter writer=getWriter(nodeid, writers);
			if(writer==null){//create a new file
				writer=new DataWriter(nodeid);
				writers.add(writer);
			}
			
			try {//write
				long done=writer.writeData(address, data);
				done-=currently_handled.getMinAddress();
				System.out.print(ProgressBar(currently_handled.getMaxAddress()-currently_handled.getMinAddress(),
											 done, writer.getMaxAddress()-writer.getGapCount(), writer.getGapPercent(), writer.getGapCount()));
			} catch (IOException e) {
				System.err.println("Error: Can't write file "+DataWriter.nodeidToPath(writer.getNodeid(),".bin"));
			}
		} else
			System.err.println("Warning: unhandled data from #"+nodeid);
	}

	public void newPong(int nodeid, long min_address, long max_address, boolean complete) {
		if(listenonly==NONE||listenonly==nodeid){
			if(erase){
				if(min_address==max_address&&max_address==0&&complete){
					System.out.println("Erase complete; node #"+nodeid+" "+min_address+"-"+max_address);
					try {
						new DataWriter(nodeid).erase();
						System.out.println("Local files erased");
					} catch (IOException e) {
						System.err.println("Can't erase local files");
					}
					if(listenonly!=NONE){
						System.exit(0);
					}
				} else if(min_address!=max_address||max_address!=0){
					System.out.println("Erase failed; node #"+nodeid+" "+min_address+"-"+max_address);
					System.out.println("Retry");
					try {
						communication.sendErase(nodeid);
					} catch (IOException e) {
						System.err.println("Can't send erase command; exiting");
						System.exit(1);
					}
				} else if(!complete){
					System.out.println("New node #"+nodeid+" "+min_address+"-"+max_address);
					System.out.println("send erase");
					try {
						communication.sendErase(nodeid);
					} catch (IOException e) {
						System.err.println("Can't send erase command; exiting");
						System.exit(1);
					}
				}
			} else {
				if(getWriter(nodeid, writers)==null){
					writers.add(new DataWriter(nodeid));
				}
				
				if(new Date().getTime()-startdownload.scheduledExecutionTime()<=0){
					System.out.println("Node #"+nodeid+" "+min_address+"-"+max_address);
					pongs.add(new Pong(nodeid, min_address, max_address));
				}else if(currently_handled.getNodeID()==nodeid&&complete){
					System.out.println("Download complete; node #"+nodeid+" "+min_address+"-"+max_address);
					getWriter(nodeid, writers).setLastModified();
					pongs.clear();
					pongs.add(new Pong(nodeid, min_address, max_address));
					new StartDownload(pongs).run();
				}else{
					System.err.println("Warning: Unhandled pong from "+nodeid+" (timeout)");
				}
			}
		} else
			System.out.println("Unhandled pong from "+nodeid);
	}
	
	public void pongError(int nodeid) {
		System.out.println("Node #"+nodeid+" Streamstorage doesn't start");
		System.out.println("Probably unexpected data in the storage. Erase advised");
		
	}
	
	public static void usage(){
		System.out.println("Usage: StreamDownloader [options]");
		System.out.println("Options: ");
		System.out.println("-c <port>:");
		System.out.println("--comm <port>: Listen on <port>. Default: sf@localhost:9002");
		System.out.println("-o <id>:");
		System.out.println("--only <id>: Only download mote Nr. <id>");
		System.out.println("-i <number>:");
		System.out.println("--pinginterval <number>: Ping interval in seconds. Default: 10");
		System.out.println("-t <number>:");
		System.out.println("--timeout <number>: Download/erase timeout in seconds. Default download: 2; Default erase: 10");
		System.out.println("-w <number>:");
		System.out.println("--pongwait <number>: the program waits <number> seconds after ping for pongs. Default: 3");
		System.out.println("-e <id>:");
		System.out.println("--erase <id>: Erase the flash of the node <id>. <id>=all will erase all nodes. Deletes local files too");
		System.exit(1);
	}
	
	public static void main(String[] args) throws Exception {
		//System.out.println(TOSSerial.getTOSCommMap());
		String source = "sf@localhost:9002";
		int listenonly=NONE, timeout=3,pinginterval=10,pongwait=3;
		boolean timeout_mod=false;
		int erase=ERASE_NO;
		if (args.length == 0||args.length == 2||args.length == 4||args.length == 6) {
			for(int i=0;i<args.length;i++){
				try{
					if (args[i].equals("--comm")||args[i].equals("-c")) {
						source = args[++i];
					} else if (args[i].equals("--only")||args[i].equals("-o")) {
						listenonly = Integer.parseInt(args[++i]);
					} else if (args[i].equals("--timeout")||(args[i].equals("-t"))) {
						timeout = Integer.parseInt(args[++i]);
						timeout_mod=true;
					} else if (args[i].equals("--pinginterval")||args[i].equals("-i")) {
						pinginterval = Integer.parseInt(args[++i]);
					} else if (args[i].equals("--pongwait")||args[i].equals("-w")) {
						pongwait = Integer.parseInt(args[++i]);
					} else if (args[i].equals("--erase")||args[i].equals("-e")) {
						try{
							erase = Integer.parseInt(args[++i]);
						} catch(NumberFormatException e) {
							if(args[i+1].equals("all")){
								erase=ERASE_ALL;
							} else
								throw e;
						}
					} else {
						StreamDownloader.usage();
					}
				}catch(NumberFormatException e) {
					StreamDownloader.usage();
				}
			}
		} else {
			StreamDownloader.usage();
		}
		if(erase==ERASE_NO)
			new StreamDownloader(listenonly, pinginterval, pongwait, timeout, source);
		else{
			if(!timeout_mod)
				timeout=10;
			new StreamDownloader(erase, timeout, source);
		}
	}	
}

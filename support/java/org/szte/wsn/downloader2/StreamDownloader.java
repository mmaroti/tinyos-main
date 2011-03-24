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

import java.io.File;
import java.io.IOException;
import java.util.ArrayList;
import java.util.HashSet;

import org.szte.wsn.CSVProcess.CSVHandler;

public class StreamDownloader{
	private Communication comm;
	private ConsoleHandler console;
	private CSVHandler timeSync;
	private ArrayList<DataWriter> writers = new ArrayList<DataWriter>();
	private Pong currently_handled;
	private boolean welcomePrinted=false;
	
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
	

	
	public StreamDownloader(String source){
		comm=new Communication(this, source);
		console=new ConsoleHandler("StreamDownloader shell", ">", "help");
		try {
			this.timeSync=new CSVHandler(new File("00000time.csv"), true, ";", 2, new Integer[]{1,3,4,5,6,7});
		} catch (IOException e) {
			System.err.println("Error: Can't open or parse the timesync file");
			System.exit(1);
		}	
		comm.discover();
	}
	
	public void discoveryComplate(HashSet<Integer> motes) {
		System.out.print("Fount "+motes.size()+" motes. Discover again?");
		if(console.readChar(new String[]{"y","n"}).endsWith("y")){
			comm.discover();
		} else 
			waitForCommands();
	}
	
	
	
	private void waitForCommands() {
		if(!welcomePrinted){
			welcomePrinted=true;
			console.printWelcome();
		}
		String command=console.readCommand();
			
	}

	private static DataWriter getWriter(int nodeid, ArrayList<DataWriter> datawriters ){
		for(int i=0;i<datawriters.size();i++){
			if(datawriters.get(i).getNodeid()==nodeid){
				return datawriters.get(i);
			}
		}
		return null;	
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
				//System.out.print(ProgressBar(currently_handled.getMaxAddress()-currently_handled.getMinAddress(),
				//							 done, writer.getMaxAddress()-writer.getGapCount(), writer.getGapPercent(), writer.getGapCount()));
				console.setProgress(done, currently_handled.getMaxAddress()-currently_handled.getMinAddress()
						, ""+(writer.getMaxAddress()-writer.getGapCount()), "Gaps: "+writer.getGapPercent()+"% ("+writer.getGapCount()+")");
			} catch (IOException e) {
				System.err.println("Error: Can't write file "+DataWriter.nodeidToPath(writer.getNodeid(),".bin"));
			}
		} else
			System.err.println("Warning: unhandled data from #"+nodeid);
			comm.stopSending(nodeid);
	}
	
	public void newTimeSync(Long moteTime, Integer bootCount, Long pcTime, Integer nodeId) {
		if(timeSync.isEmpty()){
			String[] header={"nodeId","local","localBootCount","remote","remoteBootCount","rssi","lqi"};
			timeSync.setHeader(header);
		}
		String line[]={nodeId.toString(), pcTime.toString(), "0", moteTime.toString(),
				bootCount.toString(),"0","0"};
		timeSync.addLine(line);
		try {
			timeSync.flush();
		} catch (IOException e) {
			System.err.println("Warning: Can't write timeSync file");
		}
	}	
	
	public static void main(String[] args){
		new StreamDownloader("sf@localhost:9002");		
	}


	
}

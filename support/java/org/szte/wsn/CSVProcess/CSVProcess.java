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
package org.szte.wsn.CSVProcess;

import java.io.File;
import java.io.IOException;
import java.util.ArrayList;

import org.szte.wsn.TimeSyncPoint.LinearFunction;
import org.szte.wsn.TimeSyncPoint.TSParser;

public class CSVProcess{
	private File outputfile=new File("global.csv");
	private String separator=";";
	private String nodeIdSeparator=":";
	
	private int localColumn[]={4}; //time
	private int globalColumn[]={5}; //time
	private boolean insertGlobal=true;
	private int maxerror=120;
	private String timeformat="yyyy.MM.dd/HH:mm:ss.SSS";
	private String confFile="convert.conf";
	private String csvExt=".csv";
	

	private ArrayList<Integer> dataColumns[];
	private long startTime=Long.MIN_VALUE;
	private long endTime=Long.MAX_VALUE;
	private long timewindow=900000;
	private byte timetype=CSVHandler.TIMETYPE_START;
	
	private int runningConversions;
	public ArrayList<CSVHandler> filesPerNode[]=null;	
	
	public CSVProcess(ArrayList<String> inputFiles, ArrayList<Integer> dataColumns[]){
		this.dataColumns=dataColumns;
		try {
			this.outputfile.createNewFile();
		} catch (IOException e) {
			System.err.println("Can't acces outputfile: "+this.outputfile.getAbsolutePath()+", exiting");
			System.exit(1);
		}
		runningConversions=inputFiles.size();
		for(String file:inputFiles)
			new Converter(file, confFile, csvExt, separator, new PerConversion());
	}

	private void mergeConversion(ArrayList<CSVHandler> fileGroup) {
		CSVMerger merger=null;
		CSVHandler globalFile=null;
		merger=new CSVMerger(fileGroup);
		try {
			globalFile=merger.createGlobalFile(outputfile, nodeIdSeparator, startTime, endTime);
		} catch (IOException e) {
			System.err.println("E: Can't merge inputfiles");
		}
		if(globalFile==null)
			System.exit(1);
		CSVHandler avgFile=null;
		try {
			globalFile.flush();
		} catch (IOException e2) {
			// TODO Auto-generated catch block
			e2.printStackTrace();
		}
		globalFile.fillGaps();
		try {
			avgFile=globalFile.averageInTime(timewindow, new File("avgfile.csv"),timetype);
		} catch (IOException e1) {
			// TODO Auto-generated catch block
			e1.printStackTrace();
		}
		avgFile.formatTime(timeformat);
		avgFile.formatDecimalSeparator(",");
		try {
			avgFile.flush();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
	}
	
	public class PerConversion implements ParsingReady {

		@SuppressWarnings("unchecked")
		@Override
		public void Ready(Converter output) {
			CSVHandler[] ready=null;
			try {
				ready = output.toCSVHandlers(localColumn, dataColumns);
			} catch (IOException e) {
				System.out.println("Can't open parsed file");
			} 
			if(ready!=null){
				if(filesPerNode==null){
					filesPerNode=new ArrayList[ready.length];
					for(int i=0;i<filesPerNode.length;i++)
						filesPerNode[i]=new ArrayList<CSVHandler>();
				}
				File tsFile=TSParser.searchTSFile(ready[0].getFile());
				TSParser parser=new TSParser(tsFile, maxerror);
				ArrayList<LinearFunction> func=parser.parseTimeSyncFile();
				for(int i=0;i<ready.length;i++){

					ready[i].calculateGlobal (func, globalColumn[i], insertGlobal) ;
					filesPerNode[i].add(ready[i]);
				}
			}
			runningConversions--;
			if(runningConversions==0){
				for(ArrayList<CSVHandler> fpn:filesPerNode)
					mergeConversion(fpn);
			}
			
		}
		
	}

	public static void main(String[] args){
		String[] fileNames=new File(".").list();
//		boolean convertToSI=true;
//		long avgWindow=900000;
//		int number=15;
//		Long startTime=null;
//		Long endTime=startTime;
//		String separator=",";
		ArrayList<Integer>[] datacolumns=new ArrayList[1];
		datacolumns[0]=new ArrayList<Integer>();
		datacolumns[0].add(1);
		datacolumns[0].add(2);
		datacolumns[0].add(3);
		datacolumns[0].add(4);
		File outputfile=new File("global.csv");
//		String timef=null;//"yyyy.MM.dd. HH:mm:ss.SSS";
//		String globaln="globaltime";
		outputfile.delete();
		ArrayList<String> inputfiles=new ArrayList<String>();
		for(String file:fileNames){
			if(file.endsWith(".bin")){
				inputfiles.add(file);
			}
		}
		new CSVProcess(inputfiles,datacolumns);

	}


}

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
 * Author:Andras Biro, Miklos Toth
 */
package org.szte.wsn.CSVProcess;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.IOException;
import java.util.ArrayList;

import org.szte.wsn.TimeSyncPoint.LinearFunction;
import org.szte.wsn.TimeSyncPoint.TSParser;

public class CSVProcess{

	private static String separator=";";
	private static String nodeIdSeparator=":";
	private static int maxError=120;	
	private static String timeFormat="yyyy.MM.dd/HH:mm:ss.SSS";	
	private static String confFile="convert.conf";	
	private static String csvExt=".csv";		
	private static long startTime=Long.MIN_VALUE;
	private static long endTime=Long.MAX_VALUE;	
	private static long timeWindow=900000;	
	private static byte timeType=CSVHandler.TIMETYPE_START;

	//private ArrayList<StructParams> structures;
	//TODO local variables
	private ArrayList<Integer>[] datacolumns;
	private File outputFile;
	private File avgOutputFileName;
	private int globalColumn[];
	private boolean insertGlobal;
	private int localColumn[];
	
	private int runningConversions;
	public ArrayList<CSVHandler> filesPerNode[]=null;	

	@SuppressWarnings("unchecked")
	public CSVProcess(ArrayList<String> inputFiles, ArrayList<StructParams> structs){
		
		datacolumns=new ArrayList[structs.size()];
		
			
		outputFile=structs.get(0).getOutputFile();
		avgOutputFileName=structs.get(0).getAvgOutputFile();
		globalColumn=new int[structs.size()];
		localColumn=new int[structs.size()];
		for(int i=0;i<structs.size();i++){
			globalColumn[i]=structs.get(i).getGlobalColumn();
			localColumn[i]=structs.get(i).getLocalColumn();
			datacolumns[i]=structs.get(i).getDataColumns();
			
		}
		this.insertGlobal=structs.get(0).isInsertGlobal();
		try {
			this.outputFile.createNewFile();
		} catch (IOException e) {
			System.err.println("Can't acces outputfile: "+this.outputFile.getAbsolutePath()+", exiting");
			System.exit(1);
		}
		runningConversions=inputFiles.size();
		for(String file:inputFiles)
			new Converter(file, confFile, csvExt, separator, new PerConversion());
	}


	private static ArrayList<StructParams> initParams(String fileName){

		ArrayList<StructParams> structs=new ArrayList<StructParams>();
		try {
			BufferedReader input =  new BufferedReader(new FileReader(fileName));
			try {

				int localColumn =-1;
				int globalColumn =-1;
				int[] dataColumns=null;
				String outputName="global";
				String avgName="avg";
				boolean insertGlobal = true; 
				String name=null;

				String line = null; 
				String keyWord=null;
				String value=null;
				boolean global=true;

				while (( line = input.readLine()) != null){
					if (line.contains("=")){
						keyWord=line.split("=")[0];
						keyWord=keyWord.trim();      //removing whitespace from the keyword
						value=line.split("=")[1];
						value=value.trim();
						if (value.startsWith("\""))
							value=value.substring(1, value.length());  
						if (value.endsWith("\""))            //removing " characters separately in case someone misses the end 
							value=value.substring(0, value.length()-1); 						
						if(global){
							if(keyWord.equalsIgnoreCase("separator")) 
								separator=value;
							if(keyWord.equalsIgnoreCase("nodeIdSeparator"))
								nodeIdSeparator=value;
							if(keyWord.equalsIgnoreCase("maxError")) 
								maxError=Integer.parseInt(value);
							if(keyWord.equalsIgnoreCase("timeFormat")) 
								timeFormat=value;						
							if(keyWord.equalsIgnoreCase("confFile")) 
								confFile=value;
							if(keyWord.equalsIgnoreCase("csvExt")) 
								csvExt=value;						
							if(keyWord.equalsIgnoreCase("startTime")) {
								if (value.equalsIgnoreCase("min"))
									startTime=Long.MIN_VALUE;
								else
									startTime=Long.parseLong(value);
							}
							if(keyWord.equalsIgnoreCase("endTime")) {
								if (value.equalsIgnoreCase("max"))
									endTime=Long.MAX_VALUE;
								else
									endTime=Long.parseLong(value);
							}			

							if(keyWord.equalsIgnoreCase("timeWindow")) 
								timeWindow=Long.parseLong(value);

							if(keyWord.equalsIgnoreCase("timeType")){ 
								if (value.equalsIgnoreCase("middle"))
									timeType=CSVHandler.TIMETYPE_MIDDLE;
								else if (value.equalsIgnoreCase("end"))
									timeType=CSVHandler.TIMETYPE_END;
								else if (value.equalsIgnoreCase("start"))
									timeType=CSVHandler.TIMETYPE_START;
								else
									System.out.println("WARNING: Could not identify timeType during the parsing of "+fileName +"CSVProcess config file. ");
							}
						}//end of global parameter parsing branch
						else{
							if(keyWord.equalsIgnoreCase("avgOutputFileName")) 
								avgName=value;
							if(keyWord.equalsIgnoreCase("insertGlobal")) 
								insertGlobal=value.equalsIgnoreCase("true");
							if(keyWord.equalsIgnoreCase("outputFileName"))
								outputName=value;

							try {
								if(keyWord.equalsIgnoreCase("localColumn"))
									localColumn=Integer.parseInt(value);
								if(keyWord.equalsIgnoreCase("globalColumn"))
									globalColumn=Integer.parseInt(value);
								if(keyWord.equalsIgnoreCase("dataColumns")){
									String[] values=value.split(",");
									dataColumns=new int[values.length];
									for(int i=0; i<values.length;i++)
										dataColumns[i]=Integer.parseInt(values[i]);									
								}

							} catch (NumberFormatException e) {
								System.out.println("WARNING: Could not parse columns indicators during the parsing of "+fileName +" CSVProcess config file. ");
								e.printStackTrace();
							}
						}
					}
					else if(line.contains("structure")){
						if(global)
							global=false;
						else
						{							
							localColumn=-1;
							globalColumn=-1;
							dataColumns=null;
						}
						if (line.contains(" ")){
							value=line.split(" ")[1];
							name=value;
						}
						else
							name=null;


					}	
					else if(line.contains("endofstruct")){
						if((localColumn<0)||(globalColumn<0)||(dataColumns==null)||(name==null)){
							System.err.println("ERROR: Missing parameters while parsing "+fileName +" CSVProcess config file, "+name+" struct.");
							System.err.println("LocalColumn, globalColumn, datacolumns and name are mandatory for every structure");
							System.exit(1);
						}
						structs.add(new StructParams(localColumn, globalColumn, dataColumns, outputName, avgName, insertGlobal, name));
					}
				}//end of while 

			}
			finally {
				input.close();
			}
		}
		catch(NumberFormatException e){
			System.out.println("ERROR, could not parse every parameters of "+fileName +" CSVProcess config file. ");
		}
		catch (IOException e) {			
			System.out.println("IO ERROR while working with: "+fileName);
			e.printStackTrace();
			System.exit(1);			
		}
		return structs;
	}

	private void mergeConversion(ArrayList<CSVHandler> fileGroup) {
		CSVMerger merger=null;
		CSVHandler globalFile=null;
		merger=new CSVMerger(fileGroup);
		try {
			globalFile=merger.createGlobalFile(outputFile, nodeIdSeparator, startTime, endTime);
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
			avgFile=globalFile.averageInTime(timeWindow, avgOutputFileName ,timeType); 
		} catch (IOException e1) {
			// TODO Auto-generated catch block
			e1.printStackTrace();
		}
		avgFile.formatTime(timeFormat);
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
				ready = output.toCSVHandlers(localColumn, datacolumns);
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
				TSParser parser=new TSParser(tsFile, maxError);
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
		ArrayList<String> inputfiles=new ArrayList<String>();
		for(String file:fileNames){
			if(file.endsWith(".bin")){
				inputfiles.add(file);
			}
		}
		String initFileName=(args.length>0)?args[0]:"ini.conf";
		ArrayList<StructParams> structs= initParams(initFileName);		
		
		File outputfile=structs.get(0).getOutputFile(); //TODO
		outputfile.delete();
		

		new CSVProcess(inputfiles,structs);

	}


}

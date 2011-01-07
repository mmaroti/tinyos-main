package org.szte.wsn.downloader;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;

import org.szte.wsn.TimeSyncPoint.LinearFunction;
import org.szte.wsn.TimeSyncPoint.Regression;

import argparser.ArgParser;
import argparser.BooleanHolder;
import argparser.IntHolder;
import argparser.LongHolder;
import argparser.StringHolder;

public class GlobalTime {
	
	private String timeformat;
	private CSVHandler handler;
	
	private ArrayList<LinearFunction> ParseTimeSyncFile(File tsfile,long maxerror){
		ArrayList<LinearFunction> functions=new ArrayList<LinearFunction>();
		if(tsfile.exists()&&tsfile.isFile()&&tsfile.canRead()){
			BufferedReader input;
			try {
				input = new BufferedReader(new FileReader(tsfile));
			} catch (FileNotFoundException e1) {
				System.err.println("Error: Can't read timestamp file: "+tsfile.getName());
				return null;
			}
			String line;
			Regression regr=new Regression(maxerror,(double)1000/1024);
			try {
				while (( line = input.readLine()) != null){
					String[] dates = line.split(":");
					if(dates.length<2){
						System.err.println("Warning: Too short line in file: "+tsfile.getName());
						System.err.println(line);
						continue;
					}
					Long pctime,motetime;
					try{
						pctime=Long.parseLong(dates[0]);
						motetime=Long.parseLong(dates[1]);
					} catch(NumberFormatException e){
						System.err.println("Warning: Unparsable line in file: "+tsfile.getName());
						System.err.println(line);
						continue;
					}
					if(!regr.addPoint(motetime, pctime)){//end of running: save the function, then read the next running
						functions.add(regr.getFunction());
						System.out.println("pc="+regr.getFunction().getOffset()+"+"+regr.getFunction().getSlope()+"*mote ("+tsfile.getName()+"); points:"+regr.getNumPoints());
					}
				}
			} catch (IOException e) {
				System.err.println("Error: Can't read timestamp file: "+tsfile.getName());
				return null;
			}
			functions.add(regr.getFunction());
			System.out.println("pc="+regr.getFunction().getOffset()+"+"+regr.getFunction().getSlope()+"*mote ("+tsfile.getName()+"); points:"+regr.getNumPoints());
			return functions;
		} else {
			System.err.println("Error: Can't read timestamp file: "+tsfile.getName());
			return null;
		}
	}
	
	private ArrayList<Integer> GetBreaks(CSVHandler handler, int local){

		ArrayList<Integer> ret=new ArrayList<Integer>();
		int currentline=1;
		Long lasttime=null;
		Long currenttime=null;
		while(currentline<=handler.getLineNumber()){		
			try{
				currenttime=Long.parseLong(handler.getCell(local, currentline));
			} catch(NumberFormatException e){
				System.err.println("Warning: Unparsable line in file: "+handler.getFile().getName());
				System.err.println(handler.getLine(currentline));
				continue;
			}
			if(lasttime==null||lasttime>currenttime){
				ret.add(currentline);
			}
			lasttime=currenttime;
			currentline++;
		}
		return ret;
		
	}
	
	private void WriteOutputFile(CSVHandler csvfile, int local, int global, boolean insert, ArrayList<LinearFunction> functions, ArrayList<Integer> breaks) {
		int currentrun=0;
		if(insert)
			csvfile.addColumn("globaltime", global);	
		for(int currentline=1;currentline<=csvfile.getLineNumber();currentline++){
			if(breaks.contains(currentline))
				currentrun++;
			Long currenttime=null;
			try{
				currenttime=Long.parseLong(csvfile.getCell(local, currentline));
			} catch(NumberFormatException e){
				System.err.println("Warning: Unparsable line in file: "+csvfile.getFile().getName());
				System.err.println(csvfile.getLine(currentline));
				continue;
			}
			int currentfunction=breaks.size()-currentrun;
			String currenttstring;
			if(currentfunction>=0){
				currenttime=(long) (functions.get(currentfunction).getOffset()+functions.get(currentfunction).getSlope()*currenttime);
				if(timeformat==null)
					currenttstring=currenttime.toString();
				else
					currenttstring=new SimpleDateFormat(timeformat).format(new Date(currenttime));
			} else 
				currenttstring="";
			csvfile.setCell(global, currentline, currenttstring);
		}
		try {
			if(!csvfile.flush())
				System.err.println("Error: Can't overwrite file: "+csvfile.getName());
		} catch (IOException e) {
			System.err.println("Error: Can't write tempfile");
			return;
		}
	}
	
	public GlobalTime(File timeFile, File csvFile, int local, int global, boolean insert, long maxerror, String separator, String timeformat){
		this.timeformat=timeformat;
		ArrayList<LinearFunction> functions=ParseTimeSyncFile(timeFile, maxerror);
		if(csvFile.exists()&&csvFile.isFile()){
			try {
				handler = new CSVHandler(csvFile, true, separator);
				ArrayList<Integer> breaks=GetBreaks(handler, local);
				WriteOutputFile(handler,local,global,insert,functions,breaks);
			} catch (IOException e) {
				System.err.println("Error: Can't read csvfile: "+csvFile.getName());
			}
		} else 
			System.err.println("Error: Csv file doesn't exist: "+csvFile.getName());

	}
	
	public CSVHandler getCSVHandler(){
		return handler;
	}

	public static void main(String[] args) {
		IntHolder globalcolumn=new IntHolder();
		IntHolder localcolumn=new IntHolder();
		LongHolder errorlimit=new LongHolder(120);
		StringHolder input=new StringHolder();
		BooleanHolder noinsertcolumn=new BooleanHolder();
		BooleanHolder help=new BooleanHolder();
		StringHolder timesyncex=new StringHolder(".ts");
		StringHolder csvex=new StringHolder(".txt");
		StringHolder separator=new StringHolder(",");
		StringHolder format=new StringHolder();
		
		ArgParser parser = new ArgParser("java GlobalTime [options]",false);
		parser.addOption("-h,--help %v#Displays help information",help);
	    parser.addOption("-l,--local %d#The column number of the local time (counting start with 1)",localcolumn); 
	    parser.addOption("-g,--global %d#The column number of the global time (counting start with 1)",globalcolumn);
	    parser.addOption("-n,--noinsert %v#Overwrite the given column with the globaltime instead of inserting a column",noinsertcolumn);
		parser.addOption("-i,--input %s#The filename of the binary file (default: all the file in the directory)",input);
	    parser.addOption("-e,--maxtimeerror %d#The maximum of the enabled error during timesync in seconds (default: 120)",errorlimit);
		parser.addOption("-t,--timesyncex %s#Extension of the timesync files (default: .ts)",input);
		parser.addOption("-c,--csvex %s#Extension of the csv files (default: .csv)",csvex);
		parser.addOption("-s,--separator %s#Separator character(s) in the csv (default: ,)",separator);
		parser.addOption("-f,--format %s#Timeformat, see: http://download.oracle.com/javase/1.4.2/docs/api/java/text/SimpleDateFormat.html (Default: without formatting: ellapsed milliseconds since epoch)",format);
	    parser.matchAllArgs (args);
	    
		

		if(help.value){
			System.out.println(parser.getHelpMessage());
			System.exit(0);
		}
		if(localcolumn.value==0||globalcolumn.value==0){
			System.out.println("--local (-l) and --global (-g) are obligatory option");
			System.out.println(parser.getHelpMessage());
			System.exit(1);
		}
		if(input.value!=null){
			File tsfile=new File(CSVHandler.switchExtension(input.value, timesyncex.value));
			File csvfile=new File(CSVHandler.switchExtension(input.value, csvex.value));
			new GlobalTime(tsfile,csvfile,localcolumn.value,globalcolumn.value,noinsertcolumn.value,errorlimit.value,separator.value,format.value);
		}else{
			String[] fileNames=new File(".").list();
			for(String fileName:fileNames){
				if(fileName.endsWith(timesyncex.value)){
					File current=new File(fileName);
					File currentCsv=new File(CSVHandler.switchExtension(fileName,csvex.value));
					if(current.isFile()&&current.canRead()&&currentCsv.isFile()&&currentCsv.canWrite()){
						new GlobalTime(current, currentCsv,localcolumn.value,globalcolumn.value,noinsertcolumn.value,errorlimit.value,separator.value,format.value);
					}
				}
			}
		}
	}

}

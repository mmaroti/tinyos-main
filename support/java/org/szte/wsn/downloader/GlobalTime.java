package org.szte.wsn.downloader;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.FileWriter;
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
	
	private String separator;
	private String timeformat;
	
	private static String switchExtension(String fullname, String newEx){
		return fullname.substring(0, fullname.lastIndexOf('.'))+newEx;
	}
	
	private ArrayList<LinearFunction> ParseTimeSyncFile(String inputfile,long maxerror){
		ArrayList<LinearFunction> functions=new ArrayList<LinearFunction>();
		File tsfile=new File(inputfile);
		if(tsfile.exists()&&tsfile.isFile()&&tsfile.canRead()){
			BufferedReader input;
			try {
				input = new BufferedReader(new FileReader(tsfile));
			} catch (FileNotFoundException e1) {
				System.err.println("Error: Can't read timestamp file: "+inputfile);
				return null;
			}
			String line;
			Regression regr=new Regression(maxerror,(double)1000/1024);
			try {
				while (( line = input.readLine()) != null){
					String[] dates = line.split(":");
					if(dates.length<2){
						System.err.println("Warning: Too short line in file: "+inputfile);
						System.err.println(line);
						continue;
					}
					Long pctime,motetime;
					try{
						pctime=Long.parseLong(dates[0]);
						motetime=Long.parseLong(dates[1]);
					} catch(NumberFormatException e){
						System.err.println("Warning: Unparsable line in file: "+inputfile);
						System.err.println(line);
						continue;
					}
					if(!regr.addPoint(motetime, pctime)){//end of running: save the function, then read the next running
						functions.add(regr.getFunction());
						System.out.println("pc="+regr.getFunction().getOffset()+"+"+regr.getFunction().getSlope()+"*mote ("+inputfile+"); points:"+regr.getNumPoints());
					}
				}
			} catch (IOException e) {
				System.err.println("Error: Can't read timestamp file: "+inputfile);
				return null;
			}
			functions.add(regr.getFunction());
			System.out.println("pc="+regr.getFunction().getOffset()+"+"+regr.getFunction().getSlope()+"*mote ("+inputfile+"); points:"+regr.getNumPoints());
			return functions;
		} else {
			System.err.println("Error: Can't read timestamp file: "+inputfile);
			return null;
		}
	}
	
	private ArrayList<Long> GetBreaks(File csvfile, int local){
		if(csvfile.exists()&&csvfile.isFile()&&csvfile.canRead()){
			BufferedReader input;
			try {
				input = new BufferedReader(new FileReader(csvfile));
			} catch (FileNotFoundException e1) {
				System.err.println("Error: Can't read csv file: "+csvfile.getName());
				return null;
			}
			ArrayList<Long> ret=new ArrayList<Long>();
			try {
				String line;
				long currentline=0;
				Long lasttime=null;
				while (( line = input.readLine()) != null){
					currentline++;
					String[] columns = line.split(separator);
					if(columns.length<local){
						System.err.println("Warning: Too short line in file: "+csvfile.getName());
						System.err.println(line);
						continue;
					}
					if(currentline!=1){
						Long currenttime=null;
						try{
							currenttime=Long.parseLong(columns[local-1]);
						} catch(NumberFormatException e){
							System.err.println("Warning: Unparsable line in file: "+csvfile.getName());
							System.err.println(line);
							continue;
						}
						if(lasttime==null||lasttime<currenttime){
							ret.add(currentline);
						}
					}
					return ret;
				}	
			} catch (IOException e) {
				System.err.println("Error: Can't read csv file: "+csvfile.getName());
				return null;
			}
		}
		return null;
		
	}
	
	private void WriteOutputFile(File csvfile, File tempfile, int local, int global, boolean insert, ArrayList<LinearFunction> functions, ArrayList<Long> breaks) {
		if(!tempfile.exists()){
			try {
				tempfile.createNewFile();
			} catch (IOException e2) {
				System.err.println("Error: Can't create temp file: "+tempfile.getName());
				return;
			}
		}
		BufferedReader input;
		try {
			input = new BufferedReader(new FileReader(csvfile));
		} catch (FileNotFoundException e1) {
			System.err.println("Error: Can't read csv file: "+csvfile.getName());
			return;
		}
		BufferedWriter output;
		try {
			output = new BufferedWriter(new FileWriter(tempfile));
		} catch (IOException e){
			System.err.println("Error: Can't write temp file: "+tempfile.getName());
			return;
		}
		try {
			String line;
			Long currentline=0L;
			int currentrun=0;
			while (( line = input.readLine()) != null){
				currentline++;
				if(breaks.contains(currentline))
					currentrun++;
				String[] columns = line.split(separator);
				if(columns.length<local){
					System.err.println("Warning: Too short line in file: "+csvfile.getName());
					System.err.println(line);
					output.write(line);
					output.newLine();
					continue;
				}
				if(currentline==1){//header
					if(global-1<columns.length){
						for(int i=0;i<columns.length;i++){
							if(i==global){
								if(!insert)
									columns[i]="globaltime";
								else {
									output.write("globaltime,");
								}
							}
							output.write(columns[i]);
							if(i<columns.length-1)
								output.write(separator);
						}
					} else {
						output.write(line);
						for(int i=columns.length;i<global;i++)
							output.write(separator);
						output.write("globaltime");
					}
					output.newLine();						
				} else{
					Long currenttime=null;
					try{
						currenttime=Long.parseLong(columns[local-1]);
					} catch(NumberFormatException e){
						System.err.println("Warning: Unparsable line in file: "+csvfile.getName());
						System.err.println(line);
						output.write(line);
						output.newLine();
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
					if(global-1<columns.length){
						for(int i=0;i<columns.length;i++){
							if(i==global-1){
								if(!insert)
									columns[i]=currenttstring;
								else
									output.write(currenttstring+separator);
							}
							output.write(columns[i]);
							if(i<columns.length-1)
								output.write(separator);
						}
					} else {
						output.write(line);
						for(int i=columns.length;i<global;i++)
							output.write(separator);
						output.write(currenttstring);
					}
					output.newLine();
				}
			}
			input.close();
			output.close();
		} catch (IOException e) {
			System.err.println("Error: Can't read csv file: "+csvfile.getName());
			return;
		}
		csvfile.delete();
		tempfile.renameTo(csvfile);
	}
	
	public GlobalTime(String inputfile, int local, int global, boolean insert, long maxerror, String csvex, String separator, String timeformat){
		this.separator=separator;
		this.timeformat=timeformat;
		ArrayList<LinearFunction> functions=ParseTimeSyncFile(switchExtension(inputfile,".ts"), maxerror);
		File csvFile=new File(switchExtension(inputfile, csvex));
		ArrayList<Long> breaks=GetBreaks(csvFile, local);
		WriteOutputFile(csvFile,new File(switchExtension(inputfile, ".tmp")),local,global,insert,functions,breaks);
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
		if(input.value!=null)
			new GlobalTime(input.value,localcolumn.value,globalcolumn.value,noinsertcolumn.value,errorlimit.value,csvex.value,separator.value,format.value);
		else{
			String[] fileNames=new File(".").list();
			for(String fileName:fileNames){
				if(fileName.endsWith(timesyncex.value)){
					File current=new File(fileName);
					if(current.isFile()&&current.exists()&&current.canRead()&&current.canWrite()){
						new GlobalTime(fileName,localcolumn.value,globalcolumn.value,noinsertcolumn.value,errorlimit.value,csvex.value,separator.value,format.value);
					}
				}
			}
		}
	}

}

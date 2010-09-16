package org.szte.wsn.downloader;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.util.ArrayList;
import org.szte.wsn.TimeSyncPoint.LinearFunction;
import org.szte.wsn.TimeSyncPoint.Regression;
import argparser.ArgParser;
import argparser.BooleanHolder;
import argparser.IntHolder;
import argparser.LongHolder;
import argparser.StringHolder;

public class GlobalTime {
	
	private static String switchExtension(String fullname, String newEx){
		return fullname.substring(0, fullname.lastIndexOf('.'))+newEx;
	}
	
	private static ArrayList<LinearFunction> ParseTimeSyncFile(String inputfile,long maxerror){
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
						System.out.println("pc="+regr.getFunction().getOffset()+"+"+regr.getFunction().getSlope()+"*mote; points:"+regr.getNumPoints());
					}
				}
			} catch (IOException e) {
				System.err.println("Error: Can't read timestamp file: "+inputfile);
				return null;
			}
			functions.add(regr.getFunction());
			System.out.println("pc="+regr.getFunction().getOffset()+"+"+regr.getFunction().getSlope()+"*mote; points:"+regr.getNumPoints());
			return functions;
		} else {
			System.err.println("Error: Can't read timestamp file: "+inputfile);
			return null;
		}
	}
	
	private static ArrayList<Long> GetBreaks(File csvfile, int local){
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
					String[] columns = line.split(",");
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
	
	private static void WriteOutputFile(File csvfile, File tempfile, int local, int global, boolean insert, ArrayList<LinearFunction> functions, ArrayList<Long> breaks) {
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
				String[] columns = line.split(",");
				if(columns.length<local){
					System.err.println("Warning: Too short line in file: "+csvfile.getName());
					System.err.println(line);
					output.write(line);
					output.newLine();
					continue;
				}
				if(currentline==1){//header
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
							output.write(",");
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
					if(currentfunction>=0)
						currenttime=(long) (functions.get(currentfunction).getOffset()+functions.get(currentfunction).getSlope()*currenttime);
					for(int i=0;i<columns.length;i++){
						if(i==global-1){
							if(!insert)
								columns[i]=currenttime.toString();
							else
								output.write(currenttime+",");
						}
						output.write(columns[i]);
						if(i<columns.length-1)
							output.write(",");
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
	
	public GlobalTime(String inputfile, int local, int global, boolean insert, long maxerror, String csvex){
		ArrayList<LinearFunction> functions=ParseTimeSyncFile(inputfile, maxerror);
		File csvFile=new File(switchExtension(inputfile, csvex));
		ArrayList<Long> breaks=GetBreaks(csvFile, local);
		WriteOutputFile(csvFile,new File(switchExtension(inputfile, ".tmp")),local,global,insert,functions,breaks);
	}

	public static void main(String[] args) {
		IntHolder globalcolumnh=new IntHolder();
		IntHolder localcolumnh=new IntHolder();
		LongHolder errorlimith=new LongHolder(120);
		StringHolder inputh=new StringHolder();
		BooleanHolder noinsertcolumnh=new BooleanHolder();
		BooleanHolder helph=new BooleanHolder();
		StringHolder timesyncex=new StringHolder(".ts");
		StringHolder csvex=new StringHolder(".txt");
		
		ArgParser parser = new ArgParser("java GlobalTime [options]",false);
		parser.addOption("-h,--help %v#Displays help information",helph);
	    parser.addOption("-l,--local %d#The column number of the local time (counting start with 1)",localcolumnh); 
	    parser.addOption("-g,--global %d#The column number of the global time (counting start with 1)",globalcolumnh);
	    parser.addOption("-n,--noinsert %v#Overwrite the given column with the globaltime instead of inserting a column",noinsertcolumnh);
		parser.addOption("-i,--input %s#The filename of the binary file (default: all the file in the directory)",inputh);
	    parser.addOption("-e,--maxtimeerror %d#The maximum of the enabled error during timesync in seconds (default: 120)",errorlimith);
		parser.addOption("-t,--timesyncex %s#Extension of the timesync files (default: .ts)",inputh);
		parser.addOption("-c,--csvex %s#Extension of the csv files (default: .csv)",inputh);
	    parser.matchAllArgs (args);
	    
		

		if(helph.value){
			System.out.println(parser.getHelpMessage());
			System.exit(0);
		}
		if(localcolumnh.value==0||globalcolumnh.value==0){
			System.out.println("--local (-l) and --global (-g) are obligatory option");
			System.out.println(parser.getHelpMessage());
			System.exit(1);
		}
		if(inputh.value!=null)
			new GlobalTime(inputh.value,localcolumnh.value,globalcolumnh.value,noinsertcolumnh.value,errorlimith.value,csvex.value);
		else{
			String[] fileNames=new File(".").list();
			for(String fileName:fileNames){
				if(fileName.endsWith(timesyncex.value)){
					File current=new File(fileName);
					if(current.isFile()&&current.exists()&&current.canRead()&&current.canWrite()){
						new GlobalTime(fileName,localcolumnh.value,globalcolumnh.value,noinsertcolumnh.value,errorlimith.value,csvex.value);
					}
				}
			}
		}
	}

}

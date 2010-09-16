package org.szte.wsn.downloader;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.util.ArrayList;

public class BinReader2 implements ParsingReady {
	private long avgWindow;
	private long startTime;
	private long endTime;
	private boolean nicetime;
	private String timef;
	private File outputfile;
	private String globalname;
	private ArrayList<Integer> datacolumns;
	private int number;
	
	private final class Data{
		long timestamp;
		String[] data;
		
		public Data(long time, String[] data) {
			this.timestamp=time;
			this.data=data;
		}
	}
	
	public BinReader2(long avgWindow, long startTime, long endTime, boolean nicetime, String timef, String globalname, File outputfile, ArrayList<Integer> datacolumns, int number){
		this.avgWindow=avgWindow;
		this.startTime=startTime;
		this.endTime=endTime;
		this.nicetime=nicetime;
		this.timef=timef;
		this.outputfile=outputfile;
		this.globalname=globalname;
		this.datacolumns=datacolumns;
		this.number=number;
		Converter.waitForParsing(this);
	}
	
	
	private long getTimeStamp(File file, long after){
		long time=-1;
		try {
			BufferedReader br=new BufferedReader(new FileReader(file));
			String header=br.readLine();//header line
			String[] splittedHeader=header.split(",");
			Integer globalcolumn=null;
			for(int i=0;i<splittedHeader.length;i++){
				if(splittedHeader[i].equals(globalname)){
					globalcolumn=i;
					break;
				}
			}
			if(globalcolumn==null){
				System.err.println("Can't found "+globalname+" in header: "+header);
				return -1;
			}
			String line;
			while(time<=after && ((line=br.readLine())!=null) ){					
				String[] sline=line.split(",");
				time=Long.parseLong(sline[globalcolumn]);
			}
			br.close();
		} catch (FileNotFoundException e) {
			System.err.println("W: Can't open input file: "+file.getName());
		} catch (IOException e) {
			System.err.println("W: Can't read input file: "+file.getName());
		} catch (NumberFormatException e) {
			System.err.println("W: Can't parse globaltime in file "+file.getName());
			return -1;
		}
		
		return time;
	}
	
	private String[] getDatacolumns(String[] allcolumn, ArrayList<Integer> datacolumns){
		String data[]=new String[datacolumns.size()];
		int i=0;
		for(int currentcolumn:datacolumns){
			data[i]=allcolumn[currentcolumn];
			i++;
		}
		return data;
	}
	
	private Data[] getData(File file, long after, long before ,ArrayList<Integer> datacolumns){
		ArrayList<Data> ret=new ArrayList<Data>();
		try {
			BufferedReader br=new BufferedReader(new FileReader(file));
			String header=br.readLine();//header line
			String[] splittedHeader=header.split(",");
			Integer globalcolumn=null;
			for(int i=0;i<splittedHeader.length;i++){
				if(splittedHeader[i].equals(globalname)){
					globalcolumn=i;
					break;
				}
			}
			if(globalcolumn==null){
				System.err.println("Can't found "+globalname+" in header: "+header);
				return null;
			}
			String line;
			Data current=null;
			Data previous=null;
			while(current.timestamp<=after && ((line=br.readLine())!=null) ){					
				String[] sline=line.split(",");
				previous=current;
				current=new Data(Long.parseLong(sline[globalcolumn]), getDatacolumns(sline, datacolumns));
			}
			if(current.timestamp>=after){
				ret.add(previous);
				ret.add(current);
				while(current.timestamp<before && ((line=br.readLine())!=null) ){
					String[] sline=line.split(",");
					current=new Data(Long.parseLong(sline[globalcolumn]), getDatacolumns(sline, datacolumns));
					ret.add(current);
				}
				if((line=br.readLine())!=null){
					String[] sline=line.split(",");
					ret.add(new Data(Long.parseLong(sline[globalcolumn]), getDatacolumns(sline, datacolumns)));
				}
			} else
				return null;
			br.close();
		} catch (FileNotFoundException e) {
			System.err.println("W: Can't open input file: "+file.getName());
		} catch (IOException e) {
			System.err.println("W: Can't read input file: "+file.getName());
		} catch (NumberFormatException e) {
			System.err.println("W: Can't parse filename or globaltime in file "+file.getName());
			return null;
		}
		
		if(ret.size()>0)
			return (Data[]) ret.toArray();				
		else
			return null;
	}
	
	private long getMinTime(File[] outputfiles){
		long mintime=Long.MAX_VALUE;
		for(File current:outputfiles){
			long time=getTimeStamp(current, startTime);
			if(time>=startTime&&time<mintime&&time<endTime){
				mintime=time;				
			}
			
		}
		return mintime;
	}
	
	String createHeader(File[] datafiles, ArrayList<Integer> datacolumns){
		String ret="";
		try {
			BufferedReader br=new BufferedReader(new FileReader(datafiles[0]));
			String[] header = br.readLine().split(",");
			ret+="time,";
			for(File current: datafiles){
				for(int i=0;i<header.length;i++){
					ret+=current.getName()+",";
				}
			}
			ret+=System.getProperty("line.separator");
			ret+=",";
			for(int i=0;i<datafiles.length;i++){
				for(String column:header){
					ret+=column+",";
				}
			}
		} catch (IOException e) {
			System.err.println("W: Can't read file");
		}		
		return ret;	
	}
	
	@Override
	public void Ready(File[] outputfiles) {
		long mintime=getMinTime(outputfiles);
		boolean isdata=false;
		try {
			BufferedWriter outputfile=new BufferedWriter(new FileWriter(this.outputfile));
			
			outputfile.write(createHeader(outputfiles, datacolumns));
			outputfile.newLine();
			while(mintime<endTime&&isdata){
				outputfile.write(mintime+",");
				for(File current: outputfiles){
					Data[] data=getData(current, mintime, mintime+avgWindow*1000, datacolumns);
					if(data==null){
						for(int i=0;i<datacolumns.size();i++)
							outputfile.write(",");
					} else {
						Data avgdata=calculateAverage(data,mintime, mintime+avgWindow*1000,number);
						for(String avg:avgdata.data){
							outputfile.write(avg+",");
						}
					}
				}
				mintime+=avgWindow*1000;
				outputfile.newLine();
			}
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
	}	
	
	private Data calculateAverage(Data[] data, long mintime, long maxtime,int number) {
		double[] ret=new double[data[0].data.length];
		for(double current:ret)
			current=0.0;
		try{
			for(Data current:data){
				for(int i=0;i<ret.length;i++){
					ret[i]+=Double.parseDouble(current.data[i]);
				}
			}
			
			return new Data(mintime, null);//FIXME
		}catch(NumberFormatException e){
			return new Data(mintime, null);
		}
	}


	public static void main(String[] args){
		String[] fileNames=new File(".").list();
		boolean convertToSI=true;
		long avgWindow=900000;
		int number=15;
		long startTime=0;
		long endTime=startTime;
		boolean nicetime=false;
		ArrayList<Integer> datacolumns=new ArrayList<Integer>();
		datacolumns.add(2);
		datacolumns.add(3);
		datacolumns.add(4);
		File outputfile=new File("global.csv");
		String timef="yyyy.MM.dd. HH:mm:ss.SSS";
		String globaln="globaltime";
		
		
		for(String file:fileNames){
			if(file.endsWith(".bin")){
				new Converter(file,convertToSI);
			}
		}
		new BinReader2(avgWindow, startTime, endTime, nicetime, timef, globaln,outputfile, datacolumns,number);
	}

}

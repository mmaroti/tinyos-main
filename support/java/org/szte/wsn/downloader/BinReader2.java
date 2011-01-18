package org.szte.wsn.downloader;

import java.io.File;
import java.io.IOException;
import java.util.ArrayList;

public class BinReader2{
	private File outputfile=new File("global.csv");
	private String separator=";";
	private String nodeIdSeparator=":";
	
	private String tsext=".ts";
	private int localColumn=4;
	private int globalColumn=5;
	private String globalName="globaltime";
	private boolean insertGlobal=true;
	private int maxerror=120;
	private String timeformat="yyyy.MM.dd. HH:mm:ss.SSS";
	private String confFile="convert.conf";
	private boolean hasHeader=true;
	private String csvExt=".csv";
	private int runningConversions;
	public ArrayList<CSVHandler> filesPerNode=new ArrayList<CSVHandler>();
	private ArrayList<Integer> dataColumns=new ArrayList<Integer>();
	private long startTime=Long.MIN_VALUE;
	private long endTime=Long.MAX_VALUE;
	private long timewindow=900000;
	private byte timetype=CSVHandler.TIMETYPE_START;
	
	public BinReader2(ArrayList<String> inputFiles, ArrayList<Integer> dataColumns){
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

	public void mergeConversion() {
		CSVMerger merger=null;
		CSVHandler globalFile=null;
		try {
			merger=new CSVMerger(filesPerNode, dataColumns, globalName, separator);
		} catch (NumberFormatException e) {
			System.err.println("E: global time column not found: "+globalName);
		} catch (IOException e) {
			System.err.println("E: Can't open inputfiles");
		}		
		if(merger==null){
			System.exit(1);
		}
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
			avgFile=globalFile.averageColumns(timewindow, new File("avgfile.csv"),timetype);
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

		@Override
		public void Ready(Converter output) {
			CSVHandler ready;
			try {
				ready = output.toCSVHandler(localColumn, dataColumns);
				ready.calculateGlobal(tsext, globalColumn, insertGlobal, maxerror) ;
				
				filesPerNode.add(ready);
			} catch (IOException e) {
				System.out.println("Can't open parsed file: "+output.getFile().getName());
			}
			runningConversions--;
			if(runningConversions==0)
				mergeConversion();

			
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
		ArrayList<Integer> datacolumns=new ArrayList<Integer>();
		datacolumns.add(1);
		datacolumns.add(2);
		datacolumns.add(3);
		datacolumns.add(4);
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
		new BinReader2(inputfiles,datacolumns);

	}


}

package org.szte.wsn.CSVProcess;

import java.io.File;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Timer;
import java.util.TimerTask;

import org.szte.wsn.dataprocess.BinaryInterfaceFactory;
import org.szte.wsn.dataprocess.PacketParser;
import org.szte.wsn.dataprocess.PacketParserFactory;
import org.szte.wsn.dataprocess.Transfer;
import org.szte.wsn.dataprocess.file.StringInterfaceFile;

public class Converter implements ParsingReady{
	
	private Timer timer=new Timer();
	private Transfer parser;
	private File csvFile;
	private String separator;
	
	
	private static String switchExtension(String fullname, String newEx){
		return fullname.substring(0, fullname.lastIndexOf('.'))+newEx;
	}
	
	public Converter(String file, String confFile, String csvext, String separator, ParsingReady parent){
		this.separator=separator;
		
		PacketParser[] pp=new PacketParserFactory(confFile).getParsers();
		String outputfile=switchExtension(file, csvext);
		Transfer fp=new Transfer(pp,
				BinaryInterfaceFactory.getBinaryInterface("binfile", file),
				new StringInterfaceFile(separator,outputfile , pp, false,Transfer.REWRITE, false, false),
				true);
		fp.start();
		parser=fp;
		csvFile=new File(outputfile);
		if(parent==null) parent=this;
		waitForParsing(parent);
	}
	
	public CSVHandler toCSVHandler(int timeColumn, ArrayList<Integer> dataColumns) throws IOException{
		return new CSVHandler(csvFile, true, separator, timeColumn, dataColumns);
	}
	
	public File getFile(){
		return csvFile;
	}
	
	public class ParsersRunning extends TimerTask{

		private ParsingReady report;
		private Converter parent;
		
		@Override
		public void run() {
			if(!parser.isAlive()){
				timer.cancel();
				report.Ready(parent);
			}
		}
		
		public ParsersRunning(ParsingReady report, Converter parent){
			this.report=report;
			this.parent=parent;
		}
		
	}
	
	public void waitForParsing(ParsingReady report) {
		timer.scheduleAtFixedRate(new ParsersRunning(report, this),100,100);	
	}
	
	
	
	public static void main(String[] args){
		String[] fileNames=new File(".").list();
		for(String file:fileNames){
			if(file.endsWith(".bin")){
				new Converter(file,"convert.conf", ".csv", ",", null);
			}
		}
		
	}

	@Override
	public void Ready(Converter output) {
		// nobody want to know		
	}

	public CSVHandler getCSVHandler() {
		// TODO Auto-generated method stub
		return null;
	}



}

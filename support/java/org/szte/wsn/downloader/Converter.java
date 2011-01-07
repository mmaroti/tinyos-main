package org.szte.wsn.downloader;

import java.io.File;
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
	
	public Converter(String file, boolean convertToSI, String csvext, String separator, ParsingReady parent){
		this.separator=separator;
		
		PacketParser[] pp;
		if(convertToSI)
			pp=new PacketParserFactory("convert.conf").getParsers();
		else
			pp=new PacketParserFactory("raw.conf").getParsers();
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
	
	public CSVHandler calculateGlobal(String tsext, int localColumn, int globalColumn, boolean insertGlobal, int maxerror, String timeformat) {
			File tsfile=new File(CSVHandler.switchExtension(csvFile.getAbsolutePath(),tsext));
			if(tsfile.exists()){
				GlobalTime gt=new GlobalTime(tsfile,csvFile, localColumn, globalColumn, insertGlobal, maxerror,separator,timeformat);
				return gt.getCSVHandler();
			}
			return null;
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
				new Converter(file,true, ".csv", ",", null);
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

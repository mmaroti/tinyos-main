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
	private String separator;
	private StringInterfaceFile sif;
	
	private static String switchExtension(String fullname, String newEx){
		return fullname.substring(0, fullname.lastIndexOf('.'))+newEx;
	}
	
	public Converter(String file, String confFile, String csvext, String separator, ParsingReady parent){
		this.separator=separator;
		
		PacketParser[] pp=new PacketParserFactory(confFile).getParsers();
		String outputfile=switchExtension(file, csvext);
		sif=new StringInterfaceFile(separator,outputfile , pp, false,Transfer.REWRITE, true, false);
		Transfer fp=new Transfer(pp,
				BinaryInterfaceFactory.getBinaryInterface("binfile", file),
				sif,
				true);
		fp.start();
		parser=fp;
		if(parent==null) parent=this;
		waitForParsing(parent);
	}
	
	public CSVHandler[] toCSVHandlers(int[] timeColumn, ArrayList<Integer>[] dataColumns) throws IOException{
		String[] filenames=sif.getFiles();
		CSVHandler[] files=new CSVHandler[filenames.length];
		if(files.length!=timeColumn.length||files.length!=dataColumns.length)
			return null;
		for(int i=0;i<files.length;i++){
			files[i]=new CSVHandler(new File(filenames[i]), true, separator, timeColumn[i], dataColumns[i]);
		}
		return files;
	}
	
	public File[] getFiles(){
		String[] filenames=sif.getFiles();
		File[] files=new File[filenames.length];
		for(int i=0;i<files.length;i++){
			files[i]=new File(filenames[i]);
		}
		return files;
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

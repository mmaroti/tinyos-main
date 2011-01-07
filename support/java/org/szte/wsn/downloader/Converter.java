package org.szte.wsn.downloader;

import java.io.File;
import java.util.ArrayList;
import java.util.Timer;
import java.util.TimerTask;

import org.szte.wsn.dataprocess.BinaryInterfaceFactory;
import org.szte.wsn.dataprocess.PacketParser;
import org.szte.wsn.dataprocess.PacketParserFactory;
import org.szte.wsn.dataprocess.Transfer;
import org.szte.wsn.dataprocess.file.StringInterfaceFile;

public class Converter {
	
	private static Timer timer=new Timer();
	
	private static ArrayList<Transfer> parsers=new ArrayList<Transfer>();
	
	private static ArrayList<File> outputs=new ArrayList<File>();
	
	private static String csvext,tsext,separator,timeformat;
	
	private static String switchExtension(String fullname, String newEx){
		return fullname.substring(0, fullname.lastIndexOf('.'))+newEx;
	}
	
	public Converter(String file,boolean convertToSI, String csvext, String tsext, String separator, String timeformat){
		Converter.csvext=csvext;
		Converter.tsext=tsext;
		Converter.separator=separator;
		Converter.timeformat=timeformat;
		
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
		parsers.add(fp);
		outputs.add(new File(outputfile));
	}
	
	private static void calculateGlobal(ArrayList<File> outputs2) {
		for(File current:outputs2){
			String tsfile=switchExtension(current.getAbsolutePath(),tsext);
			if(new File(tsfile).exists())
				new GlobalTime(current.getAbsolutePath(), 4, 5, true, 120, csvext,separator,timeformat);
		}		
	}
	
	public static class ParsersRunning extends TimerTask{

		private ParsingReady report;
		@Override
		public void run() {
			boolean running=false;
			for(Transfer parser:parsers){
				if(parser.isAlive()){
					running=true;
					break;
				}
			}
			
			if(!running){
				timer.cancel();
				//The binary->csv parsing is ready
				calculateGlobal(outputs);
				//golobaltime calculation is ready
				report.Ready(outputs);
			}
		}
		
		public ParsersRunning(ParsingReady report){
			this.report=report;
		}
		
	}
	
	public static void waitForParsing(ParsingReady report) {
		timer.scheduleAtFixedRate(new ParsersRunning(report),100,100);	
	}
	
	
	public static void main(String[] args){
		String[] fileNames=new File(".").list();
		boolean convertToSI=true;
		for(String file:fileNames){
			if(file.endsWith(".bin")){
				new Converter(file,convertToSI, ".csv", ".ts", ",", null);
			}
		}
		try{
			Converter.waitForParsing(null);
		}catch(NullPointerException e){
			
		}
		
	}


}

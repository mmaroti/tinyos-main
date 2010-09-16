package org.szte.wsn.downloader;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Timer;
import java.util.TimerTask;

import org.szte.wsn.dataprocess.BinaryInterfaceFactory;
import org.szte.wsn.dataprocess.PacketParserFactory;
import org.szte.wsn.dataprocess.StringInterface;
import org.szte.wsn.dataprocess.StringPacket;
import org.szte.wsn.dataprocess.Transfer;

public class Converter {
	
	private static Timer timer=new Timer();
	
	private static ArrayList<Transfer> parsers=new ArrayList<Transfer>();
	
	private static ArrayList<File> outputs=new ArrayList<File>();
	
	private static ParsingReady readyreport;
	
	private static String switchExtension(String fullname, String newEx){
		return fullname.substring(0, fullname.lastIndexOf('.'))+newEx;
	}
	
	public class FileWrite implements StringInterface {
		boolean convert;
		BufferedWriter output; 
		String tempname,lightname,huminame;
		final static int MEASURE_ERROR=65536;
		
		public FileWrite(File outputfile,boolean convert){
			this.convert=convert;
			try {
				output=new BufferedWriter(new FileWriter(outputfile));
			} catch (IOException e) {
				System.err.println("Can't open output file");
			}
		}
		
		@Override
		public void writePacket(StringPacket packet) {
			int tempnum=-1,huminum=-1,lightnum=-1;
			for(int i=0;i<packet.getFields().length;i++){
				if(packet.getFields()[i]==tempname)
					tempnum=i;
				else if(packet.getFields()[i]==huminame)
					huminum=i;
				else if(packet.getFields()[i]==lightname)
					lightnum=i;
			}
			if(tempnum>0){
				int temp=Integer.parseInt(packet.getFields()[tempnum]);
				if(temp==MEASURE_ERROR)
					packet.getFields()[tempnum]="ERROR";
				else if(convert)
					packet.getFields()[tempnum]=Double.toString(0.01*temp-39.6);
			}
			if(huminum>0){
				int humi=Integer.parseInt(packet.getFields()[huminum]);
				if(humi==MEASURE_ERROR)
					packet.getFields()[huminum]="ERROR";
				else if(convert)
					packet.getFields()[huminum]=Double.toString(-4+0.0405*humi+-2.8000E-6*humi*humi);
			}
			if(lightnum>0){
				int light=Integer.parseInt(packet.getFields()[lightnum]);
				if(light>127&&light<256){
					int s=0xf&light;
					int c=Integer.rotateRight(light-128-s,4);
					int twopowc=(int) Math.pow(2,c);
					light=(int) (Math.floor(16.5*(twopowc-1)))+s*twopowc;
					packet.getFields()[lightnum]=Integer.toString(light);
				} else
					packet.getFields()[lightnum]="ERROR";
			}
			try{
				output.write(packet.getData()[0]);
				for(int i=1;i<packet.getData().length;i++){
					output.write(","+packet.getData()[i]);
				}
			}catch(IOException e){
				System.err.println("Can't write output file");
			}
		}

		@Override
		public StringPacket readPacket() {
			// we don't need this direction
			return null;
		}



		
	}
	
	public Converter(String file,boolean convertToSI){
		File outputfile=new File(switchExtension(file, ".txt"));
		Transfer fp=new Transfer(new PacketParserFactory("structs.txt").getParsers(),
				BinaryInterfaceFactory.getBinaryInterface("file", file),
				new FileWrite(outputfile,convertToSI),
				true);
		fp=new Transfer(file,switchExtension(file, ".txt"));
		fp.start();
		parsers.add(fp);
		outputs.add(outputfile);
	}
	
	public static class ParsersRunning extends TimerTask{

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
				this.cancel();
				readyreport.Ready((File[]) outputs.toArray());
			}
		}
		
	}
	
	public static void waitForParsing(ParsingReady report) {
		timer.scheduleAtFixedRate(new ParsersRunning(),100,100);	
		readyreport=report;
	}
	
	public static void main(String[] args){
		String[] fileNames=new File(".").list();
		boolean convertToSI=true;
		for(String file:fileNames){
			if(file.endsWith(".bin")){
				new Converter(file,convertToSI);
			}
		}
		
	}


}

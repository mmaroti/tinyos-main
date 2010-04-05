import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.io.RandomAccessFile;
import java.io.Writer;
import java.text.DateFormat;
import java.util.ArrayList;
import java.util.Date;
import java.util.HashSet;

public class BinReader {
	ArrayList<DataElement> data=new ArrayList<DataElement>();
	private final byte FRAME=0x5e;
	private final byte ESCAPE=0x5d;
	private final byte XORESCAPE=0x20;
	private dataFile datas;
	private HashSet<Long> gaps;
	
	private ArrayList<Integer> readNextFrame(RandomAccessFile file_input) throws IOException{
		if(file_input.getFilePointer()>=file_input.length())
			return new ArrayList<Integer>();
		int buffer=0;
		while(buffer!=FRAME){//find the first framing byte
			while(gaps.contains(file_input.getFilePointer())){
				file_input.skipBytes(1);
			}
			buffer=file_input.read();
		}
		while(buffer==FRAME){//if there's more than one framing byte next to each other, find the last
			while(gaps.contains(file_input.getFilePointer())){
				file_input.skipBytes(1);
			}
			buffer=file_input.read();
		}
		//now in the buffer we've got the fist byte of the real data
		ArrayList<Integer> onemeas=new ArrayList<Integer>();
		while(buffer!=FRAME){
			if(gaps.contains(file_input.getFilePointer())||file_input.getFilePointer()>=file_input.length())//if there is a gap in the middle of the frame, than drop it, try, the next frame
				return readNextFrame(file_input);
			if(buffer==ESCAPE)
				buffer=file_input.read()^XORESCAPE;
			onemeas.add(buffer);
			buffer=file_input.read();
		}
		return onemeas;
	}
	
	public BinReader(String datafile, String outputfile, boolean convert, boolean converttime, boolean rewrite){
		if(!new File(datafile).exists()){
			System.err.println("Data file doesn't exist");
			System.exit(1);
		}
		if(new File(outputfile).exists()&&!rewrite){
			System.err.println("Output file exists, use --rewrite option to rewrite it");
			System.exit(1);
		} 
		try {
			datas=new dataFile(datafile);
			long timeoffset=0;
			if(datas.getTimestamps().exists()){
				BufferedReader input =  new BufferedReader(new FileReader(datas.getTimestamps()));
				long timestamps=0;
				String line;
				while (( line = input.readLine()) != null){
					String[] dates = line.split(":");
					long locale=Long.parseLong(dates[0]);
					long remote=Long.parseLong(dates[1]);
					timeoffset+=(locale-remote);
					timestamps++;
				}
				timeoffset/=timestamps;
			} else {
				System.out.println("Warning: Timestamp file doesn't exist, timeoffset=0");
			}
			
			gaps=datas.getAllGap();
			RandomAccessFile file_input = datas.dataFile;
			
			while(file_input.getFilePointer()<file_input.length()){
				//System.out.println(file_input.getFilePointer()+"|"+file_input.length());
				ArrayList<Integer> frame=readNextFrame(file_input);
				if(frame.size()==9){
					DataElement de=new DataElement();
					de.temp=frame.get(0)*256+frame.get(1);
					de.humi=frame.get(2)*256+frame.get(3);
					de.light=frame.get(4);
					if(de.light>127&&de.light<256){
						int s=0xf&de.light;
						int c=Integer.rotateRight(de.light-128-s,4);
						int twopowc=(int) Math.pow(2,c);
						de.light=(int) (Math.floor(16.5*(twopowc-1)))+s*twopowc;
					} else
						de.light=0xffff;
					
					de.time=frame.get(5)*16777216L+frame.get(6)*65536L+frame.get(7)*256L+frame.get(8);
					de.time+=timeoffset;
					data.add(de);
					//System.out.println(frame);
				} else 
					System.out.println("Bad frame");
			}
			BufferedWriter output = new BufferedWriter(new FileWriter(outputfile));
			output.write("Time,Temperature,Humidity,Brightness");
			output.newLine();
			for(DataElement de:data){
				String temp=Integer.toString(de.temp);
				String humi=Integer.toString(de.humi);
				String time=Long.toString(de.time);
				if(converttime){
					time=DateFormat.getDateTimeInstance(DateFormat.SHORT, DateFormat.MEDIUM).format(new Date(de.time));
					//time=new Date(de.time).toString();
				}	
				if(convert){
					temp=Double.toString(0.01*de.temp-39.6);
					humi=Double.toString(-4+0.0405*de.humi+-2.8000E-6*de.humi*de.humi);
				}
				System.out.append(time +","+temp+","+humi+","+de.light+"\n");
				output.append(time +","+temp+","+humi+","+de.light);
				output.newLine();
			}
			output.flush();
			output.close();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
	
//	public BinReader(int nodeid,boolean physical){
//		try {
//			datas=new dataFile(nodeid);
//			long timeoffset=0;
//			if(datas.getTimestamps().exists()){
//				BufferedReader input =  new BufferedReader(new FileReader(datas.getTimestamps()));
//				long timestamps=0;
//				String line;
//				while (( line = input.readLine()) != null){
//					String[] dates = line.split(":");
//					long locale=Long.parseLong(dates[0]);
//					long remote=Long.parseLong(dates[1]);
//					timeoffset+=(locale-remote);
//					timestamps++;
//				}
//				timeoffset/=timestamps;
//			} else {
//				System.out.println("Warning: Timestamp file doesn't exist, timeoffset=0");
//			}
//			
//			gaps=datas.getAllGap();
//			RandomAccessFile file_input = datas.dataFile;
//			
//			while(file_input.getFilePointer()<file_input.length()){
//				//System.out.println(file_input.getFilePointer()+"|"+file_input.length());
//				ArrayList<Integer> frame=readNextFrame(file_input);
//				if(frame.size()==9){
//					DataElement de=new DataElement();
//					de.temp=frame.get(0)*256+frame.get(1);
//					de.humi=frame.get(2)*256+frame.get(3);
//					de.light=frame.get(4);
//					if(de.light>127&&de.light<256){
//						int s=0xf&de.light;
//						int c=Integer.rotateRight(de.light-128-s,4);
//						int twopowc=(int) Math.pow(2,c);
//						de.light=(int) (Math.floor(16.5*(twopowc-1)))+s*twopowc;
//					} else
//						de.light=0xffff;
//					
//					de.time=frame.get(5)*16777216L+frame.get(6)*65536L+frame.get(7)*256L+frame.get(8);
//					de.time+=timeoffset;
//					data.add(de);
//					//System.out.println(frame);
//				} else 
//					System.out.println("Bad frame");
//			}
//			for(DataElement de:data){
//				if(physical){
//					double temp=0.01*de.temp-39.6;
//					double humi=-4+0.0405*de.humi+-2.8000E-6*de.humi*de.humi;
//					System.out.println(de.time +":"+temp+":"+humi+":"+de.light);
//				} else
//					System.out.println(de.time +":"+de.temp+":"+de.humi+":"+de.light);
//			}
//		} catch (FileNotFoundException e) {
//			// TODO Auto-generated catch block
//			e.printStackTrace();
//		} catch (IOException e) {
//			// TODO Auto-generated catch block
//			e.printStackTrace();
//		}
//				
//		
//	}
	
	public class DataElement{
		int temp, humi,light;
		long time;
	}
	
	public static void usageThanExit(){
		System.out.println("java BinReader [options] <datafile> <outputfile>");
		System.out.println("options:");
		System.out.println("	--convert: convert the temperature to celsius and the humidity to percent");
		System.out.println("	--converttime: convert the time to human readable format");
		System.out.println("	--rewrite: rewrite the output file if exists");
		System.exit(0);
	}
	
	public static void main(String[] args) throws Exception {
		if(args.length<2||args[args.length-2].startsWith("--")||args[args.length-1].startsWith("--"))
			BinReader.usageThanExit();
		boolean convert=false;
		boolean converttime=false;
		boolean rewrite=false;
		for(int i=0;i<args.length-2;i++){
			if(args[i].equals("--convert"))
				convert=true;
			else if(args[i].equals("--converttime"))
				converttime=true;
			else if(args[i].equals("--rewrite"))
				rewrite=true;
			else
				BinReader.usageThanExit();
		}
		new BinReader(args[args.length-2], args[args.length-1], convert, converttime, rewrite);
	}

}

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.io.RandomAccessFile;
import java.util.ArrayList;
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
	
	public BinReader(int nodeid){
		
		try {
			datas=new dataFile(nodeid);
			BufferedReader input =  new BufferedReader(new FileReader(datas.getTimestamps()));
			long timeoffset=0;
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
			
			gaps=datas.getAllGap();
			RandomAccessFile file_input = datas.dataFile;
			
			while(file_input.getFilePointer()<file_input.length()){
				//System.out.println(file_input.getFilePointer()+"|"+file_input.length());
				ArrayList<Integer> frame=readNextFrame(file_input);
				if(frame.size()==10){
					DataElement de=new DataElement();
					de.temp=0.01*(frame.get(1)*256+frame.get(0))-39.6;
					de.humi=frame.get(3)*256+frame.get(2);
					de.humi=-4+0.0405*de.humi+-2.8000E-6*de.humi*de.humi;
					de.light=frame.get(5)*256+frame.get(4);
					de.time=frame.get(9)*16777216L+frame.get(8)*65536L+frame.get(7)*256L+frame.get(6);
					de.time+=timeoffset;
					data.add(de);
					//System.out.println(frame);
				} else 
					System.out.println("Bad frame");
			}
			for(DataElement de:data){
				System.out.println(de.time +":"+de.temp+":"+de.humi+":"+de.light);
			}
		} catch (FileNotFoundException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
				
		
	}
	
	public class DataElement{
		double temp, humi,light;
		long time;
	}
	
	public static void main(String[] args) throws Exception {
		new BinReader(Integer.parseInt(args[0]));
	}

}

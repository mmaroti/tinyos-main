
import java.io.BufferedReader;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.io.RandomAccessFile;
import java.util.ArrayList;

public class RawPacketConsumer{
	RandomAccessFile dataFile;
	private ArrayList<Byte[]>frames=new ArrayList<Byte[]>();
	private ArrayList<Gap> gaps = new ArrayList<Gap>();
    
	long maxaddress;
	int nodeid;	
	private File gapFile; 	

	public Byte[] readNextFrame(byte[] buffer, int i, byte frame, byte escape, byte xorescaped){
		if(i>=buffer.length)
			return null;
		while(buffer[i]!=frame||i>=buffer.length){//find the first framing byte
			i++;
			while(gaps.contains(i)){
				i++;
			}
		}
		while(buffer[i]==frame||i>=buffer.length){//if there's more than one framing byte next to each other, find the last
			i++;
			while(gaps.contains(i)){
				i++;
			}
		}
		//now in the buffer[i] we've got the fist byte of the real data (after the frame)
		ArrayList<Byte> onemeas=new ArrayList<Byte>();
		while(buffer[i]!=frame){
			if(gaps.contains(i)||i>=buffer.length)//if there is a gap in the middle of the frame, than drop it, try, the next frame
				return readNextFrame(buffer, i, frame, escape, xorescaped);
			if(buffer[i]==escape){
				i++;
				buffer[i]=(byte) (buffer[i]^xorescaped);
			}
			onemeas.add(buffer [i]);
			i++;
		}
		return (Byte[])(onemeas.toArray());
	}
	
	public ArrayList<Byte[]> makeFrames(byte frame, byte escape, byte xorescaped) throws IOException{
		byte[] buffer=new byte[(int) dataFile.length()];//TODO: 2GB limit: is it a problem?
		synchronized (dataFile) {
			dataFile.seek(0);
			dataFile.readFully(buffer);
		}
		int pointer=0;
		ArrayList<Byte[]>ret=new ArrayList<Byte[]>();
		while(pointer<=dataFile.length()){
			Byte[] nextframe=readNextFrame(buffer, pointer, frame, escape, xorescaped);
			if(nextframe!=null)
				ret.add(nextframe);
		}
		return ret;
	}
	
	public RawPacketConsumer(String path,byte frame, byte escape, byte xorescaped) throws IOException{
		if(path.endsWith(".bin")){
			path.lastIndexOf('/');
			nodeid=Integer.parseInt(path.substring(path.lastIndexOf('/')+1, path.length()-4));
			String gapPath=path.substring(0, path.length()-4)+".gap";
			initDataFile(path, gapPath, nodeid);
						
		} else
			throw new FileNotFoundException();
	}
	
	public RawPacketConsumer(String path) throws IOException{
		this(path,(byte)0x5e,(byte)0x5d,(byte)0x20);
		
	}
	private void initDataFile(String path, String gapPath, int nodeid) throws IOException{
			this.dataFile=new RandomAccessFile(path,"r");			
			this.nodeid=nodeid;
			System.out.print("Found datafile from #"+nodeid+". opening file:");
			maxaddress=dataFile.length()-1;
			System.out.print("maxaddress="+maxaddress);
			if(gapFile.exists()){
				BufferedReader input =  new BufferedReader(new FileReader(this.gapFile));
				String line=null;
				while (( line = input.readLine()) != null){
					System.out.print("\n New gap:"+line);
					String[] vars=line.split(" ");
					if(vars.length!=3){
						//TODO error handling
					}
					if(vars[2]=="T")
						addGap(Long.parseLong(vars[0]), Long.parseLong(vars[1]),true);
					else
						addGap(Long.parseLong(vars[0]), Long.parseLong(vars[1]),false);
				}
			}else {
				System.out.print("\nGapfile doesn't exist");
				}
			System.out.println("\nFile opened");
		
	}
	
	private void addGap(long start, long end, boolean unrepairable) {
		Gap newGap = new Gap();
		newGap.setStart(start);
		newGap.setEnd(end);
		newGap.setUnrepairable(unrepairable);
		gaps.add(newGap);
		
	}	
	
	public ArrayList<Gap> getGaps() {
		return gaps;
	}
	public void setGaps(ArrayList<Gap> gaps) {
		this.gaps = gaps;
	}
	public void setFrames(ArrayList<Byte[]> frames) {
		this.frames = frames;
	}
	public ArrayList<Byte[]> getFrames() {
		return frames;
	}
}
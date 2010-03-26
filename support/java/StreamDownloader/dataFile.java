import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.io.RandomAccessFile;
import java.io.Writer;
import java.util.ArrayList;
import java.util.Date;
import java.util.HashSet;

public class dataFile {
	RandomAccessFile dataFile;
	long maxaddress;
	int nodeid;
	private ArrayList<Gap> gaps = new ArrayList<Gap>();
	private File gapFile, timestamps; 

	public File getTimestamps() {
		return timestamps;
	}

	public dataFile(int nodeid) throws IOException{
		File file=new File(String.valueOf(nodeid)+"data.bin");
		gapFile = new File(String.valueOf(nodeid)+"gaps.txt");
		timestamps = new File(String.valueOf(nodeid)+"timestamps.txt");
		if(file.exists()){
			try {
				this.dataFile=new RandomAccessFile(file,"rwd");
			} catch (FileNotFoundException e) {
				// we just checked it, it exists
				e.printStackTrace();
			}
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
			if(!timestamps.exists())
				System.out.print("\nTimestamp file doesn't exist");
			}
			System.out.println("\nFile opened");
		}else {
			maxaddress = -1;
			this.nodeid = nodeid;
			try {
				dataFile = new RandomAccessFile(new File(String.valueOf(nodeid)+"data.bin"),"rwd");
			} catch (FileNotFoundException e) {
				// couldn't happen
				e.printStackTrace();
			}
			gapFile = new File(String.valueOf(nodeid)+"gaps.txt");
			timestamps = new File(String.valueOf(nodeid)+"timestamps.txt");
		}
	}
	
	public void addGap(long start, long end) {
		Gap newGap = new Gap();
		newGap.start = start;
		newGap.end = end;
		newGap.unrepairable = false;
		gaps.add(newGap);
		writeGapFile();
	}
	
	private void addGap(long start, long end, boolean unrepairable) {
		Gap newGap = new Gap();
		newGap.start = start;
		newGap.end = end;
		newGap.unrepairable = unrepairable;
		gaps.add(newGap);
		writeGapFile();
	}

	public void removeGap(int index) {
		gaps.remove(gaps.get(index));
		writeGapFile();
	}
	
	
	public Long[] repairGap(long minaddr){
		Long[] ret=new Long[2];
		ret[0]=new Long(0);
		ret[1]=new Long(0);
		for(Gap repairGap:gaps){
			if(!repairGap.unrepairable){
				if(repairGap.end<minaddr){
					repairGap.unrepairable=true;
					writeGapFile();
				} else{
					ret[1]=repairGap.end;
					if(repairGap.start>=minaddr)
						ret[0]=repairGap.start;
					else
						ret[0]=minaddr;
					break;
				}
			}
		}
		return ret;
	}

	public Gap getGap(int index) {
		return gaps.get(index);
	}
	
	public int getGapNumber(){
		return gaps.size();
	}
	
	public HashSet<Long> getAllGap() {
		HashSet<Long> ret=new HashSet<Long>();
		for(Gap g:gaps){
			for(long i=g.start;i<=g.end;i++)
				ret.add(i);				
		}
		return ret;
	}
	
	private void writeGapFile(){
		try {
			Writer output = new BufferedWriter(new FileWriter(gapFile));
			for(Gap g:gaps){
				output.write(g.start + " " + g.end);
				if(g.unrepairable)
					output.write(" T\n");
				else
					output.write(" F\n");
			}
			output.flush();
			output.close();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
	
	public void addTimeStamp(long local, long remote){
		try {
			Writer output = new BufferedWriter(new FileWriter(timestamps,true));
			output.write(local+":"+remote+"\n");
			output.flush();
			output.close();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
	
	public static class Gap {
		long start, end;
		boolean unrepairable;
	}
}

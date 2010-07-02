/*
* Copyright (c) 2010, University of Szeged
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions
* are met:
*
* - Redistributions of source code must retain the above copyright
* notice, this list of conditions and the following disclaimer.
* - Redistributions in binary form must reproduce the above
* copyright notice, this list of conditions and the following
* disclaimer in the documentation and/or other materials provided
* with the distribution.
* - Neither the name of University of Szeged nor the names of its
* contributors may be used to endorse or promote products derived
* from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
* "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
* FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
* COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
* OF THE POSSIBILITY OF SUCH DAMAGE.
*
* Author:Andras Biro, Miklos Toth
*/
import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.io.RandomAccessFile;
import java.util.ArrayList;

public class RawPacketConsumer{
	private File dataFile;
	private ArrayList<Byte[]>frames=new ArrayList<Byte[]>();
	private ArrayList<Gap> gaps = new ArrayList<Gap>();
    
	private long maxaddress;
	private int nodeid;	

	
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
		FileInputStream filereader=new FileInputStream(dataFile);
		filereader.read(buffer);
		filereader.close();
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
			this.dataFile=new File(path);			
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
		gaps.add(new Gap(start,end,unrepairable));
		
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
	
	public long getMaxaddress() {
		return maxaddress;
	}

	public int getNodeid() {
		return nodeid;
	}

	public File getGapFile() {
		return gapFile;
	}
	
	public File getDataFile() {
		return dataFile;
	}
}
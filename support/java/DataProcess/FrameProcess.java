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
 * Author:Miklos Toth
 */
import java.io.BufferedReader;
import java.io.DataInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FilenameFilter;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.Hashtable;

public class FrameProcess {
	private String[] fileNames;
	private FrameType[] frameTypes;

	public FrameProcess(){

	}
	public void setArgs(String[] args) throws IOException{
		String frameName="";
		int frameSize=0;
		Hashtable<String, Integer> frameId = new Hashtable<String, Integer>();
		Variable[] frameVariables;
		int nrType=0, nrVar=0;
		boolean inType=false;
		frameVariables = new Variable[50];
		for (int i=0;i<50;i++){
			frameVariables[i]=new Variable();
		}
		String[] reszek;


		File path = new File(args[0]);
		if (path.isFile()) {
			fileNames=new String[1];
			fileNames[0]=args[0];
		}
		else if (path.isDirectory()){
			FilenameFilter filter = new FilenameFilter() 
			{ 
				public boolean accept(File path, String name) 
				{
					return name.endsWith(".bin");
				}
			}; 

			fileNames = path.list(filter); 
		}
		else usageThanExit(); //the first argument must be a file or a directory
		if (args.length>1) {
			frameTypes=new FrameType[20];	
			//			try{	}catch (Exception e){
			//				System.out.println("Error: Wrong struct file, no such file."+ e.getMessage());
			//			}
			FileInputStream fstream = new FileInputStream(args[1]);
			DataInputStream in = new DataInputStream(fstream);
			BufferedReader br = new BufferedReader(new InputStreamReader(in));
			String strLine;
			while ((strLine = br.readLine()) != null)   {
				if(strLine.contains("struct")){
					nrVar=0;
					frameSize=0;
					frameId=new Hashtable<String, Integer>();
					frameName="";
					inType=true;
					reszek=strLine.split("struct "); //the word after the struct keyword becomes the name of the FrameType
					if (reszek.length>1){
						if (reszek[1].endsWith("{")){
							reszek[1]=reszek[1].substring(0,reszek[1].length()-1); //eliminates "{" from the end of the FrameType
						}							
						frameName=reszek[1];
					}
					else
						System.out.println("Error: Wrong struct definition, in the line of typedef."); 
				}
				if(strLine.contains("int8_t")){
					if (inType){
						reszek=strLine.split("_t ");
						while(reszek[0].startsWith(" ")){						
							reszek[0]=reszek[0].substring(1);
						}
						while(reszek[1].contains(";")){
							reszek[1]=reszek[1].substring(0, reszek[1].length()-1);
						}
						if(reszek[1].contains("=")){
							String[] tmp=reszek[1].split("=");
							frameId.put(tmp[0],Integer.parseInt(tmp[1].substring(2),16));
							reszek[1]=tmp[0];
						}						
						frameVariables[nrVar]=new Variable(reszek[0]+"_t",reszek[1]);						
						nrVar++;
						frameSize++;

					}
				}
				if(strLine.contains("int16_t")){
					if (inType){
						reszek=strLine.split("_t ");
						while(reszek[0].startsWith(" ")) reszek[0]=reszek[0].substring(1);
						while(reszek[1].contains(";")) reszek[1]=reszek[1].substring(0, reszek[1].length()-1);
						frameVariables[nrVar]=new Variable(reszek[0]+"_t",reszek[1]);						
						nrVar++;
						frameSize+=2;
					}
				}
				if(strLine.contains("int32_t")){
					if (inType){
						reszek=strLine.split("_t ");
						while(reszek[0].startsWith(" ")) reszek[0]=reszek[0].substring(1);
						while(reszek[1].contains(";")) reszek[1]=reszek[1].substring(0, reszek[1].length()-1);
						frameVariables[nrVar]=new Variable(reszek[0]+"_t",reszek[1]);						
						nrVar++;
						frameSize+=4;
					}
				}
				if(strLine.contains("}")){							

					frameTypes[nrType]=new FrameType(frameVariables,frameSize,frameName,frameId);
					//					for (int i=0; i<nrVar; i++){
					//						System.out.println(frameTypes[nrType].getVariables()[i].getType());
					//						System.out.println(frameTypes[nrType].getVariables()[i].getName());
					//					}
					//					System.out.println(frameTypes[nrType].getName());
					//					System.out.println(frameTypes[nrType].getSize());
					//					System.out.println(frameTypes[nrType].getId());
					nrType++;
					inType=false;
				}
			}

			in.close();


		}		
	};
	public static void usageThanExit(){
		System.out.println("java FrameProcess *.bin structs.txt");
		System.out.println("java FrameProcess . structs.txt -scans the actual directory for .bin files");
		System.exit(1);
	}

	public String[] getFileNames() {
		return fileNames;
	}

	public void setFileNames(String[] fileNames) {
		this.fileNames = fileNames;
	}

	public static void main(String[] args) throws IOException {
		FrameProcess fp=new FrameProcess();
		fp.setArgs(args);
		for (int i=0; i<fp.fileNames.length;i++){
			System.out.println("p");
			String path=fp.fileNames[i];
			GapConsumer gp=new GapConsumer(path);
			//RawPacketConsumer rpc= new RawPacketConsumer(path, gp.getGaps());
		}
		//		String gapPath=path.substring(0, path.length()-4)+".gap";
		//		for (int i=0; i<fP.fileNames.length; i++)
		//			System.out.print(fP.fileNames[i]);


	}


}

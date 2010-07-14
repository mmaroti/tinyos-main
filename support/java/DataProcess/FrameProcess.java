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
import java.util.ArrayList;
import java.util.Hashtable;

public class FrameProcess {
	private String[] fileNames;
	private ArrayList<Variable> frameTypes;

	public FrameProcess(){
		frameTypes= new ArrayList<Variable>();
	}
	public void setArgs(String[] args) throws IOException{		
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
			FileInputStream file = new FileInputStream(args[1]);
			byte[] bArray = new byte[file.available ()];
			file.read(bArray);
			file.close ();
			String strLine = new String (bArray);
			String[] words;
			String[] parts = null;			 			//temporary variable for parsing
			words=strLine.split(";");
			int wc=0; 							// word counter	
			while(wc<words.length-1){
			if(!words[wc].contains("struct")){	//simple variable
				words[wc]=words[wc].trim();		//removes the white spaces from the beginning and the end of the phrase
				if(words[wc].contains("int")){  //TODO FIX it
					frameTypes.add(new IntegerVariable(words[wc]));
				}
			}
			else{								//complex type
				ArrayList<Variable> frameVariables;
				String frameName;
				String frameType=words[wc];
				int frameSize=0;
				boolean inType=true;
				frameVariables =new ArrayList<Variable>();
				if(words[wc].contains("{ ")){
				parts=words[wc].split("{ ");
				}
				else{
					parts=new String[1];
					parts[0]=words[wc];
				}
				parts=parts[0].split(" ");
				frameName=parts[parts.length-1];//the last one is the name of type								
				while(inType){
					if(words[wc].contains("{ ")){
						parts=words[wc].split("{ ");
						}
						else{
							parts=new String[1];
							parts[0]=words[wc];
						}
					String var=parts[parts.length-1].trim();				
					if(var.endsWith("}")){
						var=var.substring(0, var.length()-1);
						inType=false;						
					}
					if (var.length()>2){						
						if(var.contains("int")){ //TODO constant/array/size					
							frameVariables.add(new IntegerVariable(var));
							frameSize+=frameVariables.get(frameVariables.size()-1).getSize();
						}
					}					
					wc++;
				}  	//end of struct
				
				frameTypes.add(new Variable(frameVariables,frameType,frameName, frameSize));
			}  		//end of file
			}		//end of struct processing 
			
				
				for (int i=0; i<frameTypes.size(); i++){
					for (int j=0; i<frameTypes.get(i).getStruct().size(); j++){
					System.out.println(frameTypes.get(i).getStruct().get(j).getType());				
				}
				
			}
		
		//TODO new Variable(strLine);			
	}		
};
public static void usageThanExit(){
	System.out.println("java FrameProcess *.bin structs.txt");
	System.out.println("java FrameProcess . structs.txt -scans the actual directory for .bin files");
	System.exit(1);
}
/*
public ArrayList<Record> processFrames(ArrayList<Byte[]> frames){
	ArrayList<Record> ret=new ArrayList<Record>();
	for(int i=0; i<frames.size();i++){
		Byte[] actFrame=frames.get(i);
		int typeCounter=0;                     //goes throw the items of frameType
		while(this.frameTypes.size()>typeCounter){
			// TODO typeCounter++;
			FrameType actType=frameTypes.get(typeCounter);
			if(actFrame.length==actType.getSize()){   //the size of this frame = actFrameType size
				//TODO test ids!!
				ArrayList<Variable> vars=actType.getVariables();
				for(int j=0; j<vars.size();j++){     // goes throw every variable in the record
					Variable actVar =vars.get(j);
					boolean isLittleEndian=false;
					boolean is2complement;
					if (actVar.getType().startsWith("nx_le")){
						isLittleEndian=true;
					}
					if (actVar.getType().contains("uint")){
						is2complement=false;
					}
					else{
						is2complement=true;
					}
					if (actVar.getType().contains("int16_t")){
						byte[] part=new byte[2];
						System.arraycopy(actFrame, 0, part, 0, 2);

					}
				}
			}
		}
	} 

	return ret;
}
*/
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
		//System.out.println("p");
		String path=fp.fileNames[i];
		GapConsumer gp=new GapConsumer(path);
		RawPacketConsumer rpc= new RawPacketConsumer(path, gp.getGaps());
		//fp.processFrames(rpc.getFrames());
		//write out
	}
	//		String gapPath=path.substring(0, path.length()-4)+".gap";
	//		for (int i=0; i<fP.fileNames.length; i++)
	//			System.out.print(fP.fileNames[i]);


}


}

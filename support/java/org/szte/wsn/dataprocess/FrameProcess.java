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
package org.szte.wsn.dataprocess;
import java.io.File;
import java.io.FilenameFilter;
import java.io.IOException;
import java.util.ArrayList;

import org.szte.wsn.dataprocess.file.GapConsumer;
import org.szte.wsn.dataprocess.file.RawPacketConsumer;


public class FrameProcess {
	private String[] sourcePath;
	private boolean fileSource=false;
	private boolean serialSource=false;
	private PacketParser[] packetParsers;
	private BinaryInterface[] readers;
	private StringInterface writer;


	public void setArgs(String[] args){		
		if (args.length<3)    //it isn't necessary to give struct file
			Usage.usageThanExit();

		if(args[0].equals("file")){
			fileSource=true;
			File path = new File(args[1]);
			if (path.isFile()) {

				sourcePath=new String[]{args[1]};
			}
			else if (path.isDirectory()){

				FilenameFilter filter = new FilenameFilter() 
				{ 
					public boolean accept(File path, String name) 
					{
						return name.endsWith(".bin");
					}
				}; 
				sourcePath = path.list(filter); 
			}
			else{
				System.out.println("IO ERROR wrong sourcePath:"+args[1]);
				Usage.usageThanExit();
			}
			//if(args[2].equals(console))

		}
		else if(args[0].equals("serial")){
			serialSource=true;
			sourcePath=new String[]{args[1]};
		}

		else Usage.usageThanExit(); //the first argument must be the type of the source

	};
	public BinaryInterface[] getReaders(){
		if(fileSource){
			ArrayList<RawPacketConsumer> ret=new ArrayList<RawPacketConsumer>();
			for (String path:sourcePath){

				GapConsumer gp;
				try {
					gp = new GapConsumer(path);
					ret.add(new RawPacketConsumer(path, gp.getGaps()));
				} catch (IOException e) {				
					// TODO Auto-generated catch block
					e.printStackTrace();
					Usage.usageThanExit();
				}
			}
			return ret.toArray(new BinaryInterface[ret.size()]); 			
		}
		else 			//if(serialSource)

			return new ToSerial[]{ new ToSerial(sourcePath[0])};
	}


	public static void main(String[] args) throws IOException {
		FrameProcess fp=new FrameProcess();
		fp.setArgs(args);

		fp.packetParsers=new PacketTypes(args.length>2?args[2]:"").getParsers();		

		fp.readers=fp.getReaders();	
		fp.writer=new Console();

		for (PacketParser pp:fp.packetParsers){
		
//			for(BinaryInterface bIF:fp.readers){
			BinaryInterface bIF=fp.getReaders()[0];
//			PacketParser pp=fp.packetParsers[2];
				byte data[]=bIF.readPacket();
				while(data!=null){
					if(pp.parse(data)!=null)
						fp.writer.writePacket(pp,pp.parse(data));
					data=bIF.readPacket();
				}
			}	
		
	}

}

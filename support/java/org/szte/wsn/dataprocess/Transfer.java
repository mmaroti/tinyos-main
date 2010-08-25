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
import org.szte.wsn.dataprocess.string.StringInterfaceFactory;
import org.szte.wsn.dataprocess.string.StringPacket;

/**
 * 
 * @author Miklos Toth
 * Thread that controls communication
 *  between binary and string interfaces
 * 
 */
public class Transfer extends Thread  {
	private PacketParser[] packetParsers;
	private BinaryInterface binary;
	private StringInterface string;
	boolean toString;

	/**
	 * Sets the interfaces from String parameters
	 * @param binaryType
	 * @param binaryPath
	 * @param stringType 
	 * @param stringPath
	 * @param structPath path of the struct file
	 * @param toString if true writes from binary to string, else writes from string to binary 
	 * @param separator the string that separates the data in the output
	 * @param showName controls whether the name of the PacketParser should be written in the file
	 */
	public Transfer(String binaryType, String binaryPath, String stringType, String stringPath, String structPath, boolean toString, String separator,boolean showName){
		packetParsers=new PacketParserFactory(structPath).getParsers();
		binary=BinaryInterfaceFactory.getBinaryInterface(binaryType, binaryPath);	
		string=StringInterfaceFactory.getStringInterface(stringType, stringPath, packetParsers, separator, showName);
		this.toString=toString;
		if((binary==null)||(string==null))
			Usage.usageThanExit();
	}

	/**
	 * Simple constructor to read from a binary file to a string file, uses structs.txt
	 * @param binaryPath
	 * @param stringPath
	 */
	public Transfer(String binaryPath, String stringPath){
		packetParsers=new PacketParserFactory("structs.txt").getParsers();
		binary=BinaryInterfaceFactory.getBinaryInterface("file", binaryPath);	
		string=StringInterfaceFactory.getStringInterface("file", stringPath, packetParsers, ",", true); 
		//separates with "," writes the name of the struct
		this.toString=true;
		if((binary==null)||(string==null))
			Usage.usageThanExit();
	}


	/**
	 * Sets the interfaces from complex parameters
	 * @param packetParsers
	 * @param binary
	 * @param string
	 * @param toString
	 */
	public Transfer(PacketParser[] packetParsers, BinaryInterface binary, StringInterface string, boolean toString){
		this.packetParsers=packetParsers;
		this.binary=binary;
		this.string=string;
		this.toString=toString;
	}

	@Override
	/**
	 * implements the communication in both directions
	 */
	public void run(){
		if(toString){
			byte data[]=binary.readPacket();
			while(data!=null){
				for (PacketParser pp:packetParsers){
					if(pp.parse(data)!=null){
						string.writePacket(new StringPacket(pp.getName(),pp.getFields(),pp.parse(data)));		
					}

				}
				data=binary.readPacket();
			}	
		}
		else{			//other direction
			StringPacket sp=string.readPacket();
			while(sp!=null){
				PacketParser pp=PacketParserFactory.getParser(sp.getName(), packetParsers);

				try {
					if(pp.construct(sp.getData())!=null)
						binary.writePacket(pp.construct(sp.getData()));
				} catch (IOException e) {

					e.printStackTrace();
					Usage.usageThanExit();
				}		

				sp=string.readPacket();
			}

		}	

	}


	public static void main(String[] args) {	

		switch (args.length) {
		case 1:
			File path = new File(args[0]);
			String[] fileNames;
			if (path.isFile()) {
				fileNames=new String[]{args[0]};				
			}
			else if (path.isDirectory()){
				FilenameFilter filter = new FilenameFilter() 
				{ 
					@Override
					public boolean accept(File path, String name) 
					{
						return name.endsWith(".bin");
					}
				}; 
				fileNames = path.list(filter);
			}
			else{
				fileNames=null;
				Usage.usageThanExit();
			}
			for(String file:fileNames){
				Transfer fp=new Transfer(file,file.substring(0,file.length()-5)+".txt");
				fp.start();
			}
			break;

		case 8:
			boolean toStr=false;
			if (args[5].equals("toString"))
				toStr=true;
			boolean show=false;
			if (args[7].equals("showName"))
				show=true;
			Transfer fp1=new Transfer(args[0],args[1],args[2],args[3],args[4],toStr,args[6], show);
			fp1.start();
			break;
		default:
			Usage.usageThanExit();
			break;
		}


	}

}

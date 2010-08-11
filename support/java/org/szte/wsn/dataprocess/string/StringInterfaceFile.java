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
 * Author: Miklos Toth
 */
package org.szte.wsn.dataprocess.string;

import java.io.FileNotFoundException;
import java.io.RandomAccessFile;

import org.szte.wsn.dataprocess.PacketParser;
import org.szte.wsn.dataprocess.PacketParserFactory;
import org.szte.wsn.dataprocess.StringInterface;


/**
 * 
 * @author Miklos Toth
 *	implements StringInterface
 *	writes and reads strings with file
 */
public class StringInterfaceFile implements StringInterface {
	String separator;
	String previous;
	PacketParser[] packetParsers;
	long readPointer;
	RandomAccessFile file;

	public StringInterfaceFile(String separator, String path, PacketParser[] packetParsers ){
		this.separator=separator;  
		this.packetParsers=packetParsers;
		previous="";
		readPointer=0;
		try {
			file=new RandomAccessFile(path, "rw");
		} catch (FileNotFoundException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}	;
	}
	

	@Override
	/**
	 * implements writePacket for file application
	 * 
	 */
	public void writePacket(StringPacket packet) {
		try{			
			if (packet.getData()!=null){
				file.seek(file.length());
				
				if(!packet.getName().equals(previous)){	
					file.writeBytes(packet.getName()+separator);
					for(String head:packet.getFields())
						file.writeBytes(head+separator);
					file.seek(file.getFilePointer()-separator.length()); //deletes the last separator
					file.writeBytes("\n");
				}
				file.writeBytes(packet.getName()+separator);
				for(String data:packet.getData()){			
					file.writeBytes(data+separator);
				}
				file.seek(file.getFilePointer()-separator.length()); //deletes the last separator
				file.writeBytes("\n");
				previous=packet.getName();
			}


		}catch (Exception e){		//Catch exception if any
			System.err.println("Error: " + e.getMessage());
		}
	}


	@Override
	/**
	 * implements readPacket for file application
	 */
	public StringPacket readPacket() {	
		StringPacket ret=null;
		try{
			file.seek(readPointer);
			String line=file.readLine();
			readPointer+=line.length();
			String[] parts=line.split(separator);
			
			String structName=parts[0];
			PacketParser pp=PacketParserFactory.getParser(structName, packetParsers );
			String[] fields;
			if(!pp.getName().equals(previous))
			{
				fields=new String[pp.getFields().length];
				System.arraycopy(parts, 1, fields, 0, parts.length-1);
			}
			else{
				fields=pp.getFields();
			}
			line=file.readLine();
			readPointer+=line.length();
			parts=line.split(separator);
			
			String[] data=new String[pp.getFields().length];
			System.arraycopy(parts, 1, data, 0, parts.length-1);
			
			String temp[]=new String[data.length];
			for(int i=0;i<fields.length;i++)
				temp[i]="";
			for(int j=0;j<fields.length;j++)
				for(int i=0;i<fields.length;i++)
					if(fields[j].equals(pp.getFields()[i]))
						temp[j]=data[i];								
			 ret=new StringPacket(structName,temp);
			

		}catch (Exception e){//Catch exception if any
			System.err.println("Error: " + e.getMessage());
		}
		return ret;
	
	}
}


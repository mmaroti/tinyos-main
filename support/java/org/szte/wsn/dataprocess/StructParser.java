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



import java.util.ArrayList;
import java.util.Arrays;
public class StructParser extends PacketParser {

	private PacketParser[] packetStruct;
	
	/**
	 * sets the name and packetStruct
	 * @param name of the PacketParser struct
	 * @param packetStruct array of PacketParsers
	 */
	public StructParser(String name, PacketParser[] packetStruct){
		this.name=name;
		ArrayList<PacketParser> al=new ArrayList<PacketParser>();
		for(int i=0;i<packetStruct.length;i++){
			al.add(packetStruct[i]);
		}
		this.packetStruct=al.toArray(new PacketParser[al.size()]);
	}
	
	/**
	 * Calls parse for every PacketParser in the struct
	 */
	@Override
	public String[] parse(byte[] packet) {
		ArrayList<String> ret=new ArrayList<String>();		//temporary String[] to return;
		
		int pointer=0; 						//shows which is the first unprocessed byte
				
		for(int i=0;i<packetStruct.length;i++)		//every PacketParser
		{  				
			int length=packetStruct[i].getPacketLength();
			byte[] packetPart=new byte[length];				//bytes of one PacketParser			
			System.arraycopy(packet,pointer,packetPart,0,length);
			
			ret.addAll(Arrays.asList(packetStruct[i].parse(packetPart))); 		
			pointer+=length;
		}
		return ret.toArray(new String[ret.size()]);
	}

	@Override
	/**
	 * Calls getPacketLength for every PacketParser in the struct
	 */
	public int getPacketLength() {
		int ret=0;
		for(int i=0;i<packetStruct.length;i++)		 //every PacketParser		
			ret+=packetStruct[i].getPacketLength();
	
		return ret;
	}
	
	@Override
	/**
	 * Calls getFields for every PacketParser in the struct
	 * @return the names of the fields in String format
	 */
	public String[] getFields() {
		ArrayList<String> ret=new ArrayList<String>(); 
		
		for(int i=0;i<packetStruct.length;i++){  //every PacketParser
			ret.addAll(Arrays.asList(packetStruct[i].getFields()));		
		}
		return ret.toArray(new String[ret.size()]);
	}
	
	@Override
	/**
	 * Calls construct for every PacketParser in the struct
	 * @return the values of the String[] in byte[] format
	 */
	public byte[] construct(String[] stringValue) {
		ArrayList<Byte> ret=new ArrayList<Byte>(); 
		int pointer=0;
		for(int i=0;i<packetStruct.length;i++){ 	 //every PacketParser
			int length=packetStruct[i].getStringLength();
			String[] packetPart=new String[length];				//String of one PacketParser			
			System.arraycopy(stringValue ,pointer,packetPart,0,length);
			for(byte b:packetStruct[i].construct(packetPart))
				ret.add(b); 	
			pointer+=length;
		}
		 byte[] byteArray = new byte[ret.size()];
		    for(int i = 0; i<ret.size(); i++){
		      byteArray[i] = ret.get(i);
		    }
		return byteArray;
	}
	
	@Override
	/**
	 * @return the length of the String[] which is created during parse
	 */
	public int getStringLength() {
		int ret=0;
		for(PacketParser pp:packetStruct)		 //every PacketParser		
			ret+=pp.getStringLength();
		
		return ret;
	}
}

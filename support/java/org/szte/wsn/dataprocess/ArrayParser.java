package org.szte.wsn.dataprocess;
import java.util.ArrayList;
import java.util.Arrays;

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

public class ArrayParser extends PacketParser{
	int size;
	String type;
	PacketParser[] packetArray;	

	public ArrayParser(String name, String type, PacketParser[] packetArray, int size){
		this.name=name;
		this.type=type;
		this.size=size;
		ArrayList<PacketParser> al=new ArrayList<PacketParser>();
		for(int i=0;i<packetArray.length;i++){
			al.add(packetArray[i]);
		}
		this.packetArray=al.toArray(new PacketParser[al.size()]);
	}
	@Override
	public String[] parse(byte[] packet) {
		String[] ret=new String[size];
		int packetLength=(int)packet.length/size;
		PacketParser pp=PacketParserFactory.getPacketParser(packetArray, name, type);
		for(int i=0;i<size;i++){
			byte[] packetPart =new byte[packetLength];
			System.arraycopy(packet, i*packetLength, packetPart, 0, packetLength);						
			ret[i]=pp.parse(packetPart)[0];
			}
		return ret;
	}

	@Override
	public int getPacketLength() {
		PacketParser pp=PacketParserFactory.getPacketParser(packetArray,"", type);
			return pp.getPacketLength()*size;
	}

	@Override
	public String[] getFields() {
		ArrayList<String> ret;		//temporary String[] to return;
		ret=new ArrayList<String>(); 		
		for(int i=0;i<size;i++){  //every PacketParser
			PacketParser pp=PacketParserFactory.getPacketParser(packetArray, i+". "+type, type);
			ret.addAll(Arrays.asList(pp.getFields()));		
		}
		return ret.toArray(new String[ret.size()]);
	}

}

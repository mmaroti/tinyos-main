import java.util.ArrayList;

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
public class StructParser extends PacketParser {

	private PacketParser[] packetStruct;
	
	/**
	 * Calls parse for every PacketParser in the struct
	 */
	@Override
	public String[] parse(byte[] packet) {
		ArrayList<String> ret;						//temporary String[] to return;
		String[] stringParts;						//String[] of one PacketParser
		byte[] packetPart;							//bytes of one PacketParser
		packetPart=new byte[packet.length];	
		int pointer=0; 					//shows which is the first unprocessed byte
		ret=new ArrayList<String>(); 
		for(int i=0;i<packetStruct.length;i++){  //every PacketParser
			int length=packetStruct[i].getPacketLength();
			System.arraycopy(packet,pointer,packetPart,0,length);
			stringParts=packetStruct[i].parse(packetPart);
			for(int j=0;j<stringParts.length;j++){  //every words of the PacketParser
				ret.add(stringParts[j]);
			}
			pointer+=length;
		}
		String[] retur = new String[ret.size()];
		return ret.toArray(retur);
	}

	@Override
	/**
	 * Calls getPacketLength for every PacketParser in the struct
	 */
	public int getPacketLength() {
		int ret=0;
		for(int i=0;i<packetStruct.length;i++){  //every PacketParser
		ret+=packetStruct[i].getPacketLength();
		}
		return ret;
	}
	
	@Override
	/**
	 * Calls getFields for every PacketParser in the struct
	 * @return the types of the fields in String format
	 */
	public String[] getFields() {
		ArrayList<String> ret;		//temporary String[] to return;
		String[] stringParts;		//String[] of one PacketParser
		ret=new ArrayList<String>(); 
		for(int i=0;i<packetStruct.length;i++){  //every PacketParser
			stringParts=packetStruct[i].getFields();
			for(int j=0;j<stringParts.length;j++){  //every words of the PacketParser
				ret.add(stringParts[j]);
			}			
		}
		String[] retur = new String[ret.size()];
		return ret.toArray(retur);
	}
}

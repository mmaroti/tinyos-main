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

import org.szte.wsn.dataprocess.parser.*;
/**
 * 
 * @author Mikos Toth
 * Factory class for the PacketParser interface
 *
 */
public class PacketParserFactory {

	/**
	 * 
	 * @param packetArray existing PacketParsers
	 * @param name param of the new PacketParser
	 * @param type param of the new PacketParser
	 * @return new PacketParser according to the parameters,
	 *  null if the type doesn't fit on any available PacketParser
	 */
	public static PacketParser getPacketParser(PacketParser[] packetArray, String name, String type){
		int pos=contains(packetArray,type);
		if((name.contains("="))&&(type.contains("int"))){
			String[] parts=name.split("=");
			return new ConstParser(parts[0], type, parts[1]);
		}
		else if(pos>-1){
			return packetArray[pos];
		}	
		else if(name.contains("[")){
			//size of the array
			int size=Integer.parseInt(name.substring(name.indexOf("[")+1,name.indexOf("]")));

			return new ArrayParser(getPacketParser(packetArray, name.substring(0,name.indexOf("[")), type), size);  //deletes the [n] tag to avoid recursion 
		}
		else if(type.contains("int"))
		{ 		
			return new IntegerParser(name, type);
		}
		else
			return null;
	}
	/**
	 * 
	 * @param packetArray already existing PacketParsers
	 * @param type searched type
	 * @return the position of this type in the PacketArray, 
	 * or -1 if it isn't in it
	 */
	public static int contains(PacketParser[] packetArray,String type) {
		String parts[]=type.split(" ");
		for(int i=0;i<packetArray.length;i++)
			if(packetArray[i].getName().equals(parts[parts.length-1]))
				return i;
		return -1;
	}
}

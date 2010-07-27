package org.szte.wsn.dataprocess;
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

public class PacketParserFactory {
	public static PacketParser getPacketParser(PacketParser[] packetArray,String name, String type){
		int pos=contains(packetArray,type);
		if(name.contains("=")){
			String[] parts=name.split("=");
			return new ConstParser(parts[0], type, parts[1]);
		}
		else if(pos>-1){
			return packetArray[pos];
		}	
		else if(name.contains("[")){
			int size=Integer.parseInt(name.substring(name.indexOf("[")+1,name.indexOf("]")));
			return new ArrayParser(name, type, packetArray,size);
		}
		else	//if(type.contains("int"))
		{ 		
			return new IntegerParser(name, type);
		}
	}

	public static int contains(PacketParser[] packetArray,String type) {
		for(int i=0;i<packetArray.length;i++)
			if(packetArray[i].getName().equals(type))
				return i;
		return -1;
	}
}

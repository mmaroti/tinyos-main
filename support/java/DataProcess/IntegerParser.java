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
public class IntegerParser extends PacketParser{
	String type;
	boolean isLittleEndian=false;
	boolean is2complement;
	/**
	 * sets the flags according to the type
	 * @param name integer variable name
	 * @param type integer variable type
	 */
	public IntegerParser(String name, String type){
		this.name=name;
		this.type=type;
		if (type.startsWith("nx_le")){
			isLittleEndian=true;
		}
		if (type.contains("uint")){
			is2complement=false;
		}
		else{
			is2complement=true;
		}		
	}
	
	@Override
	/**
	 * @return the value of the integer at the first place of the String array
	 */
	public String[] parse(byte[] packet) {
		long ret=0;		//the mask is needed to cut the leading sign bits the java added
		for(int i=0;i<packet.length;i++){
			if(isLittleEndian)
				ret|=(packet[i] << i*8)&(0xff<<(i*8));
			else
				ret|=packet[i] << ((packet.length-i-1)*8)&((0xff<<(packet.length-i-1)*8));
		}
		boolean negative=false;
		if(is2complement){
			if(isLittleEndian){
				if(packet[packet.length-1]<0)
					negative=true;
			} else {
				if(packet[0]<0)
					negative=true;
			}
		}
		if(negative)
			ret=(-1&~((1<<packet.length*8)-1))|ret; //add the missing leading sign bits
		String[] strArray;
		strArray= new String[1];
		strArray[0]=""+ret;
		return strArray;
	}

	@Override
	/**
	 * @return the size of the integer
	 */
	public int getPacketLength() {
		if(this.type.contains("int8_t")){
			return 1;
		}
		if(this.type.contains("int16_t")){
			return 2;
		}
		if(this.type.contains("int32_t")){
			return 4;
		}
		return 0;
	}

	@Override
	/**
	 * @return type
	 */
	public String[] getFields() {
		String[] ret;
		ret =new String[1];
		ret[0]=type;
		return ret;
	}

}

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
package org.szte.wsn.dataprocess;

import java.util.ArrayList;
import java.io.Console;

public class Consol implements StringInterface {
	String separator;
	String[] header;
	
	public Consol(String[] header){
		this.header=header;
		separator="	";
	}

	@Override
	public void writePackets(ArrayList<String[]> parsedData) {
		writePacket(parsedData.get(0));				//writes the header file and the first line
		
		for(int i=1;i<parsedData.size();i++){
			for(String data:parsedData.get(i))
				System.out.print(data+separator);
		}
			
		
	}

	@Override
	public void writePacket(String[] parsedData) {
		for(String head:header)
			System.out.print(head+separator);
		if (header!=null)
			System.out.println();
		for(String data:parsedData){			
				System.out.print(data+separator);
		}
		
	}

	@Override
	public ArrayList<String[]> readPackets() {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public String[] readPacket() {
		String[] ret=new String[header.length];
		Console console = System.console();

		for(int i=0; i<header.length;i++)
			ret[i]=console.readLine();
		return ret;
	}

}

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
import java.util.Scanner;

import org.szte.wsn.dataprocess.StringInterface;

/**
 * 
 * @author Miklos Toth
 *	implements StringInterface
 *	writes and reads Console
 */
public class StringInterfaceConsole implements StringInterface {
	String separator;
	String previous;

	public StringInterfaceConsole(String separator){
		this.separator=separator;
		previous="";
	}
	public StringInterfaceConsole(){
		this("	");      		//default separator is Tabulator
	}

	@Override
	/**
	 * implements writePacket for Console application
	 * 
	 */
	public void writePacket(StringPacket packet) {
		if (packet.getData()!=null){			
			if(!packet.getName().equals(previous)){
				System.out.print(packet.getName()+separator);
			for(String head:packet.getFields())
				System.out.print(head+separator);

			System.out.println();			
			}
			System.out.print(packet.getName()+separator);
			for(String data:packet.getData()){			
				System.out.print(data+separator);
			}
			System.out.println();
		previous=packet.getName();
		}
	}


	@Override
	/**
	 * implements readPacket for Console application
	 */
	public StringPacket readPacket() {
		/*
		String[] ret=new String[parser.getFields().length];
		Scanner in = new Scanner(System.in);

		for(int i=0; i<parser.getFields().length;i++)
			ret[i]=in.nextLine();
		in.close();  
		*/
		return new StringPacket();
	}

}

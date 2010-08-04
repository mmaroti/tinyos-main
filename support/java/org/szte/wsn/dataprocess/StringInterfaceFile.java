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
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import java.util.Scanner;

/**
 * 
 * @author Miklos Toth
 *	implements StringInterface
 *	writes and reads strings with file
 */
public class StringInterfaceFile implements StringInterface {
	String separator;
	String filePath;
	PacketParser previous;

	public StringInterfaceFile(String separator, String path){
		this.separator=separator;  
		filePath=path;
		previous=null;
	}
	public StringInterfaceFile(){
		this("	","default.txt");      		//default separator is Tabulator
	}

	@Override
	/**
	 * implements writePacket for file application
	 * 
	 */
	public void writePacket(PacketParser parser,String[] parsedData) {
		try{
			BufferedWriter out = new BufferedWriter(new FileWriter(filePath));

			if (parsedData!=null){
				out.write(parser.getName()+separator);
				if(!parser.equals(previous)){					
					for(String head:parser.getFields())
						out.write(head+separator);
					out.write("\n");
				}
				out.write(parser.getName()+separator);
				for(String data:parsedData){			
					out.write(data+separator);
				}
				out.write("\n");
			}

			out.close();

		}catch (Exception e){		//Catch exception if any
			System.err.println("Error: " + e.getMessage());
		}
	}


	@Override
	/**
	 * implements readPacket for file application
	 */
	public String[] readPacket(PacketParser parser) {
		String[] ret=new String[parser.getFields().length];

		try{
			Scanner in=new Scanner(new File(filePath) );
			for(int i=0; i<parser.getFields().length;i++)
				ret[i]=in.nextLine();
			in.close(); 
		}catch (Exception e){//Catch exception if any
			System.err.println("Error: " + e.getMessage());
		}
		return ret;
	}

}


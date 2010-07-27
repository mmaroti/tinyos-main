package org.szte.wsn.dataprocess;


import java.io.FileInputStream;
import java.io.IOException;
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
public class PacketTypes {	
	PacketParser[] packetParsers;
	/**
	 * if no structure file name is provided, structs.txt
	 *  will be loaded from the current directory
	 */
	public PacketTypes(){
		this("structs.txt");
	}
	/**
	 * 
	 * @param fileName location of configuration file
	 */
	public PacketTypes(String fileName){		
		loadConfig(fileName);
	}
	/**
	 * 
	 * @param fileName loads configuration from the fileName
	 */
	void loadConfig(String fileName){
		
		ArrayList<PacketParser> returnArray=new ArrayList<PacketParser>();
		
		FileInputStream file;
		byte[] bArray=null; 
		
		try {
			file = new FileInputStream(fileName);
			bArray = new byte[file.available ()];
			file.read(bArray);
			file.close ();
		} 
		catch (IOException e) {
			
				System.out.println("IO ERROR while opening: "+fileName);
				e.printStackTrace();
				System.exit(1);			
		}

		String strLine = new String (bArray);
		String[] words;

		words=strLine.split(";");					//separates the input into commands
		int wc=0; 									// word counter	
		while(wc<words.length-1){
			words[wc]=words[wc].trim();				//removes the white spaces from the beginning and the end of the phrase
			if(!words[wc].contains("struct")){			//simple variable

				String[] parts=words[wc].split(" ");
				String parserName=parts[parts.length-1];
				String parserType=words[wc].substring(0, (words[wc].length()-parserName.length())).trim();
				PacketParser pp=PacketParserFactory.getPacketParser(returnArray.toArray(new PacketParser[returnArray.size()]), parserName, parserType);
				if(pp!=null)						
					returnArray.add(pp);
				else{
					System.out.println("Error: not existing type: \""+parserType+"\" in "+ fileName);
					System.exit(1);
				}
			}
			else{
				ArrayList<PacketParser> variableArray=new ArrayList<PacketParser>();
				String[] parts=words[wc].split(" ");
			    
				String parserName=parts[1].replaceAll("[^\\w]", "");;
			    
				parts=words[wc].split("\\{");
				words[wc]=parts[1];
				while((wc<words.length)&&(!words[wc].contains("}"))){
					
					words[wc]=words[wc].trim();
					parts=words[wc].split(" ");
					String variableName=parts[parts.length-1];
					String variableType=words[wc].substring(0, (words[wc].length()-variableName.length())).trim();
					
					PacketParser pp=PacketParserFactory.getPacketParser(returnArray.toArray(new PacketParser[returnArray.size()]), variableName, variableType);
					if(pp!=null)						
						variableArray.add(pp);
					else{
						System.out.println("Error: not existing type: \""+variableType+"\" in "+ fileName);
						System.exit(1);
					}
						
					wc++;
					
				}				
				returnArray.add(new StructParser(parserName, variableArray.toArray(new PacketParser[variableArray.size()])));						
			}
			wc++;
			
			
		}//while ends here
		packetParsers=returnArray.toArray(new PacketParser[returnArray.size()]);
	}
	
	/**
	 * 
	 * @return returns the PacketParsers which are available 
	 */
	PacketParser[] getParsers(){				
		return packetParsers;
	}

	/**
	 * 
	 * @param name returns the PacketParser from the packetParsers array
	 *  which has the same name
	 * @return PacketParser
	 */
	PacketParser getParser(String name){
		for(int i=0;i<packetParsers.length;i++){
			if(packetParsers[i].getName()==name)
				return packetParsers[i];
		}			
		return null;
	}
}
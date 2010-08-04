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

import java.io.IOException;


public class FrameProcess {
	private PacketParser[] packetParsers;
	private BinaryInterface reader;
	private StringInterface writer;
	
	public static void main(String[] args) throws IOException {			//handle exception
		FrameProcess fp=new FrameProcess();

		fp.packetParsers=new PacketTypes(args.length>4?args[4]:"").getParsers();		
		
		if(args.length<4)
			Usage.usageThanExit();
		fp.reader=BinaryInterfaceFactory.getBinaryInterface(args[0], args[1]);	
		fp.writer=StringInterfaceFactory.getStringInterface(args[2], args[3]);
		if((fp.reader==null)||(fp.writer==null))
			Usage.usageThanExit();		

		byte data[]=fp.reader.readPacket();
		while(true){
			for (PacketParser pp:fp.packetParsers){
				if(pp.parse(data)!=null)
					fp.writer.writePacket(pp,pp.parse(data));				
			}
			data=fp.reader.readPacket();
		}	

	}

}

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
* Author: Andras Biro
*/
package org.szte.wsn.dataprocess;

import java.io.IOException;
import java.util.ArrayList;
import net.tinyos.packet.BuildSource;
import net.tinyos.packet.PacketListenerIF;
import net.tinyos.packet.PhoenixSource;
import net.tinyos.util.PrintStreamMessenger;

public class ToSerial implements BinaryInterface{
	
	private PhoenixSource phoenix;
	private ArrayList<byte[]> readbuffer=new ArrayList<byte[]>();
	
	public class Listener implements PacketListenerIF{
		@Override
		public void packetReceived(byte[] packet) {
			readbuffer.add(packet);
		}		
	}
	
	public ToSerial(String source){
		//TODO: Maybe we should process the error messages
		if (source == null) {
			phoenix = BuildSource.makePhoenix(PrintStreamMessenger.err);
		} else {
			phoenix = BuildSource.makePhoenix(source, PrintStreamMessenger.err);
		}
		phoenix.registerPacketListener(new Listener());		
		phoenix.setResurrection();
		phoenix.start();
	}
	
	
	public ToSerial(){
		new ToSerial(null);
	}

	@Override
	public byte[] readPacket() {
		if(!readbuffer.isEmpty()){
			byte[] ret =readbuffer.get(0);
			readbuffer.remove(0);
			return ret;
		} else
			return null;
	}

	@Override
	public void writePacket(byte[] frame) throws IOException {
		phoenix.writePacket(frame);
		
	}
	
	public static void main(String[] args){
		ToSerial ts=new ToSerial(args[0]);
		while(true){
			byte[] buffer=ts.readPacket();
			if(buffer!=null){
				System.out.println(buffer);
			}
		}
	}

}

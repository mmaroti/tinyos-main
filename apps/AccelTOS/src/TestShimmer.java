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
* Author: Miklos Maroti
*/

import net.tinyos.packet.*;
import net.tinyos.util.PrintStreamMessenger;
import java.io.*;
import java.util.*;

public class TestShimmer implements PacketListenerIF
{
	protected java.text.SimpleDateFormat timestamp = new java.text.SimpleDateFormat("HH:mm:ss");

	static final int PACKET_TYPE_FIELD = 7;
	static final int PACKET_LENGTH_FIELD = 5;
	static final int PACKET_DATA_FIELD = 8;

	static final byte AM_TYPE = (byte)0x37;
	static final int CHANNEL_COUNT = 5;

	protected PhoenixSource forwarder;
	
	public TestShimmer(PhoenixSource forwarder)
	{
		this.forwarder = forwarder;
		forwarder.registerPacketListener(this);
	}

	public void run()
	{
		forwarder.run();
	}

	public static void main(String[] args) throws Exception 
	{
		PhoenixSource phoenix = null;

		if( args.length == 0 )
			phoenix = BuildSource.makePhoenix(PrintStreamMessenger.err);
		else if( args.length == 2 && args[0].equals("-comm") )
			phoenix = BuildSource.makePhoenix(args[1], PrintStreamMessenger.err);
		else
		{
			System.err.println("usage: java TestShimmer [-comm <source>]");
			System.exit(1);
		}

		TestShimmer listener = new TestShimmer(phoenix);
		listener.run();
	}

	byte[] packet;
	int head, end;

	protected void check(int len) throws Exception
	{
		if( head + len > end )
			throw new Exception("illegal message format");
	}
	
	protected byte getInt8() throws Exception 
	{
		check(1);
		byte ret = packet[head];
		head += 1;
		return ret;
	}
	
	protected int getUInt8() throws Exception
	{
		return getInt8() & 0xFF;
	}

	protected short getInt16() throws Exception 
	{
		short a,b;
		check(2);
		
		a = packet[head];            a &= 0x00FF;
		b = packet[head+1]; b <<= 8; b &= 0xFF00; a |= b;
		
		head += 2;
		return a;
	}
	
	protected int getUInt16() throws Exception
	{
		return getInt16() & 0xFFFF;
	}

	protected int getInt32() throws Exception 
	{
		int a,b;
		check(4);
		
		a = packet[head];             a &= 0x000000FF;
		b = packet[head+1]; b <<= 8;  b &= 0x0000FF00; a |= b;
		b = packet[head+2]; b <<= 16; b &= 0x00FF0000; a |= b;
		b = packet[head+3]; b <<= 24; b &= 0xFF000000; a |= b;
		
		head += 4;
		return a;
	}

	protected long getUInt32() throws Exception
	{
		return ((long)getInt32()) & 0x00000000FFFFFFFF;
	}

	public void packetReceived(byte[] packet) 
	{
		if( packet[PACKET_TYPE_FIELD] == AM_TYPE )
		{
			try 
			{
				this.packet = packet;

				head = PACKET_DATA_FIELD;
				end = PACKET_DATA_FIELD + packet[PACKET_LENGTH_FIELD];

				if( end < head || end > packet.length  )
					throw new Exception("illegal message length");

				while( head < end )
				{
					String line = timestamp.format(new java.util.Date()) + "," + getUInt32();
					
					for(int i = 0; i < CHANNEL_COUNT; ++i)
						line += "," + getUInt16();

					System.out.println(line);
				}

				if( head != end )
					throw new Exception("illegal number of field");
			}
			catch(Exception e) 
			{
				System.out.println(e.getMessage());
			}
		}
	}
}

/*
 * Copyright (c) 2003-2007, Vanderbilt University
 * All rights reserved.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose, without fee, and without written agreement is
 * hereby granted, provided that the above copyright notice, the following
 * two paragraphs and the author appear in all copies of this software.
 * 
 * IN NO EVENT SHALL THE VANDERBILT UNIVERSITY BE LIABLE TO ANY PARTY FOR
 * DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
 * OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF THE VANDERBILT
 * UNIVERSITY HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 * THE VANDERBILT UNIVERSITY SPECIFICALLY DISCLAIMS ANY WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS
 * ON AN "AS IS" BASIS, AND THE VANDERBILT UNIVERSITY HAS NO OBLIGATION TO
 * PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
 * 
 * Author: Miklos Maroti
 */

import net.tinyos.packet.*;
import net.tinyos.util.PrintStreamMessenger;
import java.io.*;
import java.util.*;

public class EchoRanger implements PacketListenerIF
{
	protected java.text.SimpleDateFormat timestamp = new java.text.SimpleDateFormat("HH:mm:ss");

	static final int PACKET_LENGTH_FIELD = 5;
    static final int PACKET_TYPE_FIELD = 7;
    static final int PACKET_DATA_FIELD = 8;
    static final byte AM_ECHORANGER_MSG = (byte)0x77;
    
    protected PhoenixSource forwarder;
    
    public EchoRanger(PhoenixSource forwarder)
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
			System.err.println("usage: java TestFastSerial [-comm <source>]");
			System.exit(1);
		}

		EchoRanger listener = new EchoRanger(phoenix);
		listener.run();
	}

	boolean resync = false;
	int[] samples = new int[0];

	int getWord(byte[] packet, int index)
	{
		return (packet[index] & 0xFF) + ((packet[index+1] & 0xFF) << 8);
	}

	public void packetReceived(byte[] packet) 
    {
        if( packet[PACKET_TYPE_FIELD] != AM_ECHORANGER_MSG )
		{
			System.out.println("incorrect msg format");
			return;
		}
	
		int length = (getWord(packet, PACKET_LENGTH_FIELD) - 2) / 2;
		int start = getWord(packet, PACKET_DATA_FIELD);

		if( start != samples.length && resync == false )
		{
			System.out.println("missing samples");
			resync = true;
			samples = new int[0];
		}
		else if( start == samples.length )
			resync = false;

		int[] newSamples = new int[samples.length + length];
		System.arraycopy(samples, 0, newSamples, 0, samples.length);
		samples = newSamples;

		for(int i = 0; i < length; ++i)
			samples[start + i] = getWord(packet, PACKET_DATA_FIELD + 2 + 2*i);

		if( samples.length == 400 )
		{
			report(samples);
			samples = new int[0];
		}
    }

	public void report(int[] samples)
	{
		System.out.print(timestamp.format(new java.util.Date()));
		for(int i = 0; i < samples.length; ++i)
			System.out.print("," + samples[i]);

		System.out.println();
	}
}

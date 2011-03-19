/*
* Copyright (c) 2009, University of Szeged
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
import net.tinyos.util.*;
import java.util.*;

public class RF230RipsEngine implements PacketListenerIF
{
	static final int RIPS_BEACON_COUNT = 10;
	static final int RIPS_HISTORY_SIZE = 9;
	static final int[] RIPS_NODES = { 1, 2, 3, 4 };

	public static void main(String[] args) throws Exception 
	{
		PhoenixSource phoenix = null;

		if( args.length == 0 )
		{
//			phoenix = BuildSource.makePhoenix(PrintStreamMessenger.err); 
			phoenix = BuildSource.makePhoenix("serial@com8:57600", PrintStreamMessenger.err);
		}
		else if( args.length == 2 && args[0].equals("-comm") )
			phoenix = BuildSource.makePhoenix(args[1], PrintStreamMessenger.err);
		else
		{
			System.err.println("usage: java RF230RipsEngine [-comm <source>]");
			System.exit(1);
		}

		RF230RipsEngine listener = new RF230RipsEngine(phoenix);
		listener.run();
	}

	static class Measurement
	{
		Measurement(byte[] packet, int beaconId)
		{
			measurementId = getStartMsgMeasurementId(packet);
			beaconNode = getStartMsgBeaconNode(packet);
			assist1Node = getStartMsgAssist1Node(packet);
			assist2Node = getStartMsgAssist2Node(packet);
			assist1Tuning = getStartMsgAssist1Tuning(packet);
			assist2Tuning = getStartMsgAssist2Tuning(packet);
			this.beaconId = beaconId;
			
			data = new int[RIPS_NODES.length];
			for(int i = 0; i < data.length; ++i)
				data[i] = -1;
		}

		void setData(int nodeid, int value)
		{
			for(int i = 0; i < RIPS_NODES.length; ++i)
			{
				if( RIPS_NODES[i] == nodeid )
				{
					if( data[i] == -1 )
						data[i] = value;
					else if( data[i] != value )
						System.out.println("Inconsistent measurement data");

					return;
				}
			}
			
			System.err.println("Unknown nodeid: " + nodeid);
		}
		
		int measurementId;
		int beaconNode;
		int assist1Node;
		int assist2Node;
		int assist1Tuning;
		int assist2Tuning;
		int beaconId;
		int[] data;
		
		public String toString()
		{
			String s = "" + measurementId + "\t" + beaconNode + "\t" +  assist1Node
				+ "\t" + assist2Node + "\t" + assist1Tuning + "\t" + assist2Tuning
				+ "\t" + beaconId;
			
			for(int i = 0; i < data.length; ++i)
				s += "\t" + data[i];
			
			return s;
		}
	};
	
	void processMeasurement(Measurement measurement)
	{
		System.out.println(measurement);
	}
	
	LinkedList<Measurement> measurements = new LinkedList<Measurement>();

	void readStartMessage(byte[] packet)
	{
		int measurementId = getStartMsgMeasurementId(packet);
	
		if( ! measurements.isEmpty() )
		{
			int last = measurements.getLast().measurementId;
			byte offset = (byte)(measurementId - last);
			
			if( offset <= 0 || offset > 10 )
			{
				System.err.println("Too big jump from " + last + " to " 
						+ measurementId + " in measurement ids");

				measurements.clear();
			}
		}

		for(int beaconId = 0; beaconId < RIPS_BEACON_COUNT; ++beaconId)
				measurements.add(new Measurement(packet, beaconId));
		
		while( measurements.size() > RIPS_BEACON_COUNT * RIPS_HISTORY_SIZE )
			processMeasurement(measurements.removeFirst());
	}

	Measurement getMeasurement(int measurementId, int beaconId)
	{
		for(Measurement measurement : measurements)
		{
			if( measurement.measurementId == measurementId
					&& measurement.beaconId == beaconId )
				return measurement;
		}
		
		return null;
	}
	
	
	void readBeaconMessage(byte[] packet)
	{
		int measurementId = getBeaconMsgMeasurementId(packet);
		int beaconId = getBeaconMsgBeaconId(packet);

		if( ! measurements.isEmpty() && measurementId != measurements.getLast().measurementId )
			System.err.println("Beacon message for not the last measurement");

		Measurement reporter = getMeasurement(measurementId, beaconId);
		if( reporter == null )
		{
			System.err.println("Beacon message without a start message " + measurementId);
			return;
		}
		
		byte[] data = getBeaconMsgPreviousData(packet);
		for(int i = 0; i < data.length; ++i)
		{
			Measurement measurement = getMeasurement((measurementId-1- i) & 0xFF, beaconId);
			if( measurement == null )
			{
				if( measurements.size() == RIPS_BEACON_COUNT * RIPS_HISTORY_SIZE )
					System.err.println("No matching measurement for history data");
				
				continue;
			}
			
			measurement.setData(reporter.beaconNode, data[i] & 0xFF);
		}
	}
	
	public void packetReceived(byte[] packet) 
	{
		if( isDiagMsgOverflow(packet) )
			System.err.println("DiagMsgOverflow");
		else if( isPacketSnifferMessage(packet) )
		{
			byte[] payload = getPacketSnifferPayload(packet);
			
			if( isStartMessage(payload) )
				readStartMessage(payload);
			else if( isBeaconMessage(payload) )
				readBeaconMessage(payload);
			else if( payload.length == 3 )
				;
			else
				System.err.println("Non-RIPS message is received");
		}
		else
			System.err.println("Incorrect message received, please use RF230Sniffer");
	}

	protected PhoenixSource forwarder;
	
	public RF230RipsEngine(PhoenixSource forwarder)
	{
		this.forwarder = forwarder;
		forwarder.registerPacketListener(this);
	}

	public void run()
	{
		forwarder.run();
	}

// ------- SerialPacket
	
	static int getByte(byte[] packet, int index) 
	{
		return packet[index] & 0xFF;
	}
	
	static int getShort(byte[] packet, int index) 
	{
		return (packet[index] & 0xFF)
			| (packet[index+1] & 0xFF) << 8;
	}

	static int getInt(byte[] packet, int index) 
	{
		return (packet[index] & 0xFF)
			| (packet[index+1] & 0xFF) << 8
			| (packet[index+2] & 0xFF) << 16
			| (packet[index+3] & 0xFF) << 24;
	}

	static int getTargetAddress(byte[] packet) { return getShort(packet, 1); }
	static int getSourceAddress(byte[] packet) { return getShort(packet, 3); }
	static int getPacketLength(byte[] packet) { return getByte(packet, 5); }
	static int getActiveMessageType(byte[] packet) { return getByte(packet, 7); }
	
	static byte[] getPayload(byte[] packet)
	{
		byte[] payload = new byte[packet.length - 8];
		System.arraycopy(packet, 8, payload, 0, payload.length);
		return payload;
	}
	
// ------- PacketSniffer/DiagMsg

	static boolean isDiagMsgOverflow(byte[] packet)
	{
		byte[] payload = getPayload(packet);
		
		return getActiveMessageType(packet) == 0xB1
			&& getByte(payload, 0) == 0x0F
			&& getByte(payload, 1) == 0xBF
			&& getByte(payload, 2) == 0x44	// 'D'
			&& getByte(payload, 3) == 0x69	// 'i'
			&& getByte(payload, 4) == 0x61	// 'a'
			&& getByte(payload, 5) == 0x67;	// 'g'
	}
	
	static boolean isPacketSnifferMessage(byte[] packet)
	{
		byte[] payload = getPayload(packet);
		
		return getActiveMessageType(packet) == 0xB1
			&& getByte(payload, 0) == 0x8B
			&& getByte(payload, 1) == 0x72	// 'r'
			&& getByte(payload, 6) == 0x15
			&& (getByte(payload, 10) & 0x0F) == 0x0F
			&& (getByte(payload, 11) & 0xF0) == 0x30;
	}
	
	static int getPacketSnifferTimestamp(byte[] packet)
	{
		byte[] payload = getPayload(packet);
		return getInt(payload, 2);
	}
	
	static int getPacketSnifferLength(byte[] packet)
	{
		byte[] payload = getPayload(packet);
		return getByte(payload, 9);
	}

	static byte[] getPacketSnifferPayload(byte[] packet)
	{
		byte[] payload = getPayload(packet);

		byte[] embedded = new byte[getByte(payload, 11) & 0x0F];
		System.arraycopy(payload, 12, embedded, 0, embedded.length);
		return embedded;
	}

	static void printPacket(byte[] packet)
	{
		String s = "";
		for(int i = 0; i < packet.length; ++i)
		{
			if( s.length() > 0 )
				s += " ";
			
			String v = Integer.toHexString(packet[i] & 0xFF);
			if( v.length() == 1 )
				s += "0";
			
			s += v;
		}
		System.out.println(s);
	}

// ------- StartMessage

	static boolean isStartMessage(byte[] packet)
	{
		return getByte(packet, 0) == 0x41
			&& getByte(packet, 1) == 0x88
			&& getByte(packet, 5) == 0xFF
			&& getByte(packet, 6) == 0xFF
			&& getByte(packet, 9) == 0x3F
			&& getByte(packet, 10) == 0x27;
	}
	
	static int getStartMsgMeasurementId(byte[] packet)
	{
		return getByte(packet, 11);
	}
	
	static int getStartMsgAssist1Tuning(byte[] packet)
	{
		return getByte(packet, 12);
	}
	
	static int getStartMsgAssist2Tuning(byte[] packet)
	{
		return getByte(packet, 13);
	}

	static int getStartMsgBeaconNode(byte[] packet)
	{
		int node = getByte(packet, 14);

		if( node < 0 && node > 4 )
			System.err.println("Illegal nodeid: " + node);
		
		return node;
	}
	
	static int getStartMsgAssist1Node(byte[] packet)
	{
		int node = getStartMsgBeaconNode(packet);
		
		node += 1;
		if( node > 4 )
			node -= 4;
		
		return node;
	}
	
	static int getStartMsgAssist2Node(byte[] packet)
	{
		int node = getStartMsgBeaconNode(packet);
		
		node += 2;
		if( node > 4 )
			node -= 4;
		
		return node;
	}
	
// ------- BeaconMessage

	static boolean isBeaconMessage(byte[] packet)
	{
		return getByte(packet, 0) == 0x21
			&& getByte(packet, 1) == 0x08
			&& getByte(packet, 5) == 0x73
			&& getByte(packet, 6) == 0xFF;
	}

	static int getBeaconMsgBeaconId(byte[] packet)
	{
		return getByte(packet, 2);
	}
	
	static int getBeaconMsgMeasurementId(byte[] packet)
	{
		return getByte(packet, 7);
	}
	
	static byte[] getBeaconMsgPreviousData(byte[] packet)
	{
		byte[] data = new byte[packet.length - 8];
		System.arraycopy(packet, 8, data, 0, data.length);
		return data;
	}
}

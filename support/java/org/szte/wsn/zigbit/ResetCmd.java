package org.szte.wsn.zigbit;

import java.io.IOException;

import net.tinyos.packet.*;

public class ResetCmd implements ByteSource
{
	public static void main(String[] args) throws IOException
	{
		Packetizer packetizer = new Packetizer("hihi", new ResetCmd());
		packetizer.open(null);
		packetizer.writePacket(new byte[] { 0x72, 0x52, 0x53, 0x54} );
	}

	public void close()
	{
	}

	public void open() throws IOException
	{
	}

	public byte readByte() throws IOException
	{
		return 0;
	}

	public void writeBytes(byte[] packet) throws IOException
	{
		System.out.print("packet[" + packet.length + "]={");
		for(int i = 0; i < packet.length; ++i)
			System.out.print(Integer.toHexString(packet[i] & 0xFF) + " ");
		System.out.println("}");
	}
}

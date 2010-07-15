/** Copyright (c) 2010, University of Szeged
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

package org.szte.wsn.zigbit;

// override the default one
import org.szte.wsn.zigbit.TOSSerial;

import net.tinyos.comm.*;
import java.io.IOException;
import java.util.*;
import java.io.*;

public class Bootloader implements SerialPortListener
{
	String port;

	TOSSerial serial;
	protected InputStream is;
	protected OutputStream os;
	
	public byte[] readBytes(int count, long timeout) throws IOException
	{
		long deadline = System.currentTimeMillis() + timeout;
		byte[] bytes = new byte[count];

		for(int i = 0; i < count; )
		{
			synchronized(is)
			{
				if( is.available() > 0 )
					bytes[i++] = (byte)is.read();
				else
				{
					timeout = deadline - System.currentTimeMillis();
					if( timeout <= 0 )
						return null;
						
					try
					{
						is.wait(timeout);
					}
					catch(InterruptedException e)
					{
						return null;
					}
				}
			}
		}

		return bytes;
	}
	
	public void serialEvent(SerialPortEvent ev)
	{
		synchronized(is)
		{
			is.notify();
		}
	}

	public void writeBytes(byte[] bytes) throws IOException
	{
		os.write(bytes);
		os.flush();
	}

	public void closePort() throws IOException
	{
		if( serial != null )
		{
			is.close();
			os.close();
			serial.close();
			
			serial = null;
			is = null;
			os = null;
		}
	}
	
	public void openProgrammer() throws IOException
	{
		closePort();
		
		serial = new TOSSerial(port);
		serial.setSerialPortParams(38400, 8, SerialPort.STOPBITS_1, false);
		serial.addListener(this);
		serial.notifyOn(SerialPortEvent.DATA_AVAILABLE, true);

		is = serial.getInputStream();
		os = serial.getOutputStream();
	}

	public void accessBootloder() throws IOException
	{
		byte[] req = new byte[] { (byte)0xB2, (byte)0xA5, (byte)0x65, (byte)0x4B };
		byte[] ack = new byte[] { (byte)0x69, (byte)0xD3, (byte)0xD2, (byte)0x26 };
		
		System.out.print("connecting to bootloader ..");
		for(int i = 0; i < 20; ++i)
		{
			System.out.print('.');
			
			writeBytes(req);
			byte[] response = readBytes(4, 500);
			if( response != null && Arrays.equals(response, ack) )
			{
				System.out.println(" connected");
				return;
			}
		}
		
		System.out.println(" failed");
		System.exit(1);
	}

	public void uploadFile(String filename) throws FileNotFoundException, IOException
	{
		Scanner scanner = new Scanner(new File(filename));
		
		ArrayList<String> lines = new ArrayList<String>();
		while( scanner.hasNextLine() ) 
			lines.add(scanner.nextLine());

		byte[] ack = new byte[] { (byte)0x4D, (byte)0x5A, (byte)0x9A, (byte)0xB4 };
		byte[] nak = new byte[] { (byte)0x2D, (byte)0x59, (byte)0x5A, (byte)0xB2 };
		
		System.out.print("writing " + lines.size() + " pages ...");
		
		for(int i = 0; i < lines.size(); ++i)
		{
			String line = lines.get(i);
			byte[] bytes = new byte[1 + line.length()/2];
			
			bytes[0] = (byte)line.charAt(0);
			bytes[1] = (byte)line.charAt(1);

			for(int j = 2; j < bytes.length; ++j)
				bytes[j] = (byte)Integer.parseInt(line.substring(-2 + j*2, j*2), 16);
			
			writeBytes(bytes);
			byte[] response = readBytes(4, 50);

			if( response == null )
			{
				System.out.println(" no response");
				System.exit(2);
			}
			else if( Arrays.equals(response, ack) )
			{
				if( i % 16 == 0 )
					System.out.print(".");
			}
			else if( Arrays.equals(response, nak) )
			{
				System.out.println(" not accepted");
				System.exit(3);
			}
			else
			{
				System.out.println(" incorrect response");
				System.exit(4);
			}
		}
		System.out.println(" done");
	}
	
	public static void main(String[] args) throws IOException
	{
		Bootloader bootloader = new Bootloader();
		bootloader.port = "com25";
		bootloader.openProgrammer();
		bootloader.accessBootloder();
		bootloader.uploadFile("C:\\tinyos\\tinyos-2.x\\apps\\Blink\\build\\ucmote900\\main.srec");
		bootloader.closePort();
	}
}

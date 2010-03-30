/** Copyright (c) 2009, University of Szeged
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
* Author: Zoltan Kincses
*/

import static java.lang.System.out;
import java.io.IOException;
import java.util.ArrayList;
import java.lang.Math;
import net.tinyos.message.*;
import net.tinyos.util.*;

class TimeSync implements MessageListener{

	private MoteIF mif;
			
	public TimeSync()
	{
		mif = new MoteIF(PrintStreamMessenger.err);
		mif.registerListener(new TimeMsg(),this);
	}
	public void messageReceived(int dest_addr,Message msg)
	{
		long timeDiff = 0;
		
		if(msg instanceof TimeMsg)
		{
			TimeMsg timemsg=(TimeMsg)msg;
			if (timemsg.get_locNodeID()>timemsg.get_remNodeID())
			{
				timeDiff=timemsg.get_locTime()-timemsg.get_remTime();
			}
			else
			{
				timeDiff=timemsg.get_remTime()-timemsg.get_locTime();
			}
			out.printf("%-15d %-15d %-15d %-15d %-15d%n",timemsg.get_remNodeID(),timemsg.get_remTime(),timemsg.get_locNodeID(),timemsg.get_locTime(),timeDiff);	
		}
	}
	public static void main (String[] args)
	{
		TimeSync timesync= new TimeSync();
		out.printf("%-15s %-15s %-15s %-15s %-15s%n","RemoteID","RemoteTime","LocalID","LocalTime","TimeDiff");
	}	
}

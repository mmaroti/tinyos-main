/*
 * Copyright (c) 2009, Vanderbilt University
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
 * Author: Miklos Maroti, Gabor Pap, Janos Sallai, Andras Biro
 */

module FatSpanningTreePolicyP
{
	provides interface DfrfPolicy;
	uses interface Convergecast;
	uses interface AMPacket;
}

implementation
{
	/**** flooding policy ****/

/*
	0 --sent--> 1 --tick--> 3 --tick--> 4 --sent--> 5 --tick--> 6 --sent--> 7
	7 --tick--> 9 --tick--> ... --tick--> 65 --tick--> 0xff
*/

	command uint16_t DfrfPolicy.getLocation()
	{
		return call Convergecast.grandParent();
	}

	command uint8_t DfrfPolicy.sent(uint8_t priority)
	{

		if( priority == 4 && call AMPacket.address() == call Convergecast.root() )
			return 6;
		else if( priority == 0 || priority == 4 || priority == 6 )
			return priority + 1;
		else
			return priority;
	}

	command bool DfrfPolicy.accept(uint16_t location)
	{
		if(location == call AMPacket.address()||
		   location == call Convergecast.parent()||
		   location == call Convergecast.grandParent()||
		   location == call Convergecast.greatGrandParent()||
		   location == call Convergecast.greatGreatGrandParent())
		{
		  return TRUE;
		}
		else
		{
 		  return FALSE;
		}
	}

	command uint8_t DfrfPolicy.received(uint16_t location, uint8_t priority)
	{
	  
		if( priority == 0 && call AMPacket.address() == call Convergecast.root() )
			return 6;
		else if( priority < 7 && (location == call Convergecast.greatGrandParent() || location == call Convergecast.greatGreatGrandParent() ))
			return 7;
		else if( priority > 7 && (location == call AMPacket.address() ||location == call Convergecast.parent() || location == call Convergecast.grandParent()))
			return 7;
		else
			return priority;
	}

	command uint8_t DfrfPolicy.age(uint8_t priority)
	{
		if( (priority & 0x01) == 0 )
			return priority;
		else if( priority == 3 || priority == 5 )
			return priority + 1;
		else if( priority < 65 )
			return priority + 2;
		else
			return 0xFF;
	}
}

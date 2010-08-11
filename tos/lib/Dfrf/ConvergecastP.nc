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

#include "Convergecast.h"

module ConvergecastP
{
	provides
	{
		interface Convergecast;
		interface Init;
	}

	uses
	{
		interface ActiveMessageAddress;
	}
}

implementation
{
	nx_struct convergecast_t current;

	command error_t Init.init()
	{
		current.hopCount = 0xFF;
		current.root = 0xFFFF;
		current.parent = 0xFFFF;
		current.grandParent = 0xFFFF;
		current.greatGrandParent = 0xFFFF;
		current.greatGreatGrandParent = 0xFFFF;
		return SUCCESS;
	}

	command void Convergecast.writeHeader(convergecast_t *data)
	{
		current.seqNum += 1;
		current.root = call ActiveMessageAddress.amAddress();
		current.hopCount = 0;

		*data = current;
		data->parent = current.root;
	}

	command bool Convergecast.readHeader(convergecast_t* data)
	{
		if( current.root == data->root && current.seqNum == data->seqNum )
			return TRUE;

		if( data->hopCount >= 63 )
			return FALSE;

		data->hopCount += 1;
		current = *data;

		data->parent = call ActiveMessageAddress.amAddress();
		data->grandParent = current.parent;
		data->greatGrandParent = current.grandParent;
		data->greatGreatGrandParent = current.greatGrandParent;
		

		return TRUE;
	}

	command am_addr_t Convergecast.root()
	{
		return current.root;
	}

	command am_addr_t Convergecast.parent()
	{
		return current.parent;
	}

	command am_addr_t Convergecast.greatGrandParent()
	{
		return current.greatGrandParent;
	}
	
	command am_addr_t Convergecast.greatGreatGrandParent()
	{
		return current.greatGreatGrandParent;
	}
	
	command am_addr_t Convergecast.grandParent()
	{
		return current.grandParent;
	}

	command uint8_t Convergecast.hopCount()
	{
		return current.hopCount;
	}

	async event void ActiveMessageAddress.changed()
	{
	}
}

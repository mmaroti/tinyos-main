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
 
#ifndef __CONVERGECAST_H__
#define __CONVERGECAST_H__

#include "AM.h"
#include "DfrfEngine.h"

typedef nx_struct convergecast_t
{
	nx_uint8_t seqNum;		// sequence number
	nx_am_addr_t root;		// address of the root 
	nx_am_addr_t parent;		// address of the sender 
	nx_am_addr_t grandParent;	// parent of the sender
	nx_am_addr_t greatGrandParent;	// grandparent of the sender
	nx_am_addr_t greatGreatGrandParent;	// great-grandparent of the sender
	nx_uint8_t hopCount;		// hop count of the sender (max 64)
} convergecast_t;

enum
{
	CONVERGECAST_UNIQUE_LEN = 3,
	CONVERGECAST_MAXDATA=FLOODROUTING_MAXDATA-sizeof(convergecast_t),
};

#endif//__CONVERGECAST_H__

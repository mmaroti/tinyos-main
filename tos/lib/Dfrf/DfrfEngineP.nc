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
 **/

 /** 
 *   The FloodRouting component provides a generic framework for writing and using 
 * routing protocols based on directed flooding. The user of the FloodRouting 
 * component can send and receive regular sized data packets, can select the
 * flooding policy (like broadcast, convergecats, tree routing, etc.). Multiple
 * data packet types and flooding policies can be used simultaneously. The 
 * framework automatically supports the aggregation of multiple data packets
 * into a single TOS_Msg, and allows the modification and control of the routed
 * messages in the network. 
 * See FloodRouting.txt for more details.
 *
 *   @author Miklos Maroti
 *   @author Brano Kusy, kusy@isis.vanderbilt.edu
 *   @author Janos Sallai
 */

#include "DfrfEngine.h"
#include <string.h>

module DfrfEngineP
{
	provides
	{
		interface DfrfControl[uint8_t id];
		interface DfrfSend[uint8_t appId];
		interface DfrfReceive[uint8_t appId];
	}
	uses
	{
		interface DfrfPolicy[uint8_t id];

		interface Timer<TMilli>;
		interface Leds;

		interface AMSend;
		interface Receive;
		interface Packet;
	}
}

implementation
{
	/**
	* Descriptors are stored as a linked list.
	*/
	dfrf_desc_t *firstDesc = NULL;

	/**
	* Find descriptor for a specific parametrized interface.
	*/
	static inline dfrf_desc_t* getDescriptor(uint8_t appId)
	{
		dfrf_desc_t* desc = firstDesc;
		while( desc != 0 )
		{
			if( desc->appId == appId )
				return desc;

			desc = desc->nextDesc;
		}
		return NULL;
	}

	/**
	 * Get the length of a block
	 */
	static inline uint8_t blockLength(dfrf_desc_t* desc) {
		return sizeof(dfrf_block_t) + desc->dataLength;
	}

	/**
	* Return the next block in the descriptor.
	*/
	static inline dfrf_block_t* nextBlock(dfrf_desc_t* desc, dfrf_block_t* block)
	{
		return (dfrf_block_t*)( (uint8_t*)block + blockLength(desc) );
	}

	/**
	* Returns match or block with lowest priority (set to 0xFF).
	*/
	dfrf_block_t* getBlock(dfrf_desc_t* desc, void *data)
	{
		dfrf_block_t* block = desc->blocks;
		dfrf_block_t* selected = block;

		do
		{
			if( block->priority != 0xFF
				&& memcmp(block->data, data, desc->uniqueLength) == 0 )
				return block;

			if( block->priority > selected->priority )
				selected = block;

			block = nextBlock(desc, block);
		} while( block < desc->blocksEnd );

		selected->priority = 0xFF;
		return selected;
	}

	/**
	* States of the desc->dirty flag in the struct descriptor.
	* dirty flag contains information for all blocks in the buffer;
	* allows for optimization in routing engine - can skip the whole
	*   descriptor, no need to visit all blocks in the descriptor
	*/
	enum
	{
		DIRTY_CLEAN = 0x00,	// no action is needed for this descriptor
		DIRTY_AGING = 0x01,	// only aging is required, no sending
		DIRTY_SENDING = 0x03,   // some packets are ready to be sent
	};

	/*
	* There are three concurrent activities, that routing engine performs.
	*  (1) sending of packets,
	*  (2) processing of a received msg, and
	*  (3) aging of packets
	*/
	enum
	{
		STATE_IDLE = 0x00,
		STATE_PROCESSING = 0x01,
		STATE_SENDING = 0x02,
	};

	uint8_t state = STATE_IDLE;

	static inline bool isProcessing() {
		return (state & STATE_PROCESSING) != 0;
	}

	static inline void setProcessing() {
		state |= STATE_PROCESSING;
	}

	static inline void clearProcessing() {
		state &= ~STATE_PROCESSING;
	}

	static inline bool isSending() {
		return (state & STATE_SENDING) != 0;
	}

	static inline void setSending() {
		state |= STATE_SENDING;
	}

	static inline void clearSending() {
		state &= ~STATE_SENDING;
	}


	message_t rxMsgBuf, txMsg;
	message_t* rxMsg = &rxMsgBuf;

	/**
	 * Get the maximum number of packets that fit in one message
	 */
	uint8_t maxPacketsPerMsg(dfrf_desc_t* desc) {
		return (call Packet.maxPayloadLength() - sizeof(dfrf_msg_t)) / (desc->dataLength);
	}

	/**
	 * Cast a message_t* to dfrf_msg_t*
	 **/
	static inline dfrf_msg_t* dfrfMsg(message_t* m) {
		return (dfrf_msg_t*)m->data;
	}

	/**
	 * Get the first packet in a message_t payload
	 **/
	static inline void* firstPacket(message_t* m) {
		return dfrfMsg(m)->data;
	}

	/**
	 * Get the next packet in a message_t payload
	 **/
	static inline uint8_t* nextPacket(message_t* m, uint8_t msgLength, void* currentPacket, uint8_t dataLength) {
		currentPacket += dataLength;
		if( currentPacket >= (void*)m->data + msgLength )
			return NULL;
		else
			return currentPacket;
	}

	// see selectData comments
	struct block freeBlock = { 0xFF };

	/**
	* Selects blocks for transmission from desc and stores them in selection.
	*   selection buffer is provided by caller.
	*	blocks are selected based on priority.
	*	blocks are sorted in decreasing order (priority field) in selection.
	*/
	void selectData(struct descriptor *desc, struct block **selection)
	{
		uint8_t maxPriority = 0xFF;
		dfrf_block_t* block = desc->blocks;
		dfrf_block_t** s = selection + maxPacketsPerMsg(desc);
		dfrf_block_t stopBlock = { 0x00 };

		// the blocks in selection are in decreasing order, initialization:
		//  - the last block has highest priority (0x00)
		//  - all other blocks have lowest priority (0xFF)
		//  - free Block needs to be a global variable, since if there is
		//  less to be transmitted packets in desc, than the maximum we
		//  can fit to the FloodRouting message, selection would point
		//  to non-existent data after returning
		*s = &stopBlock;
		do {
			*(--s) = &freeBlock;
		}
		while( s != selection );

		// go through all blocks in desc, find the highest priority block
		// and insert them in selection in decreasing order
		do
		{
			uint8_t priority = block->priority;
			// only block with even priority can be transmitted
			// see DfrfPolicy.nc for more details
			if( (priority & 0x01) == 0 && priority < maxPriority )
			{
				s = selection;
				while( priority < (*(s+1))->priority )
				{
					*s = *(s+1);
					++s;
				}

				*s = block;
				maxPriority = (*selection)->priority;
			}

			block = nextBlock(desc, block);
		} while( block < desc->blocksEnd );
	}

	/**
	* Creates FloodRoutingSync message by copying selection of blocks into the message.
	*	selection provides blocks in the decreasing order, the last
	*	 block having the highest priority, that's why we want to
	*	 start copying data from the end of selection.
	*	copyData() will be called after selectData().
	*/
	void copyData(dfrf_desc_t* desc, dfrf_block_t** selection)
	{
		dfrf_block_t** s = selection + maxPacketsPerMsg(desc);
		void* pkt = firstPacket(&txMsg);
		dfrf_msg_t* msg = dfrfMsg(&txMsg);

		// set initial (empty) msg length
		call Packet.setPayloadLength(&txMsg, sizeof(dfrf_msg_t));

		// set appId and location
		msg->appId = desc->appId;
		msg->location = call DfrfPolicy.getLocation[desc->appId]();

		// copy selection to msg
		while( s != selection && (*(--s))->priority != 0xFF )
		{
			// copy packet to msg
			memcpy(pkt, (*s)->data, desc->dataLength);

			// update msg length
			call Packet.setPayloadLength(&txMsg, call Packet.payloadLength(&txMsg) + desc->dataLength);

			pkt = nextPacket(&txMsg, call Packet.maxPayloadLength(), pkt, desc->dataLength);
		}
	}

	/**
	*  Task sendMsg() goes through the descriptions list and schedules radio messages for transmission.
	*   first a buffer is created, where the selected blocks will be stored, then for each
	*   description, we call selectData() (puts blocks into the selection buffer) and
	*   copyData() (copies selection blocks into a radio message).
	*   we transmit 1 TOSMSG containing the first blocks found in the first descriptor
	*   that had at least one block to be sent, 1 msg contains only blocks from 1 descriptor.
	*   if at least one block from a descriptor is transmitted, desc->dirty is set to aging.
	*/
	task void sendMsg()
	{
		// allocate selection buffer, assuming that the data part of each block is at least 
		// 1 byte long, note that selection contains certain number blocks that are copied into
		// a radio message
		struct block *selection[1 + (call Packet.maxPayloadLength() - sizeof(dfrf_msg_t))];

		struct descriptor *desc;

		if( isSending() )
			return;

		desc = firstDesc;
		while( desc != 0 )
		{
			// if DIRTY_SENDING, then there exists at least one block that need to be
			// transmitted in desc
			if( desc->dirty == DIRTY_SENDING )
			{
				selectData(desc, selection);
				copyData(desc, selection);

				// if there is at least one block to be sent
				if( call Packet.payloadLength(&txMsg) > sizeof(dfrf_msg_t) )
				{
					if( call AMSend.send(TOS_BCAST_ADDR, &txMsg, call Packet.payloadLength(&txMsg)) != SUCCESS )
						post sendMsg();
					else
						setSending();

					call Leds.led0Toggle();
					return;
				}

				// we have sent at least one packet, no more are pending
				desc->dirty = DIRTY_AGING;
			}
			desc = desc->nextDesc;
		}
	}

	/**
	* Upon successfull sending, we call task sendMsg() again, to transmit all the data which
	* are waiting to be transmitted.
	*/
	task void sendMsgDone()
	{
		dfrf_msg_t *msg = dfrfMsg(&txMsg);
		uint8_t appId = msg->appId;
		struct descriptor *desc = getDescriptor(appId);

		clearSending();

		if( desc != 0 )
		{
			// call policy.sent() on each of the transmitted blocks
			// this allows to update priority of the block according to the policy

			void* pkt = firstPacket(&txMsg);
			while( pkt != NULL )
			{
				struct block *block = getBlock(desc, pkt);

				if( block->priority != 0xFF )
					block->priority = call DfrfPolicy.sent[appId](block->priority);

				pkt = nextPacket(&txMsg, call Packet.payloadLength(&txMsg), pkt, desc->dataLength);
			}
		}

		post sendMsg();
	}

	event void AMSend.sendDone(message_t* p, error_t success)
	{
		if( success != SUCCESS )
			post sendMsg();
		else
			post sendMsgDone();
	}


	/**
	* Blocks are extracted out from the received message and stored in routing
	* buffer, if the routing policy accepts the packet and the application that
	* initialized FloodRouting accepts the packet as well(signal
	* FloodRouting.receive[](data) == SUCCESS).
	**/
	task void procMsg()
	{
		dfrf_msg_t* msg = dfrfMsg(rxMsg);
		uint8_t appId = msg->appId;
		struct descriptor *desc = getDescriptor(appId);

		call Leds.led1Toggle();

		if( desc != 0 && call DfrfPolicy.accept[appId](msg->location) )
		{
			void* pkt = firstPacket(rxMsg);
			while( pkt != NULL )
			{
				struct block *block = getBlock(desc, pkt);
				if( block->priority == 0xFF )
				{
					if( signal DfrfReceive.receive[appId](pkt) != TRUE )
					{
						pkt = nextPacket(rxMsg, call Packet.payloadLength(rxMsg), pkt, desc->dataLength);
						continue;
					}

					memcpy(block->data, pkt, desc->dataLength);
					block->priority = 0x00;
				}

				block->priority = call DfrfPolicy.received[appId](msg->location, block->priority);
				if ( (block->priority & 0x01) == 0 )
					desc->dirty = DIRTY_SENDING;
				else
					desc->dirty |= DIRTY_AGING;

				pkt = nextPacket(rxMsg, call Packet.payloadLength(rxMsg), pkt, desc->dataLength);
			}

			if( desc->dirty == DIRTY_SENDING )
				post sendMsg();
		}

		clearProcessing();
	}

	/**
	* Routing message from a different mote is scheduled for processing.
	*  since the pointer p which we obtain in the receive event can not be used
	*   after we return from the event handler, and we need to take long time to
	*   process the received message (i.e. we post a task), we need to save the
	*   pointer to some local variable (rxMsg).
	**/
	event message_t* Receive.receive(message_t* p, void* payload, uint8_t len)
	{
		call Leds.led2On();

		if( !isProcessing() )
		{
			message_t* tmp;
			
			tmp = rxMsg;
			rxMsg = p;
			p = tmp;

			post procMsg();
			setProcessing();
		}

		return p;
	}

	/**
	* Packets need to be aged, until they are thrown out from the buffer.
	*  moreover, after a packet has been aged, it may have to be resend (depending on
	*   the current policy), therefore we need to check for this and post a send task
	*   if it happens.
	*  dirty flag for desc is set to DIRTY_AGING, if there is at least one packet that
	*   needs to be aged, and set to DIRTY_SENDING if it needs to be sent().
	*/
	task void age()
	{
		struct descriptor *desc = firstDesc;

		while( desc != 0 )
		{
			if( desc->dirty != DIRTY_CLEAN )
			{
				struct block *block = desc->blocks;
				desc->dirty = DIRTY_CLEAN;
				do
				{
					if( block->priority != 0xFF )
					{
						block->priority = call DfrfPolicy.age[desc->appId](block->priority);

						if( (block->priority & 0x01) == 0 )
							desc->dirty = DIRTY_SENDING;
						else
							desc->dirty |= DIRTY_AGING;
					}
					block = nextBlock(desc, block);
				} while( block < desc->blocksEnd );

				if( desc->dirty == DIRTY_SENDING )
					post sendMsg();
			}
			desc = desc->nextDesc;
		}
	}

	/**
	* Each timer event triggers aging of the blocks in descriptors.
	*  this may result in sending a radio message.
	*/
	event void Timer.fired()
	{
		post age();
	}

	/** Find the actual block in a descriptor based on the match of unique data
	*   part, the new block gets assigned 0x00 priority and is sent from a task.
	*/
	command error_t DfrfSend.send[uint8_t id](void *data)
	{
	  struct descriptor *desc = getDescriptor(id);

	  if( desc != 0 )
	  {
		  struct block *block = getBlock(desc, data);
		  if( block->priority == 0xFF )
		  {
			memcpy(block->data, data, desc->dataLength);
			block->priority = 0x00;

			desc->dirty = DIRTY_SENDING;

			post sendMsg();

			call Leds.led2Toggle();
			return SUCCESS;
		  }
	  }

	  return FAIL;
	}

	command error_t DfrfControl.init[uint8_t id](uint8_t dataLength, uint8_t uniqueLength,
		void *buffer, uint16_t bufferLength)
	{
		dfrf_block_t *block;
		dfrf_desc_t *desc;

		if( dataLength == 0 //dataLength is too small
			|| dataLength > call Packet.maxPayloadLength() - sizeof(dfrf_msg_t) //single packet does not fit in TOSMSG
			|| uniqueLength > dataLength
			|| bufferLength <= //single packet does not fit in the buffer
				sizeof(dfrf_desc_t) + sizeof(dfrf_block_t)
			|| getDescriptor(id)!=0 ) //the descriptor for id already exists
			return FAIL;

		desc = (dfrf_desc_t*)buffer;
		desc->appId = id;
		desc->dataLength = dataLength;
		desc->uniqueLength = uniqueLength;
		desc->dirty = DIRTY_CLEAN;

		buffer += bufferLength - (blockLength(desc)-1); // this is the first invalid position
		block = desc->blocks;
		while( (void*)block < buffer )
		{
			block->priority = 0xFF;
			block = nextBlock(desc, block);
		}
		desc->blocksEnd = block;

		desc->nextDesc = firstDesc;
		firstDesc = desc;

		if( !call Timer.isRunning() )
			call Timer.startPeriodic(1024);

		return SUCCESS;
	}

	/** Just remove the descriptor from the linked list, the information is lost.
	*  stop() can not be undone (i.e. restarted).
	*/
	command void DfrfControl.stop[uint8_t id]()
	{
		dfrf_desc_t **desc = &firstDesc;
		while( *desc != 0 )
		{
			if( (*desc)->appId == id )
			{
				*desc = (*desc)->nextDesc;
				return;
			}
			desc = &((*desc)->nextDesc);
		}

		// stop timer if all clients are stopped
		if( firstDesc == NULL) {
			call Timer.stop();
		}
	}

	default command uint16_t DfrfPolicy.getLocation[uint8_t id]() { return 0; }
	default command uint8_t DfrfPolicy.sent[uint8_t id](uint8_t priority) { return 0xFF; }
	default command bool DfrfPolicy.accept[uint8_t id](uint16_t location) { return FALSE; }
	default command uint8_t DfrfPolicy.received[uint8_t id](uint16_t location, uint8_t priority) { return 0xFF; }
	default command uint8_t DfrfPolicy.age[uint8_t id](uint8_t priority) { return priority; }
	default event bool DfrfReceive.receive[uint8_t id](void *data) { return FALSE; }
}

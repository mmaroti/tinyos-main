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

#include <Tasklet.h>
#include <RadioAssert.h>
#include <RF230Radio.h>
#include "RF230RipsEngine.h"

module RF230RipsEngineP
{
	provides
	{
		interface RadioSend;
		interface RadioReceive;
	}

	uses
	{
		interface RadioSend as SubSend;
		interface RadioReceive as SubReceive;

		interface RadioAlarm;
		interface RadioRegister;
		interface RadioPacket as PayloadPacket;
		interface ActiveMessageAddress;
		interface Tasklet;

		interface PacketField<uint8_t> as PacketRSSI;
		interface PacketField<uint8_t> as PacketLinkQuality;

#ifdef RADIO_DEBUG
		interface DiagMsg;
		interface Leds;
#endif
	}
}

implementation
{
// -------- Measurement Store

	enum
	{
		RIPS_BEACON_COUNT = 1,
		RIPS_HISTORY_SIZE = 9,

		RIPS_BEACON_TIMEOUT = (uint16_t)(1300 * RADIO_ALARM_MICROSEC),
		RIPS_MEASUREMENT_TIMEOUT = (uint16_t)(25000 * RADIO_ALARM_MICROSEC),
	};

	tasklet_norace uint8_t measurement_data[RIPS_BEACON_COUNT][RIPS_HISTORY_SIZE];
	tasklet_norace uint8_t history_id;
	tasklet_norace uint8_t measurement_id;

	void copyPreviousData(uint8_t beacon_id, uint8_t* previous_data)
	{
		uint8_t i;
		uint8_t *p = measurement_data[beacon_id];

		ASSERT( beacon_id < RIPS_BEACON_COUNT );
		ASSERT( history_id < RIPS_HISTORY_SIZE );

		for(i = history_id + 1; i < RIPS_HISTORY_SIZE; ++i)
			*(previous_data++) = p[i];

		for(i = 0; i < history_id; ++i)
			*(previous_data++) = p[i];
	}

	void setMeasurementId(uint8_t id)
	{
		ASSERT( history_id < RIPS_HISTORY_SIZE );

		// to prevent multiple clearing of data
		if( (uint8_t)(id - measurement_id - 1) >= RIPS_HISTORY_SIZE )
			measurement_id = id - RIPS_HISTORY_SIZE;

		// clear out the data store
		do
		{
			uint8_t i;

			if( history_id-- == 0 )
				history_id = RIPS_HISTORY_SIZE - 1;

			for(i = 0; i < RIPS_BEACON_COUNT; ++i)
				measurement_data[i][history_id] = 0xFF;

		} while( ++measurement_id != id );
	}

	void setMeasurementData(uint8_t beacon_id, uint8_t data)
	{
		measurement_data[beacon_id][history_id] = data;
	}

// -------- Beacon Message

	typedef struct rips_beacon_message_t
	{
		nxle_uint8_t length;
		nxle_uint16_t fcf;
		nxle_uint8_t dsn;
		nxle_uint16_t destpan;
		nxle_uint16_t dest;
		uint8_t measurement_id;
		uint8_t previous_data[RIPS_HISTORY_SIZE-1];
	} rips_beacon_message_t;

	enum
	{
		RIPS_BEACON_FCF = IEEE154_TYPE_DATA << IEEE154_FCF_FRAME_TYPE
			| 1 << IEEE154_FCF_ACK_REQ
			| IEEE154_ADDR_NONE << IEEE154_FCF_SRC_ADDR_MODE
			| IEEE154_ADDR_SHORT << IEEE154_FCF_DEST_ADDR_MODE,
		RIPS_BEACON_DEST = 0xFF73,
		RIPS_BEACON_LENGTH = sizeof(rips_beacon_message_t) - 1 + 2,
		RIPS_BEACON_MIN_LENGTH = RIPS_BEACON_LENGTH - (RIPS_HISTORY_SIZE-1),
	};
		
	rips_beacon_message_t* getBeaconMessage(message_t* msg)
	{
		return ((void*)msg) + call PayloadPacket.headerLength(msg)
			- sizeof(rf230packet_header_t);
	}

	bool isBeaconMessage(message_t* msg)
	{
		rips_beacon_message_t* beacon = getBeaconMessage(msg);

		return beacon->length >= RIPS_BEACON_MIN_LENGTH
			&& beacon->fcf == RIPS_BEACON_FCF
			&& beacon->dest == RIPS_BEACON_DEST
			&& beacon->destpan == call ActiveMessageAddress.amGroup();
	}
	
	void createFirstBeaconMessage(message_t* msg)
	{
		rips_beacon_message_t* beacon = getBeaconMessage(msg);

		call PayloadPacket.clear(msg);

		beacon->length = RIPS_BEACON_LENGTH;
		beacon->fcf = RIPS_BEACON_FCF;
		beacon->dsn = 0;
		beacon->destpan = call ActiveMessageAddress.amGroup();
		beacon->dest = RIPS_BEACON_DEST;
		beacon->measurement_id = measurement_id;

		copyPreviousData(0, beacon->previous_data);
	}

	void createNextBeaconMessage(message_t* msg)
	{
		rips_beacon_message_t* beacon = getBeaconMessage(msg);

		copyPreviousData(++(beacon->dsn), beacon->previous_data);

		ASSERT( beacon->dsn < RIPS_BEACON_COUNT );
	}

// -------- Ack Message

	typedef struct rips_ack_message_t
	{
		nxle_uint8_t length;
		nxle_uint16_t fcf;
		nxle_uint8_t dsn;
	} rips_ack_message_t;

	enum
	{
		RIPS_ACK_FCF = IEEE154_TYPE_ACK << IEEE154_FCF_FRAME_TYPE,
		RIPS_ACK_LENGTH = sizeof(rips_ack_message_t) - 1 + 2,
	};

	rips_ack_message_t* getAckMessage(message_t* msg)
	{
		return ((void*)msg) + call PayloadPacket.headerLength(msg)
			- sizeof(rf230packet_header_t);
	}

	bool isAckMessage(message_t* msg)
	{
		rips_ack_message_t* ack = getAckMessage(msg);

		return ack->length == RIPS_ACK_LENGTH
			&& ack->fcf == RIPS_ACK_FCF
			&& ack->dsn < RIPS_BEACON_COUNT;
	}

	uint8_t getBeaconId(message_t* msg)
	{
		ASSERT( isBeaconMessage(msg) || isAckMessage(msg) );

		return getAckMessage(msg)->dsn;
	}

// -------- TinyOS Message

	void* getTinyOsPayload(message_t* msg)
	{
		return ((void*)msg) + call PayloadPacket.headerLength(msg);
	}

	rf230packet_header_t* getTinyOsHeader(message_t* msg)
	{
		return getTinyOsPayload(msg) - sizeof(rf230packet_header_t);
	}

	bool isStartMessage(message_t* msg)
	{
		rf230packet_header_t* header = getTinyOsHeader(msg);
		
		return header->rf230.length > sizeof(rf230packet_header_t)
#ifndef TFRAMES_ENABLED
			&& header->network.network == TINYOS_6LOWPAN_NETWORK_ID
#endif
			&& header->am.type == RIPS_AMTYPE;
	}

// -------- State Machine

	tasklet_norace uint8_t state;
	enum
	{
		STATE_IDLE = 0,
		STATE_BEACON_PENDING = 1,
		STATE_BEACON_SENDING = 2,
		STATE_ASSIST_TUNING = 3,
		STATE_ASSIST_RECEIVE = 4,
		STATE_ASSIST_RESTORE = 5,
		STATE_OTHERS_INIT = 6,
		STATE_OTHERS_LISTEN = 7,
		STATE_OTHERS_RESTORE = 8,
	};

	tasklet_norace uint8_t assistTuning;

	tasklet_norace message_t myMsgBuffer;
	tasklet_norace message_t* myMsg = &myMsgBuffer;


	void receiveStartMessage(message_t* msg)
	{
		rips_start_message_t* startMsg = getTinyOsPayload(msg);

		ASSERT( state == STATE_IDLE );
		ASSERT( isStartMessage(msg) );

		call Leds.led2Toggle();

		if( call RadioAlarm.isFree() )
		{
			setMeasurementId(startMsg->measurement_id);

			if( startMsg->beacon_nodeid == TOS_NODE_ID )
			{
				createFirstBeaconMessage(myMsg);
				state = STATE_BEACON_PENDING;
			}
			else if( startMsg->assist1_nodeid == TOS_NODE_ID )
			{
				assistTuning = startMsg->assist1_tuning;
				state = STATE_ASSIST_TUNING;
			}
			else if( startMsg->assist2_nodeid == TOS_NODE_ID )
			{
				assistTuning = startMsg->assist2_tuning;
				state = STATE_ASSIST_TUNING;
			}
			else
				state = STATE_OTHERS_INIT;
		}
	}

	tasklet_async event void SubSend.ready()
	{
		if( state == STATE_BEACON_PENDING )
		{
			if( call SubSend.send(myMsg) == SUCCESS )
				state = STATE_BEACON_SENDING;
		}
		else if( state == STATE_ASSIST_TUNING )
		{
			if( call RadioAlarm.isFree() && call RadioRegister.access() == SUCCESS )
			{
				call RadioRegister.write(RF230_SHORT_ADDR_0, RIPS_BEACON_DEST & 0xFF);
				call RadioRegister.write(RF230_SHORT_ADDR_1, RIPS_BEACON_DEST >> 8);
				call RadioRegister.write(RF230_PHY_TX_PWR, RF230_TX_AUTO_CRC_ON | (assistTuning & 0x0F & RF230_TX_PWR_MASK));
				call RadioRegister.write(RF230_XOSC_CTRL, 0xF0 | (assistTuning >> 4));
				call RadioRegister.release();

				state = STATE_ASSIST_RECEIVE;
				call RadioAlarm.wait(RIPS_MEASUREMENT_TIMEOUT);
			}
		}
		else if( state == STATE_ASSIST_RESTORE )
		{
			if( call RadioRegister.access() == SUCCESS )
			{
				uint16_t temp = call ActiveMessageAddress.amAddress();
				call RadioRegister.write(RF230_SHORT_ADDR_0, temp & 0xFF);
				call RadioRegister.write(RF230_SHORT_ADDR_1, temp >> 8);
				call RadioRegister.write(RF230_PHY_TX_PWR, RF230_TX_AUTO_CRC_ON | (RF230_DEF_RFPOWER & RF230_TX_PWR_MASK));
				call RadioRegister.write(RF230_XOSC_CTRL, 0xF0);
				call RadioRegister.release();

				state = STATE_IDLE;
			}
		}
		else if( state == STATE_OTHERS_INIT )
		{
			if( call RadioAlarm.isFree() && call RadioRegister.access() == SUCCESS )
			{
				call RadioRegister.write(RF230_TRX_STATE, RF230_RX_ON);
				call RadioRegister.release();

				state = STATE_OTHERS_LISTEN;
				call RadioAlarm.wait(RIPS_MEASUREMENT_TIMEOUT);
			}
		}
		else if( state == STATE_OTHERS_RESTORE )
		{
			if( call RadioRegister.access() == SUCCESS )
			{
				call RadioRegister.write(RF230_TRX_STATE, RF230_RX_AACK_ON);
				call RadioRegister.release();

				state = STATE_IDLE;
			}
		}

		if( state == STATE_IDLE )
			signal RadioSend.ready();
	}

	tasklet_async event void RadioAlarm.fired()
	{
		if( state == STATE_ASSIST_RECEIVE )
		{
			state = STATE_ASSIST_RESTORE;

			// SubSend.ready will be called
			call Tasklet.schedule();
		}
		else if( state == STATE_OTHERS_LISTEN )
		{
			state = STATE_OTHERS_RESTORE;
			call Tasklet.schedule();
		}
		else
			ASSERT( FALSE );
	}

	void receiveBeaconMessage(message_t* msg)
	{
		ASSERT( state == STATE_ASSIST_RECEIVE || state == STATE_OTHERS_LISTEN );
		ASSERT( isBeaconMessage(msg) );

		call Leds.led1Toggle();

		if( state == STATE_ASSIST_RECEIVE )
		{
			uint8_t beacon_id = getBeaconId(msg);

			if( call PacketRSSI.isSet(msg) )
			{
				uint8_t rssi = call PacketRSSI.get(msg);

				setMeasurementData(beacon_id, rssi);
			}

			if( beacon_id == RIPS_BEACON_COUNT-1 )
			{
				call RadioAlarm.cancel();

				// we cannot restore the radio yet, since the ACK message was not transmitted
				call RadioAlarm.wait(RIPS_BEACON_TIMEOUT);
			}
		}
	}

	void receiveAckMessage(message_t* msg)
	{
		ASSERT( state == STATE_OTHERS_LISTEN );
		ASSERT( isAckMessage(msg) );

		call Leds.led0Toggle();

		if( state == STATE_OTHERS_LISTEN )
		{
			uint8_t beacon_id = getBeaconId(msg);

			if( call PacketRSSI.isSet(msg) && call PacketLinkQuality.isSet(msg) )
			{
				uint8_t rssi = call PacketRSSI.get(msg);
				uint8_t lqi = call PacketLinkQuality.get(msg);

				setMeasurementData(beacon_id, (lqi == 0xFF ? 0x00 : 0x80) | (rssi & 0x7F));
			}

			if( beacon_id == RIPS_BEACON_COUNT-1 )
			{
				call RadioAlarm.cancel();
				state = STATE_OTHERS_RESTORE;
			}
		}

	}

// -------- RadioSend

	tasklet_norace message_t* txMsg;

	tasklet_async command error_t RadioSend.send(message_t* msg)
	{
		if( state != STATE_IDLE )
			return EBUSY;

		txMsg = msg;
		return call SubSend.send(msg);
	}

	tasklet_async event void SubSend.sendDone(error_t error)
	{
		if( state == STATE_IDLE )
		{
			if( error == SUCCESS && isStartMessage(txMsg) )
				receiveStartMessage(txMsg);

			signal RadioSend.sendDone(error);
		}
		else if( state == STATE_BEACON_SENDING )
		{
			uint8_t beacon_id = getBeaconId(myMsg);

			if( call RadioRegister.access() == SUCCESS )
			{
				setMeasurementData(beacon_id, 
					call RadioRegister.read(RF230_PHY_ED_LEVEL));

				call RadioRegister.release();
			}

			if( beacon_id < RIPS_BEACON_COUNT-1 )
			{
				createNextBeaconMessage(myMsg);
				state = STATE_BEACON_PENDING;
			}
			else
				state = STATE_IDLE;
		}
	}

	async event void ActiveMessageAddress.changed()
	{
	}

	tasklet_async event void Tasklet.run()
	{
	}

// -------- RadioReceive

	tasklet_async event bool SubReceive.header(message_t* msg)
	{
		return signal RadioReceive.header(msg);
	}

	tasklet_async event message_t* SubReceive.receive(message_t* msg)
	{
		if( (state == STATE_ASSIST_RECEIVE || state == STATE_OTHERS_LISTEN) && isBeaconMessage(msg) )
			receiveBeaconMessage(msg);
		else if( state == STATE_OTHERS_LISTEN && isAckMessage(msg) )
			receiveAckMessage(msg);
		else
		{
			if( state == STATE_IDLE && isStartMessage(msg) )
				receiveStartMessage(msg);

			msg = signal RadioReceive.receive(msg);
		}

		return msg;
	}
}

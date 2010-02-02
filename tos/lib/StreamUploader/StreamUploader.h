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
* Author:Andras Biro
*/
#ifndef STREAM_UPLOADER_H
#define STREAM_UPLOADER_H
enum{
	MESSAGE_SIZE=27,
	AM_LAST_DATA_MSG_T=0,
	AM_DATA_MSG_T=0,
	AM_RESP_MSG_T=0,
	AM_HELLO_MSG_T=0,
};

typedef nx_struct hello_msg_t {
	nx_uint16_t nodeid;
} hello_msg;


typedef nx_struct resp_msg_t {
	nx_uint32_t min_address;
	nx_uint32_t max_address;
	nx_uint16_t src;
	nx_uint16_t dest;
} resp_msg;

typedef nx_struct data_msg_t {
	nx_uint8_t address;
	nx_uint8_t data[MESSAGE_SIZE];
} data_msg;

typedef nx_struct last_data_msg_t {
	nx_uint8_t data[MESSAGE_SIZE-1];
	nx_uint8_t len;
} last_data_msg;
#endif /* STREAM_UPLOADER_H */

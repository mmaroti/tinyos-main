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
* Author: Ali Baharev
*/

module AccelP {

   uses interface Boot;
   uses interface LedHandler;
   uses interface SimpleFile as SF;
   uses interface SplitControl as SFCtrl;
   uses interface StdControl as RadioHandler;
}

implementation{
   
   event void Boot.booted() {
		// TODO
		error_t error;
		
		//error = call SFCtrl.start();
		
		//if (error)
		//	call LedHandler.error();

		error = call RadioHandler.start();

		if (error)
			call LedHandler.error();

   }

	event void SFCtrl.startDone(error_t error){

		if (error == SUCCESS)
			call LedHandler.diskReady();			
		else
			call LedHandler.error();
	}
	
	event void SF.formatDone(error_t error){
		// TODO Auto-generated method stub
	}

	event void SF.appendDone(error_t error){
		// TODO Auto-generated method stub
	}

	event void SF.seekDone(error_t error){
		// TODO Auto-generated method stub
	}

	event void SF.readDone(error_t error, uint16_t length){
		// TODO Auto-generated method stub
	}

	event void SFCtrl.stopDone(error_t error){
		// TODO Auto-generated method stub
	}

}

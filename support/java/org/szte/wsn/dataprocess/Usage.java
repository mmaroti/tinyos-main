/*
 * Copyright (c) 2010, University of Szeged
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
 * Author: Miklos Toth
 */
package org.szte.wsn.dataprocess;

public class Usage {
	public static void usageThanExit(){
		System.out.println("Usage:");
		System.out.println("java Transfer readMode sourcePath writeMode destinationPath structureFile toString seperator showname");
		System.out.println("java Transfer file 0.bin console stdOut structs.txt	toBinary , no		-reads from console writes to 0.bin, separator=, doesn't shows name");
		System.out.println("java Transfer serial serial@/dev/ttyUSB1:57600 file foo.txt structs.txt toString : showName -reads from the serialSource on USB1, writes to foo.txt, separator=: shows the name of struct ");
		System.out.println("If you want to write from .bin to a file, than only need 1 argument");
		System.out.println("java Transfer sourcefile");		
		System.out.println("java Transfer 0.bin  -reads 0.bin to 0.txt");		
		System.out.println("java Transfer .  -scans the directory for .bin files, parse them, and writes to txt");	
		System.exit(1);
	}
}

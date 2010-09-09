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
* */

package org.szte.wsn.SnifferGraph;
import java.util.Random;

public class RandomString {
	
	public static boolean getAnewInput(){
		Random r = new Random();
		int random = r.nextInt(10000000);
		if(random<20) return true;
		return false;
	}
	
	public static String[] getRandomMoteStrings(int mutch){
		Random rn = new Random();
		String[] devices = new String[]{"Device01", "Device02", "Device03", "Device04", "Device05", "Device06"};
		String[] types = new String[]{"Types01", "Types02", "Types03", "Types04", "Types05", "Types06"};
		int hanyas = rn.nextInt(6);
		String[] r = new String[mutch];
		String inputChars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
		r[0] = devices[hanyas];
		for(int i = 1; i<mutch; i+=2){
			String c = "";
			for(int k = 0; k<10; k++){
				c = c + inputChars.charAt(rn.nextInt(61));
			}
			r[i] = types[rn.nextInt(6)];
			r[i+1] = c;
		}
		return r;
	}
	
	public static int getAnumber(int number){
		Random rn = new Random();
		return number + rn.nextInt(3)+1;
	}

}
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
class ReceivedData {
	private short nodeID;
	private int min;
	private int max;
	private long average;
	private long energy;
	private int sampleCnt;
	private int micSampPer;
	private int micSampNum;
	
	public short getNodeID() {
		return (this.nodeID); 
	}

	public int getMin() {
		return (this.min); 
	}

	public int getMax() {
		return (this.max); 
	}

	public long getAverage() {
		return (this.average); 
	}

	public long getEnergy() {
		return (this.energy); 
	}
	
	public int getsampleCnt() {
		return (this.sampleCnt); 
	}
	public int getmicSampPer() {
		return (this.micSampPer); 
	}
	public int getmicSampNum() {
		return (this.micSampNum); 
	}
		
	public void setNodeID(short nodeID) {
		this.nodeID = nodeID; 
	}

	public void setMin(int min) {
		this.min = min; 
	}

	public void setMax(int max) {
		this.max = max; 
	}

	public void setAverage(long average) {
		this.average = average; 
	}

	public void setEnergy(long energy) {
		this.energy = energy; 
	}
	public void setsampleCnt(int sampleCnt) {
		this.sampleCnt = sampleCnt; 
	}
	public void setmicSampPer(int micSampPer) {
		this.micSampPer = micSampPer; 
	}
	public void setmicSampNum(int micSampNum) {
		this.micSampNum = micSampNum; 
	}
}

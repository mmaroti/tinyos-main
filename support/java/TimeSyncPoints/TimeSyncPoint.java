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
import java.util.ArrayList;
import java.util.HashSet;


public class TimeSyncPoint {
	ArrayList<TimeSyncPointPair> points=new ArrayList<TimeSyncPointPair>();
	ArrayList<TimeSyncFunction> functions=new ArrayList<TimeSyncFunction>();
	
	
	public void AddPoint(long xtime,long ytime, int xid, int yid){
		points.add(new TimeSyncPointPair(xtime,ytime,xid,yid));
	}
	
	private void regression(ArrayList<TimeSyncPointPair> usedpoints){
		double av_x=0,av_y=0;
		for(int i=0;i<usedpoints.size();i++){
			av_x+=usedpoints.get(i).xtime;
			av_y+=usedpoints.get(i).ytime;
		}
		av_x/=usedpoints.size();
		av_y/=usedpoints.size();
		double denom=0,numer=0;
		for(int i=0;i<usedpoints.size();i++){
			numer+=(usedpoints.get(i).xtime-av_x)*(usedpoints.get(i).ytime-av_y);
			denom+=(usedpoints.get(i).xtime-av_x)*(usedpoints.get(i).xtime-av_x);
		}
		double b=numer/denom, a=av_y-b*av_x;
		functions.add(new TimeSyncFunction(usedpoints.get(0).xid, usedpoints.get(0).yid, a, b));
	}
	
	private void createfunctions(){
		HashSet<Integer> nodes = new HashSet<Integer>();
		for(int i=0;i<points.size();i++){
			nodes.add(points.get(i).xid);
			nodes.add(points.get(i).yid);
		}
		Integer[] nodes_arr=(Integer[]) nodes.toArray();
		for(int i=0;i<nodes_arr.length;i++){
			for(int j=i+1;j<nodes_arr.length;j++){
				ArrayList<TimeSyncPointPair> usedpoints=new ArrayList<TimeSyncPointPair>();
				for(int k=0;k<points.size();k++){
					if(points.get(k).xid==i&&points.get(k).yid==j){
						usedpoints.add(points.get(k));
					}
					//limit?
					if(usedpoints.size()>0){
						regression(usedpoints);
					}
				}
				
			}
		}		
	}
	
	public void calculate(int base){
		createfunctions();
		if(base>0xffff)
			base=functions.get(0).x;
		//TODO: közös bázisra hozás
		//TODO: kiszámítani minden pontra a közelítő regressziós egyenest
	}

	/**
	 * y=a+b*x
	 * @author andris
	 *
	 */
	public class TimeSyncFunction{
		double a, b;
		int x,y;
		public TimeSyncFunction(int x, int y,double a, double b){
			this.x=x;
			this.y=y;
			this.a=a;
			this.b=b;
		}
	}
	
	public class TimeSyncPointPair{
		long xtime,ytime;
		int xid,yid;

		public TimeSyncPointPair(long xtime,long ytime, int xid, int yid){
			//the ID of x is always less then the ID of y
			if(xid<yid){
				this.xtime=xtime;
				this.xid=xid;
				this.ytime=ytime;
				this.yid=yid;
			} else {
				this.ytime=xtime;
				this.yid=xid;
				this.xtime=ytime;
				this.xid=yid;
			}
		}
		
		public long getXtime() {
			return xtime;
		}

		public long getYtime() {
			return ytime;
		}
		
		public int getXid() {
			return xid;
		}
		
		public int getYid() {
			return yid;
		}
	}

}

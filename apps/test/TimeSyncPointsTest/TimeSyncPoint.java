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
import java.io.BufferedReader;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.util.ArrayList;
import java.util.HashSet;

import Jama.Matrix;
import Jama.SingularValueDecomposition;


public class TimeSyncPoint {
	private ArrayList<TimeSyncPointPair> points=new ArrayList<TimeSyncPointPair>();
	private Matrix A,B;
	private Integer[] indexToNodeID=null;
	
	
	public void AddPoint(long xtime,long ytime, int xid, int yid){
		points.add(new TimeSyncPointPair(xtime,ytime,xid,yid));
	}
	
	private TimeSyncFunction regression(ArrayList<TimeSyncPointPair> usedpoints){
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
		return new TimeSyncFunction(usedpoints.get(0).xid, usedpoints.get(0).yid, a, b);
	}
	
	public void createMatrices(){
		HashSet<Integer> nodes = new HashSet<Integer>();
		for(int i=0;i<points.size();i++){
			nodes.add(points.get(i).xid);
			nodes.add(points.get(i).yid);
		}
		Object[] nodes_obj=nodes.toArray();
		indexToNodeID=new Integer[nodes_obj.length];
		for(int i=0;i<nodes_obj.length;i++){
			indexToNodeID[i]=(Integer)nodes_obj[i];
		}
		ArrayList<TimeSyncFunction> functions=new ArrayList<TimeSyncFunction>();
		for(int i=0;i<indexToNodeID.length;i++){
			for(int j=i+1;j<indexToNodeID.length;j++){
				ArrayList<TimeSyncPointPair> usedpoints=new ArrayList<TimeSyncPointPair>();
				for(int k=0;k<points.size();k++){
					if(points.get(k).xid==indexToNodeID[i]&&points.get(k).yid==indexToNodeID[j]){
						usedpoints.add(points.get(k));
					}
				}
				if(usedpoints.size()>0){ //limit?
					functions.add(regression(usedpoints));
				}
				
			}
		}		
		A=new Matrix(functions.size()+1,nodes.size());
		B=new Matrix(functions.size()+1,1);
		
		for(int i=0;i<functions.size();i++){
			TimeSyncFunction func=functions.get(i);
			int y=0,x=0;
			while(indexToNodeID[y]!=func.y)
				y++;
			while(indexToNodeID[x]!=func.x)
				x++;
			A.set(i,y,1);
			A.set(i,x,-1*func.b);
			B.set(i,0,func.a);	
		}
		
	}
	
	private Times calculate(int where,double when,boolean createMatrices){
		if(indexToNodeID==null||createMatrices)
			createMatrices();
		
		//Recreate the X=constant row
		A.setMatrix(A.getRowDimension()-1, A.getRowDimension()-1, 0, A.getColumnDimension()-1, new Matrix(1,A.getColumnDimension()));
		int i=0;
		while(indexToNodeID[i]!=where)
			i++;
		A.set(A.getRowDimension()-1,i,1);
		B.set(B.getRowDimension()-1,0,when);
				
		SingularValueDecomposition decomp=A.svd();
		//solution=V*S*Ut*B
		Matrix solution=decomp.getV().times(decomp.getS().inverse()).times(decomp.getU().transpose()).times(B);
		return new Times(solution.getColumnPackedCopy(),indexToNodeID);

	}

	//y=a+b*x
	private class TimeSyncFunction{
		double a, b;
		int x,y;
		public TimeSyncFunction(int x, int y,double a, double b){
			this.x=x;
			this.y=y;
			this.a=a;
			this.b=b;
		}
	}
	
	public class Times{
		Integer where[];
		Double when[];
		public int size;
		
		public Times(double when[],Integer where[]){
			if(when.length==where.length){
				size=where.length;
				this.where=new Integer[where.length];
				this.when=new Double[when.length];
				for(int i=0;i<when.length;i++){
					this.where[i]=where[i];
					this.when[i]=when[i];
				}
			}
		}
		
	}
	
	private class TimeSyncPointPair{
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
	
	public static void main(String[] args) {
		if(args.length<2){
			System.err.println("Usage: TimeSyncPoints <BASENAME> <MAXINDEX>");
			System.exit(1);
		}
		int maxindex=Integer.parseInt(args[1]);
		TimeSyncPoint tsp=new TimeSyncPoint();
		for(int i=0;i<=maxindex;i++){
			String filename;
			if(i<10)
				filename=args[0]+"0"+Integer.toString(i);
			else
				filename=args[0]+Integer.toString(i);
			
			try {
				BufferedReader input =  new BufferedReader(new FileReader(filename));
				long x=0;
				String line = null;
				line = input.readLine();
				int xid=Integer.parseInt(line);
				line = input.readLine();
				int yid=Integer.parseInt(line);
		        while (( line = input.readLine()) != null){
		        	tsp.AddPoint(x, (long)(1000*Double.parseDouble(line)), xid, yid);
		        	x+=1000;
		        }
				input.close();
			} catch (FileNotFoundException e) {
				System.out.println("File not found: "+filename);
			} catch (IOException e) {
				System.out.println("Can't close file: "+filename);
			}

		}
		
		Times t=tsp.calculate(3,0, true);
		for(int i=0;i<t.size;i++){
			System.out.println(t.where[i]+"="+t.when[i]);
		}
		
	}

}

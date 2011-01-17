package org.szte.wsn.downloader;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.util.ArrayList;

public class CSVHandler {
	private File csvfile;
	private String separator;
	private ArrayList<String> header;
	private ArrayList<String[]> data;
	private int timeColumn;
	private ArrayList<Integer> dataColumns;
	
	public static String switchExtension(String fullname, String newEx){
		return fullname.substring(0, fullname.lastIndexOf('.'))+newEx;
	}
	
	private void initEmptyFile(){
		header=null;
		data=new ArrayList<String[]>();
	}
	
	private void openFile(boolean hasheader) throws IOException{
		if(!csvfile.exists()){
			initEmptyFile();
			return;
		}
		BufferedReader input=new BufferedReader(new FileReader(csvfile));
		String line=input.readLine();
		if(line==null){
			input.close();
			initEmptyFile();
			return;
		}
		if(hasheader){
			header=new ArrayList<String>();
			for(String column:line.split(separator)){
				header.add(column);
			}
			line=input.readLine();
		} else
			header=null;
		data=new ArrayList<String[]>();
		while(line!=null){
			data.add(line.split(separator));
			line=input.readLine();
		}
		input.close();
	}
	
	public CSVHandler(File csvfile, boolean hasheader, String separator, int timeColumn, ArrayList<Integer> dataColumns) throws IOException{
		this.separator=separator;
		this.csvfile=csvfile;
		this.timeColumn=timeColumn;
		this.dataColumns=dataColumns;
		openFile(hasheader);
	}
	
	public void reReadFile(boolean hasheader) throws IOException{
		openFile(hasheader);
	}
	
	public ArrayList<String> getHeader(){
		return header;
	}
	
	public String getHeaderId(int id){
		return header.get(id-1);
	}
	
	public void setHeader(ArrayList<String> h){
		header=h;
	}
	
	public String getHeaderLine(){
		String ret="";
		for(int i=0;i<header.size()-1;i++)
			ret+=header.get(i)+separator;
		ret+=header.get(header.size()-1);
		return ret;
	}
	
	public void setHeaderLine(String h){
		for(String column:h.split(separator)){
			header.add(column);
		}
	}



	public void clearHeader(){
		header=null;
	}
	
	public boolean flush() throws IOException{
		File tempfile=new File(switchExtension(csvfile.getName(), ".tmp"));
		BufferedWriter output=new BufferedWriter(new FileWriter(tempfile));
		if(header!=null){
			for(int i=0;i<header.size()-1;i++)
				output.append(header.get(i)+separator);
			output.append(header.get(header.size()-1));
			output.newLine();
		}
		for(String[] line:data){
			for(int i=0;i<line.length-1;i++)
				output.append(line[i]+separator);
			output.append(line[line.length-1]);
			output.newLine();
		}
		output.close();
		if(csvfile.exists()&&!csvfile.delete())
			return false;
		if(!tempfile.renameTo(csvfile))
			return false;
		return true;
	}
	
	public int getColumnNum(String name){
		for(String column:header)
			if(column.equals(name))
				return header.indexOf(column)+1;
		return -1;
	}
	
	public String getLine(int line){
		line--;
		String ret="";
		for(int i=0;i<data.get(line).length-1;i++)
			ret+=data.get(line)[i]+separator;
		ret+=data.get(line)[data.get(line).length-1];
		return ret;
	}
	
	public String getCell(int column, int line){
		column--;line--;
		if(line>=data.size())
			return null;
		if(column>=data.get(line).length)
			return "";
		return data.get(line)[column];
	}
	
	public boolean setCell(int column, int line, Object value){
		column--;line--;
		if(line>=data.size())
			return false;
		if(column>=data.get(line).length){
			String[] newstr=new String[column];
			for(int i=0;i<newstr.length;i++){
				if(i<data.get(line).length)
					newstr[i]=data.get(line)[i];
				else
					newstr[i]="";
			}
			data.set(line,newstr);
		}
		data.get(line)[column]=value.toString();
		return true;
	}
	
	public void addColumn(String name, int column){
		column--;//start counting from 0 instead of 1
		if(header!=null)
			header.add(column, name);
		for(int i=0;i<data.size();i++){
			String[] oldstr = data.get(i);
			String[] newstr;
			if(oldstr.length>=column)
				newstr=new String[oldstr.length+1];
			else
				newstr=new String[column];
			for(int j=0;j<newstr.length;j++){
				if(j<column&&j<oldstr.length)
					newstr[j]=oldstr[j];
				else if(j>column&&j<oldstr.length+1)
					newstr[j]=oldstr[j-1];
				else
					newstr[j]="";
			}
			data.set(i, newstr);
		}
	}
	
	public void removeColumn(int column){
		column--;//start counting from 0 instead of 1
		if(header!=null)
			header.remove(column);
		for(int i=0;i<data.size();i++){
			String[] oldstr = data.get(i);
			if(oldstr.length<column+1)
				continue;
			String[] newstr=new String[oldstr.length-1];
			for(int j=0;j<newstr.length;j++){
				if(j<column)
					newstr[j]=oldstr[j];
				else 
					newstr[j]=oldstr[j-1];
			}
			data.set(i, newstr);
		}
	}
	
	public void addLine(int line, String[] values){
		line--;
		data.add(line, values);
	}
	
	public void addLine(String[] values){
		data.add(values);
	}
	public void addLine(ArrayList<String> values){
		String[] arrayvalues=new String[values.size()];
		for(int i=0;i<values.size();i++)
			arrayvalues[i]=values.get(i);
		data.add(arrayvalues);
	}
	
	public void removeLine(int line){
		line--;
		data.remove(line);
	}
	
	public int getLineNumber(){
		return data.size();
	}
	
	public File getFile(){
		return csvfile;
	}
	
	public String getName(){
		return csvfile.getName();
	}
	
	public void setTimeColumn(int timeColumn) {
		this.timeColumn = timeColumn;
	}

	public int getTimeColumn() {
		return timeColumn;
	}

	public void setDataColumns(ArrayList<Integer> dataColumns) {
		this.dataColumns = dataColumns;
	}

	public ArrayList<Integer> getDataColumns() {
		return dataColumns;
	}

	public void onDestroy() throws IOException{
		if(!flush())
			System.err.println("Can't overwrite file: "+csvfile.getName());
	}
	
	private Double getValueAt(long time, int column, int afterLine) throws NumberFormatException{
		while(Long.parseLong(getCell(timeColumn, afterLine))<time){
			afterLine++;
		}
		if(Long.parseLong(getCell(timeColumn, afterLine))==time&&getCell(column,afterLine)!="")
				return Double.parseDouble(getCell(column,afterLine));
		else {
			afterLine++;
			int beforeLine=afterLine-1;
			try{
				String cell=getCell(column,beforeLine);
				while("".equals(cell)){
					cell=getCell(column,--beforeLine);
				}
				cell=getCell(column,afterLine);
				while("".equals(cell)){
					cell=getCell(column,++afterLine);
				}
			} catch (ArrayIndexOutOfBoundsException e){
				return null;
			}
			if(beforeLine<1||afterLine>data.size())
				return null;
			long beforeTime=Long.parseLong(getCell(timeColumn, beforeLine));
			long timeDiff=Long.parseLong(getCell(timeColumn, afterLine))-beforeTime;

			double beforeValue=Double.parseDouble(getCell(column,beforeLine));
			double afterValue=Double.parseDouble(getCell(column,afterLine));
			
			double spine=(afterValue-beforeValue)/timeDiff;
			
			return beforeValue+spine*(time-beforeTime);
		}
	}
	
	public void fillGaps(){
		for(int column:dataColumns){
			for(int line=1;line<=data.size();line++){
				if("".equals(getCell(column, line))){
					long time=Long.parseLong(getCell(1, line));
					Double value=getValueAt(time, column, line);
					if(value!=null)
						setCell(column, line, value);
				}
			}
		}
	}
	
	private class Integral{
		private Double integral[]=new Double[dataColumns.size()];
		private int lastLine;
		
		public Integral(Double integral2[],int lastLine2){
			setLastLine(lastLine2);
			setIntegral(integral2);
		}

		private void setLastLine(int lastLine2) {
			lastLine=lastLine2;
			
		}

		private void setIntegral(Double integral2[]) {
			integral=integral2;
		}

		public int getLastLine() {
			return lastLine;
		}
		
		public String[] createLine(long time){
			String ret[]=new String[header.size()];
			int maxColumn=timeColumn>dataColumns.get(dataColumns.size()-1)?timeColumn:dataColumns.get(dataColumns.size()-1);
			int dataIndex=-1;
			for(int i=0;i<maxColumn;i++){
				if(i+1==timeColumn)
					ret[i]=String.valueOf(time);
				else if(dataColumns.contains(i+1)){
					dataIndex++;
					if(integral[dataIndex]!=null)
						ret[i]=String.valueOf(integral[dataIndex]);
					else
						ret[i]="";
				}else
					ret[i]="";
			}
			return ret;
		}
	}
	
	private Integral getIntegral(long from, long to, int afterLine) throws NumberFormatException{
		while(Long.parseLong(getCell(timeColumn, afterLine))<from){
			afterLine++;
		}
		Double ret[]=new Double[dataColumns.size()];
		int line=afterLine;
		for(int j=0;j<dataColumns.size();j++){
			line=afterLine;
			double prevValue,returnElement;
			double currValue;
			try{
				currValue=Double.parseDouble(getCell(dataColumns.get(j),line));
			} catch(NumberFormatException e){
				ret[j]=null;
				continue;
			}
			long prevTime=from,currTime=Long.parseLong(getCell(timeColumn, line));
			if(Long.parseLong(getCell(timeColumn, line))!=from){
				returnElement=((getValueAt(from, dataColumns.get(j), line)+currValue)/2)*(currTime-prevTime);
			} else{
				returnElement=0;
			}
			
			prevValue=currValue;
			prevTime=currTime;
			currTime=Long.parseLong(getCell(timeColumn, ++line));
			boolean outOfData=false;
			while(currTime<to){
				try{
					currValue=Double.parseDouble(getCell(dataColumns.get(j),line));
					returnElement+=((prevValue+currValue)/2)*(currTime-prevTime);
					prevValue=currValue;
					prevTime=currTime;
					currTime=Long.parseLong(getCell(timeColumn, ++line));
				} catch(NumberFormatException e){
					outOfData=true;
					break;
				} catch(IndexOutOfBoundsException e){
					outOfData=true;
					break;
				}
			}
			if(outOfData)
				continue;
			
			if(currTime==to){
				currValue=Double.parseDouble(getCell(dataColumns.get(j),line));
			} else if(currTime<to){
				currValue=getValueAt(to, dataColumns.get(j), line);
			} 
			returnElement+=((prevValue+currValue)/2)*(currTime-prevTime);
			ret[j]=returnElement/(to-from);
		}
		return new Integral(ret,line);
	}
	
	
	public static final byte TIMETYPE_START=0;
	public static final byte TIMETYPE_END=1;
	public static final byte TIMETYPE_MIDDLE=2;
	
	public CSVHandler averageColumns(long timeWindow, File newFile, byte timeType) throws IOException{
		CSVHandler ret=new CSVHandler(newFile, header==null?false:true , separator, getTimeColumn(), getDataColumns());
		ret.setHeader(getHeader());
		int currentLine=1;
		long currentTime=-1;
		while(currentTime<0){
			try{
				currentTime=Long.parseLong(getCell(timeColumn,currentLine));
			}catch(NumberFormatException e){
				currentLine++;	//don't care these lines, probably no globaltime
			}
		}
		while(currentLine<data.size()){
			try{
				Integral avg=getIntegral(currentTime, currentTime+timeWindow, currentLine);
				if(timeType==TIMETYPE_END)
					ret.addLine(avg.createLine(currentTime+timeWindow));
				else if(timeType==TIMETYPE_MIDDLE)
					ret.addLine(avg.createLine(currentTime+timeWindow/2));
				else
					ret.addLine(avg.createLine(currentTime));
				
				currentLine=avg.getLastLine();
				currentTime=currentTime+timeWindow;
			}catch(NumberFormatException e){
				currentTime=Long.parseLong(getCell(timeColumn,++currentLine)); //don't care these lines, probably no globaltime
			}
		}
		
		return ret;
	}
	
	

}

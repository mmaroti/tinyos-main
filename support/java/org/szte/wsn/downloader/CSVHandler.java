package org.szte.wsn.downloader;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.util.ArrayList;

public class CSVHandler {
	File csvfile;
	String separator;
	ArrayList<String> header;
	ArrayList<String[]> data;
	int columns;
	
	private void initEmptyFile(){
		header=null;
		data=new ArrayList<String[]>();
		columns=0;
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
			for(String column:line.split(separator)){
				header.add(column);
			}
			line=input.readLine();
		} else
			header=null;
		while(line!=null){
			data.add(line.split(separator));
			line=input.readLine();
		}
		input.close();
	}
	
	public CSVHandler(File csvfile, boolean hasheader, String separator) throws IOException{
		this.separator=separator;
		this.csvfile=csvfile;
		openFile(hasheader);
	}
	
	public void reReadFile(boolean hasheader) throws IOException{
		openFile(hasheader);
	}
	
	public ArrayList<String> getHeader(){
		return header;
		
	}
	
	public void setHeader(String h){
		for(String column:h.split(separator)){
			header.add(column);
		}
	}


	public void clearHeader(){
		header=null;
	}
	
	public void flush() throws IOException{
		csvfile.delete();
		BufferedWriter output=new BufferedWriter(new FileWriter(csvfile));
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
	}
	
	public String getCell(int column, int line){
		return data.get(line)[column];
	}
	
	public void setCell(int column, int line, Object value){
		data.get(line)[column]=value.toString();
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
					newstr[j]=oldstr[j+1];
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
		data.add(line, values);
	}
	
	public void removeLine(int line){
		data.remove(line);
	}
	
	

}

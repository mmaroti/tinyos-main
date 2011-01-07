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
		if(!csvfile.delete())
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
	
	public void onDestroy() throws IOException{
		if(!flush())
			System.err.println("Can't overwrite file: "+csvfile.getName());
	}
	
	

}

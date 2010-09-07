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
import java.awt.Color;
import java.util.ArrayList;

import javax.swing.BorderFactory;
import javax.swing.JCheckBox;
import javax.swing.JPanel;
import javax.swing.JTextField;

public class Packages {
	
	public static ArrayList<String> colors = SnifferGraph.colors;
	public static ArrayList<JCheckBox> devices = SnifferGraph.devices;
	public static ArrayList<String> dataTypes = SnifferGraph.dataTypes;
	public static int which=0;
	protected int startTime;
	protected int stopTime;
	protected int timeLenght;
	protected int firstPos;
	protected String[] labelNames;
	protected JTextField[] cells;
	protected JPanel motePanel;
	protected JPanel motePanel2;
	
	//getters and setters
	public int getFirstPos() {
		return firstPos;
	}

	public void setFirstPos(int firstPos) {
		this.firstPos = firstPos;
	}
	
	public int getStartTime() {
		return startTime;
	}
	
	public void setStartTime(int startTime) {
		this.startTime = startTime;
	}
	
	public int getStopTime() {
		return stopTime;
	}
	
	public void setStopTime(int stopTime) {
		this.stopTime = stopTime;
	}
	
	public int getTimeLenght() {
		return timeLenght;
	}
	
	public void setTimeLenght(int timeLenght) {
		this.timeLenght = timeLenght;
	}
	
	public String[] getLabelNames() {
		return labelNames;
	}
	
	public void setLabelNames(String[] labelNames) {
		this.labelNames = labelNames;
	}
	
	public JTextField[] getCells() {
		return cells;
	}
	
	public void setCells(JTextField[] cells) {
		this.cells = cells;
	}
	
	public JPanel getMotePanel() {
		return motePanel;
	}
	public JPanel getMotePanel2() {
		return motePanel2;
	}
	
	public void setMotePanel(JPanel motePanel) {
		this.motePanel = motePanel;
	}
	public void setMotePanel2(JPanel motePanel2) {
		this.motePanel2 = motePanel2;
	}
	
	//other futures
	/**
	 * Restore a panel with the given properties mote draws
	 * 
	 * @return motPanel: The panel requested features drawn
	 *  */
	public JPanel getAPanel(ArrayList<JCheckBox> devices){
		this.firstPos = 130+this.startTime*100;
		motePanel2 = new JPanel();
		motePanel2.setLayout(null);
		motePanel2.setBounds(firstPos, 270, this.timeLenght*100, 2*70-50);		//bejelĂśld dobozok * dobozmĂŠret - egy fĂŠl dobozmĂŠret
		motePanel2.setBackground(Color.orange);
		motePanel2.setBorder(BorderFactory.createEtchedBorder());
		int k = 0, j = 0;
		for(int i = 0; i<devices.size(); i++){
			if(devices.get(i).isSelected()){
				cells[j] = new JTextField(this.labelNames[i]);
				cells[j].setBackground(null);
				cells[j].setBorder(BorderFactory.createEtchedBorder());
				cells[j].setBounds(0, k, this.timeLenght*100, 20);
				motePanel2.add(cells[j++]);
				k+=70;
			}
		}
		motePanel2.setVisible(true);
		return motePanel2;
	}
	
	
	public JPanel getABigPanel(ArrayList<JCheckBox> device, ArrayList<Color> colorList){
		which++;
		motePanel = new JPanel();

		//eszkĂśzmeghatĂĄrozĂĄs
		int devicespos = 0;
		int type =0;
		if(dataTypes.size()==0){
			//System.err.println("üres vagyok!!");
			dataTypes.add(this.labelNames[1]);
		}else{
			int p=0;
			for(p=1;p<this.labelNames.length;p+=2){
				boolean talalat = false;
				for(type =0; type<dataTypes.size(); type++){
					if(this.labelNames[p].equals(dataTypes.get(type))){
						System.err.println("Ilyen mar volt: "+dataTypes.get(type));
						talalat = true;
					}
				}
				if(!talalat){dataTypes.add(this.labelNames[p]);}
			}
		}
		if(devices.size()==0){
			Process.addDevice(this.labelNames[0], colorList.get(0));
		}
		else{
			boolean talalat = false;
			for(devicespos = 0; devicespos<devices.size(); devicespos++){
				if(this.labelNames[0].equals(devices.get(devicespos).getText())){
					talalat = true;
					break;
				}
			}
			if(!talalat){
				Process.addDevice(this.labelNames[0], colorList.get(0));
				devicespos++;
			}
		
		}
		//devicespos++;
		
		//szinmeghatĂĄrozĂĄs
		int colornumber = 0;
		if(colors.size()==0){
			colors.add(this.labelNames[0]);
		}
		else{
			 boolean talalat = false;
			for(colornumber = 0; colornumber<colors.size(); colornumber++){
				if(colors.get(colornumber).equals(this.labelNames[0])){
					System.out.println("nezzik mi van itt:" + colornumber);
					System.out.println(colors.get(colornumber) + " " + this.labelNames[1]);
					talalat = true;
					break;
				}
			}
			if(!talalat){
				colors.add(this.labelNames[0]);
				colornumber++;
			}
		}
		//szin ĂŠs poziciĂłmeghatĂĄrozĂĄs, kĂŠsz, panel lĂŠtrehozĂĄsa, szinezĂŠse ĂŠs a multiline tooltop lĂŠtrehozĂĄsa szĂźksĂŠges sanya
		
		
		
		System.err.println(dataTypes.size());
		for(int f=0;f<dataTypes.size();f++){
			
			JTextField savedtypes = new JTextField(dataTypes.get(f));
			savedtypes.setEditable(false);
			savedtypes.setLayout(null);
			savedtypes.setBounds(150+75*((f)), 25, 75, 39);
			SnifferGraph.backgPanel2.add(savedtypes);
		System.err.println(dataTypes.get(f));
		}

		
		motePanel.setBounds(75 ,(which+1)*25, 75*(dataTypes.size()+1) ,45);		//koordinĂĄtĂĄk mĂŠg nem pontossak
		motePanel.setBorder(BorderFactory.createEtchedBorder());
		motePanel.setBackground(colorList.get(colornumber));
		String information ="<html>";
		
		JTextField datas2 = new JTextField(this.labelNames[0]);
		datas2.setEditable(false);
		datas2.setLayout(null);
		datas2.setBounds(0, 3, 75, 39);
		motePanel.add(datas2);
		for(int i =2; i<labelNames.length;i+=2){
			JTextField datas = new JTextField(this.labelNames[i]);
			datas.setEditable(false);
			datas.setLayout(null);
			motePanel.setLayout(null);
			for(int k=0;k<dataTypes.size();k++){
				if(this.labelNames[i-1].equals(dataTypes.get(k)))
					datas.setBounds(75+75*((k)), 3, 75, 39);
					motePanel.add(datas);
			}
			information=information+this.labelNames[i]+"<br>";
		}
		/*"<html>Eszkďż˝z neve: <br>"+this.labelNames[3]+"<br>Hďż˝mďż˝rsďż˝klet: <br>"+"adat"+"<br>valami: <br>"+"adat2"*/
		motePanel.setToolTipText(information);

		motePanel.setBackground(colorList.get(devicespos));

		
		return motePanel; 
	}
	
	
	public JPanel getALilPanel(ArrayList<JCheckBox> devices, ArrayList<Color> colorList){
		motePanel = new JPanel();
		
		//eszkĂśzmeghatĂĄrozĂĄs
		int devicespos = 0;
		if(devices.size()==0){
			Process.addDevice(this.labelNames[0], colorList.get(0));
		}
		else{
			boolean talalat = false;
			for(devicespos = 0; devicespos<devices.size(); devicespos++){
				if(this.labelNames[0].equals(devices.get(devicespos).getText())){
					talalat = true;
					break;
				}
			}
			if(!talalat){
				Process.addDevice(this.labelNames[0], colorList.get(devicespos));
				//devicespos++;
			}
		}
		
		//szinmeghatĂĄrozĂĄs
		/*int colornumber = 0;
		if(colors.size()==0){
			colors.add(this.labelNames[0]);
		}
		else{
			boolean talalat = false;
			for(colornumber = 0; colornumber<colors.size(); colornumber++){
				if(colors.get(colornumber).equals(this.labelNames[0])){
					System.out.println("nezzik mi van itt:" + colornumber);
					System.out.println(colors.get(colornumber) + " " + this.labelNames[1]);
					talalat = true;
					break;
				}
			}
			if(!talalat){
				colors.add(this.labelNames[0]);
				colornumber++;
			}
		}*/
		//szin ĂŠs poziciĂłmeghatĂĄrozĂĄs, kĂŠsz, panel lĂŠtrehozĂĄsa, szinezĂŠse ĂŠs a multiline tooltop lĂŠtrehozĂĄsa szĂźksĂŠges sanya
		
		
		this.firstPos = 130+this.startTime*40;
		motePanel.setBounds(130+this.startTime*40 ,230+devicespos*40, this.timeLenght*40 ,30);		//koordinĂĄtĂĄk mĂŠg nem pontossak
		motePanel.setBorder(BorderFactory.createEtchedBorder());

		motePanel.setBackground(colorList.get(devicespos));
		String information ="<html>";
		for(int i =1; i<labelNames.length;i++){
			information=information+this.labelNames[i]+"<br>";
		}
		/*"<html>EszkĂśz neve: <br>"+this.labelNames[3]+"<br>HĂśmĂŠrsĂŠklet: <br>"+"adat"+"<br>valami: <br>"+"adat2"*/
		motePanel.setToolTipText(information);

		motePanel.setBackground(colorList.get(devicespos));

		
		return motePanel; 
	}
	
	//konstrukor
	public Packages(int startTime, int stopTime, String[] labelNames) {
		super();
		this.startTime = startTime;
		this.stopTime = stopTime;
		this.timeLenght = stopTime - startTime;
		this.labelNames = new String[labelNames.length];
		
		for(int i = 0; i < labelNames.length; i++){
			this.labelNames[i] = labelNames[i];
		}
	}	
}
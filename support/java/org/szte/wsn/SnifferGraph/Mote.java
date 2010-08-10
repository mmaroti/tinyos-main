package org.szte.wsn.SnifferGraph;
import java.awt.Color;

import javax.swing.BorderFactory;
import javax.swing.JCheckBox;
import javax.swing.JPanel;
import javax.swing.JTextField;


public class Mote {

	protected int startTime;
	protected int stopTime;
	protected int timeLenght;
	protected int firstPos;
	
	protected String[] labelNames;
	protected JTextField[] cells;
	protected JPanel motePanel;
	
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
	
	public void setMotePanel(JPanel motePanel) {
		this.motePanel = motePanel;
	}
	
	public JPanel getAPanel(JCheckBox[] box){
		int boxLength = 0;
		for(int i = 0;i<box.length; i++){
			if(box[i].isSelected()){
				boxLength++;
			}
		}
		this.firstPos = 130+this.startTime*100;
		motePanel = new JPanel();
		motePanel.setLayout(null);
		motePanel.setBounds(firstPos, 270, this.timeLenght*100, boxLength*70-50);		//bejelöld dobozok * dobozméret - egy fél dobozméret
		motePanel.setBackground(Color.orange);
		motePanel.setBorder(BorderFactory.createEtchedBorder());
		cells = new JTextField[boxLength];
		int k = 0, j = 0;
		for(int i = 0; i<box.length; i++){
			if(box[i].isSelected()){
				cells[j] = new JTextField(this.labelNames[i]);
				cells[j].setBackground(null);
				cells[j].setBorder(BorderFactory.createEtchedBorder());
				cells[j].setBounds(0, k, this.timeLenght*100, 20);
				motePanel.add(cells[j++]);
				k+=70;
			}
		}
		motePanel.setVisible(true);
		return motePanel;
	}
	
	public Mote(int startTime, int stopTime, String[] labelNames) {
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

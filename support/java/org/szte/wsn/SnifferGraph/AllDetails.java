package org.szte.wsn.SnifferGraph;

import java.awt.Font;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JPanel;
import javax.swing.JTextField;

public class AllDetails {
	
	public final int screenHeight = DataBase.screenHeight/2;							
	public final int screenWidth = DataBase.screenWidth/2;
	public static Font k = new Font("Arial", 1 ,12);
	public static Font n = new Font("Arial", 2 ,12);
	
	JFrame allDetails;
	
	public AllDetails(String aC) {
		
		int number = -1;
		try{
			number = Integer.parseInt(aC)-1;	
		}catch(Exception e){
			System.out.println("Valami nagy gubanc van... reméljük sosem lesz itt ilyen.");
		}
		String[] allDetailsString = SnifferGraph.motes.get(number).labelNames;
		int timeFirst = SnifferGraph.motes.get(number).startTime;
		int timeLast = SnifferGraph.motes.get(number).stopTime;
		
		allDetails = new JFrame("Details of " + allDetailsString[0] + " time: " + String.valueOf(timeFirst) + " - " + String.valueOf(timeLast));
		allDetails.setSize(500, 40*allDetailsString.length);
		allDetails.setLayout(null);
		allDetails.setLocation(screenWidth-(allDetails.getSize().width/2), screenHeight-(allDetails.getSize().width/2));
		
		JButton okButton = new JButton("Ok");
		okButton.setBounds(allDetails.getSize().width-80, allDetails.getSize().height-68, 70, 28);
		okButton.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent event) {
            	allDetails.setVisible(false);
            	System.gc();
            }
		});
		okButton.setToolTipText("Close the window");
		
		allDetails.add(okButton);
		allDetails.add(fixValues(allDetailsString[0], timeFirst, timeLast));
		int pos = 105; //mivel a felső 100 sor el van foglalva a fejléccel, innen kezdünk
		
		for(int i = 1;i<allDetailsString.length;i+=2){
			JTextField typeText = new JTextField(allDetailsString[i]);
			JTextField type = new JTextField(" " + allDetailsString[i+1]);
			
			typeText.setBackground(null);
			typeText.setBorder(null);
			typeText.setEditable(false);
			typeText.setFont(k);
			typeText.setBounds(15, pos, 150, 28);
			
			type.setBackground(null);
			type.setEditable(false);
			type.setFont(n);
			type.setBounds(165, pos, 320, 28);
			
			allDetails.add(typeText);
			allDetails.add(type);
			pos+=28;
		}
		
		allDetails.setVisible(true);
	}

	private JPanel fixValues(String dN, int first, int last) {
		JPanel return01 = new JPanel();
		
		return01.setLayout(null);
		JTextField deviceNameText = new JTextField("Device name:");
		deviceNameText.setBorder(null);
		deviceNameText.setBackground(null);
		deviceNameText.setEditable(false);
		deviceNameText.setFont(k);
		deviceNameText.setBounds(10, 10, 150, 28);
		
		JTextField deviceName = new JTextField(" " + dN);
		deviceName.setBackground(null);
		deviceName.setEditable(false);
		deviceName.setFont(n);
		deviceName.setBounds(160, 10, 315, 28);
		
		JTextField timeText = new JTextField("Time: ");
		timeText.setBorder(null);
		timeText.setBackground(null);
		timeText.setEditable(false);
		timeText.setFont(k);
		timeText.setBounds(10, 38, 150, 28);
		
		JTextField time = new JTextField(" " + String.valueOf(first) + " - " + String.valueOf(last));
		time.setBackground(null);
		time.setEditable(false);
		time.setFont(n);
		time.setBounds(160, 38, 315, 28);
		
		JTextField type = new JTextField("Type:");
		type.setBorder(null);
		type.setBackground(null);
		type.setEditable(false);
		type.setFont(k);
		type.setBounds(10, 70, 150, 28);
		
		JTextField massage = new JTextField("Massage:");
		massage.setBorder(null);
		massage.setBackground(null);
		massage.setEditable(false);
		massage.setFont(k);
		massage.setBounds(160, 70, 315, 28);
		
		return01.add(deviceNameText);
		return01.add(deviceName);
		return01.add(timeText);
		return01.add(time);
		return01.add(type);
		return01.add(massage);
		
		return01.setBounds(5, 5, 490, 100);
		
		return return01;
	}

}

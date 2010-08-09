package config;
import java.awt.Color;
import java.awt.Dimension;
import java.awt.Toolkit;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.util.ArrayList;

import javax.swing.*;

import xmlReadWrite.XmlWriter;



/**
 * 
 * @authors Németh Gábor, Nyilas Sándor Károly
 *Class creates the window for configuration creation
 */
public class CreatConfWindow {
	
	Dimension screenSize = Toolkit.getDefaultToolkit().getScreenSize();			
	
	public final int screenHeight = screenSize.height;							
	public final int screenWidth = screenSize.width;
	
	JFrame generating;
	
	JButton next = new JButton("next >");
	JButton back = new JButton("< back");
	JButton finish = new JButton("finish");
	JButton cancel = new JButton("cancel");
	
	JTextField startByte = new JTextField("Start byte:");
	JTextField stopByte = new JTextField("Stop byte:");
	JTextField type = new JTextField("Type:");
	JTextField name = new JTextField("Name:");
	
	JTextField startByteIn = new JTextField();
	JTextField stopByteIn = new JTextField();
	JTextField typeIn = new JTextField();
	JTextField nameIn = new JTextField();
	
	ArrayList<String> inputs = new ArrayList<String>();
	
	public static int szam = 1;
	/**
	 * Generates windows as much entity we need
	 * @param input This gives how many entity needed to be created
	 */
	public CreatConfWindow(String input){
		int numbersOfEntity;
		try{
			numbersOfEntity = Integer.parseInt(input);
		}catch(Exception e){
			numbersOfEntity = 0;
		}
		final int numbers = numbersOfEntity;
		generating = new JFrame("Error");
		
		generating.setSize(500,300);
		generating.setLayout(null);
		
		if(numbersOfEntity>0 && numbersOfEntity<=10){
			generating.setTitle("Input " + szam + " of " +numbersOfEntity);
			
			back.setEnabled(false);
			back.setBounds(230, 220, 80, 28);
			next.setBounds(315, 220, 80, 28);
			cancel.setBounds(400, 220, 80, 28);
			generating.add(back);
			generating.add(next);
			generating.add(cancel);
			
			startByte.setEnabled(false);
			stopByte.setEnabled(false);
			type.setEnabled(false);
			name.setEnabled(false);
			
			startByte.setBorder(null);
			stopByte.setBorder(null);
			type.setBorder(null);
			name.setBorder(null);
			
			startByte.setDisabledTextColor(Color.black);
			stopByte.setDisabledTextColor(Color.black);
			type.setDisabledTextColor(Color.black);
			name.setDisabledTextColor(Color.black);
			
			startByte.setBackground(null);
			stopByte.setBackground(null);
			type.setBackground(null);
			name.setBackground(null);
			
			startByte.setBounds(70, 20, 100, 28);
			stopByte.setBounds(70, 70, 100, 28);
			type.setBounds(70, 120, 100, 28);
			name.setBounds(70, 170, 100, 28);
			
			generating.add(startByte);
			generating.add(stopByte);
			generating.add(type);
			generating.add(name);
			
			startByteIn.setBounds(180, 20, 180, 28);
			stopByteIn.setBounds(180, 70, 180, 28);
			typeIn.setBounds(180, 120, 180, 28);
			nameIn.setBounds(180, 170, 180, 28);
			
			generating.add(startByteIn);
			generating.add(stopByteIn);
			generating.add(typeIn);
			generating.add(nameIn);
			
			generating.setLocation(screenWidth/2-250, screenHeight/2-150);			
			
			back.addActionListener(new ActionListener() {
	            public void actionPerformed(ActionEvent event) {
	            	back();
	            }
			});
			next.addActionListener(new ActionListener() {
	            public void actionPerformed(ActionEvent event) {
	            	try {
						next(numbers);
					} catch (Exception e) {
						// TODO Auto-generated catch block
						e.printStackTrace();
					}
	            }
			});
			cancel.addActionListener(new ActionListener() {
	            public void actionPerformed(ActionEvent event) {
	            	generating.setVisible(false);
	            	szam = 0;
	            }
			});
		}
		else{
			generating = notValid(numbersOfEntity);
		}
		generating.setVisible(true);
	}
	/**
	 * This function read the inputs to an ArrayList and gets next page
	 * @param numbersOfEntity How much maximal inputs can be
	 * @throws Exception	error manage
	 */
	protected void next(int numbersOfEntity) throws Exception {
		// TODO Még nincs, input ellenörzés.
		String input = startByteIn.getText();
		if(input.length() == 0){
			inputs.add("Err - startByte");
		}
		else{
			inputs.add(input);
		}
		
		input = stopByteIn.getText();
		if(input.length() == 0){
			inputs.add("Err - stopByte");
		}
		else{
			inputs.add(input);
		}
		
		input = typeIn.getText();
		if(input.length() == 0){
			inputs.add("Err - type");
		}
		else{
			inputs.add(input);
		}
		
		input = nameIn.getText();
		if(input.length() == 0){
			inputs.add("Err - name");
		}
		else{
			inputs.add(input);
		}
		back.setEnabled(true);
		startByteIn.setText("");
		stopByteIn.setText("");
		typeIn.setText("");
		nameIn.setText("");
		if(szam+1 == numbersOfEntity){
			next.setText("Finish");
		}
		else if(szam == numbersOfEntity){
			XmlWriter.test2(inputs);
			generating.setVisible(false);
			szam = 0;
		}
		generating.setTitle("Input " + szam + " of " +numbersOfEntity);
		++szam;
		
	}

	/**
	 * 
	 */
	protected void back() {
		// TODO Auto-generated method stub
		
	}

		/**
		 * This returns if we give valid value for an entity
		 * @param numbersOfEntity  gives the non-correct value
		 * @return	The error pane
		 */
	private JFrame notValid(int numbersOfEntity) {
		JTextField errMass = new JTextField("Invaild number, the number is: " + String.valueOf(numbersOfEntity));
		JButton errOk = new JButton("OK");
		errMass.setBounds(100, 50, 300, 28);
		errMass.setBorder(null);
		errMass.setEditable(false);
		errOk.setBounds(145, 100, 75, 28);
		generating.setSize(400, 200);
		generating.setLocation(screenWidth/2-200, screenHeight/2-100);
		generating.add(errMass);
		generating.add(errOk);
		errOk.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent event) {
            	generating.setVisible(false);
            }
		});
		return generating;
		
	}
	
	

}

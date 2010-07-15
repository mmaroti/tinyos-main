package config;
import java.awt.Dimension;
import java.awt.Point;
import java.awt.Toolkit;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.util.Locale;

import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JTextField;
/**
 * 
 * @author Ember
 *	This class cares  if we want to create configuration XML file to be saved
 *
 */

public class CreateConfGUI {
	
	Dimension screenSize = Toolkit.getDefaultToolkit().getScreenSize();			
	
	public final int screenHeight = screenSize.height;							
	public final int screenWidth = screenSize.width;
	
	JFrame entity = new JFrame("Number's of...");
	
	JButton ok = new JButton("OK");
	JButton cancel = new JButton("Cancel");
	
	JTextField input = new JTextField();
	
	public CreateConfGUI(){
		entity.setLayout(null);
		input.setBounds(30, 55, 150, 28);
		ok.setBounds(190, 55, 75, 28);
		cancel.setBounds(270, 55, 75, 28);
		
		entity.add(input);
		entity.add(ok);
		entity.add(cancel);
		
		ok.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent event) {
            	entity.setVisible(false);
            	new CreatConfWindow(input.getText());
            }
		});
		
		cancel.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent event) {
            	entity.setVisible(false);
            }
		});
		
		entity.setSize(400, 200);
		entity.setLocation((screenWidth/2)-200, (screenHeight/2)-100);
		entity.setVisible(true);
	}

}

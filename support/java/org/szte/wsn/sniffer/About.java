package org.szte.wsn.sniffer;

import java.awt.Color;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.*;

/**
 * 
 */

/**
 * @author Nemeth Gabor, Nyilas Sandor Karoly
 *A class gives information about the version
 */
public class About{				

	/**
	 * 
	 */
	private static final long serialVersionUID = 2L;
	/**
	 * 
	 */
	
	JFrame about = new JFrame("About");
	JTextField line = new JTextField("  v0.002b  ");
	JButton OK = new JButton("OK");
	
	public About() {
		about.setSize(300, 200);
		about.setLayout(null);
		line.setEditable(false);
		line.setBackground(Color.white);
		
		OK.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent event) {
                about.setVisible(false);
            }
		});
		
		about.add(OK);
		about.add(line);
		OK.setBounds(10, 30, 60, 28);
		line.setBounds(80, 30, 60, 28);
		about.setVisible(true);
	}
}

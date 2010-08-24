package org.szte.wsn.SnifferGraph.config;
import java.awt.Dimension;
import java.awt.Toolkit;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JTextField;
/**Copyright (c) 2010, University of Szeged
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
 * 
 * @author Németh Gábor
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
		input.setBounds(30, 50, 150, 28);
		ok.setBounds(190, 50, 75, 28);
		cancel.setBounds(270, 50, 90, 28);
		
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
		
		entity.setSize(400, 150);
		entity.setLocation((screenWidth/2)-200, (screenHeight/2)-75);
		entity.setVisible(true);
	}

}

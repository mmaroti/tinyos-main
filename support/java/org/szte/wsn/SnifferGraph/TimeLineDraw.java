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

import java.awt.BasicStroke;
import java.awt.Graphics;
import java.awt.Graphics2D;

import java.util.ArrayList;

import javax.swing.JPanel;


public class TimeLineDraw extends JPanel {

    /**
	 *This class draws in Graphical2d the Vertical Lines for the timeLine. 
	 */
	private static final long serialVersionUID = 1L;

	public void paintComponent(Graphics g) {										
            super.paintComponent(g);
            Graphics2D g2d = (Graphics2D) g;
            g2d.setStroke(new BasicStroke(2));
            ArrayList<Graphics2D> verticalLines = new ArrayList<Graphics2D>();
            for(int i = 0;i<100;i++){
            	verticalLines.add((Graphics2D) g);
            }
            
            int k=0;
            for(int i = 0;i<100;i++){
            	verticalLines.get(i).drawLine(1+k, 1, 1+k, 12);
            	k+=100;
            }
            
            g2d.setStroke(new BasicStroke(5));									
            g2d.drawLine(1, 4, 168000, 4);										
    }

	
}

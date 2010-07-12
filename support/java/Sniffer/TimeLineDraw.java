

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
            for(int i = 0;i<1000;i++){
            	verticalLines.add((Graphics2D) g);
            }
            
            int k=0;
            for(int i = 0;i<1000;i++){
            	verticalLines.get(i).drawLine(1+k, 1, 1+k, 12);
            	k+=70;
            }
            
            g2d.setStroke(new BasicStroke(5));									
            g2d.drawLine(1, 4, 168000, 4);										
    }

	
}



import java.awt.BasicStroke;
import java.awt.Graphics;
import java.awt.Graphics2D;

import java.util.ArrayList;

import javax.swing.JPanel;


public class TimeLineDraw extends JPanel {

    /**
	 * 
	 */
	private static final long serialVersionUID = 1L;

	public void paintComponent(Graphics g) {										//Time Line is a Graphics2D object.
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
            	k+=70;
            }
            
            g2d.setStroke(new BasicStroke(5));									//To have More tougher object 
            g2d.drawLine(1, 4, 168000, 4);										//And it will be shown in so long
    }

	
}

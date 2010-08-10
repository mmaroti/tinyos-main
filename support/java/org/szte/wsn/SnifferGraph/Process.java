package org.szte.wsn.SnifferGraph;
import java.util.ArrayList;
import java.util.Calendar;
import java.util.Date;
import java.util.GregorianCalendar;

import javax.swing.JCheckBox;
import javax.swing.JPanel;
/**
 * 
 * @author Nyilas Sandor Karoly , Nemeth Gabor
 * 
 * This class will get datas from sniffer and used after process 
 *
 */

public class Process {
	
	static ArrayList<JPanel> dataPanel = SnifferGraph.dataPanel;
	public static JPanel backgPanel = SnifferGraph.backgPanel;
	static ArrayList<Mote> motes = SnifferGraph.motes;
	static JCheckBox box[] = SnifferGraph.box;
	
	static Date date = new Date();
	public static int currentlyMillisec = Integer.valueOf((int) (System.currentTimeMillis()%100));
	
	public static String whatTimeIsNow(){
		return String.valueOf(secundum()) + "s : " +String.valueOf(milliSec()) + "ms";
	}
	
	private static String secundum() {
		Calendar cal = new GregorianCalendar();
		long k = cal.get(Calendar.SECOND);
		return String.valueOf(k);
	}

	public static int milliSec(){
		Calendar cal = new GregorianCalendar();
		int k = cal.get(Calendar.MILLISECOND);
		return k;
	}

	public static String dataCodeing(int i, int j) {
		
		return "Meg semmi";
	}

	public static String makeTimeLineSting(int i) {
		i++;
		if(i<10) return "000" + String.valueOf(i);
		else if(i<100) return "00" + String.valueOf(i);
		else if(i<1000) return "0" + String.valueOf(i);
		return "over";
	}
	
	public static String whatMoteTimeNow(){
		return "#" + whatTimeIsNow();
		
	}

	public static void reDraw() {
		for(int i = 0; i<dataPanel.size();i++){
			dataPanel.get(i).setVisible(false);
			backgPanel.remove(dataPanel.get(i));
		}
		int length = dataPanel.size();
		for(int i = 0; i<length; i++){
			dataPanel.remove(0);
		}
		for(int i = 0; i<motes.size();i++){
			dataPanel.add(motes.get(i).getAPanel(box));
			dataPanel.get(dataPanel.size()-1).setVisible(true);
			backgPanel.add(dataPanel.get(dataPanel.size()-1));
		}
		SnifferGraph.scrollbar.setMaximum((motes.get(motes.size()-1).firstPos));
	}

}

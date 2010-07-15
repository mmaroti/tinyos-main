import java.util.Calendar;
import java.util.Date;
import java.util.GregorianCalendar;
/**
 * 
 * @author Nyilas Sandor Karoly , Nemeth Gabor
 * 
 * This class will get datas from sniffer and used after process 
 *
 */

public class Process {
	
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

}

import java.util.Random;


public class RandomString {
	
	public static boolean getAnewInput(){
		Random r = new Random();
		int random = r.nextInt(1000000000);
		if(random<20) return true;
		return false;
	}
	
	public static String[] getRandomMoteStrings(int mutch){
		Random rn = new Random();
		String[] r = new String[mutch];
		String inputChars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
		for(int i = 0; i<mutch; i++){
			String c = null;
			for(int k = 0; k<10; k++){
				c = c + inputChars.charAt(rn.nextInt(61));
			}
			r[i] = c;
		}
		return r;
	}
	
	public static int getAnumber(int number){
		Random rn = new Random();
		return number + rn.nextInt(3)+1;
	}

}

import java.net.*;
import java.io.*;

public class server {
	public static void main(String[] args) throws IOException {
	ServerSocket serverSocket = null;
	Socket clientSocket = null;
	BufferedReader in= null;
	PrintWriter out = null;
	String message=null;
	int recBufferSize=0,recByteNum=0;
	char[] data;
	
	if(args.length!=1){
		System.out.println("Usage: java server [port]");
		System.exit(1);
	}

	try {
		serverSocket = new ServerSocket(Integer.parseInt(args[0]));
	} catch (IOException e) {
		System.err.println("Could not listen on port:" + args[0] + ".");
		System.exit(1);
	}
	try {
		clientSocket = serverSocket.accept();
	} catch (IOException e) {
		System.err.println("Accept failed.");
		System.exit(1);
	}
	
	System.out.println("Connected");
	
	recBufferSize=clientSocket.getReceiveBufferSize();
	in = new BufferedReader(new InputStreamReader(clientSocket.getInputStream()));
	out = new PrintWriter(clientSocket.getOutputStream(), true);
	data = new char[recBufferSize];

	do{
		if((message=in.readLine())==null){
			break;
		}else{
			for(int i=8;i<message.length();++i){
				System.out.print((char)message.charAt(i));
			}			
			out.println(message);
			
		}
	}while(true);

	in.close();
	clientSocket.close();
	serverSocket.close();
	}
}
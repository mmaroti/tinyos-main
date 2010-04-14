import java.io.IOException;
//import java.util.ArrayList;

import net.tinyos.message.Message;
import net.tinyos.message.MessageListener;
import net.tinyos.message.MoteIF;
import net.tinyos.packet.BuildSource;
import net.tinyos.packet.PhoenixSource;
import net.tinyos.util.PrintStreamMessenger;


public class StreamEraser implements MessageListener {
	private MoteIF moteIF;
	private int nodeid;
	private boolean cmdSent=false;
	//private ArrayList<dataFile> files = new ArrayList<dataFile>();
	
	public StreamEraser(String source, int nodeid) {
		PhoenixSource phoenix;
		this.nodeid=nodeid;
		if (source == null) {
			phoenix = BuildSource.makePhoenix(PrintStreamMessenger.err);
		} else {
			phoenix = BuildSource.makePhoenix(source, PrintStreamMessenger.err);
		}
		this.moteIF = new MoteIF(phoenix);
		this.moteIF.registerListener(new ctrltsMsg(), this);
		System.out.println("Waiting for node #"+nodeid);
	}

	public void messageReceived(int to, Message message) {
		if (message instanceof ctrltsMsg && message.dataLength() == ctrltsMsg.DEFAULT_MESSAGE_SIZE) {
			ctrltsMsg msg = (ctrltsMsg) message;
			if(msg.getSerialPacket().get_header_src()==nodeid){
				if(cmdSent==false){
					System.out.println("Found node, sending erase command");
					ctrlMsg response = new ctrlMsg();
					response.set_min_address(0);
					response.set_max_address(0);
					try {
						moteIF.send(nodeid, response);
						cmdSent=true;
					} catch (IOException e) {
						// TODO Auto-generated catch block
						e.printStackTrace();
					}
				} else {
					System.out.print("New message from node: ");
					System.out.println("MinAddress: " + msg.get_min_address()+" MaxAddress: "+msg.get_max_address());
					System.exit(0);
				}
			}
		}
	}
	
	private static void usage() {
		System.out.println("Usage: StreamEraser <NODEID> [-comm port]");
		System.exit(1);
	}
	
	public static void main(String[] args) throws Exception {
		String source = null;
		if (args.length == 3) {
			if (args[1].equals("-comm")) {
				source = args[2];
			} else
				usage();
		}
		int node_id=0;
		try{
			node_id=Integer.valueOf(args[0]);
		}catch(NumberFormatException e){
			usage();
		}

		new StreamEraser(source, node_id);
	}
}

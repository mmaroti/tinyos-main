package org.szte.wsn.downloader2;

import java.io.IOException;
import net.tinyos.message.Message;
import net.tinyos.message.MessageListener;
import net.tinyos.message.MoteIF;
import net.tinyos.packet.BuildSource;
import net.tinyos.packet.PhoenixSource;
import net.tinyos.util.PrintStreamMessenger;

public class Communication  implements MessageListener {

	private MoteIF moteIF;
	private StreamDownloader sd;
	private short seqnum=0;
	
	@Override
	public void messageReceived(int to, Message m) {
		if(m instanceof CtrlMsg){
			CtrlMsg rec=(CtrlMsg)m;
			if(seqnum==rec.get_seq_num())
				sd.newPong(rec.get_source(),rec.get_min_address(),rec.get_max_address(),true);
			else
				sd.newPong(rec.get_source(),rec.get_min_address(),rec.get_max_address(),false);
		} else if(m instanceof DataMsg){
			DataMsg rec=(DataMsg)m;
			byte[] data=new byte[rec.get_length()];
			for(short i=0;i<data.length;i++){
				data[i]=rec.getElement_payload(i);
			}
			sd.newData(rec.get_source(),rec.get_address(),data);
		}
	}
	
	public void sendGet(int nodeID, long minaddress, long maxaddress) throws IOException{
		GetMsg get=new GetMsg();
		get.set_nodeid(nodeID);
		get.set_min_address(minaddress);
		get.set_max_address(maxaddress);
		get.set_seq_num(++seqnum);
		moteIF.send(MoteIF.TOS_BCAST_ADDR, get);
	}
	
	private void sendCommnad(short command) throws IOException{
		CommandMsg cmd=new CommandMsg();
		cmd.set_cmd(command);
		moteIF.send(MoteIF.TOS_BCAST_ADDR, cmd);
	}

	public void sendPing() throws IOException{
		sendCommnad(CommandMsg.COMMAND_PING);
	}
	
	public void sendErase() throws IOException{
		//sendGet(MoteIF.TOS_BCAST_ADDR, 0, 0);
		sendCommnad(CommandMsg.COMMAND_ERASE);
	}
	
	public Communication(StreamDownloader sd,String source){
		this.sd=sd;
		PhoenixSource phoenix;
		if (source == null) {
			phoenix = BuildSource.makePhoenix(PrintStreamMessenger.err);
		} else {
			phoenix = BuildSource.makePhoenix(source, PrintStreamMessenger.err);
		}
		this.moteIF = new MoteIF(phoenix);
		this.moteIF.registerListener(new DataMsg(), this);
		this.moteIF.registerListener(new CtrlMsg(), this);
	}

}

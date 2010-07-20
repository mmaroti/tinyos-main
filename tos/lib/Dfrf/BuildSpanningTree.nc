#include "AM.h"
generic module BuildSpanningTree(typedef payload_t)
{
	provides
	{
		interface DfrfSend as Send;
		interface DfrfReceive as Receive;
		interface SpanningTree;
		interface Get<uint16_t> as GetRank; //just for testing! use spanningtree interface
	}
	uses
	{
		interface AMPacket;
		interface DfrfSend;
		interface DfrfReceive;
	}
}

implementation
{
	typedef nx_struct spanningtree_packet {
		nx_uint16_t rootAddress;	// address of the root
		payload_t payload;
		nx_uint16_t source;
	} spanningtree_packet_t;
	
	
	uint16_t parent=0xffff;
	uint16_t rootAddress = 0xffff;

	
	command error_t Send.send(void *send){
		error_t err;
		spanningtree_packet_t data;
		
		rootAddress = call AMPacket.address();
		parent=0xffff;

		data.rootAddress = rootAddress;
		data.source = rootAddress;
		data.payload=*((payload_t*)send);
		

		err=call DfrfSend.send(&data);
		return err;
	}
	

	command am_addr_t SpanningTree.rootAddress()
	{
		return rootAddress;
	}

	command void SpanningTree.setRootAddress(am_addr_t ra)
	{
		rootAddress = ra;
	}

	command am_addr_t SpanningTree.getParent(){
		return parent;
	}

	command void SpanningTree.setParent(am_addr_t par)
	{
		parent=par;
	}

	/**** implementation ****/

	event bool DfrfReceive.receive(void* raw_data)
	{
		spanningtree_packet_t *data=(spanningtree_packet_t*)raw_data;

		// detect a new beaconing round
		if(( rootAddress ) != ( data->rootAddress  )) {
			rootAddress = data->rootAddress;
			parent = data->source;
			data->source=call AMPacket.address();
		}
		if(data->rootAddress!=call AMPacket.address()){
			signal Receive.receive(&(data->payload));
			return TRUE;
		} else 
			return FALSE;
	}


	command uint16_t GetRank.get(){
		return call SpanningTree.getParent();
	}

}

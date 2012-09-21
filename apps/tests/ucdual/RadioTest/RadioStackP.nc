#include <Si443xRadio.h>
#include <Tasklet.h>

module RadioStackP {
	provides {
		interface Si443xDriverConfig as Config;
		interface PacketTimeStamp<TRadio, uint32_t> as PacketTimeStampRadio;

		interface PacketFlag as TransmitPowerFlag;
		interface PacketFlag as RSSIFlag;
		interface PacketFlag as TimeSyncFlag;
	}
}

implementation {
	async command uint8_t Config.headerLength(message_t* msg)
	{
		return offsetof(message_t, data) - sizeof(si443xpacket_header_t);
	}

	async command uint8_t Config.maxPayloadLength()
	{
		return sizeof(si443xpacket_header_t) + TOSH_DATA_LENGTH;
	}

	async command uint8_t Config.metadataLength(message_t* msg)
	{
		return 0;
	}

	async command uint8_t Config.headerPreloadLength()
	{
		// we need the fcf, dsn, destpan and dest
		return 7;
	}

	async command bool Config.requiresRssiCca(message_t* msg)
	{
		return FALSE;
	}
	
	
	async command bool PacketTimeStampRadio.isValid(message_t* msg)	{ return FALSE;	}
	async command uint32_t PacketTimeStampRadio.timestamp(message_t* msg) { return FALSE; }
	async command void PacketTimeStampRadio.clear(message_t* msg) { }
	async command void PacketTimeStampRadio.set(message_t* msg, uint32_t value)	{ }
	
	async command bool TransmitPowerFlag.get(message_t* msg) { return FALSE;	}
	async command void TransmitPowerFlag.set(message_t* msg)	{	}
	async command void TransmitPowerFlag.clear(message_t* msg)	{	}
	async command void TransmitPowerFlag.setValue(message_t* msg, bool value)	{	}
	
	async command bool RSSIFlag.get(message_t* msg) { return FALSE;	}
	async command void RSSIFlag.set(message_t* msg)	{	}
	async command void RSSIFlag.clear(message_t* msg)	{	}
	async command void RSSIFlag.setValue(message_t* msg, bool value)	{	}
	
	async command bool TimeSyncFlag.get(message_t* msg) { return FALSE;	}
	async command void TimeSyncFlag.set(message_t* msg)	{	}
	async command void TimeSyncFlag.clear(message_t* msg)	{	}
	async command void TimeSyncFlag.setValue(message_t* msg, bool value)	{	}
	

}

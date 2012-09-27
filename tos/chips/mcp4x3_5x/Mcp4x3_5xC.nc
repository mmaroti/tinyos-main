configuration Mcp4x3_5xC
{
	provides interface Write<uint16_t> as Write0;
	provides interface Write<uint16_t> as Write1;
	provides interface Read<uint16_t> as Read0;
	provides interface Read<uint16_t> as Read1;
	provides interface SplitControl as Control0;
	provides interface SplitControl as Control1;
}
implementation
{
	components new Mcp4x3_5xP(0, 0) as Pot0, new Mcp4x3_5xP(4, 1) as Pot1;
	Write0 = Pot0.Write;
	Write1 = Pot1.Write;
	Read0 = Pot0.Read;
	Read1 = Pot1.Read;
	Control0 = Pot0.SplitControl;
	Control1 = Pot1.SplitControl;
	
	components HplMcp4x3_5xC, LedsC, MainC;
	Pot0.GetAddress -> HplMcp4x3_5xC;
	Pot1.GetAddress -> HplMcp4x3_5xC;
	
	Pot0.I2C -> HplMcp4x3_5xC.Pot0I2CPacket;
	Pot1.I2C -> HplMcp4x3_5xC.Pot1I2CPacket;
	Pot0.I2CResource -> HplMcp4x3_5xC.Pot0I2CResource;
	Pot1.I2CResource -> HplMcp4x3_5xC.Pot1I2CResource;
	
	Pot0.OtherTCONPart -> Pot1.MyTCONPart;
	Pot1.OtherTCONPart -> Pot0.MyTCONPart;
	
	Pot0.SelfPower -> HplMcp4x3_5xC.SelfPower;
	Pot1.SelfPower -> HplMcp4x3_5xC.SelfPower;
	
	Pot0.Init <- MainC.SoftwareInit;
	Pot1.Init <- MainC.SoftwareInit;
	
	components NoDiagMsgC as DiagMsgC;
	Pot0.DiagMsg -> DiagMsgC;
	Pot1.DiagMsg -> DiagMsgC;
}
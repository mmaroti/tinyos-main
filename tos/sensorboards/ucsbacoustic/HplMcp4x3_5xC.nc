configuration HplMcp4x3_5xC{
	provides interface I2CPacket<TI2CBasicAddr> as Pot0I2CPacket;
	provides interface I2CPacket<TI2CBasicAddr> as Pot1I2CPacket;
	provides interface Resource as Pot0I2CResource;
	provides interface Resource as Pot1I2CResource;
	provides interface GetNow<uint8_t>;
	provides interface BusPowerManager as SelfPower;
}
implementation{
	components new Atm128I2CMasterC() as Pot0I2C, new Atm128I2CMasterC() as Pot1I2C, Mcp4x3_5xAddrC;

	Pot0I2CPacket = Pot0I2C.I2CPacket;
	//Pot0I2CResource  = Pot0I2C.Resource;
	Pot1I2CPacket = Pot1I2C.I2CPacket;
	//Pot1I2CResource  = Pot1I2C.Resource;
	
	GetNow = Mcp4x3_5xAddrC;
	components GainPotPowerManagerC;
	//components new DummyBusPowerManagerC() as GainPotPowerManagerC;
	SelfPower = GainPotPowerManagerC;
	
	components new I2CPowerC() as I2CPower0C, new I2CPowerC() as I2CPower1C;
	Pot0I2CResource = I2CPower0C.Resource;
	Pot1I2CResource = I2CPower1C.Resource;
	I2CPower0C.SubResource -> Pot0I2C.Resource;
	I2CPower1C.SubResource -> Pot1I2C.Resource;
	
	components I2CWireC;
}
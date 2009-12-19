COMPONENT=RSSINodeAppC
PFLAGS = -ID:/Projekt_szeged/Eclipse_Projects/RSSI_06
BUILD_EXTRA_DEPS = Int8Msg.java Uint8Msg.java Uint16Msg.java
CLEAN_EXTRA = $(BUILD_EXTRA_DEPS) Int8Msg.java Uint8Msg.java Uint16Msg.java

Int8Msg.java: ../Packet.h
	mig -target=null -java-classname=Int8Msg java ../Packet.h dataInt8 -o $@

Uint8Msg.java: ../Packet.h
	mig -target=null -java-classname=Uint8Msg java ../Packet.h dataUint8 -o $@

Uint16Msg.java: ../Packet.h
	mig -target=null -java-classname=Uint16Msg java ../Packet.h dataUint16 -o $@

include $(MAKERULES)

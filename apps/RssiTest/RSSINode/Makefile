COMPONENT=RSSINodeAppC
PFLAGS = -I$(TOSROOT)apps/rssiTest
BUILD_EXTRA_DEPS = DataMsg.java
CLEAN_EXTRA = $(BUILD_EXTRA_DEPS) RSSIMsg.java

DataMsg.java: ../Packet.h
	mig -target=null -java-classname=DataMsg java ../Packet.h datapacket -o $@

include $(MAKERULES)

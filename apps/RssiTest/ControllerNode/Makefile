COMPONENT=ControllerAppC
PFLAGS = -ID:/Projekt_szeged/Eclipse_Projects/!apps_RssiTest
BUILD_EXTRA_DEPS = ControlMsg.java
CLEAN_EXTRA = $(BUILD_EXTRA_DEPS) ControlMsg.java

ControlMsg.java: ../Packet.h
	mig -target=null -java-classname=ControlMsg java ../Packet.h controlpacket -o $@


include $(MAKERULES)

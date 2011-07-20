#COMPONENT=SerialAppC
COMPONENT=TempWriteAppC
CFLAGS = -DCC2420_DEF_CHANNEL=16
CFLAGS += -DLOW_POWER_LISTENING
BUILD_EXTRA_DEPS = SerialMsg.java
CLEAN_EXTRA = $(BUILD_EXTRA_DEPS) SerialMsg.class

SerialMsg.java: TempStorage.h
	mig -target=null -java-classname=SerialMsg java	TempStorage.h BlinkToRadioMsg -o $@

PFLAGS += -DTIMESYNC_RATE=3
#PFLAGS += -DTIMESYNC_DEBUG

PFLAGS += -I$(TOSDIR)/lib/ftsp -I$(TOSDIR)/../apps/RadioCountToLeds

include $(MAKERULES)


#COMPONENT=SerialAppC
#COMPONENT=TempWriteAppC
COMPONENT=TempReadAppC
#COMPONENT=TempEraseAppC
CFLAGS = -DCC2420_DEF_CHANNEL=11
BUILD_EXTRA_DEPS = SerialMsg.java
CLEAN_EXTRA = $(BUILD_EXTRA_DEPS) SerialMsg.class

SerialMsg.java: TempStorage.h
	mig -target=null -java-classname=SerialMsg java	TempStorage.h BlinkToRadioMsg -o $@

include $(MAKERULES)


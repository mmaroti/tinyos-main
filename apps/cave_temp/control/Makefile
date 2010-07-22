COMPONENT=SerialAppC
#COMPONENT=BlinkToRadioAppC
CFLAGS = -DCC2420_DEF_CHANNEL=1
BUILD_EXTRA_DEPS = SerialMsg.java
CLEAN_EXTRA = $(BUILD_EXTRA_DEPS) SerialMsg.class

SerialMsg.java: BlinkToRadio.h
	mig -target=null -java-classname=SerialMsg java	BlinkToRadio.h BlinkToRadioMsg -o $@

include $(MAKERULES)


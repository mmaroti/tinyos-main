COMPONENT=MicAdcReadAppC
BUILD_EXTRA_DEPS = DataMsg.java CtrlMsg.java
CLEAN_EXTRA = $(BUILD_EXTRA_DEPS) DataMsg.java CtrlMsg.java

DataMsg.java: MicAdcRead.h
	mig -target=null -java-classname=DataMsg java MicAdcRead.h data_msg -o $@
CtrlMsg.java: MicAdcRead.h
	mig -target=null -java-classname=CtrlMsg java MicAdcRead.h ctrl_msg -o $@

include $(MAKERULES)

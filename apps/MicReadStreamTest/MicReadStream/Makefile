COMPONENT=MicReadStreamAppC
CFLAGS += -DASSERT_NONE
CFLAGS += -DTASKLET_IS_TASK
CFLAGS += -DTOSH_DATA_LENGTH=114
CFLAGS += -I$(SZTETOSDIR)/lib/Atm128FastAdc
CFLAGS += -I$(TOSDIR)/sensorboards/mts300
BUILD_EXTRA_DEPS = MicMsg.java CtrlMsg.java ReadyMsg.java
CLEAN_EXTRA = $(BUILD_EXTRA_DEPS) MicMsg.java CtrlMsg.java ReadyMsg.java

MicMsg.java: MicReadStream.h
	mig -target=null -java-classname=MicMsg java MicReadStream.h datamsg -o $@
CtrlMsg.java: MicReadStream.h
	mig -target=null -java-classname=CtrlMsg java MicReadStream.h ctrlmsg -o $@
ReadyMsg.java: MicReadStream.h
	mig -target=null -java-classname=ReadyMsg java MicReadStream.h readymsg -o $@

include $(MAKERULES)

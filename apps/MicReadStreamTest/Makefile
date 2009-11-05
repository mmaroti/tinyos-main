COMPONENT=MicStreamAppC
PFLAGS = -I$(TOSDIR)/sensorboards/mts300 -I$(TOSDIR)/lib/MicReadStream
BUILD_EXTRA_DEPS = MicMsg.java CtrlMsg.java ReadyMsg.java
CLEAN_EXTRA = $(BUILD_EXTRA_DEPS) MicMsg.java CtrlMsg.java ReadyMsg.java

MicMsg.java: MicStream.h
	mig -target=null -java-classname=MicMsg java MicStream.h datamsg -o $@
CtrlMsg.java: MicStream.h
	mig -target=null -java-classname=CtrlMsg java MicStream.h ctrlmsg -o $@
ReadyMsg.java: MicStream.h
	mig -target=null -java-classname=ReadyMsg java MicStream.h readymsg -o $@


include $(MAKERULES)

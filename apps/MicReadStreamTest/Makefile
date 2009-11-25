COMPONENT=MicReadStreamAppC
PFLAGS = -I$(TOSDIR)/sensorboards/mts300 -I$(TOSDIR)/lib/MicReadStream
BUILD_EXTRA_DEPS = MicMsg.java CtrlMsg.java ReadyMsg.java
CLEAN_EXTRA = $(BUILD_EXTRA_DEPS) MicMsg.java CtrlMsg.java ReadyMsg.java

MicMsg.java: MicReadStream.h
	mig -target=null -java-classname=MicMsg java MicReadStream.h datamsg -o $@
CtrlMsg.java: MicReadStream.h
	mig -target=null -java-classname=CtrlMsg java MicReadStream.h ctrlmsg -o $@
ReadyMsg.java: MicReadStream.h
	mig -target=null -java-classname=ReadyMsg java MicReadStream.h readymsg -o $@


include $(MAKERULES)

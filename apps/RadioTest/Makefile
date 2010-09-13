COMPONENT=RadioTestAppC
BUILD_EXTRA_DEPS = CtrlMsgT.java ResponseMsgT.java StatT.java SetupT.java ProfileT.java
CLEAN_EXTRA      = CtrlMsgT.java ResponseMsgT.java StatT.java SetupT.java ProfileT.java main.elf *.class

CFLAGS += -I$(SZTETOSDIR)/lib/CodeProfile

CFLAGS += -DCC2420_DEF_CHANNEL=11
#CFLAGS += -DRF230_DEF_CHANNEL=11
CFLAGS += -DLOW_POWER_LISTENING
CFLAGS += -DTOSH_DATA_LENGTH=50
#CFLAGS += -DRF230_BACKOFF_INIT=9920

#setup_t.py: RadioTest.h
#	mig python -target=$(PLATFORM) $(CFLAGS) -python-classname=setup_t RadioTest.h setup_t -o $@

#ctrlmsg_t.py: RadioTest.h
#	mig python -target=$(PLATFORM) $(CFLAGS) -python-classname=ctrlmsg_t RadioTest.h ctrlmsg_t -o $@

StatT.java: RadioTest.h
	mig -target=$(PLATFORM) -I$(SZTETOSDIR)/lib/CodeProfile -java-classname=StatT java RadioTest.h stat_t -o $@

SetupT.java: RadioTest.h
	mig -target=$(PLATFORM) -I$(SZTETOSDIR)/lib/CodeProfile -java-classname=SetupT java RadioTest.h setup_t -o $@

CtrlMsgT.java: RadioTest.h
	mig -target=$(PLATFORM) -I$(SZTETOSDIR)/lib/CodeProfile -java-classname=CtrlMsgT java RadioTest.h ctrlmsg_t -o $@

ResponseMsgT.java: RadioTest.h
	mig -target=$(PLATFORM) -I$(SZTETOSDIR)/lib/CodeProfile -java-classname=ResponseMsgT java RadioTest.h responsemsg_t -o $@

ProfileT.java: $(SZTETOSDIR)/lib/CodeProfile/CodeProfile.h
	mig -target=$(PLATFORM) -I$(SZTETOSDIR)/lib/CodeProfile -java-classname=ProfileT java $(SZTETOSDIR)/lib/CodeProfile/CodeProfile.h profile_t -o $@

include $(MAKERULES)

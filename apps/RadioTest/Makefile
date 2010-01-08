COMPONENT=RadioTestAppC
BUILD_EXTRA_DEPS = ctrlmsg_t.py statmsg_t.py setup_t.py StatT.java CtrlMsgT.java StatMsgT.java SetupT.java
CLEAN_EXTRA      = ctrlmsg_t.py statmsg_t.py setup_t.py StatT.java CtrlMsgT.java StatMsgT.java SetupT.java main.elf *.class

setup_t.py: RadioTest.h
	mig python -target=$(PLATFORM) $(CFLAGS) -python-classname=setup_t RadioTest.h setup_t -o $@

ctrlmsg_t.py: RadioTest.h
	mig python -target=$(PLATFORM) $(CFLAGS) -python-classname=ctrlmsg_t RadioTest.h ctrlmsg_t -o $@

statmsg_t.py: RadioTest.h
	mig python -target=$(PLATFORM) $(CFLAGS) -python-classname=statmsg_t RadioTest.h statmsg_t -o $@

StatT.java: RadioTest.h
	mig -target=$(PLATFORM) -java-classname=StatT java RadioTest.h stat_t -o $@

SetupT.java: RadioTest.h
	mig -target=$(PLATFORM) -java-classname=SetupT java RadioTest.h setup_t -o $@

CtrlMsgT.java: RadioTest.h
	mig -target=$(PLATFORM) -java-classname=CtrlMsgT java RadioTest.h ctrlmsg_t -o $@

StatMsgT.java: RadioTest.h
	mig -target=$(PLATFORM) -java-classname=StatMsgT java RadioTest.h statmsg_t -o $@

include $(MAKERULES)

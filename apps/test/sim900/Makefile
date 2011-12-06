COMPONENT=TestC
CFLAGS +=-I$(SZTETOSDIR)/chips/sim900
CFLAGS +=-I$(SZTETOSDIR)/sensorboards/ucsim900
BUILD_EXTRA_DEPS = GsmMsg.java DataTest.class
CLEAN_EXTRA = $(BULID_EXTRA_DEPS) *.class GsmMsg.java
GsmMsg.java: gsm.h
		mig -target=null -java-classname=GsmMsg java gsm.h gsmmsg -o $@

DataTest.class: GsmMsg.java DataTest.java
	javac *.java
include $(MAKERULES)


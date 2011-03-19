COMPONENT=TimeSyncUseAppC
PFLAGS = -I$(SZTETOSDIR)/lib/TimeSyncPoints
BUILD_EXTRA_DEPS = TimeMsg.java TimeSync.class
CLEAN_EXTRA = $(BUILD_EXTRA_DEPS) TimeMsg.java *.class

TimeMsg.java: TimeSyncUse.h
	mig -target=null -java-classname=TimeMsg java TimeSyncUse.h timemsg -o $@

TimeSync.class: TimeMsg.java TimeSync.java
	javac TimeSync.java

include $(MAKERULES)

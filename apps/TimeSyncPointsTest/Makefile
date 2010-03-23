COMPONENT=TimeSyncUseAppC
PFLAGS = -I$(SZTETOSDIR)/lib/TimeSyncPoints
BUILD_EXTRA_DEPS = TimeMsg.java 
CLEAN_EXTRA = $(BUILD_EXTRA_DEPS) TimeMsg.java 

TimeMsg.java: TimeSyncUse.h
	mig -target=null -java-classname=TimeMsg java TimeSyncUse.h timemsg -o $@

include $(MAKERULES)

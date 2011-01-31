COMPONENT=TestRadioRangeAppC
BUILD_EXTRA_DEPS=CommandMsg.java RangeTest.class
CLEAN_EXTRA = $(BUILD_EXTRA_DEPS) CommandMsg.class

CommandMsg.java: TestRadioRange.h
	mig -java-classname=CommandMsg java $? ser_msg -o $@

RangeTest.class: CommandMsg.java RangeTest.java
	javac $?

include $(MAKERULES)


COMPONENT=TestRadioRangeAppC
BUILD_EXTRA_DEPS=CommandMsg.java DataMsg.java RangeTest.class
CLEAN_EXTRA = $(BUILD_EXTRA_DEPS) CommandMsg.class

CommandMsg.java: TestRadioRange.h
	mig -java-classname=CommandMsg java $? ser_msg -o $@

DataMsg.java: TestRadioRange.h
	mig -java-classname=DataMsg java $? test_msg -o $@

RangeTest.class: CommandMsg.java DataMsg.java RangeTest.java
	javac $?

include $(MAKERULES)


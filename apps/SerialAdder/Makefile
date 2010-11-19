COMPONENT=SerialAdderAppC
BUILD_EXTRA_DEPS += TestSerial.class

TestSerial.class: $(wildcard *.java) TestSerialMsg.java
	javac *.java

TestSerialMsg.java:
	mig java -target=null -java-classname=TestSerialMsg SerialAdder.h test_serial_msg -o $@


include $(MAKERULES)

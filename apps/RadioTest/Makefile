COMPONENT=RadioTestAppC
BUILD_EXTRA_DEPS = setup_t.py SetupT.java
CLEAN_EXTRA = setup_t.py SetupT.java SetupT.class main.elf

setup_t.py: RadioTest.h
	mig python -target=$(PLATFORM) $(CFLAGS) -python-classname=setup_t RadioTest.h setup_t -o $@

SetupT.java: RadioTest.h
	mig -target=$(PLATFORM) -java-classname=SetupT java RadioTest.h setup_t -o $@

include $(MAKERULES)

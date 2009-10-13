COMPONENT=RadioTestAppC
BUILD_EXTRA_DEPS = setup_t.py edge_t.py testmsg_t.py
CLEAN_EXTRA = setup_t.py edge_t.py testmsg_t.py

setup_t.py: RadioTest.h
	mig python -target=$(PLATFORM) $(CFLAGS) -python-classname=setup_t RadioTest.h setup_t -o $@

edge_t.py: RadioTest.h
	mig python -target=$(PLATFORM) $(CFLAGS) -python-classname=edge_t RadioTest.h edge_t -o $@

testmsg_t.py: RadioTest.h
	mig python -target=$(PLATFORM) $(CFLAGS) -python-classname=testmsg_t RadioTest.h testmsg_t -o $@

include $(MAKERULES)

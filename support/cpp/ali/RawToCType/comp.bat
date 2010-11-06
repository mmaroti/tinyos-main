gcc -c -Os -fdata-sections -ffunction-sections Win32BlockDevice.c

g++ -c -Os -fdata-sections -ffunction-sections BlockChecker.cpp BlockDevice.cpp BlockIterator.cpp Header.cpp Sample.cpp SDCard.cpp SDCardImpl.cpp Tracker.cpp main.cpp

g++ -static BlockChecker.o BlockDevice.o BlockIterator.o Header.o Sample.o SDCard.o SDCardImpl.o Tracker.o Win32BlockDevice.o main.o -o test -Wl,-static,--gc-sections 

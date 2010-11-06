
gcc -c raw_device.c

g++ -c main.cpp

g++ raw_device.o main.o -o test

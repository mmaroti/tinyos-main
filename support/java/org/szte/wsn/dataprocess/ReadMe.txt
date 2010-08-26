Using Transfer from console:
----------------------------------------------------------------------------------------
You have to provide 8 or 1 parameter. Here are some examples.

java Transfer readMode sourcePath writeMode destinationPath structureFile toString seperator showname
java Transfer file 0.bin console stdOut structs.txt	toBinary , no		
	-reads from console writes to 0.bin, separator=, doesn't shows name
java Transfer serial serial@/dev/ttyUSB1:57600 file foo.txt structs.txt toString : showName 
	-reads from the serialSource on USB1, writes to foo.txt, separator=: shows the name of struct

If you want to write from .bin to a file, than only need 1 argument
java Transfer sourcefile
java Transfer 0.bin  
	-reads 0.bin to 0.txt	
java Transfer .  
	-scans the directory for .bin files, parse them, and writes to .txt


Using Transfer as Thread:
----------------------------------------------------------------------------------------
You can use Transfer as a thread in your application. You have to determine the binary and string media and the direction of communication.
Here are the constructors:

public Transfer(String binaryType, String binaryPath, String stringType, String stringPath, String structPath, boolean toString, String separator,boolean showName)
public Transfer(PacketParser[] packetParsers, BinaryInterface binary, StringInterface string, boolean toString)
public Transfer(String binaryPath, String stringPath)


Making new struct:
----------------------------------------------------------------------------------------
If you want to use a unique structure, you have to add it to the structs.txt (or the file you store the structures).
-To declare a new structure, you have to use "struct" keyword, the word after it will be the name of the structure. 
-Don't use special characters in the name of structures and variables!
-The content of a strucure must be between '{' and '}' and must end with ';'

-To declare a variable you have to write it's type and name followed by ';'
-Integers start with endiannes indicator, "nx_le" means little endian, othwerwise the variable will be big endian.
-The next one is sign indicator, "uint" means unsigned integer, "int" means signed integer.
-The next is the size of the variable in bit, only 8, 16, 32, 64 is supported
-const
-complex  TODO
-array
-The parser is case sensitive!
-The structure file mustn't contain other text like comments, license, author etc.


Sample:

struct simple{
  int8_t id=0x11;
  nx_le_uint16_t foo;
};

struct complex{
  int8_t id;
  simple const;
  nx_le_uint16_t humi;
  uint8_t light;
  omit uint32_t time;
  uint16_t value[1024];
};






		

	


#include <fstream>
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <string>
#include <sstream>
#include <list>
#include <cassert>

using namespace std;

namespace {

const int N_FIELDS = 7;

const unsigned int SAMPLING_RATE = 160;
const unsigned int TOLERANCE = 4;

struct dat {

	unsigned int   time;
	unsigned short counter;
	unsigned short ax;
	unsigned short ay;
	unsigned short az;
	unsigned short volt;
	unsigned short temp;
};

const char header[] = "Time,sequence_number,Accel_X,Accel_Y,Accel_Z,Gyro_X,Gyro_Y,Gyro_Z,Volt,Temp";

typedef list<list<dat>* >::iterator lli;
list<list<dat>*> data;

}

void exit(const string& msg) {
	cout << "Error: " << msg << "!" << endl;
	exit(1);
}

const string int2string(const int i) {
	ostringstream os;
	os << i << flush;
	return os.str();
}

void grab_content(const char* filename) {

	FILE* in = fopen(filename, "r");

	if (!in) {
		exit("failed to open input file");
	}

	cout << "Reading file " << filename << endl;

	//==========================================================================

	char first_line[sizeof(header)];

	fgets(first_line, sizeof(first_line), in);

	if (string(header)!=first_line) {
		exit("wrong file format");
	}

	//==========================================================================

	for (lli i=data.begin(); i!=data.end(); ++i) {
		delete *i;
	}

	data.clear();

	//==========================================================================

	const char* fmt = "%u,%hu,%hu,%hu,%hu,%*hu,%*hu,%*hu,%hu,%hu";

	unsigned int lines = 1;
	dat b;
	int n;
	unsigned short prevCnt = 1;
	unsigned int   prevTime   = 0;

	list<dat>* samples = 0;

	while ( (!ferror(in)) && (!feof(in)) ) {

		n = fscanf(in,fmt,&b.time,&b.counter,&b.ax,&b.ay,&b.az,&b.volt,&b.temp);

		if (n!= N_FIELDS) {
			cout << "Conversion failed on line " << lines << endl;
			break;
		}

		unsigned short cnt = b.counter;

		if (cnt == 1 && prevCnt != 0) {
			cout << "Found a re-boot on line " << lines << endl;
			samples = new list<dat>();
			data.push_back(samples);
		}
		else {

			if ((cnt-prevCnt != 1) && cnt) {
				cout << "Warning: data loss on line " << lines << endl;
			}

			unsigned int expected = prevTime + SAMPLING_RATE;

			unsigned int dt = (expected>b.time?expected:b.time) - expected;

			if (dt > TOLERANCE) {
				cout << "Warning: time gap is too long on line " << lines << endl;
			}
		}

		prevTime = b.time;
		prevCnt = cnt;

		samples->push_back(b);

		++lines;
	}

	cout << "Read " << lines << endl;

	fclose(in);

	in = 0;

}

int main(int argc, char* argv[]) {

	if (argc!=2) {
		exit("specify input file");
	}

	const char* filename = argv[1];

	grab_content(filename);

	return 0;
}

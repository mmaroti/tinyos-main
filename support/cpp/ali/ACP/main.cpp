
#include <fstream>
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <cstdio>
#include <string>
#include <sstream>
#include <list>
#include <vector>
#include <cassert>

using namespace std;

namespace {

const int N_FIELDS = 7;

typedef unsigned short uint16;
typedef unsigned int   uint32;

const uint32 TICKS_PER_SEC = 32768;
const uint32 SAMPLING_RATE = 160;
const uint32 TOLERANCE = 4;

struct dat {

	uint32 time;
	uint16 counter;
	uint16 ax;
	uint16 ay;
	uint16 az;
	uint16 volt;
	uint16 temp;
};

const char header[] = "Time,sequence_number,Accel_X,Accel_Y,Accel_Z,Gyro_X,Gyro_Y,Gyro_Z,Volt,Temp";

vector<list<dat>*> data;

typedef list<dat>::reverse_iterator rli;

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

const string time2str(uint32 t) {

	ostringstream os;

	uint32 hour, min, sec, milli;

	hour = t/(3600*TICKS_PER_SEC);
	t =    t%(3600*TICKS_PER_SEC);

	min = t/(60*TICKS_PER_SEC);
	t   = t%(60*TICKS_PER_SEC);

	sec = t/TICKS_PER_SEC;
	t   = t%TICKS_PER_SEC;

	milli = t/(TICKS_PER_SEC/1000.0);

	os << setfill('0') << setw(2) << hour << ":";
	os << setfill('0') << setw(2) << min  << ":";
	os << setfill('0') << setw(2) << sec  << ".";
	os << setfill('0') << setw(3) << milli<< flush;

	return os.str();
}

bool str2ticks(uint32& ticks) {

	uint32 hour, min, sec;
	string sh;
	string sm;
	string ss;
	getline(cin, sh, ':');
	getline(cin, sm, ':');
	getline(cin, ss);
	hour = atoi(sh.c_str());
	min = atoi(sm.c_str());
	sec = atoi(ss.c_str());

	if ((hour<0 || hour>99) || (min<0 || min>59) || (sec<0 || sec>59)) {
		cout << "Error: incorrect format for time string" << endl;
		ticks = 0;
		return true;
	}

	ticks = (3600*TICKS_PER_SEC)*hour + (60*TICKS_PER_SEC)*min + TICKS_PER_SEC*sec;

	return false;
}

void check_data_consistency(const dat& b, uint32 lines, uint32 prevTime, uint16 prevCnt) {

	uint16 cnt = b.counter;

	if ((cnt-prevCnt != 1) && cnt) {
		cout << "Warning: line " << lines << flush;
		cout << ", missing " << (cnt-prevCnt-1) << " samples" << endl;
	}

	uint32 expected = prevTime + SAMPLING_RATE;
	uint32 dt = (expected>b.time?expected:b.time) - expected;

	if (dt > TOLERANCE) {
		cout << "Warning: line " << lines << flush;
		cout << ", " << dt << " ticks error" << endl;
	}
}

void allocate_container(list<dat>*& samples, const dat& b, uint32& t0, uint32 lines) {

	cout << "Found a re-boot on line " << lines << endl;
	samples = new list<dat>();
	data.push_back(samples);
	t0 = b.time;
}

void print_info() {

	const int n = static_cast<int> (data.size());
	cout << n << " measurements" << endl;

	for (int i=0; i<n; ++i) {
		list<dat>* samples = data.at(i);
		rli rbeg(samples->rbegin());

		cout << "Length of #" << (i+1) << " is "<< time2str(rbeg->time) << endl;
	}
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

	const int m = static_cast<int> (data.size());
	for (int i=0; i<m; ++i) {
		delete data.at(i);
	}

	data.clear();

	//==========================================================================

	const char* fmt = "%u,%hu,%hu,%hu,%hu,%*hu,%*hu,%*hu,%hu,%hu";

	uint32 lines = 1;
	dat b;
	int n;
	uint16 prevCnt = 1;
	uint32 prevTime   = 0;

	list<dat>* samples = 0;
	uint32 t0 = 0;

	//==========================================================================

	while ( (!ferror(in)) && (!feof(in)) ) {

		n = fscanf(in,fmt,&b.time,&b.counter,&b.ax,&b.ay,&b.az,&b.volt,&b.temp);

		if (n!= N_FIELDS) {
			cout << "Conversion failed on line " << lines << endl;
			break;
		}

		++lines;

		if (b.counter == 1 && prevCnt != 0) {

			allocate_container(samples, b, t0, lines);
		}
		else {

			check_data_consistency(b, lines, prevTime, prevCnt);
		}

		prevTime = b.time;
		prevCnt  = b.counter;

		b.time -= t0;

		samples->push_back(b);
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

	print_info();

	while (true) {

		print_info();

		cout << "Select measurement (or -1 to quit): " << flush;

		int n = 0;
		cin >> n;

		if (n==-1) {
			break;
		}
		else if (n < 1 || n > data.size()) {
			cout << "Error: " << n << " is out of range (" << flush;
			cout << 1 << ", " << data.size() << ")" << endl;
			continue;
		}

		cout << "Length of #" << n << " is "<< flush;
		cout << time2str(data.at(n-1)->rbegin()->time) << endl;


		cout << "Enter start in hh:mm:ss format" << endl;

		uint32 start;
		if (str2ticks(start))
			continue;

		cout << "Enter end in hh:mm:ss format" << endl;

		uint32 end;
		if (str2ticks(end))
			continue;

		cout << "Cropping from " << time2str(start) << " to " << time2str(end) << endl;
		cout << "Check the time strings, are they OK? [y,n]" << endl;

		char c = cin.get();

		if (c!='y')
			continue;

		cout << "Writing chopped file" << endl;

	}

	cout << "Terminated ..." << endl;

	return 0;
}

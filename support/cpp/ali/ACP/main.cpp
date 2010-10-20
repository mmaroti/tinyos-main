/** Copyright (c) 2010, University of Szeged
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions
* are met:
*
* - Redistributions of source code must retain the above copyright
* notice, this list of conditions and the following disclaimer.
* - Redistributions in binary form must reproduce the above
* copyright notice, this list of conditions and the following
* disclaimer in the documentation and/or other materials provided
* with the distribution.
* - Neither the name of University of Szeged nor the names of its
* contributors may be used to endorse or promote products derived
* from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
* "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
* FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
* COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
* OF THE POSSIBILITY OF SUCH DAMAGE.
*
* Author: Ali Baharev
*/

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

const int N_FIELDS = 10;

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
	uint16 wx;
	uint16 wy;
	uint16 wz;
	uint16 volt;
	uint16 temp;
};

const char header[] = "Time,sequence_number,Accel_X,Accel_Y,Accel_Z,Gyro_X,Gyro_Y,Gyro_Z,Volt,Temp";

vector<list<dat>*> data;

typedef list<dat>::const_iterator cli;
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

bool str2ticks(uint32& ticks, const uint32 max) {

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

	if (ticks > max) {
		cout << "Error: the entered time is greater that the length!" << endl;
		return true;
	}

	return false;
}

void check_data_consistency(const dat& b, uint32 lines, uint32 prevTime, uint16 prevCnt) {

	uint16 cnt = b.counter;

	if (!((cnt-prevCnt == 1) || (cnt==0 && prevCnt==0xFFFF))) {
		cout << "Warning: line " << lines << flush;
		cout << ", missing " << (cnt-prevCnt-1) << " samples" << endl;
	}

	uint32 expected = prevTime + SAMPLING_RATE;
	int dt = b.time - expected;

	if (abs(dt) > TOLERANCE) {
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

	const char* fmt = "%u,%hu,%hu,%hu,%hu,%hu,%hu,%hu,%hu,%hu";

	uint32 lines = 1;
	dat b;
	int n;
	uint16 prevCnt = 1;
	uint32 prevTime   = 0;

	list<dat>* samples = 0;
	uint32 t0 = 0;

	//==========================================================================

	while ( (!ferror(in)) && (!feof(in)) ) {

		n = fscanf(in,fmt,&b.time,&b.counter,&b.ax,&b.ay,&b.az,&b.wx, &b.wy, &b.wz, &b.volt,&b.temp);

		++lines;

		if (n!= N_FIELDS) {
			cout << "Conversion failed on line " << lines << endl;
			break;
		}

		if (b.time==0 && b.counter==0) {
			cout << "Warning: found a line with zeros, at " << lines;
			cout << ", reading stopped!" << endl;
			break;
		}

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

void dump_data(const char* filename, int index, uint32 start, uint32 end) {

	const list<dat>* const samples = data.at(index-1);

	cli i(samples->begin());

	while (i!=samples->end() && i->time < start) {
		++i;
	}

	ostringstream os;

	os << filename << '_' << index << '_' << start << '_' << end << flush;

	ofstream out(os.str().c_str());

	if (!out) {
		exit("failed to create output file");
	}

	out << "#" << header << ",RotMat[9]" << endl;

	uint32 lines = 0;

	while (i!=samples->end() && i->time <= end) {

		dat b = *i;
		// b.counter removed, currently not supported in TestShimmer
		out << b.time << ',' << b.ax << ',' << b.ay << ',' << b.az;
		out << ',' << b.wx << ',' << b.wy << ',' << b.wz  << ',' << b.volt << ',' << b.temp ;
		out << ',' << 1 << ',' << 0 << ',' << 0;
		out << ',' << 0 << ',' << 1 << ',' << 0;
		out << ',' << 0 << ',' << 0 << ',' << 1;
		out << endl;

		++i;
		++lines;
	}

	out.close();

	cout << lines << " lines written" << endl;
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
		else if (n < 1 || n > static_cast<int> (data.size())) {
			cout << "Error: " << n << " is out of range (" << flush;
			cout << 1 << ", " << data.size() << ")" << endl;
			continue;
		}

		const uint32 ticks = data.at(n-1)->rbegin()->time;

		cout << "Length of #" << n << " is "<< flush;
		cout << time2str(ticks) << endl;


		cout << "Enter start in hh:mm:ss format" << endl;

		uint32 start;
		if (str2ticks(start, ticks))
			continue;

		cout << "Enter end in hh:mm:ss format" << endl;

		uint32 end;
		if (str2ticks(end, ticks))
			continue;

		if (start >= end) {

			cout << "Error: start must be before end!" << endl;
			continue;
		}

		cout << "Cropping from " << time2str(start) << " to " << time2str(end) << endl;
		cout << "Check the time strings, are they OK? [y,n]" << endl;

		char c = cin.get();

		if (c!='y')
			continue;

		cout << "Writing cropped file" << endl;

		dump_data(filename, n, start, end);
	}

	cout << "Terminated ..." << endl;

	return 0;
}

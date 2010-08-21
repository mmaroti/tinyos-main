
#include <iostream>
#include <cassert>

using namespace std;

const unsigned int cardSize = 130;

int sector[cardSize];

void reset(int k) {

	assert (k<=cardSize);

	for (int i=0; i<cardSize; ++i) {
		sector[i] = 0;
	}

	for (int i=0; i<k; ++i) {
		sector[i] = 1;
	}
}

void search() {

	unsigned int mid;
	int length;
	int steps = 0;
	unsigned int low = 1;
	unsigned int high = cardSize -1;

	while (low!=mid && low<=high) {
		++steps;
		mid = low + ((high-low) >> 1);

		length = sector[mid];

		if (length == 0) {

			high = mid;
		}
		else {
			low = mid+1;
		}

	}

	cout << "low, mid, high, length, steps: " << low << ", " << mid << ", " << high << ", " << length << ", " << steps<< endl;

}

int main() {

	for (int i=1; i<=cardSize; ++i) {
		cout << "Running: " << i << endl;
		reset(i);
		search();
	}

	return 0;
}


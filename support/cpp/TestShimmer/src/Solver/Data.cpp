#include "DataRecorder.h"

// FIXME Eliminate this hideous workaround
DataRecorder* dr = 0;

namespace ipo {

int n_samples() {

    return dr->size();
}

void at(int i, double data[SIZE]) {

    dr->at(i, data);
}

}


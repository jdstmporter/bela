/*
 * filter.cpp
 *
 *  Created on: 20 Jun 2020
 *      Author: julianporter
 */



#include "filter.hpp"

Filter::Filter(const uint16_t n,const uint32_t b) : nTaps(n), blockSize(b), filter() {
	taps = new float[nTaps];
	state = new float[nTaps+blockSize-1];
	output = new float[blockSize];
}
Filter::~Filter() {
		delete [] output;
		delete [] state;
		delete [] taps;
	}

void Filter::load(float *ir) {
		for(auto i=0;i<nTaps;i++) taps[i]=ir[i];
		ne10_fir_init_float(&filter, nTaps, taps, state, blockSize);
	}
void Filter::operator()(float *input) {
		ne10_fir_float_neon(&filter, input, output, blockSize);
	}

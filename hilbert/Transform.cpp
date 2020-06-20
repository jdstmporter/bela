/*
 * Transform.cpp
 *
 *  Created on: 20 Jun 2020
 *      Author: julianporter
 */

#include "Transform.hpp"

HilbertTransform::HilbertTransform(const uint16_t nStages,const uint32_t b) : nTaps(2*nStages+1), blockSize(b), hilbert(nTaps,blockSize), delay(nTaps,blockSize),
		buffer(new ncomplex[blockSize])   {

		auto hilbertTaps=new float[nTaps];
		for(uint16_t i=0;i<nStages;i++) {
			float value = (0==(i&1)) ? 0.0 : 2.0/(M_PI*i);
			hilbertTaps[nStages+i]=value;
			hilbertTaps[nStages-i]=-value;
		}
		hilbert.load(hilbertTaps);
		delete [] hilbertTaps;

		auto delayTaps=new float[nTaps];
		for(uint16_t i=0;i<nTaps;i++) {
			delayTaps[i] = (i==nStages) ? 1.0 : 0.0;
		}
		delay.load(delayTaps);
		delete [] delayTaps;
	}
HilbertTransform::~HilbertTransform() {
		delete [] buffer;
	}

void HilbertTransform::operator()(float *input) {
		hilbert(input);
		delay(input);
		for(uint32_t i=0;i<blockSize;i++) buffer[i]=ncomplex(hilbert[i],delay[i]);
	}

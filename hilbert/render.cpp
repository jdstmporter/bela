/*
 * render.cpp
 *
 *  Created on: 19 Jun 2020
 *      Author: julianporter
 */


#define ENABLE_NE10_FIR_FLOAT_NEON	// Define needed for Ne10 library


#include <Bela.h>
#include <cmath>
#include <complex>
#include <cstdint>
#include <ne10/NE10.h> // neon library
#include <libraries/Scope/Scope.h>


typedef ne10_float32_t nfloat;
typedef std::complex<float> ncomplex;

class Filter {
	private:
	uint16_t nTaps;
	uint32_t blockSize;
	ne10_fir_instance_f32_t filter;
	float *taps;
	float *state;
	float *output;


	public:
	Filter(const uint16_t n,const uint32_t b) : nTaps(n), blockSize(b), filter() {
		taps = new nfloat[nTaps];
		state = new nfloat[nTaps+blockSize-1];
		output = new nfloat[blockSize];
	}

	virtual ~Filter() {
		delete [] output;
		delete [] state;
		delete [] taps;
	}

	void load(nfloat *ir) {
		for(auto i=0;i<nTaps;i++) taps[i]=ir[i];
		ne10_fir_init_float(&filter, nTaps, taps, state, blockSize);
	}

	void operator()(nfloat *input) {
		ne10_fir_float_neon(&filter, input, output, blockSize);
	}

	nfloat operator[](const uint32_t idx) const { return output[idx]; }
};



// filter vars
ne10_fir_instance_f32_t hilbertFilter;
nfloat *hilbertTaps;
nfloat *hilbertState;
ne10_fir_instance_f32_t delayFilter;
nfloat *delayTaps;
nfloat *delayState;

nfloat *input;
nfloat *imag;
nfloat *delayed;

ncomplex *inter;
ncomplex *shifter;

#define NSTAGES 66
#define FILTER_TAP_NUM (1+2*NSTAGES)

template<typename T>
T * get(uint32_t n) {
	return (T *)NE10_MALLOC(n*sizeof(T));
}

#define FREQ 1000.0

unsigned blockSize;
unsigned nChannels;

nfloat phase;
//nuint offset=0;
nfloat invSampleRate;
nfloat shiftInvSampleRate;
nfloat shiftPhase=0;

nfloat shiftFreq;

Scope scope;

void toComplex() {
	ne10_fir_float_neon(&hilbertFilter, input, imag, blockSize);
	ne10_fir_float_neon(&delayFilter, input, delayed, blockSize);
	for(uint32_t i=0;i<blockSize;i++) inter[i]=ncomplex(input[i],delayed[i]);
}


bool setup(BelaContext *context, void *userData)
{
	// Retrieve a parameter passed in from the initAudio() call

	// tell the scope how many channels and the sample rate
	scope.setup(2, context->audioSampleRate);

	blockSize = context->audioFrames;
	nChannels = context -> audioOutChannels;

	input=get<nfloat>(blockSize);
	imag=get<nfloat>(blockSize);
	delayed=get<nfloat>(blockSize);

	shifter=get<ncomplex>(blockSize);

	inter=new ncomplex[blockSize];

	hilbertTaps=get<nfloat>(FILTER_TAP_NUM);
	for(uint16_t i=0;i<NSTAGES;i++) {
		nfloat value = (0==(i&1)) ? 0.0 : 2.0/(M_PI*i);
		hilbertTaps[NSTAGES+i]=value;
		hilbertTaps[NSTAGES-i]=-value;
	}
	hilbertState=get<float>(FILTER_TAP_NUM+blockSize-1);
	ne10_fir_init_float(&hilbertFilter, FILTER_TAP_NUM, hilbertTaps, hilbertState, blockSize);

	delayTaps=get<float>(FILTER_TAP_NUM);
	for(uint16_t i=0;i<FILTER_TAP_NUM;i++) {
		delayTaps[i] = (i==NSTAGES) ? 1.0 : 0.0;
	}
	delayState=get<float>(FILTER_TAP_NUM+blockSize-1);
	ne10_fir_init_float(&delayFilter, FILTER_TAP_NUM, delayTaps, delayState, blockSize);

	shiftFreq = 1000.0 * analogRead(context,0,0);
	invSampleRate = 2.0*(nfloat)M_PI*FREQ/(nfloat)context->audioSampleRate;
	shiftInvSampleRate = 2.0*(nfloat)M_PI/(nfloat)context->audioSampleRate;

	return true;
}

void render(BelaContext *context, void *userData)
{
	shiftFreq = (analogRead(context,0,0)-0.5) * 8000.0;
	auto nSamples = context->audioFrames;
	for(unsigned n = 0; n < nSamples; n++) {
		input[n] = audioRead(context,n,0);

		shifter[n]=std::polar<float>(1.0,shiftPhase);
		shiftPhase += shiftInvSampleRate*shiftFreq;
		while (shiftPhase > M_PI) shiftPhase -= 2.0f * (nfloat)M_PI;
		while (shiftPhase < -M_PI) shiftPhase += 2.0f * (nfloat)M_PI;

	}
	ne10_fir_float_neon(&hilbertFilter, input, imag, blockSize);
	ne10_fir_float_neon(&delayFilter, input, delayed, blockSize);
	for(uint32_t i=0;i<nSamples;i++) {
		inter[i]=ncomplex(imag[i],delayed[i]) * shifter[i];
	}

	for(unsigned n=0;n<blockSize;n++) {
		scope.log(input[n],inter[n].real());
		audioWrite(context, n, 0, input[n]);
		audioWrite(context, n, 1, inter[n].real());
	}
}




void cleanup(BelaContext *context, void *userData)
{
	NE10_FREE(hilbertTaps);
	NE10_FREE(hilbertState);
	NE10_FREE(delayTaps);
	NE10_FREE(delayState);
	NE10_FREE(input);
	NE10_FREE(imag);
	NE10_FREE(shifter);
	delete[] inter;
}




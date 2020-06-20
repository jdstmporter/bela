/*
 * Transform.hpp
 *
 *  Created on: 20 Jun 2020
 *      Author: julianporter
 */

#ifndef TRANSFORM_HPP_
#define TRANSFORM_HPP_

#define ENABLE_NE10_FIR_FLOAT_NEON
#include <Bela.h>
#include <cmath>
#include <complex>
#include <cstdint>
#include <ne10/NE10.h> // neon library
#include "filter.hpp"

typedef std::complex<float> ncomplex;

class HilbertTransform {
	private:
	uint16_t nTaps;
	uint32_t blockSize;
	Filter hilbert;
	Filter delay;
	ncomplex *buffer;

	public:
	HilbertTransform(const uint16_t nStages,const uint32_t b);
	virtual ~HilbertTransform();
	void operator()(float *input);
	ncomplex operator[](const uint32_t idx) const { return buffer[idx]; }
};





#endif /* TRANSFORM_HPP_ */

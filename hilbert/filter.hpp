/*
 * filter.hpp
 *
 *  Created on: 20 Jun 2020
 *      Author: julianporter
 */

#ifndef FILTER_HPP_
#define FILTER_HPP_

#define ENABLE_NE10_FIR_FLOAT_NEON
#include <Bela.h>
#include <cmath>
#include <cstdint>
#include <ne10/NE10.h> // neon library

class Filter {
	private:
	uint16_t nTaps;
	uint32_t blockSize;
	ne10_fir_instance_f32_t filter;
	float *taps;
	float *state;
	float *output;

	public:
	Filter(const uint16_t n,const uint32_t b);
	virtual ~Filter();

	void load(float *ir);
	void operator()(float *input);
	float operator[](const uint32_t idx) const { return output[idx]; }
};


#endif /* FILTER_HPP_ */

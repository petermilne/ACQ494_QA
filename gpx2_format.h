/*
 * gpx2_format.h
 *
 *  Created on: 12 Jun 2022
 *      Author: pgm
 */

#include <assert.h>

#ifndef GPX2_FORMAT_H_
#define GPX2_FORMAT_H_

// define the fields ONCE
#define GPX_SITE_MASK	0xf000000000000000ULL
#define GPX_CHAN_MASK   0x0f00000000000000ULL
#define GPX_FLAGS_MASK  0x00ff000000000000ULL
#define GPX_NREF_MASK	0x0000ffffff000000ULL
#define GPX_STOP_MASK	0x0000000000ffffffULL

#define EGU_NREF	100e-9
#define EGU_STOP	1e-12

#define GPX_STOP_MAX	100000

#define GPX_PPSSIG_MASK 0xff00000000000000ULL
#define GPX_PPSREF_MASK 0x00ffff0000000000ULL
#define GPX_PPSTAI_MASK 0x000000ffffffffffULL

#define GPX_PPSSIG	0xa100000000000000ULL

/* calculating shift value at run time .. inefficient, but duplication/error free */
int bit(unsigned long long mask){
	for (int bit = 0; bit < 64; ++bit){
		if ((mask & (1ULL<<bit)) != 0){
			return bit;
		}
	}
	return 0;
}

// Bit shift positions are derived ONCE from the MASK and kept as constants for min runtime overhead
namespace GPX_BITS {
	const unsigned SITE = bit(GPX_SITE_MASK);
	const unsigned CHAN = bit(GPX_CHAN_MASK);
	const unsigned FLAGS = bit(GPX_FLAGS_MASK);
	const unsigned NREF = bit(GPX_NREF_MASK);
	const unsigned STOP = bit(GPX_STOP_MASK);

	const unsigned PPSSIG = bit(GPX_PPSSIG_MASK);
	const unsigned PPSREF = bit(GPX_PPSREF_MASK);
	const unsigned PPSTAI = bit(GPX_PPSTAI_MASK);

	const unsigned PPSID  = GPX_PPSSIG >> PPSSIG;
};

unsigned long long gpx_to_raw(unsigned sc, unsigned nref, unsigned stop){
	unsigned long long raw_site = (sc/10); raw_site <<= GPX_BITS::SITE;
	unsigned long long raw_ch   = (sc%10); raw_ch   <<= GPX_BITS::CHAN;
	unsigned long long raw_nref = nref;    raw_nref <<= GPX_BITS::NREF;

	assert(bit(GPX_STOP_MASK) == 0);

	unsigned long long raw = raw_site | raw_ch | raw_nref | stop;
	return raw;
}

int gpx_from_raw(unsigned long long raw, unsigned& sc, unsigned& nref, unsigned& stop){
	sc = ((raw&GPX_SITE_MASK) >> GPX_BITS::SITE)*10 +
	     ((raw&GPX_CHAN_MASK) >> GPX_BITS::CHAN);
	nref = (raw&GPX_NREF_MASK) >> GPX_BITS::NREF;
	stop = (raw&GPX_STOP_MASK);
	return 0;
}

double gpx_from_raw(unsigned long long raw, unsigned& sc){
	unsigned nref, stop;
	gpx_from_raw(raw, sc, nref, stop);

	double seconds = (double)nref*EGU_NREF + (double)stop*EGU_STOP;
	return seconds;
}

// sc==0 : TAI seconds
bool gpx2_valid_sc(unsigned sc){
	unsigned site = sc/10;
	unsigned ch = sc%10;
	return site>=0 && site<=6 && ch>=0 && ch<=4;
}


bool gpx2_is_pps(unsigned long long raw, unsigned long long& tai, short& nref) {
	if ((raw&GPX_PPSSIG_MASK) == GPX_PPSSIG){
		tai = raw&GPX_PPSTAI_MASK;
		nref = (raw&GPX_PPSREF_MASK) >> GPX_BITS::PPSREF;
		return true;
	}else{
		return false;
	}
}
#endif /* GPX2_FORMAT_H_ */

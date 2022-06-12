/*
 * gpx2_format.h
 *
 *  Created on: 12 Jun 2022
 *      Author: pgm
 */

#include <assert.h>

#ifndef GPX2_FORMAT_H_
#define GPX2_FORMAT_H_

#define GPX_SITE_MASK	0xff00000000000000ULL
#define GPX_CHAN_MASK   0x00ff000000000000ULL
#define GPX_NREF_MASK	0x0000ffffff000000ULL
#define GPX_STOP_MASK	0x0000000000ffffffULL

#define EGU_NREF	100e-9
#define EGU_STOP	1e-12


/* calculating shift value at run time .. inefficient, but duplication/error free */
int bit(unsigned long long mask){
	for (int bit = 0; bit < 64; ++bit){
		if ((mask & (1ULL<<bit)) != 0){
			return bit;
		}
	}
	return 0;
}



unsigned long long gpx_to_raw(unsigned sc, unsigned nref, unsigned stop){
	unsigned long long raw_site = (sc/10); raw_site <<= bit(GPX_SITE_MASK);
	unsigned long long raw_ch   = (sc%10); raw_ch   <<= bit(GPX_CHAN_MASK);
	unsigned long long raw_nref = nref;    raw_nref <<= bit(GPX_NREF_MASK);

	assert(bit(GPX_STOP_MASK) == 0);

	unsigned long long raw = raw_site | raw_ch | raw_nref | stop;
	return raw;
}

int gpx_from_raw(unsigned long long raw, unsigned& sc, unsigned& nref, unsigned& stop){
	sc = ((raw&GPX_SITE_MASK) >> bit(GPX_SITE_MASK))*10 +
	     ((raw&GPX_CHAN_MASK) >> bit(GPX_CHAN_MASK));
	nref = (raw&GPX_NREF_MASK) >> bit(GPX_NREF_MASK);
	stop = (raw&GPX_STOP_MASK);
	return 0;
}

double gpx_from_raw(unsigned long long raw, unsigned& sc){
	unsigned nref, stop;
	gpx_from_raw(raw, sc, nref, stop);

	double seconds = (double)nref*EGU_NREF + (double)stop*EGU_STOP;
	return seconds;
}

#endif /* GPX2_FORMAT_H_ */
